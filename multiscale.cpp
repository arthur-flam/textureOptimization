#include "synth_texture.h"


void synth_texture::update_level(int level){
	cout << "changed to level " << level << endl;
	// update parameters
	switch(level){
		case 0:
		scale=4;
		grid_step=4;
		break;
		case 1:
		scale=4;
		grid_step=8;
		break;
		case 2:
		scale=2;
		grid_step=4;
		break;
		case 3:
		scale=1;
		grid_step=16; // 16 is what they used in the article
		break;
		case 4:
		scale=1;
		grid_step=8;
		break;
		case 5:
		scale=1;
		grid_step=4;
		break;
	} 
	pixelsInNeighborhood = grid_step*grid_step*4;
	current_size_out = Size((int) (out_size.width/scale), (int)(out_size.height/scale));
	current_size_in = Size(raw_image.size().width/scale, raw_image.size().height/scale);
	cout << "current size out     :" << current_size_out << endl;
	cout << "current size texture :" << current_size_in << endl;
	cout << "gridstep :" << grid_step << endl;

	// rescale
	if(level==0)
		out_image = Mat(current_size_out, CV_8UC3);
	Mat out_image_old = out_image;
	out_image = Mat(current_size_out, CV_8UC3);
	resize(out_image_old, out_image, current_size_out, 0, 0, INTER_CUBIC);
	image = Mat(current_size_in, CV_8UC3);
	resize(raw_image, image, current_size_in);

	// update parameters
	Zp = Mat(out_size, CV_8UC2);
	out_width  = out_image.size().width;
	in_width  = image.size().width;
	out_height = out_image.size().height;
	in_height = image.size().height;
}
