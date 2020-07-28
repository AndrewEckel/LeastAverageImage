#ifndef DIFFERENCEFUNCTIONS_H
#define DIFFERENCEFUNCTIONS_H

#include <math.h>
#include <cmath>

#include "netpbm.h"

class DifferenceFunctions{
private:
	static double perceived_Brightness(Pixel color);
public:
	//All functions are static.
	static double difference_Regular(Pixel p1, Pixel p2);
	static double difference_ColorRatio(Pixel p1, Pixel p2);
	static double difference_InvertedColorRatio(Pixel p1, Pixel p2);
	static double difference_PerceivedBrightness(Pixel p1, Pixel p2);
	static double difference_Combined(Pixel p1, Pixel p2);
	static double difference_InvertedEnumeratorColorRatio(Pixel p1, Pixel p2);
	static double difference_HalfInvertedColorRatio(Pixel p1, Pixel p2);
	static double difference_Experiment(Pixel p1, Pixel p2);
};

#endif //DIFFERENCEFUNCTIONS_H