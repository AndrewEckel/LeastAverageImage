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
	std::cout << "LeastAverageImage Version 0.05\n";

	//Constants for now, could be inputs in a later version of this program.
	const std::string INPUT_PATH = "G:\\LeastAverageImage inputs\\Castle Island\\main set\\";
	const std::string SCENE_NAME = "castleisland_";
	const int FIRST_FRAME = 1;
	const int LAST_FRAME = 408;
	const int INPUT_NUM_DIGITS = 6;
	const std::string OUTPUT_PATH = "";
	const bool SKIP_AVERAGING_PHASE = false;
	const int NUM_PIXELS_TO_RANK = 12;
	const bool COUNTDOWN = true;
	//const bool SQUARE_SCORE = false;
	const bool INVERT_SCORES = false;

	std::vector<double> powersOfScore;
	powersOfScore.push_back(1.0);
	powersOfScore.push_back(2.0);
	powersOfScore.push_back(2.5);
	powersOfScore.push_back(3.5);
	
	//Really constants
	const int NUM_COLOR_CHANNELS = 3; //R,G,B.
	const int RED_INDEX = 0;
	const int GREEN_INDEX = 1;
	const int BLUE_INDEX = 2;
	const int CHAR_LENGTH_LEEWAY = 100;

	std::stringstream ss0;
	ss0 << std::setw(INPUT_NUM_DIGITS) << std::setfill('0') << FIRST_FRAME << "-" << LAST_FRAME;
	const std::string OUTPUT_TAG = SCENE_NAME + ss0.str();


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
	Image meanAverageImage;
	sprintf(filenameWithPath, "%s%s%0*d.ppm", INPUT_PATH.c_str(), SCENE_NAME.c_str(), INPUT_NUM_DIGITS, FIRST_FRAME);
	Image img = readImage(filenameWithPath);
	int height = img.height;
	int width = img.width;
	
	if(SKIP_AVERAGING_PHASE){
		std::cout << "SKIPPING AVERAGING PHASE. Reading in pre-averaged file.\n";
		meanAverageImage = readImage("C:\\Code\\LeastAverageImage\\input_averages\\ive_eddy_000000-152avg.ppm");
		deleteImage(img);
	}
	else{
		std::cout << "Beginning averaging phase. First image should take the longest.\n";
		std::vector<std::vector<std::vector< unsigned long long > > > totals;
		//First image
		
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
		meanAverageImage = createImage(height, width);
		for(int i = 0; i < height; ++i){
			for(int j = 0; j < width; ++j){
				meanAverageImage.map[i][j].r = (unsigned char) round(1.0 * totals[i][j][RED_INDEX] / numImages);
				meanAverageImage.map[i][j].g = (unsigned char) round(1.0 * totals[i][j][GREEN_INDEX] / numImages);
				meanAverageImage.map[i][j].b = (unsigned char) round(1.0 * totals[i][j][BLUE_INDEX] / numImages);
			}
		}
		writeImage(meanAverageImage, OUTPUT_PATH + OUTPUT_TAG + "avg.ppm");
	}

	//Second pass: Find the most different.
	std::cout << "Beginning differenciating phase. First image should take the longest.\n";

	//Variables using regular difference:
	Image mostDifferentImage_regular = createImage(height, width);
	std::vector<std::vector<double > > biggestRegularDifference(height, std::vector<double>(width, 0));
	//Variables using color ratio difference:
	// Image mostDifferentImage_colorRatio = createImage(height, width);
	std::vector<std::vector<std::vector<Pixel > > > mostDifferentPixels_ColorRatio(height, std::vector<std::vector<Pixel > >(width, std::vector<Pixel>(NUM_PIXELS_TO_RANK, Pixel())));
	//std::vector<std::vector<std::vector<Pixel > > > mostDifferentPixels_ColorRatio;
	//std::vector<std::vector<std::vector<Pixel > > > mostDifferentPixels_ColorRatio(height, std::vector<std::vector<Pixel > >(width, std::vector<Pixel>));
	std::vector<std::vector<std::vector<double > > > biggestRatioDifference(height, std::vector<std::vector<double > >(width, std::vector<double>(NUM_PIXELS_TO_RANK, 0)));
	
	for(int i = 0; i < height; ++i){
		mostDifferentPixels_ColorRatio.push_back(std::vector<std::vector<Pixel > >());
		for(int j = 0; j < width; ++j){
			mostDifferentPixels_ColorRatio[i].push_back(std::vector<Pixel>());
			for(int k = 0; k < NUM_PIXELS_TO_RANK; ++k){
				Pixel blah;
				blah.r = 255;
				blah.g = 255;
				blah.b = 255;
				mostDifferentPixels_ColorRatio[i][j].push_back(blah);
			}
		}
	}


	// //Variables using perceived brightness difference:
	Image mostDifferentImage_Bright = createImage(height, width);
	std::vector<std::vector<double > > biggestBrightDifference(height, std::vector<double>(width, 0));
	// //Variables using combined difference:
	Image mostDifferentImage_Combined = createImage(height, width);
	std::vector<std::vector<double > > biggestCombinedDifference(height, std::vector<double>(width, 0));

	// mostDifferentPixels_ColorRatio[0][3][0].r = 111;
	// mostDifferentPixels_ColorRatio[0][3][0].g = 111;
	// mostDifferentPixels_ColorRatio[0][3][0].b = 111;
	// for(int j = 0; j < 5; ++j){
	// 	std::cout << "(0," << j << "): (" << mostDifferentPixels_ColorRatio[0][j][0].r << ", " << mostDifferentPixels_ColorRatio[0][j][0].g << ", " << mostDifferentPixels_ColorRatio[0][j][0].b << ")\n";
	// }


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
				// if(diff_colorRatio > biggestRatioDifference[i][j]){
				// 	biggestRatioDifference[i][j] = diff_colorRatio;
				// 	copyPixel(&mostDifferentImage_colorRatio.map[i][j], &img.map[i][j]);
				// }
				if(diff_colorRatio > biggestRatioDifference[i][j][NUM_PIXELS_TO_RANK - 1]){
					int rank = NUM_PIXELS_TO_RANK - 1;
					while(rank >= 0 && diff_colorRatio > biggestRatioDifference[i][j][rank]){
						--rank;
					}
					++rank;
					//std::cout << "Rank of " << rank << "\n";
					if(rank < 0 || rank >= NUM_PIXELS_TO_RANK){
						std::cerr << "ERROR: rANK = " << rank << " for (" << i << ", " << j << ")\n";
						exit(1);
					}
					for(size_t backwards_iterator = NUM_PIXELS_TO_RANK - 1; backwards_iterator > rank; --backwards_iterator){
						biggestRatioDifference[i][j][backwards_iterator] = biggestRatioDifference[i][j][backwards_iterator - 1];
						copyPixel(&mostDifferentPixels_ColorRatio[i][j][backwards_iterator], &mostDifferentPixels_ColorRatio[i][j][backwards_iterator - 1]);
					}
					biggestRatioDifference[i][j][rank] = diff_colorRatio;
					copyPixel(&mostDifferentPixels_ColorRatio[i][j][rank], &img.map[i][j]);
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

	// double score_power = 1.0;
	// if(SQUARE_SCORE){
	// 	score_power = 2.0;
	// }

	int lastloop = 0;
	if(COUNTDOWN){
		lastloop = NUM_PIXELS_TO_RANK - 1;
	}
	for(int rank_this_many_fewer = 0; rank_this_many_fewer <= lastloop; ++rank_this_many_fewer){
		int num_pixels_to_rank_this_round = NUM_PIXELS_TO_RANK - rank_this_many_fewer;
		for(size_t score_power_index = 0; score_power_index < powersOfScore.size(); ++score_power_index){
			Image mostDifferentImage_colorRatio = createImage(height, width);
			for(int i = 0; i < height; ++i){
				for(int j = 0; j < width; ++j){
					double totalScore = 0.0;
					for(int k = 0; k < num_pixels_to_rank_this_round; ++k){
						totalScore += pow(biggestRatioDifference[i][j][k], powersOfScore[score_power_index]);
					}
					if(totalScore == 0.0){
						std::cout << "WARNING: All pixels at position " << i << ", " << j << " are equal to the average.\n";
						copyPixel(&mostDifferentImage_colorRatio.map[i][j], &meanAverageImage.map[i][j]);
					}
					else{
						//std::vector<double> weights;
						std::vector<double> newRGB(NUM_COLOR_CHANNELS, 0.0);
						for(int k = 0; k < num_pixels_to_rank_this_round; ++k){
							//weights.push_back(biggestRatioDifference[i][j][k] / totalScore);
							double weight = pow(biggestRatioDifference[i][j][k], powersOfScore[score_power_index]) / totalScore;
							if(INVERT_SCORES){
								weight = 1 - weight;
							}
							newRGB[RED_INDEX] += mostDifferentPixels_ColorRatio[i][j][k].r * weight;
							newRGB[GREEN_INDEX] += mostDifferentPixels_ColorRatio[i][j][k].g * weight;
							newRGB[BLUE_INDEX] += mostDifferentPixels_ColorRatio[i][j][k].b * weight;
						}
						mostDifferentImage_colorRatio.map[i][j].r = (unsigned char) round(newRGB[RED_INDEX]);
						mostDifferentImage_colorRatio.map[i][j].g = (unsigned char) round(newRGB[GREEN_INDEX]);
						mostDifferentImage_colorRatio.map[i][j].b = (unsigned char) round(newRGB[BLUE_INDEX]);
					}
				}
			}
			std::stringstream ss1;
			ss1 << num_pixels_to_rank_this_round;
			std::string numPixelsToRankAsString = ss1.str();
			std::string outputFilenameColorRatio = OUTPUT_PATH + OUTPUT_TAG + "mostdiff_colorratio_rank" + numPixelsToRankAsString;
			// if(SQUARE_SCORE){
			// 	outputFilenameColorRatio += "_squarescore"; 
			// }
			std::stringstream ss2;
			ss2 << powersOfScore[score_power_index];
			std::string powerAsString = ss2.str();
			outputFilenameColorRatio += "_power" + powerAsString;
			if(INVERT_SCORES){
				outputFilenameColorRatio += "_invertscore";
			}
			outputFilenameColorRatio += ".ppm";
			writeImage(mostDifferentImage_colorRatio, outputFilenameColorRatio);
			deleteImage(mostDifferentImage_colorRatio);
		}
	}	


	writeImage(mostDifferentImage_regular, OUTPUT_PATH + OUTPUT_TAG + "mostdiff_regular.ppm");
	
	writeImage(mostDifferentImage_Bright, OUTPUT_PATH + OUTPUT_TAG + "mostdiff_brightness.ppm");
	writeImage(mostDifferentImage_Combined, OUTPUT_PATH + OUTPUT_TAG + "mostdiff_combined_weighed4.ppm");  //easymath, hardmath, and weighed1.

	deleteImage(mostDifferentImage_regular);
	deleteImage(mostDifferentImage_Bright);
	deleteImage(mostDifferentImage_Combined);

	return 0;
}