// A hello world program in C++

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <math.h> //round
#include <cmath> //abs
#include <stdlib.h>  //for malloc
#include <algorithm>    // std::max

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
	to->r = from.r;
	to->g = from.g;
	to->b = from.b;
}

void copyPixel(Pixel* to, Pixel* from){
	to->r = from->r;
	to->g = from->g;
	to->b = from->b;
}

double difference_Regular(Pixel p1, Pixel p2){
	//Possible range: 0 to 512

	//Green counts 2x as much.
	//return std::abs(((int) p1.r) - p2.r) + 2 * std::abs(((int) p1.g) - p2.g) + std::abs(((int) p1.b) - p2.b);
	//"harder math version": requires doubles instead of ints as outputs. I like this version SLIGHTLY better.
	return sqrt(pow(p1.r - p2.r, 2.0) + 2 * pow(p1.g - p2.g, 2.0) + pow(p1.b - p2.b, 2.0));
}

double difference_ColorRatio(Pixel p1, Pixel p2){
	//Possible range: 0 to 440

	//The "max" thing is there to avoid a divide-by-zero.
	double p1_RtoG = (1.0 * p1.r) / std::max((int) p1.g, 1);
	double p1_RtoB = (1.0 * p1.r) / std::max((int) p1.b, 1);
	double p1_GtoB = (1.0 * p1.g) / std::max((int) p1.b, 1);

	double p2_RtoG = (1.0 * p2.r) / std::max((int) p2.g, 1);
	double p2_RtoB = (1.0 * p2.r) / std::max((int) p2.b, 1);
	double p2_GtoB = (1.0 * p2.g) / std::max((int) p2.b, 1);

	//"Color difference difference version"!!!!!!
	// double p1_RtoG = (1.0 * p1.r) - p1.g;
	// double p1_RtoB = (1.0 * p1.r) - p1.b;
	// double p1_GtoB = (1.0 * p1.g) - p1.b;

	// double p2_RtoG = (1.0 * p2.r) - p2.g;
	// double p2_RtoB = (1.0 * p2.r) - p2.b;
	// double p2_GtoB = (1.0 * p2.g) - p2.b;


	return sqrt(pow(p1_RtoG - p2_RtoG, 2.0) + 2 * pow(p1_RtoB - p2_RtoB, 2.0) + pow(p1_GtoB - p2_GtoB, 2.0)); //original
	//return sqrt(pow(p1_RtoG - p2_RtoG, 2.0) + 2 * pow(p1_RtoB - p2_RtoB, 2.0) + pow(p1_GtoB - p2_GtoB, 2.0)); //green counts twice
	//return sqrt(0.299*pow(p1_RtoG - p2_RtoG, 2.0) + 0.587*pow(p1_RtoB - p2_RtoB, 2.0) + 0.144*pow(p1_GtoB - p2_GtoB, 2.0));  //"brightness scores" version
	//return std::abs(p1_RtoG - p2_RtoG) + abs(p1_RtoB - p2_RtoB) + abs(p1_GtoB - p2_GtoB);   //"easier math" version: runtime difference is miniscule, and results look NOTICEABLY worse!!
}

double perceived_Brightness(Pixel color){
  //possible range: 0 to 255 (really!)
  //given a Pixel, returns the perceived brightness of the color
  //on a scale from 0 to 255.

  //The single line of code here is by me (Andrew Eckel) but the formula used
  //was created by Darel Rex Finley, based on his attempts to
  //reverse-engineer Photoshop’s RGB-to-Greyscale mode conversion.
  //http://alienryderflex.com/hsp.html

  //The version in the ErrorSpreader returns an int, but this version returns a double.

  int r = (int)(color.r);
  int g = (int)(color.g);
  int b = (int)(color.b);

  //pow() cannot be used here, because pow on a non-tiny number just returns
  //something stupid called HUGEVAL.   <---- wait what???
  return sqrt(0.299*r*r + 0.587*g*g + 0.114*b*b);
}

double difference_PerceivedBrightness(Pixel p1, Pixel p2){
	//Possible range: 0 to 255.
	return std::abs(perceived_Brightness(p1) - perceived_Brightness(p2));
}

double difference_Combined(Pixel p1, Pixel p2){
	//Easy Math
	// return difference_Regular(p1, p2) / 512.0 +
	// 	difference_ColorRatio(p1, p2) / 440.0 +
	// 	difference_PerceivedBrightness(p1, p2) / 255.0;
	//Hard Math
	// return sqrt(pow(difference_Regular(p1, p2) / 512.0, 2.0) + pow(difference_ColorRatio(p1, p2) / 440.0, 2.0) + pow(difference_PerceivedBrightness(p1, p2) / 255.0, 2.0));
	//Weighed 3 (first were weights on CR of 1.5 , 3.0)
	// return difference_Regular(p1, p2) / 512.0 + 
	// 	9.0 * difference_ColorRatio(p1, p2) / 440.0 +
	// 	difference_PerceivedBrightness(p1, p2) / 255.0;
	//weighted 4
	return difference_Regular(p1, p2) / 512.0 + 
		9.0 * sqrt(difference_ColorRatio(p1, p2)) / sqrt(440.0) +
		difference_PerceivedBrightness(p1, p2) / 255.0;
	//written in a stupid way:
	//return (difference_Regular(p1, p2) / 512.0) + (9.0 * difference_ColorRatio(p1, p2) / 440.0) + (difference_PerceivedBrightness(p1, p2) / 255.0);
}


int main()
{
	std::cout << "LeastAverageImage Version 0.03\n";

	//Constants for now, could be inputs in a later version of this program.
	const std::string INPUT_PATH = "G:\\LeastAverageImage project 2 (A610)\\";
	const std::string SCENE_NAME = "a610_";
	const int FIRST_FRAME = 0;
	const int LAST_FRAME = 10316;
	const int INPUT_NUM_DIGITS = 6;
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

	//Variables using regular difference:
	Image mostDifferentImage_regular = createImage(height, width);
	std::vector<std::vector<double > > biggestRegularDifference(height, std::vector<double>(width, 0));
	//Variables using color ratio difference:
	Image mostDifferentImage_colorRatio = createImage(height, width);
	std::vector<std::vector<double > > biggestRatioDifference(height, std::vector<double>(width, 0));
	//Variables using perceived brightness difference:
	Image mostDifferentImage_Bright = createImage(height, width);
	std::vector<std::vector<double > > biggestBrightDifference(height, std::vector<double>(width, 0));
	//Variables using combined difference:
	Image mostDifferentImage_Combined = createImage(height, width);
	std::vector<std::vector<double > > biggestCombinedDifference(height, std::vector<double>(width, 0));

	for(int x = FIRST_FRAME; x <= LAST_FRAME; ++x){
		sprintf(filenameWithPath, "%s%s%0*d.ppm", INPUT_PATH.c_str(), SCENE_NAME.c_str(), INPUT_NUM_DIGITS, x);
		Image img = readImage(filenameWithPath);
		for(int i = 0; i < height; ++i){
			for(int j = 0; j < width; ++j){
				double diff_regular = difference_Regular(meanAverageImage.map[i][j], img.map[i][j]);
				double diff_colorRatio = difference_ColorRatio(meanAverageImage.map[i][j], img.map[i][j]);
				double diff_bright = difference_PerceivedBrightness(meanAverageImage.map[i][j], img.map[i][j]);
				double diff_combined = difference_Combined(meanAverageImage.map[i][j], img.map[i][j]);

				if(diff_regular > biggestRegularDifference[i][j]){
					biggestRegularDifference[i][j] = diff_regular;
					copyPixel(&mostDifferentImage_regular.map[i][j], &img.map[i][j]);
				}
				if(diff_colorRatio > biggestRatioDifference[i][j]){
					biggestRatioDifference[i][j] = diff_colorRatio;
					copyPixel(&mostDifferentImage_colorRatio.map[i][j], &img.map[i][j]);
				}
				if(diff_bright > biggestBrightDifference[i][j]){
					biggestBrightDifference[i][j] = diff_bright;
					copyPixel(&mostDifferentImage_Bright.map[i][j], &img.map[i][j]);
				}
				if(diff_combined > biggestCombinedDifference[i][j]){
					biggestCombinedDifference[i][j] = diff_combined;
					copyPixel(&mostDifferentImage_Combined.map[i][j], &img.map[i][j]);
				}
			}
		}
		deleteImage(img);
		std::cout << "Differenciating: Processed image #" << (x - FIRST_FRAME + 1) << " of " << numImages << "\n";
	}
	writeImage(mostDifferentImage_regular, OUTPUT_PATH + OUTPUT_TAG + "mostdiff_regular.ppm");
	writeImage(mostDifferentImage_colorRatio, OUTPUT_PATH + OUTPUT_TAG + "mostdiff_colorratio.ppm");
	writeImage(mostDifferentImage_Bright, OUTPUT_PATH + OUTPUT_TAG + "mostdiff_brightness.ppm");
	writeImage(mostDifferentImage_Combined, OUTPUT_PATH + OUTPUT_TAG + "mostdiff_combined_weighed4.ppm");  //easymath, hardmath, and weighed1.

	deleteImage(mostDifferentImage_regular);
	deleteImage(mostDifferentImage_colorRatio);
	deleteImage(mostDifferentImage_Bright);
	deleteImage(mostDifferentImage_Combined);

	return 0;
}