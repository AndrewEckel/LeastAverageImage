//Abstract Image
#ifndef ABSTRACT_IMAGE
#define ABSTRACT_IMAGE

#include <iostream>
#include <string>
#include <string.h> //for strlen
#include <stdlib.h>  //for malloc

#include "netpbm.h"

class AbstractImage
{
private:
	char* path_and_filename;
	Image img;
	bool hold_in_memory;
public:
	//Constructors
	AbstractImage(const std::string the_path_and_filename, bool pref_hold_in_memory);
	AbstractImage(const AbstractImage &ai); //copy constructor
	//Destructor
	~AbstractImage();

	//Public functions
	Pixel at(int i, int j);
	int getHeight();
	int getWidth();
};

#endif //ABSTRACT_IMAGE