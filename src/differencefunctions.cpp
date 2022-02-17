#include "differencefunctions.h"

//DifferenceFunctions note:
//All commented-out code here should be considered legitimate documenation of alternatives and their pros and cons.

//Private--------------------------------------------------------------------------------------------------------------

//helper function for difference_PerceivedBrightness
double DifferenceFunctions::perceived_Brightness(Pixel color){
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

  //pow() should not be used here, because pow on a non-tiny number just returns something stupid called HUGEVAL.
  return sqrt(0.299*r*r + 0.587*g*g + 0.114*b*b);
}

//Public---------------------------------------------------------------------------------------------------------------
double DifferenceFunctions::difference_Regular(Pixel p1, Pixel p2){
	//Possible range: 0 to 512

	//Green counts 2x as much.
	//return std::abs(((int) p1.r) - p2.r) + 2 * std::abs(((int) p1.g) - p2.g) + std::abs(((int) p1.b) - p2.b);
	//"harder math version": requires doubles instead of ints as outputs. I like this version SLIGHTLY better.
	return sqrt(pow(p1.r - p2.r, 2.0) + 2 * pow(p1.g - p2.g, 2.0) + pow(p1.b - p2.b, 2.0));
}

double DifferenceFunctions::difference_ColorRatio(Pixel p1, Pixel p2){
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

double DifferenceFunctions::difference_InvertedColorRatio(Pixel p1, Pixel p2){
	//Possible range: 0 to 440

	//The "max" thing is there to avoid a divide-by-zero.
	double p1_RtoG = (255.0 - p1.r) / std::max(255 - p1.g, 1);
	double p1_RtoB = (255.0 - p1.r) / std::max(255 - p1.b, 1);
	double p1_GtoB = (255.0 - p1.g) / std::max(255 - p1.b, 1);

	double p2_RtoG = (255.0 - p2.r) / std::max(255 - p2.g, 1);
	double p2_RtoB = (255.0 - p2.r) / std::max(255 - p2.b, 1);
	double p2_GtoB = (255.0 - p2.g) / std::max(255 - p2.b, 1);

	return sqrt(pow(p1_RtoG - p2_RtoG, 2.0) + 2 * pow(p1_RtoB - p2_RtoB, 2.0) + pow(p1_GtoB - p2_GtoB, 2.0));
}

double DifferenceFunctions::difference_PerceivedBrightness(Pixel p1, Pixel p2){
	//Possible range: 0 to 255.
	return std::abs(perceived_Brightness(p1) - perceived_Brightness(p2));
}

double DifferenceFunctions::difference_Combined(Pixel p1, Pixel p2){
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

double DifferenceFunctions::difference_InvertedEnumeratorColorRatio(Pixel p1, Pixel p2){
	double p1_RtoG = (255.0 - p1.r) / std::max((int) p1.g, 1);
	double p1_RtoB = (255.0 - p1.r) / std::max((int) p1.b, 1);
	double p1_GtoB = (255.0 - p1.g) / std::max((int) p1.b, 1);

	double p2_RtoG = (255.0 - p2.r) / std::max((int) p2.g, 1);
	double p2_RtoB = (255.0 - p2.r) / std::max((int) p2.b, 1);
	double p2_GtoB = (255.0 - p2.g) / std::max((int) p2.b, 1);

	return sqrt(pow(p1_RtoG - p2_RtoG, 2.0) + 2 * pow(p1_RtoB - p2_RtoB, 2.0) + pow(p1_GtoB - p2_GtoB, 2.0));
}

double DifferenceFunctions::difference_HalfInvertedColorRatio(Pixel p1, Pixel p2){
	double p1_RtoG = (128.0 - p1.r) / std::max(128 - p1.g, 1);
	double p1_RtoB = (128.0 - p1.r) / std::max(128 - p1.b, 1);
	double p1_GtoB = (128.0 - p1.g) / std::max(128 - p1.b, 1);

	double p2_RtoG = (128.0 - p2.r) / std::max(128 - p2.g, 1);
	double p2_RtoB = (128.0 - p2.r) / std::max(128 - p2.b, 1);
	double p2_GtoB = (128.0 - p2.g) / std::max(128 - p2.b, 1);

	return sqrt(pow(p1_RtoG - p2_RtoG, 2.0) + 2 * pow(p1_RtoB - p2_RtoB, 2.0) + pow(p1_GtoB - p2_GtoB, 2.0));
}

std::string DifferenceFunctions::NAME_OF_CURRENT_EXPERIMENT_DIFFERENCE_FUNCTION = "Color Ratio Flipped";

double DifferenceFunctions::difference_Experiment(Pixel p1, Pixel p2){
	//This function is for whatever new idea is being tested.
	//Any idea that produces interesting results should be made into its own function.
	
	//ColorRatio Flipped
	double p1_GtoR = (1.0 * p1.g) / std::max((int) p1.r, 1);
	double p1_BtoR = (1.0 * p1.b) / std::max((int) p1.r, 1);
	double p1_BtoG = (1.0 * p1.b) / std::max((int) p1.g, 1);

	double p2_GtoR = (1.0 * p2.g) / std::max((int) p2.r, 1);
	double p2_BtoR = (1.0 * p2.b) / std::max((int) p2.r, 1);
	double p2_BtoG = (1.0 * p2.b) / std::max((int) p2.g, 1);

	return sqrt(pow(p1_GtoR - p2_GtoR, 2.0) + 2 * pow(p1_BtoR - p2_BtoR, 2.0) + pow(p1_BtoG - p2_BtoG, 2.0));
}