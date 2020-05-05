#include "abstractimage.h"

//Constructors
AbstractImage::AbstractImage(const std::string the_path_and_filename, bool pref_hold_in_memory) :
	hold_in_memory(pref_hold_in_memory)
{
	path_and_filename = (char*) malloc(the_path_and_filename.length() + 1);
	sprintf(path_and_filename, the_path_and_filename.c_str());
	if(hold_in_memory){
		img = readImage(path_and_filename);
	}
}

AbstractImage::AbstractImage(const AbstractImage &ai) : //copy constructor
	hold_in_memory(ai.hold_in_memory)
{
	path_and_filename = (char*) malloc(strlen(ai.path_and_filename));
	sprintf(path_and_filename, ai.path_and_filename);
	if(hold_in_memory){
		img = readImage(path_and_filename);
	}
	//std::cout << "copy constructor lol\n";
}

//Destructor
AbstractImage::~AbstractImage(){
	if(hold_in_memory){
		deleteImage(img);
	}
	free(path_and_filename);
}

//Public functions
Pixel AbstractImage::at(int i, int j){
	if(hold_in_memory){
		return img.map[i][j];
	}

	img = readImage(path_and_filename);
	Pixel ret = img.map[i][j];
	deleteImage(img);
	return ret;
}

int AbstractImage::getHeight(){
	if(hold_in_memory){
		return img.height;
	}

	img = readImage(path_and_filename);
	int ret = img.height;
	deleteImage(img);
	return ret;
}

int AbstractImage::getWidth(){
	if(hold_in_memory){
		return img.width;
	}

	img = readImage(path_and_filename);
	int ret = img.width;
	deleteImage(img);
	return ret;
}