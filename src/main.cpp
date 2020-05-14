// A hello world program in C++

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <algorithm>

#include "netpbm.h"
#include "differencefunctions.h"
#include "utility.h"
#include "iniparser.h"

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


int main(int argc, char *argv[])
{
	std::cout << "LeastAverageImage Version 1.01" << std::endl << std::endl;

	std::string settingsFilenameAndPath;
	if(argc < 2){
		settingsFilenameAndPath = "../input/settings.ini";
	}
	else{
		settingsFilenameAndPath = argv[1];
	}
	ini opts_ini(settingsFilenameAndPath);
	std::cout << "Parsing input file " << settingsFilenameAndPath << std::endl;

	//General settings
	const std::string OUTPUT_PATH = Utility::endWithSlash(opts_ini.atat("general_output_path"));
	const bool INVERT_SCORES = Utility::stob(opts_ini.atat("general_invert_scores"));
	const bool HIDE_WARNINGS = Utility::stob(opts_ini.atat("general_hide_warnings"));
	const bool SAVE_AVERAGE = Utility::stob(opts_ini.atat("general_save_average"));

	const std::string split_chars = ",";
	std::vector<double> powersOfScore = Utility::toDoubles(Utility::splitByChars(opts_ini.atat("general_powers_of_score"), split_chars), true);
	std::vector<int> rankingsToSave = Utility::toInts(Utility::splitByChars(opts_ini.atat("general_rankings_to_save"), split_chars), true);
	std::sort(rankingsToSave.begin(), rankingsToSave.end(), std::greater<int>());  //Reverse-sort numbers of rankings.
	const int NUM_PIXELS_TO_RANK = rankingsToSave[0];  //Whater is the greatest number of rankings desired, that's how many we'll need to do.

	//Which difference functions should we use?
	const bool DO_REGULAR = Utility::stob(opts_ini.atat("difference_functions_do_regular"));
	const bool DO_PERCEIVED_BRIGHTNESS = Utility::stob(opts_ini.atat("difference_functions_do_perceived_brightness"));
	const bool DO_COLOR_RATIO = Utility::stob(opts_ini.atat("difference_functions_do_color_ratio"));
	const bool DO_COMBO = Utility::stob(opts_ini.atat("difference_functions_do_combo"));

	if(DO_REGULAR + DO_PERCEIVED_BRIGHTNESS + DO_COLOR_RATIO + DO_COMBO == 0){
		std::cerr << "ERROR: No difference functions selected.\n";
		exit(1);
	}

	//Pre-Averaged
	const bool SKIP_AVERAGING_PHASE = Utility::stob(opts_ini.atat("pre_averaged_skip_averaging_phase"));
	std::string preAveragedFilenameWithPath;
	if(SKIP_AVERAGING_PHASE){
		preAveragedFilenameWithPath = Utility::endWithSlash(opts_ini.atat("pre_averaged_pre_averaged_path")) +
	                                                opts_ini.atat("pre_averaged_pre_averaged_filename");
	}

	//List mode settings
	const bool LIST_MODE = Utility::stob(opts_ini.atat("list_mode_list_mode"));
	const bool USE_ALBUM_INPUT_PATH_FOR_LIST_MODE = Utility::stob(opts_ini.atat("list_mode_use_input_path_from_album_mode"));
	const bool USE_ALTERNATIVE_TAG_FOR_LIST_MODE = Utility::stob(opts_ini.atat("list_mode_use_alternative_tag"));
	const std::string ALTERNATIVE_TAG_FOR_LIST_MODE = opts_ini.atat("list_mode_alternative_tag");

	//Album mode settings
	//(New mode coming soon...)
	const std::string ALBUM_INPUT_PATH = Utility::endWithSlash(opts_ini.atat("album_mode_input_path"));
	const std::string ALBUM_NAME = opts_ini.atat("album_mode_name");
	const int FIRST_FRAME = std::stoi(opts_ini.atat("album_mode_first_frame"));
	const int LAST_FRAME = std::stoi(opts_ini.atat("album_mode_last_frame"));
	const int ALBUM_NUM_DIGITS = std::stoi(opts_ini.atat("album_mode_num_digits"));

	if(LAST_FRAME <= FIRST_FRAME || FIRST_FRAME < 0){
		std::cerr << "Invalid frame numbers for album mode: " << FIRST_FRAME << " through " << LAST_FRAME << "\n";
		exit(1);
	}

	std::cout << "Finished parsing." << std::endl;

	//Constants
	const int NUM_COLOR_CHANNELS = 3; //R,G,B.
	const int RED_INDEX = 0;
	const int GREEN_INDEX = 1;
	const int BLUE_INDEX = 2;
	
	std::string output_tag;
	std::vector<std::string> inputFilenames; //INCLUDES PATHS
	if(LIST_MODE){
		//LIST MODE
		//Input tag
		if(USE_ALTERNATIVE_TAG_FOR_LIST_MODE){
			output_tag = ALTERNATIVE_TAG_FOR_LIST_MODE;
		}
		else{
			//The tag will the the settings filename, without the path or extension (.ini).
			//First, get everything after the last slash:
			std::string slashes = "/\\";
			std::vector<std::string> v1 = Utility::splitByChars(settingsFilenameAndPath, slashes);
			std::string s1 = v1[v1.size() -1];
			//Next, get everything before the last period:
			std::string s2;
			if(s1.find(".") == std::string::npos){
				s2 = s1;
			}
			else{
				std::vector<std::string> v2 = Utility::splitByChars(s1, ".");
				s2 = "";
				for(int x = 0; x < v2.size() - 1; ++x){
					s2 += v2[x];
				}
				output_tag = s2;
			}
		}
		//Input filenames: Every non-blank line after "[list]" in the settings file
		std::ifstream listfile(settingsFilenameAndPath);
		std::string line = "";
		while(std::getline(listfile, line) && line != "[list]"){ }
		while(std::getline(listfile, line)){
			if(line.length() > 0){
				if(USE_ALBUM_INPUT_PATH_FOR_LIST_MODE){
					inputFilenames.push_back(ALBUM_INPUT_PATH + line);
				}
				else{
					inputFilenames.push_back(line);
				}
			}
		}
		if(inputFilenames.size() == 0){
			std::cerr << "ERROR: No list found for list mode in " << settingsFilenameAndPath << "\n";
			exit(1);
		}
	}
	else{
		//ALBUM MODE
		output_tag = ALBUM_NAME + Utility::intToString(FIRST_FRAME, ALBUM_NUM_DIGITS) + "-" + Utility::intToString(LAST_FRAME, ALBUM_NUM_DIGITS);
		for(int x = FIRST_FRAME; x <= LAST_FRAME; ++x){
			inputFilenames.push_back(ALBUM_INPUT_PATH + ALBUM_NAME + Utility::intToString(x, ALBUM_NUM_DIGITS) + ".ppm");
		}
	}
	const int NUM_IMAGES = inputFilenames.size();

	Image first_image = readImage(inputFilenames[0]);
	const int HEIGHT = first_image.height;
	const int WIDTH = first_image.width;

	//First pass: Sum all the values in the input files.
	Image meanAverageImage;
	
	if(SKIP_AVERAGING_PHASE){
		std::cout << "\nSKIPPING AVERAGING PHASE. Reading in pre-averaged file." << std::endl;
		meanAverageImage = readImage(preAveragedFilenameWithPath);
		deleteImage(first_image);
	}
	else{
		std::cout << "\nBeginning averaging phase. First image should take the longest." << std::endl;
		//For now, averaging means a mean average.
		//I can't think of any efficient algorithm for median average with reasonable memory usage.
		//Maybe such a thing is possible?  https://stackoverflow.com/questions/3372006/incremental-median-computation-with-max-memory-efficiency
		std::vector<std::vector<std::vector< unsigned long long > > > totals;
		
		//First image
		for(int i = 0; i < HEIGHT; ++i){
			totals.push_back(std::vector<std::vector<unsigned long long > >());
			for(int j = 0; j < WIDTH; ++j){
				totals[i].push_back(std::vector<unsigned long long >());
				totals[i][j].push_back(first_image.map[i][j].r); //0
				totals[i][j].push_back(first_image.map[i][j].g); //1
				totals[i][j].push_back(first_image.map[i][j].b); //2
			}
		}
		deleteImage(first_image);
		
		std::cout << "Averaging: Processed 1st image ok" << std::endl;
		for(int x = 1; x < NUM_IMAGES; ++x){  //starting loop with the second image because we already did the first as a special case
			Image img = readImage(inputFilenames[x]);
			
			if(img.height != HEIGHT || img.width != WIDTH){
				std::cerr << "ERROR: Image  \"" << inputFilenames[x] << "\" dimensions do not match those of image \"" << inputFilenames[0] << "\".\n";
				deleteImage(img);
				exit(1);
			}

			for(int i = 0; i < HEIGHT; ++i){
				for(int j = 0; j < WIDTH; ++j){
					totals[i][j][RED_INDEX] += img.map[i][j].r;
					totals[i][j][GREEN_INDEX] += img.map[i][j].g;
					totals[i][j][BLUE_INDEX] += img.map[i][j].b;
				}
			}
			deleteImage(img);
			std::cout << "Averaging: Processed image #" << x + 1 << " of " << NUM_IMAGES << std::endl;
		}
		meanAverageImage = createImage(HEIGHT, WIDTH);
		for(int i = 0; i < HEIGHT; ++i){
			for(int j = 0; j < WIDTH; ++j){
				meanAverageImage.map[i][j].r = (unsigned char) round(1.0 * totals[i][j][RED_INDEX] / NUM_IMAGES);
				meanAverageImage.map[i][j].g = (unsigned char) round(1.0 * totals[i][j][GREEN_INDEX] / NUM_IMAGES);
				meanAverageImage.map[i][j].b = (unsigned char) round(1.0 * totals[i][j][BLUE_INDEX] / NUM_IMAGES);
			}
		}
		if(SAVE_AVERAGE){
			writeImage(meanAverageImage, (OUTPUT_PATH + output_tag + "avg.ppm"));
		}
	}

	std::vector<DifferenceRecord> drs;
	if(DO_REGULAR){
		DifferenceRecord dr;
		dr.name = "Regular";
		dr.difference_function = DifferenceFunctions::difference_Regular;
		drs.push_back(dr);
	}
	if(DO_PERCEIVED_BRIGHTNESS){
		DifferenceRecord dr;
		dr.name = "PerceivedBrightness";
		dr.difference_function = DifferenceFunctions::difference_PerceivedBrightness;
		drs.push_back(dr);
	}
	if(DO_COLOR_RATIO){
		DifferenceRecord dr;
		dr.name = "ColorRatio";
		dr.difference_function = DifferenceFunctions::difference_ColorRatio;
		drs.push_back(dr);
	}
	if(DO_COMBO){
		DifferenceRecord dr;
		dr.name = "Combo";
		dr.difference_function = DifferenceFunctions::difference_Combined;
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
		drs[drs_index].mostDifferentPixels = std::vector<std::vector<std::vector<Pixel > > >(HEIGHT, std::vector<std::vector<Pixel > >(WIDTH, std::vector<Pixel>(NUM_PIXELS_TO_RANK, white)));
		drs[drs_index].biggestDifferences = std::vector<std::vector<std::vector<double > > >(HEIGHT, std::vector<std::vector<double > >(WIDTH, std::vector<double>(NUM_PIXELS_TO_RANK, 0)));
	}

	//Second pass: Find the most different.
	std::cout << "\nBeginning differentiating phase. First image should take the longest." << std::endl;
	for(int x = 0; x < NUM_IMAGES; ++x){
		Image img = readImage(inputFilenames[x]);
		for(int i = 0; i < HEIGHT; ++i){
			for(int j = 0; j < WIDTH; ++j){
				for(size_t drs_index = 0; drs_index < drs.size(); ++drs_index){
					double diff = drs[drs_index].difference_function(meanAverageImage.map[i][j], img.map[i][j]);

					if(diff > drs[drs_index].biggestDifferences[i][j][drs[drs_index].num_pixels_to_rank - 1]){  ///almost positive you can remove the if-statement here.
						int rank = drs[drs_index].num_pixels_to_rank - 1;
						while(rank >= 0 && diff > drs[drs_index].biggestDifferences[i][j][rank]){
							--rank;
						}
						++rank;
						if(rank < 0 || rank >= drs[drs_index].num_pixels_to_rank){
							std::cerr << "ERROR: RANK " << rank << " for " << drs[drs_index].name << "(" << i << ", " << j << ")" << std::endl;
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
		std::cout << "Differentiating: Processed image #" << x + 1 << " of " << NUM_IMAGES << std::endl;
	}

	std::cout << "\nBeginning output file creation phase." << std::endl;

	bool use_tag_as_entire_filename = false;
	if(LIST_MODE && drs.size() == 1 && rankingsToSave.size() == 1 && powersOfScore.size() == 1){
		use_tag_as_entire_filename = true;
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
				Image result_img = createImage(HEIGHT, WIDTH);
				for(int i = 0; i < HEIGHT; ++i){
					for(int j = 0; j < WIDTH; ++j){
						double totalScore = 0.0;
						for(int k = 0; k < num_pixels_to_rank_this_round; ++k){
							totalScore += pow(drs[drs_index].biggestDifferences[i][j][k], current_power);
						}
						if(totalScore <= 0.0 && !HIDE_WARNINGS){
							std::cout << "WARNING: All pixels at position " << i << ", " << j << " are equal to the average, for " << drs[drs_index].name << "." << std::endl;
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
				//outputFilename variable DOES NOT INCLUDE PATH
				std::string outputFilename;
				if(use_tag_as_entire_filename){
					outputFilename = output_tag + ".ppm";
				}
				else{
					outputFilename = output_tag + drs[drs_index].name
												+ "_rank" + Utility::intToString(num_pixels_to_rank_this_round)
												+ "_power" + Utility::doubleToString(current_power, 1);
					if(drs[drs_index].invert_scores){
						outputFilename += "_invertscore";
					}
					outputFilename += ".ppm";
				}
				writeImage(result_img, OUTPUT_PATH + outputFilename);
				std::cout << "Created file " << outputFilename << std::endl;
				deleteImage(result_img);
			}
		}
	}

	//Success
	std::cout << "\n\n     ___    __  __  _    ___  _     \n    /  _]  /  ]|  |/ ]  /  _]| |    \n   /  [_  /  / |  ' /  /  [_ | |    \n  |    _]/  /  |    \\ |    _]| |___ \n  |   [_/   \\_ |     \\|   [_ |     |\n  |     \\     ||  .  ||     ||     |\n  |_____|\\____||__|\\_||_____||_____|\n" << std::endl;

	return 0;
}