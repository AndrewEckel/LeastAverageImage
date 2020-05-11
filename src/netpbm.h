#ifndef NETPBM
#define NETPBM

#include <string>

// netpbm.h
// Functions for reading and writing binary PBM, PGM, and PPM image files.
// V2.2B by Andrew Eckel, based on...
// V2.2 by Marc Pomplun on 10/19/2013
// This version has the "i" channel removed. It is more efficient for PPM files but it no longer supports PBM, PGM.
// It also has versions of functions that accept C++ std::strings instead of C-style string literals.

#define SQR(x) ((x)*(x))
#define PI 3.14159265358979323846
#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define MAX(X,Y) ((X)>(Y)?(X):(Y))

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

// Matrices are like Images except that they hold one real number (double) as
// each entry instead of r, g, b, and i values.
typedef struct
{
	int height, width;
	double **map;
} Matrix;

// The ONE supported file type using  24 bits per pixel!!!!
typedef enum format {PPM} Format;

// Create a new image of the given size and fill it with white pixels.
// When you don't need the image anymore, don't forget to free its memory using deleteImage.
Image createImage(int height, int width);

// Delete a previously created image and free its allocated memory on the heap.
void deleteImage(Image img);

// Create a new matrix of the given size and fill it with zeroes.
// When you don't need the matrix anymore, don't forget to free its memory using deleteMatrix.
Matrix createMatrix(int height, int width);

// Create a new matrix of the given size and fill it with content of 2D double array.
// Call this function with a pointer to the first element of the array, e.g., &a[0][0].
// When you don't need the matrix anymore, don't forget to free its memory using deleteMatrix.
Matrix createMatrixFromArray(double *entry, int height, int width);

// Delete a previously created matrix and free its allocated memory on the heap.
void deleteMatrix(Matrix mx);

// Read an image from a file and allocate the required heap memory for it.
// Notice that only binary Netpbm files are supported. Regardless of the
// file type, all fields r, g, b, and i are filled in, with values from 0 to 255.
Image readImage(const char *filename);
Image readImage(const std::string filename);

// Write an image to a file. The file format (binary PBM, PGM, or PPM) is automatically
// chosen based on the given file name. For PBM and PGM files, only the intensity
// (i) information is used, and for PPM files, only r, g, and b are relevant.
void writeImage(Image img, const char *filename);
void writeImage(Image img, const std::string filename);

// Convert the intensity components of an image into a matrix of identical size.
Matrix image2Matrix(Image img);

// Convert a matrix into an image with corresponding, r, g, b, and i components and size.
// If scale == 0 then values remain unchanged but if they are below 0 or above 255, they
// are set to 0 or 255, respectively.
// If scale != 0 the values are scaled so that minimum value is zero and maximum is 255.
// Setting the gamma value allows for exponential scaling, with gamma == 1.0 enabling
// linear scaling.
Image matrix2Image(Matrix mx, int scale, double gamma);

// Set color for pixel (vPos, hPos) in image img.
// If r, g, b, or i are set to NO_CHANGE, the corresponding color channels are left unchanged in img.
// If they are set to INVERT, the corresponding channels are inverted, i.e., set to 255 minus their original value
void setPixel(Image img, int vPos, int hPos, int r, int g, int b);

// Draw filled ellipse in image img centered at (vCenter, hCenter) with radii vRadius and hRadius.
// Radius (0, 0) will draw an individual pixel. For setting the r, g, b, and i color values, see setPixel function.
void filledEllipse(Image img, int vCenter, int hCenter, int vRadius, int hRadius, int r, int g, int b);

// Draw filled rectangle in image img with opposite edges (v1, h1) and (v2, h2).
// For setting the r, g, b, and i color values, see setPixel function.
void filledRectangle(Image img, int v1, int h1, int v2, int h2, int r, int g, int b);

// Draw straight line in image img between (v1, h1) and (v2, h2) with a given width, dash pattern, and color.
// Width 0 indicates single-pixel width. The inputs dash and gap determine the length in pixels of the dashes
// and the gaps between them, resp. Use 0 for either input to draw a solid line.
// For setting the r, g, b, and i color values, see setPixel function.
void line(Image img, int v1, int h1, int v2, int h2, int width, int dash, int gap, int r, int g, int b);

// Draw rectangle in image img with opposite corners (v1, h1) and (v2, h2) with a given width, dash pattern, and color.
// Inputs are otherwise identical to the line function.
void rectangle(Image img, int v1, int h1, int v2, int h2, int width, int dash, int gap, int r, int g, int b);

// Draw ellipse in image img centered at (vCenter, hCenter) and radii (vRadius, hRadius) with a given width, dash pattern, and color.
// Width 0 indicates single-pixel width. The inputs dash and gap determine the length in pixels of the dashes
// and the gaps between them, resp. Use 0 for either input to draw a solid line.
// For setting the r, g, b, and i color values, see setPixel function.
void ellipse(Image img, int vCenter, int hCenter, int vRadius, int hRadius, int width, int dash, int gap, int r, int g, int b);

#endif // NETPBM