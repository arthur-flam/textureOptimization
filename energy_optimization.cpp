#include "synth_texture.h"


void synth_texture::minimize_energy(){
    //resize(image, out_image, current_size_out);
  nlNewContext(); 
  nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI);
  nlSolverParameteri(NL_NB_VARIABLES, out_width*out_height*3);
  nlSolverParameteri(NL_MAX_ITERATIONS, out_width*out_height*10);
  nlSolverParameterd(NL_THRESHOLD, 1e-10);
  nlBegin(NL_SYSTEM);    
  image_to_solver();
  setup_matrix() ;
  nlEnd(NL_SYSTEM) ;

   //solver_to_image();
    //return;

  std::cout << "...solving" << std::endl ;
  nlSolve() ;
 // recover results and clean up
  cout << "...recovering results" << endl ;
  solver_to_image();

  nlDeleteContext(nlGetCurrent());
};

// send initial guess to solver internals
void synth_texture::image_to_solver(){
  int counter=0;
  for(int v=0; v<out_height; ++v){
    for(int h=0; h<out_width; ++h){
      for(int color=0; color<3; ++color){
        int var = out_image.at<Vec3b>(v,h)[color];
        nlSetVariable(counter++, (NLdouble) var);
     }
  }
}
};

  // recover solution from solver internals
void synth_texture::solver_to_image(){
  int counter=0;
  for(int v=0; v<out_height; ++v){
    for(int h=0; h<out_width; ++h){
      Vec3b colors_p;
      for(int color=0; color<3; ++color){
         colors_p[color] = nlGetVariable(counter++);
      }
      out_image.at<Vec3b>(v,h) = colors_p;
   }
}
out_image_bw = Mat(current_size_out, CV_8UC1);
cvtColor(out_image, out_image_bw, CV_BGR2GRAY);
};

// send image information to solver
void synth_texture::setup_matrix(){
  nlBegin(NL_MATRIX);
  int counter=0;
    // for all points    
  for(int v=0; v<out_height; ++v){
    for(int h=0; h<out_width; ++h){
	     Point p = Point(h, v); // x,y
	     // for neighbooring gridpoints
	     Vec3d sum_color_sources=Vec3d(0,0,0); // double pour éviter les overflows.....
	     int n_grid=0;
	     for(int h_dir=0; h_dir<2;++h_dir){
         for(int v_dir=0; v_dir<2; ++v_dir){
          int h_to_grid = -(h % grid_step) + h_dir*grid_step;
          int v_to_grid = -(v % grid_step) + v_dir*grid_step;
          Point to_grid_point = Point(h_to_grid, v_to_grid);
          Point grid_point = p + to_grid_point;
          if(grid_point.x<=0 || grid_point.y<=0 || grid_point.y>=out_height || grid_point.x>=out_width)
            continue;
         ++n_grid;
	        Vec2b source_point = Zp.at<Vec2b>(grid_point); // source gridpoints coordinates in texture
	        Point source_coords = Point(source_point[0]-h_to_grid, source_point[1]-v_to_grid);
	        for(int color=0; color<3; ++color)
             sum_color_sources[color] += (double) image.at<Vec3b>(source_coords)[color];
         }
        }
        for(int color=0; color<3; ++color){
           nlRowParameterd(NL_RIGHT_HAND_SIDE, (NLdouble) -sum_color_sources[color]/n_grid);     // b term
   	     nlBegin(NL_ROW);
   	     nlCoefficient(counter++, (NLdouble) 1);
   	     nlEnd(NL_ROW);
         }
      }
   }
nlEnd(NL_MATRIX); 
};
