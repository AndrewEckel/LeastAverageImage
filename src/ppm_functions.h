#ifndef PPM_FUNCTIONS
#define PPM_FUNCTIONS

#include <string>
#include <utility>
#include <iostream>

// ppm_functions.h
// Marc Pomplun
// Functions for reading and writing binary PPM image files.
// V2.2E
// Based on V2.2, 2013-10-19 Marc Pomplun + Marc's bicubic resampling functions sent by e-mail
// Last updated 2022-02-17

// Andrew's changes for LeastAverageImage:
// This version has the "i" channel removed. It is more efficient for PPM files but it no longer supports PBM, PGM.
// It also has versions of functions that accept C++ std::strings instead of C-style string literals.
// Added copyPixel functions
// Added readHeightAndWidth function -- pretty much just readImage without the actual image reading.
// Added Marc's bicubic resampling function (with my CLAMP() definition)
// Removed functions pertaining to matrices, shapes, and lines
// Changed the C file to a C++ file (although the bulk of the code is still C style)
// Renamed from NETPBM to PPM_Functions
// Added resize_and_crop function

#define SQR(x) ((x)*(x))
#define PI 3.14159265358979323846
#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define CLAMP(X) MIN(MAX(X,0),255)

// Additional color options for drawing lines and shapes.
#define NO_CHANGE -1
#define INVERT    -2

// For each pixel, we store the R, G, and B values scaled from 0 to 255
// and the intensity (brightness), which is automatically set to the
// average of R, G, and B when creating or reading a new image.
typedef struct
{
	unsigned char r, g, b;
} Pixel;

// Notice that the pixel map uses "matrix notation," i.e., map[i][j] refers to
// the pixel in row i and column j (counting rows and columns starts with 0).
typedef struct
{
	int height, width;
	Pixel **map;
} Image;

// The supported file type, using 24 bits per pixel
typedef enum format {PPM} Format;

// Create a new image of the given size and fill it with white pixels.
// When you don't need the image anymore, don't forget to free its memory using deleteImage.
Image createImage(int height, int width);

// Delete a previously created image and free its allocated memory on the heap.
void deleteImage(Image img);

// Read an image from a file and allocate the required heap memory for it.
// Notice that only PPM files are supported. Regardless of the
// file type, all fields r, g, b, and i are filled in, with values from 0 to 255.
Image readImage(const char *filename);
Image readImage(const std::string filename);

// Write an image to a file. The file format (binary PBM, PGM, or PPM) is automatically
// chosen based on the given file name. For PBM and PGM files, only the intensity
// (i) information is used, and for PPM files, only r, g, and b are relevant.
void writeImage(Image img, const char *filename);
void writeImage(Image img, const std::string filename);

// Set color for pixel (vPos, hPos) in image img.
// If r, g, b, or i are set to NO_CHANGE, the corresponding color channels are left unchanged in img.
// If they are set to INVERT, the corresponding channels are inverted, i.e., set to 255 minus their original value
void setPixel(Image img, int vPos, int hPos, int r, int g, int b);

//Copy a Pixel
void copyPixel(Pixel* to, Pixel from);
void copyPixel(Pixel* to, Pixel* from);

// Helper function for resampleBicubic
double c(double s, int n);
// Rescale a color image using bicubic interpolation so that the new image size is vTarget by hTarget pixels.
Image resampleBicubic(Image inImage, int vTarget, int hTarget);

//Read in the height and width information only and return it in a pair, with height first, width second.
std::pair<int, int> readHeightAndWidth(const std::string filename);

//Creates a copy of the image img, resized to the given height or width, whichever is a greater percent enlargement,
//then crops to match the exact dimensions. Returns the copy and only deletes the original if delete_original is true
Image resize_and_crop(Image img, const int OUTPUT_HEIGHT, const int OUTPUT_WIDTH, bool delete_original);

#endif // PPM_FUNCTIONS