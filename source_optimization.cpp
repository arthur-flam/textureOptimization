#include "synth_texture.h"


void synth_texture::update_neighborhoods(){
	// create hierachical structure for fast SearchParam

	// update kd-tree features
	cout << "...computing features for kdtree" << endl;
	Mat image_search, out_image_search; // versions used for source search
	if(color==3){
		image_search=image;
		out_image_search=out_image;
	} else{
		out_image_search = Mat(current_size_out, CV_32F);
		image_search = Mat(current_size_in, CV_32F);
		cvtColor(out_image,out_image_search,CV_RGB2GRAY);
		cvtColor(image,image_search,CV_RGB2GRAY);
	}

	numData = (in_height-2*grid_step)*(in_width-2*grid_step);
	features = Mat(numData,pixelsInNeighborhood*color,CV_32F);
	int counter=0;
	Mat neigh;
	for(int v=grid_step; v<in_height-grid_step; ++v){
		for(int h=grid_step; h<in_width-grid_step; ++h){
			neigh = extract_neighborhood(image_search, h, v);
			neigh.convertTo(features.row(counter++),CV_32F);
		}
	}	

	// update kd-tree
	cout << "...creating kdtree" << endl;
	flann::Index kdtree(features, flann::KMeansIndexParams(4));
	cout << "...done creating kdtree" << endl;

	energy_previous = energy;
	energy=0;
	Mat query_m; 
	cout << "...updating origins" << endl;
	for(int v=grid_step; v<=out_height-grid_step; v+=grid_step){
		for(int h=grid_step; h<=out_width-grid_step; h+=grid_step){
			extract_neighborhood(out_image_search ,h, v).convertTo(query_m,CV_32F); // voisins de du point


			// update neighborhoods
			vector<float> singleQuery;
			vector<int> index(1);
			vector<float> dist(1);
			for(int i = 0 ; i < pixelsInNeighborhood*color ;i++)
				singleQuery.push_back((float)query_m.at<float>(0,i));	  
			kdtree.knnSearch(singleQuery, index, dist, 1, flann::SearchParams(32));
			int h_ = index[0]%(in_width-2*grid_step) + grid_step;
			int v_ = (index[0]-h_+grid_step)/(in_width-2*grid_step) + grid_step;
			Zp.at<Vec2b>(v, h) = Vec2b(h_, v_);

			// update energy
			double energy_p=0;
			Mat neigh_source;
			Vec2b source = Zp.at<Vec2b>(v,h);
			extract_neighborhood(image_search, source[0], source[1]).convertTo(neigh_source, CV_32F);
			Mat diffs = neigh_source - query_m;
			for (int i=0;i<pixelsInNeighborhood*color;i++){
				double diff = diffs.at<float>(0,i); 
				energy_p = energy_p + diff*diff;
			}
			energy = energy + energy_p*energy_p;
		}
	}
};

