#include "synth_texture.h"


void synth_texture::minimize_energy(){
  cout << "...starting energy optimization" << endl;
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

  std::cout << "......solving" << std::endl ;
  nlSolve() ;
 // recover results and clean up
  cout << "......recovering results" << endl ;
  solver_to_image();


  nlDeleteContext(nlGetCurrent());
};

// send initial guess to solver internals
void synth_texture::image_to_solver(){
  std::cout << "......image to solver (init)" << std::endl ;
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
  cout << "......solver to image" << endl ;
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
};

// send image information to solver
void synth_texture::setup_matrix(){
  cout << "......image to solver (params)" << std::endl ;
  nlBegin(NL_MATRIX);
   // for all pixels in the image    
  int counter=0;
  for(int v=0; v<out_height; ++v){
    for(int h=0; h<out_width; ++h){
	     Point p = Point(h, v);
        // for its neighbooring gridpoints
	     Vec3d sum_color_sources=Vec3d(0,0,0); // we need to do a weighted average of the values at gridpoint around a given pixel 
        double weight_spatial;                        // gridpoints closer are more important
        double weight_similarity=1;                     // gridpoints more similar are more important
        double sum_weight=0;
	     for(int h_dir=0; h_dir<2;++h_dir){    // up or down 
         for(int v_dir=0; v_dir<2; ++v_dir){  // left or right
          int h_to_grid = -(h % grid_step) + h_dir*grid_step;
          int v_to_grid = -(v % grid_step) + v_dir*grid_step;
          Point to_grid_point = Point(h_to_grid, v_to_grid);
          Point grid_point = p + to_grid_point;
          if(grid_point.x<=0 || grid_point.y<=0 || grid_point.y>=out_height || grid_point.x>=out_width)
            continue; // -> out of bounds
	       Vec2b source_point = Zp.at<Vec2b>(grid_point); // source gridpoints coordinates in texture
	       Point source_coords = Point(source_point[0]-h_to_grid, source_point[1]-v_to_grid);
          
          // Robust optimization
          if(grid_point.x<=current_size_out.width-grid_step && grid_point.y<=current_size_out.height-grid_step && grid_point.x>=grid_step && grid_point.y>=grid_step){
             // -> is the gridpoint close ?
             double dist = sqrt(to_grid_point.x*to_grid_point.x + to_grid_point.y*to_grid_point.y);
             //weight_spatial = 1;
             weight_spatial = max(0.01,exp(-dist*dist/2/(grid_step*grid_step/9)));             
             // -> is this gridpoint an outlier ?
             Mat n_source = extract_neighborhood(image ,source_point[0], source_point[1]);
             //cout << ".........done neighs1" << endl ;
             //cout << out_image.size() << "   on " << grid_point << grid_step<< endl;
             Mat n_out = extract_neighborhood(out_image,grid_point.x,grid_point.y);
             //cout << ".........done neighs2" << endl ;
             Mat diff = n_out-n_source;
             double norm = sqrt(diff.dot(diff));
             double r=0.8;
             weight_similarity = max(min(pow(norm,r-2),10000.0),0.0001);
             //weight_similarity = 1;
          }
          double weight = weight_similarity*weight_spatial;
          sum_weight+=weight;
	       for(int color=0; color<3; ++color)
               sum_color_sources[color] += (double) image.at<Vec3b>(source_coords)[color] * weight;
         }
        }
        for(int color=0; color<3; ++color){
           nlRowParameterd(NL_RIGHT_HAND_SIDE, (NLdouble) -sum_color_sources[color]/sum_weight);     // b term
   	     nlBegin(NL_ROW);
   	     nlCoefficient(counter++, (NLdouble) 1);
   	     nlEnd(NL_ROW);
         }
      }
   }
nlEnd(NL_MATRIX); 
};
