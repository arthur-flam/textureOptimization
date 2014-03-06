#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "NL/nl.h"
#include <math.h>

#include "synth_texture.h"

using namespace cv;
using namespace std;


// TODO
// - color
// - robust optimization 
// - graph évolution energie vs iterations
// - test sur toutes les images


// What I did not do:
// - the authors use a memory efficient kd tree. They only record the position of the source and recover more info as needed.
// - we could compute the kdtree only once per scale level
//      * I ran into segfaults errors when I tried...
// - we could use a different kind of kdtree
// - we could get rig of edge effects using some padding

synth_texture::synth_texture(Mat texture, Size size){
  out_size = size;
  raw_image = texture;
  max_iter=500;
  level=0;
  update_level(level);
};

Mat synth_texture::extract_neighborhood(Mat M, int h, int v){
    // output as a 1xp vector
  return M(Range(v-grid_step, v+grid_step), Range(h-grid_step, h+grid_step)).clone().reshape(1,1);
}

void synth_texture::synthetize(){
    namedWindow( "Output", WINDOW_AUTOSIZE );    // Create windows for display
    namedWindow( "Texture", WINDOW_AUTOSIZE );
    moveWindow("Output", 10, 50);
    moveWindow("Texture", 10, 500); waitKey(10);

    //imshow( "Output", out_image_bw );imshow( "Texture", image_bw );waitKey(1000);

    // Initialize neighboroods randomly 
    randu(Zp, Scalar(grid_step+1,grid_step+1), Scalar(in_width-grid_step-1, in_height-grid_step-1));
    for(int iter=1;iter<max_iter;++iter){
      // Alternatively optimize for energy and source locations
      cout << "==== iteration [" << iter << "] ===="<<endl;
      minimize_energy();
      imshow( "Output", out_image );imshow( "Texture", image );waitKey(1000);
      //return;
      update_neighborhoods();

      // Change scale if needed
      double improv = abs(energy_previous-energy)/energy_previous;
      double treshold = pow(10,-level-1);
      cout << "...improvement: " << improv;
      cout << " (next level: 10e" << -level-1 << ")" << endl;
      if(improv<treshold){ // we refine progressively
       if(level>5) return;
       update_level(++level);
       update_neighborhoods();
     }
   }
 };






// MAIN FUNCTION
 int main(int argc, char** argv )
 {
  // 1. Load texture image
  if( argc != 2)
  {
    cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
    return -1;
  }
  Mat image;
  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  if(! image.data )
  {
    cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }

  // 2. Create synthesized image
  Size size =  Size(image.size().width*2,image.size().height*2);
  cout << "input size  : " << image.size() << endl;
  cout << "output size : " << size << endl;
  synth_texture out = synth_texture(image,size); 
  out.synthetize();
  
  // 3. Output and save result
  imwrite( "out.jpg", out.out_image );

  waitKey(0);                                          // Wait for a keystroke in the window
  cout << "programm ended" << endl;
  return 0;
}
