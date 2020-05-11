// A hello world program in C++

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <math.h> //round
#include <cmath> //abs
#include <stdlib.h>  //for malloc
#include <algorithm>

#include "netpbm.h"
#include "utility.h"
#include "iniparser.h"

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

typedef struct
{
	std::string name;
	double (*difference_function)(Pixel, Pixel);  //This is a pointer to a difference function.
	unsigned int num_pixels_to_rank;
	bool invert_scores;
	std::vector<double> score_powers;
	std::vector<int> rankings_to_save;
	std::vector<std::vector<std::vector<Pixel > > > mostDifferentPixels;
	std::vector<std::vector<std::vector<double > > > biggestDifferences;
} DifferenceRecord;

std::string intToString(int x, size_t numberOfDigits){
	std::stringstream ss;
	ss << std::setw(numberOfDigits) << std::setfill('0') << x;
	return ss.str();
}

std::string intToString(int x){
	std::stringstream ss;
	ss << x;
	return ss.str();
}

std::string doubleToString(double x, int fixed_precision){
	std::stringstream ss;
	ss << std::setprecision(1) << std::fixed << x;
	return ss.str();
}

int main(int argc, char *argv[])
{
	std::cout << "LeastAverageImage Version 0.11\n";

	std::string settingsFilenameAndPath;
	if(argc < 2){
		settingsFilenameAndPath = "../input/settings.ini";
	}
	else{
		settingsFilenameAndPath = argv[1];
	}
	ini opts_ini(settingsFilenameAndPath);
	std::cout << "Parsing input file " << settingsFilenameAndPath << "\n";

	//General settings
	const std::string OUTPUT_PATH = Utility::endWithSlash(opts_ini.at("general_output_path"));
	const bool INVERT_SCORES = Utility::stob(opts_ini.at("general_invert_scores"));
	const bool HIDE_WARNINGS = Utility::stob(opts_ini.at("general_hide_warnings"));

	const std::string split_chars = ",";
	std::vector<double> powersOfScore = Utility::toDoubles(Utility::splitByChars(opts_ini.at("general_powers_of_score"), split_chars), true);
	std::vector<int> rankingsToSave = Utility::toInts(Utility::splitByChars(opts_ini.at("general_rankings_to_save"), split_chars), true);
	std::sort(rankingsToSave.begin(), rankingsToSave.end(), std::greater<int>());  //Reverse-sort numbers of rankings.
	const int NUM_PIXELS_TO_RANK = rankingsToSave[0];  //Whater is the greatest number of rankings desired, that's how many we'll need to do.

	//Which difference functions should we use?
	const bool DO_REGULAR = Utility::stob(opts_ini.at("difference_functions_do_regular"));
	const bool DO_PERCEIVED_BRIGHTNESS = Utility::stob(opts_ini.at("difference_functions_do_perceived_brightness"));
	const bool DO_COLOR_RATIO = Utility::stob(opts_ini.at("difference_functions_do_color_ratio"));
	const bool DO_COMBO = Utility::stob(opts_ini.at("difference_functions_do_combo"));

	if(DO_REGULAR + DO_PERCEIVED_BRIGHTNESS + DO_COLOR_RATIO + DO_COMBO == 0){
		std::cerr << "ERROR: No difference functions selected.\n";
		exit(1);
	}

	//Pre-Averaged
	const bool SKIP_AVERAGING_PHASE = Utility::stob(opts_ini.at("pre_averaged_skip_averaging_phase"));
	std::string preAveragedFilenameWithPath;
	if(SKIP_AVERAGING_PHASE){
		preAveragedFilenameWithPath = Utility::endWithSlash(opts_ini.at("pre_averaged_pre_averaged_path")) +
	                                                opts_ini.at("pre_averaged_pre_averaged_filename");
	}

	//Album mode settings
	//(New mode coming soon...)
	const std::string ALBUM_INPUT_PATH = Utility::endWithSlash(opts_ini.at("album_mode_input_path"));
	const std::string ALBUM_NAME = opts_ini.at("album_mode_name");
	const int FIRST_FRAME = std::stoi(opts_ini.at("album_mode_first_frame"));
	const int LAST_FRAME = std::stoi(opts_ini.at("album_mode_last_frame"));
	const int ALBUM_NUM_DIGITS = std::stoi(opts_ini.at("album_mode_num_digits"));

	std::cout << "Finished parsing.\n";

	//Constants
	const int NUM_COLOR_CHANNELS = 3; //R,G,B.
	const int RED_INDEX = 0;
	const int GREEN_INDEX = 1;
	const int BLUE_INDEX = 2;

	const int NUM_IMAGES = 1 + LAST_FRAME - FIRST_FRAME;
	const std::string OUTPUT_TAG = ALBUM_NAME + intToString(FIRST_FRAME, ALBUM_NUM_DIGITS) + "-" + intToString(LAST_FRAME, ALBUM_NUM_DIGITS);

	std::string filenameWithPath;

	//First pass: Sum all the values in the input files.
	Image meanAverageImage;
	filenameWithPath = ALBUM_INPUT_PATH + ALBUM_NAME + intToString(FIRST_FRAME, ALBUM_NUM_DIGITS) + ".ppm";
	Image img = readImage(filenameWithPath);
	int height = img.height;
	int width = img.width;
	
	if(SKIP_AVERAGING_PHASE){
		std::cout << "SKIPPING AVERAGING PHASE. Reading in pre-averaged file.\n";
		meanAverageImage = readImage(preAveragedFilenameWithPath);
		deleteImage(img);
	}
	else{
		std::cout << "Beginning averaging phase. First image should take the longest.\n";
		//For now, averaging means a mean average.
		//I can't think of any efficient algorithm for median average with reasonable memory usage.
		//Maybe such a thing is possible?  https://stackoverflow.com/questions/3372006/incremental-median-computation-with-max-memory-efficiency
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
			filenameWithPath = ALBUM_INPUT_PATH + ALBUM_NAME + intToString(x, ALBUM_NUM_DIGITS) + ".ppm";
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
			std::cout << "Averaging: Processed image #" << (x - FIRST_FRAME + 1) << " of " << NUM_IMAGES << "\n";
		}
		meanAverageImage = createImage(height, width);
		for(int i = 0; i < height; ++i){
			for(int j = 0; j < width; ++j){
				meanAverageImage.map[i][j].r = (unsigned char) round(1.0 * totals[i][j][RED_INDEX] / NUM_IMAGES);
				meanAverageImage.map[i][j].g = (unsigned char) round(1.0 * totals[i][j][GREEN_INDEX] / NUM_IMAGES);
				meanAverageImage.map[i][j].b = (unsigned char) round(1.0 * totals[i][j][BLUE_INDEX] / NUM_IMAGES);
			}
		}
		writeImage(meanAverageImage, (OUTPUT_PATH + OUTPUT_TAG + "avg.ppm"));
	}

	std::vector<DifferenceRecord> drs;

	if(DO_REGULAR){
		DifferenceRecord dr;
		dr.name = "Regular";
		dr.difference_function = difference_Regular;
		drs.push_back(dr);
	}
	if(DO_PERCEIVED_BRIGHTNESS){
		DifferenceRecord dr;
		dr.name = "PerceivedBrightness";
		dr.difference_function = difference_PerceivedBrightness;
		drs.push_back(dr);
	}
	if(DO_COLOR_RATIO){
		DifferenceRecord dr;
		dr.name = "ColorRatio";
		dr.difference_function = difference_ColorRatio;
		drs.push_back(dr);
	}
	if(DO_COMBO){
		DifferenceRecord dr;
		dr.name = "Combo";
		dr.difference_function = difference_Combined;
		drs.push_back(dr);
	}

	Pixel white;
	white.r = 255;
	white.g = 255;
	white.b = 255;

	//Settings that are the same for all of the difference functions used:
	for(size_t drs_index = 0; drs_index < drs.size(); ++drs_index){
		//These are the same for now but may be separated later:
		drs[drs_index].num_pixels_to_rank = NUM_PIXELS_TO_RANK;
		drs[drs_index].invert_scores = INVERT_SCORES;
		drs[drs_index].score_powers = powersOfScore;
		drs[drs_index].rankings_to_save = rankingsToSave;

		//Initialize mostDifferentPixels (all white) and biggestDifferences (all zeroes).
		drs[drs_index].mostDifferentPixels = std::vector<std::vector<std::vector<Pixel > > >(height, std::vector<std::vector<Pixel > >(width, std::vector<Pixel>(NUM_PIXELS_TO_RANK, white)));
		drs[drs_index].biggestDifferences = std::vector<std::vector<std::vector<double > > >(height, std::vector<std::vector<double > >(width, std::vector<double>(NUM_PIXELS_TO_RANK, 0)));
	}

	//Second pass: Find the most different.
	std::cout << "Beginning differenciating phase. First image should take the longest.\n";
	for(int x = FIRST_FRAME; x <= LAST_FRAME; ++x){
		filenameWithPath = ALBUM_INPUT_PATH + ALBUM_NAME + intToString(x, ALBUM_NUM_DIGITS) + ".ppm";
		Image img = readImage(filenameWithPath);
		for(int i = 0; i < height; ++i){
			for(int j = 0; j < width; ++j){
				for(size_t drs_index = 0; drs_index < drs.size(); ++drs_index){
					double diff = drs[drs_index].difference_function(meanAverageImage.map[i][j], img.map[i][j]);

					if(diff > drs[drs_index].biggestDifferences[i][j][drs[drs_index].num_pixels_to_rank - 1]){  ///almost positive you can remove the if-statement here.
						int rank = drs[drs_index].num_pixels_to_rank - 1;
						while(rank >= 0 && diff > drs[drs_index].biggestDifferences[i][j][rank]){
							--rank;
						}
						++rank;
						if(rank < 0 || rank >= drs[drs_index].num_pixels_to_rank){
							std::cerr << "ERROR: RANK " << rank << " for " << drs[drs_index].name << "(" << i << ", " << j << ")\n";
						}
						for(size_t backwards_iterator = drs[drs_index].num_pixels_to_rank - 1; backwards_iterator > rank; --backwards_iterator){
							drs[drs_index].biggestDifferences[i][j][backwards_iterator] = drs[drs_index].biggestDifferences[i][j][backwards_iterator - 1];
							copyPixel(&drs[drs_index].mostDifferentPixels[i][j][backwards_iterator], &drs[drs_index].mostDifferentPixels[i][j][backwards_iterator - 1]);
						}
						drs[drs_index].biggestDifferences[i][j][rank] = diff;
						copyPixel(&drs[drs_index].mostDifferentPixels[i][j][rank], &img.map[i][j]);
					}
				}
			}
		}
		deleteImage(img);
		std::cout << "Differenciating: Processed image #" << (x - FIRST_FRAME + 1) << " of " << NUM_IMAGES << "\n";
	}

	for(size_t drs_index = 0; drs_index < drs.size(); ++drs_index){
		for(size_t ranking_index = 0; ranking_index < drs[drs_index].rankings_to_save.size(); ++ranking_index){
			int num_pixels_to_rank_this_round = drs[drs_index].rankings_to_save[ranking_index];
			int num_powers_this_round = drs[drs_index].score_powers.size();
			if(num_pixels_to_rank_this_round == 1){
				num_powers_this_round = 1;
			}
			for(size_t sp_index = 0; sp_index < num_powers_this_round; ++sp_index){
				double current_power = drs[drs_index].score_powers[sp_index];
				if(num_pixels_to_rank_this_round == 1){
					current_power = 1.0;
				}
				Image result_img = createImage(height, width);
				for(int i = 0; i < height; ++i){
					for(int j = 0; j < width; ++j){
						double totalScore = 0.0;
						for(int k = 0; k < num_pixels_to_rank_this_round; ++k){
							totalScore += pow(drs[drs_index].biggestDifferences[i][j][k], current_power);
						}
						if(totalScore <= 0.0 && !HIDE_WARNINGS){
							std::cout << "WARNING: All pixels at position " << i << ", " << j << " are equal to the average, for " << drs[drs_index].name << ".\n";
							copyPixel(&result_img.map[i][j], &meanAverageImage.map[i][j]);
						}
						else{
							std::vector<double> newRGB(NUM_COLOR_CHANNELS, 0.0);
							for(int k = 0; k < num_pixels_to_rank_this_round; ++k){
								double weight = pow(drs[drs_index].biggestDifferences[i][j][k], current_power) / totalScore;
								if(drs[drs_index].invert_scores){
									weight = 1 - weight;
								}
								newRGB[RED_INDEX] += drs[drs_index].mostDifferentPixels[i][j][k].r * weight;
								newRGB[GREEN_INDEX] += drs[drs_index].mostDifferentPixels[i][j][k].g * weight;
								newRGB[BLUE_INDEX] += drs[drs_index].mostDifferentPixels[i][j][k].b * weight;
							}
							result_img.map[i][j].r = (unsigned char) round(newRGB[RED_INDEX]);
							result_img.map[i][j].g = (unsigned char) round(newRGB[GREEN_INDEX]);
							result_img.map[i][j].b = (unsigned char) round(newRGB[BLUE_INDEX]);
						}
					}
				}
				std::string numPixelsToRankAsString = intToString(num_pixels_to_rank_this_round);
				std::string outputFilename = OUTPUT_PATH + OUTPUT_TAG + drs[drs_index].name + "_rank" + numPixelsToRankAsString;
				std::string powerAsString = doubleToString(current_power, 1);
				outputFilename += "_power" + powerAsString;
				if(drs[drs_index].invert_scores){
					outputFilename += "_invertscore";
				}
				outputFilename += ".ppm";
				writeImage(result_img, outputFilename.c_str());
				deleteImage(result_img);
			}
		}
	}

	return 0;
}