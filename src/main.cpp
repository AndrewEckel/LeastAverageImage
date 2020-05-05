// A hello world program in C++

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <math.h> //round
#include <cmath> //abs
#include <stdlib.h>  //for malloc

#include "netpbm.h"
//#include "ini.h"  //not yet used
//#include "abstractimage.h" //used in version 0.01 but not in 0.02

void writeImage(Image img, std::string path_and_filename){
	char* s = (char*) malloc(path_and_filename.length() + 1);
	sprintf(s, path_and_filename.c_str());
	writeImage(img, s);
	free(s);
	std::cout << "Wrote file " << path_and_filename << "\n";
}

void copyPixel(Pixel* to, Pixel from){
	//Don't bother with the intensity layer i.
	to->r = from.r;
	to->g = from.g;
	to->b = from.b;
}

void copyPixel(Pixel* to, Pixel* from){
	//Don't bother with the intensity layer i.
	to->r = from->r;
	to->g = from->g;
	to->b = from->b;
}

int difference(Pixel a, Pixel b){
	//Green counts 2x as much.
	return std::abs(((int) a.r) - b.r) + 2 * std::abs(((int) a.g) - b.g) + std::abs(((int) a.b) - b.b);
}


int main()
{
	std::cout << "LeastAverageImage Version 0.02\n";

	//Constants for now, could be inputs in a later version of this program.
	const std::string INPUT_PATH = "..\\input\\";
	const std::string SCENE_NAME = "dollars_";
	const int FIRST_FRAME = 0;
	const int LAST_FRAME = 23;
	const int INPUT_NUM_DIGITS = 4;
	const std::string OUTPUT_PATH = "";

	std::stringstream ss0;
	ss0 << std::setw(INPUT_NUM_DIGITS) << std::setfill('0') << FIRST_FRAME << "-" << LAST_FRAME;
	const std::string OUTPUT_TAG = SCENE_NAME + ss0.str();

	//Really constants
	const int NUM_COLOR_CHANNELS = 3; //R,G,B.
	const int RED_INDEX = 0;
	const int GREEN_INDEX = 1;
	const int BLUE_INDEX = 2;
	const int CHAR_LENGTH_LEEWAY = 100;

	int numImages = 1 + LAST_FRAME - FIRST_FRAME;
	char* filenameWithPath = (char*) malloc(INPUT_PATH.length() + SCENE_NAME.length() + INPUT_NUM_DIGITS + 4 + CHAR_LENGTH_LEEWAY);
	// unsigned long long limit = 18446744073709551615/128;
	// unsigned long long limit_hours = limit/24/60/60;
	// //"Dude I can process up to 144115188075855871 images, or 1667999861989 hours of 24 fps video."
	// std::cout << "Dude I can process up to " << limit << " images, or " << limit_hours << " hours of 24 fps video.\n";
	// if(numImages > limit){
	// 	std::cerr << "ERROR: Cannot process " << numImages << " images.\n";
	// 	exit(1);
	// }	

	//First pass: Sum all the values in the input files.
	std::cout << "Beginning averaging phase. First image should take the longest.\n";
	std::vector<std::vector<std::vector< unsigned long long > > > totals;
	//First image
	sprintf(filenameWithPath, "%s%s%0*d.ppm", INPUT_PATH.c_str(), SCENE_NAME.c_str(), INPUT_NUM_DIGITS, FIRST_FRAME);
	Image img = readImage(filenameWithPath);
	int height = img.height;
	int width = img.width;
	for(int i = 0; i < height; ++i){
		totals.push_back(std::vector<std::vector<unsigned long long > >());
		for(int j = 0; j < width; ++j){
			totals[i].push_back(std::vector<unsigned long long >());
			totals[i][j].push_back(img.map[i][j].r); //0
			totals[i][j].push_back(img.map[i][j].g); //1
			totals[i][j].push_back(img.map[i][j].b); //2
		}
	}
	deleteImage(img);
	std::cout << "Averaging: Processed 1st image ok\n";
	for(int x = FIRST_FRAME + 1; x <= LAST_FRAME; ++x){ //starting loop with 2nd image because we already did the first as a special case.
		sprintf(filenameWithPath, "%s%s%0*d.ppm", INPUT_PATH.c_str(), SCENE_NAME.c_str(), INPUT_NUM_DIGITS, x);
		Image img = readImage(filenameWithPath);
		
		if(img.height != height || img.width != width){
			std::cerr << "ERROR: Image #" << x << " dimensions do not match those of image #" << FIRST_FRAME << "!\n";
			deleteImage(img);
			exit(1);
		}

		for(int i = 0; i < height; ++i){
			for(int j = 0; j < width; ++j){
				totals[i][j][RED_INDEX] += img.map[i][j].r;
				totals[i][j][GREEN_INDEX] += img.map[i][j].g;
				totals[i][j][BLUE_INDEX] += img.map[i][j].b;
			}
		}
		deleteImage(img);
		std::cout << "Averaging: Processed image #" << (x - FIRST_FRAME + 1) << " of " << numImages << "\n";
	}
	Image meanAverageImage = createImage(height, width);
	for(int i = 0; i < height; ++i){
		for(int j = 0; j < width; ++j){
			meanAverageImage.map[i][j].r = (unsigned char) round(1.0 * totals[i][j][RED_INDEX] / numImages);
			meanAverageImage.map[i][j].g = (unsigned char) round(1.0 * totals[i][j][GREEN_INDEX] / numImages);
			meanAverageImage.map[i][j].b = (unsigned char) round(1.0 * totals[i][j][BLUE_INDEX] / numImages);
		}
	}
	writeImage(meanAverageImage, OUTPUT_PATH + OUTPUT_TAG + "avg.ppm");

	//Second pass: Find the most different.
	std::cout << "Beginning differenciating phase. First image should take the longest.\n";
	Image mostDifferentImage = createImage(height, width);
	std::vector<std::vector<int > > biggestDifference(height, std::vector<int>(width, 0));
	for(int x = FIRST_FRAME; x <= LAST_FRAME; ++x){
		sprintf(filenameWithPath, "%s%s%0*d.ppm", INPUT_PATH.c_str(), SCENE_NAME.c_str(), INPUT_NUM_DIGITS, x);
		Image img = readImage(filenameWithPath);
		for(int i = 0; i < height; ++i){
			for(int j = 0; j < width; ++j){
				int diff = difference(meanAverageImage.map[i][j], img.map[i][j]);
				if(diff > biggestDifference[i][j]){
					biggestDifference[i][j] = diff;
					copyPixel(&mostDifferentImage.map[i][j], &img.map[i][j]);
				}
			}
		}
		std::cout << "Differenciating: Processed image #" << (x - FIRST_FRAME + 1) << " of " << numImages << "\n";
	}
	writeImage(mostDifferentImage, OUTPUT_PATH + OUTPUT_TAG + "mostdifferent.ppm");

	return 0;
}