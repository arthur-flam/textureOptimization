#ifndef SYNTH__TEXTURE__H
#define SYNTH__TEXTURE__H

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "NL/nl.h"
#include <math.h>


using namespace cv;
using namespace std;


class synth_texture{
private:
  Mat raw_image;               // input texture
  int out_width, out_height, in_width, in_height;
  int grid_step, scale;        // grid parameter, also half neighborhood width
  Size out_size,current_size_out;// size of the wanted/current synthetized image
public:
  Mat image, image_bw;         // input texture, rescaled as needed
  Mat out_image, out_image_bw; // output image
  Mat Zp;                      // source patch position for each pixel
  int max_iter;
  Size size;                   // the size we want in the end

  Mat features;
  int pixelsInNeighborhood;
  int numData;

  // constructor
  synth_texture(Mat texture, Size size);

  // create new image
  void synthetize();


private:
  // get pixels surrounding as a vector 
  Mat extract_neighborhood(Mat M, int h, int v);
  

  // ENERGY
  double energy, energy_previous;
  // Pixels are changed to minimize energy
  void minimize_energy();
  // send initial guess to OpenNL internals
  void image_to_solver();
  // recover solution from OpenNL internals
  void solver_to_image();
  // send image information to solver
  void setup_matrix();

  // SOURCES
  //flann::Index kdtree_s;
  void update_neighborhoods();


  // MULTISCALE
  int level;              // where are we in the multi-scale optimization ?
  // when no more progress is made, be more local or initilize a bigger image
  void update_level(int level);

};

#endif
