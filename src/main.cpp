// A hello world program in C++

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <math.h> //round
#include <cmath> //abs

#include "ini.h"  //not yet used
#include "netpbm.h"
#include "abstractimage.h"

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

int difference(Pixel a, Pixel b){
	//Green counts 2x as much.
	return std::abs(((int) a.r) - b.r) + 2 * std::abs(((int) a.g) - b.g) + std::abs(((int) a.b) - b.b);
}


int main()
{
	std::cout << "LeastAverageImage Version 0.01\n";

	//Constants for now, could be inputs in a later version of this program.
	const std::string INPUT_PATH = "..\\input\\";
	const std::string SCENE_NAME = "dollars_";
	const int FIRST_FRAME = 32;
	const int LAST_FRAME = 33;
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

	//First, determine if we can hold all of the images in memory or not.
	bool canHoldInMemory;
	std::stringstream ss1;
	ss1 << std::setw(INPUT_NUM_DIGITS) << std::setfill('0') << FIRST_FRAME;
	AbstractImage firstImage = AbstractImage(INPUT_PATH + SCENE_NAME + ss1.str() + ".ppm", true);
	int height = firstImage.getHeight();
	int width = firstImage.getWidth();
	int numImages = 1 + LAST_FRAME - FIRST_FRAME;
	unsigned char example_unsigned_char = 0;
	std::size_t memoryNeeded = 4 * sizeof(example_unsigned_char) * height * width * numImages;
	int gigabytesWillingToUse = 8;
	std::size_t memoryWillingToUse = gigabytesWillingToUse * 1000000000;
	std::cout << "Memory needed to hold images at once: " << ((1.0 * memoryNeeded)/1000000000) << " GB (" << memoryNeeded << 
		" bytes)\nMemory we're willing to use: " << gigabytesWillingToUse << " GB (" << memoryWillingToUse << " bytes)\n";
	if(memoryNeeded <= memoryWillingToUse){
		canHoldInMemory = true;
		std::cout << "wooo!  We can hold all the images in memory.\n";
	}
	else{
		canHoldInMemory = false;
		std::cout << "This might be SLOW, because we can't hold everything in memory.\n";
	}

	//temp
	std::cout << "Psych! We are temporarily overwriting canHoldInMemory as FALSE.\n";
	canHoldInMemory = false;

	//Next, set up the vector.
	std::vector<AbstractImage> images;
	for(int x = FIRST_FRAME; x <= LAST_FRAME; ++x){
		std::stringstream ss2;
		ss2 << std::setw(INPUT_NUM_DIGITS) << std::setfill('0') << x;

		std::string path_and_filename = INPUT_PATH + SCENE_NAME + ss2.str() + ".ppm";
		AbstractImage img = AbstractImage(path_and_filename, canHoldInMemory);

		if(img.getHeight() != height || img.getWidth() != width){
			std::cerr << "Image " << (SCENE_NAME + ss2.str()) << " dimensions do not match that of the first image.\n";
			exit(1);
		}

		images.push_back(img);
		//images.push_back(AbstractImage(path_and_filename, canHoldInMemory));
	}
	std::cout << "Successfully read in the images.\n";

	if(images.size() != numImages){
		std::cerr << "ERROR: images.size() != numImages.\n";
		exit(1);
	}

	//Calculate the mean average Pixel at each spot.
	Image meanAverageImage = createImage(height, width);
	for(int i = 0; i < height; ++i){
		for(int j = 0; j < width; ++j){
			long long total[NUM_COLOR_CHANNELS] = {0,0,0};
			for(int n = 0; n < numImages; ++n){
				//Oh God this is so stupid. Why didn't you just make a huge multidimensional vector of totals, and then go through the images one at a time?
				//Then you would never even WANT to open all the images at the same time.
					//NO you moron. The second half of the algorithm requires you to go one pixel at a time. You know, the part where you calculate the differences.
						//NO DUDE it would seriously just be fine.  You'd just make 2 2-dimensional vectors. One for biggestDifference and one for indexBiggestDiff.
				Pixel p = images[n].at(i, j);
				total[RED_INDEX] += p.r;
				total[GREEN_INDEX] += p.g;
				total[BLUE_INDEX] += p.b;
			}
			meanAverageImage.map[i][j].r = (unsigned char) round(1.0 * total[RED_INDEX] / numImages);
			meanAverageImage.map[i][j].g = (unsigned char) round(1.0 * total[GREEN_INDEX] / numImages);
			meanAverageImage.map[i][j].b = (unsigned char) round(1.0 * total[BLUE_INDEX] / numImages);
		}
	}
	writeImage(meanAverageImage, OUTPUT_PATH + OUTPUT_TAG + "avg.ppm");

	//Find the most different pixel at each position.
	Image mostDifferentImage = createImage(height, width);
	for(int i = 0; i < height; ++i){
		for(int j = 0; j < width; ++j){
			int biggestDifference = 0;
			size_t indexOfBiggestDiff = 0;
			for(int n = 0; n < numImages; ++n){
				Pixel p = images[n].at(i, j);
				int diff = difference(p, meanAverageImage.map[i][j]);
				if(diff > biggestDifference){
					biggestDifference = diff;
					indexOfBiggestDiff = n;
				}
			}
			copyPixel(&mostDifferentImage.map[i][j], images[indexOfBiggestDiff].at(i, j));
		}
	}
	writeImage(mostDifferentImage, OUTPUT_PATH + OUTPUT_TAG + "mostdifferent.ppm");


	return 0;
}