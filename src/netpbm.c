// netpbm.c 
// Functions for reading and writing binary PPM image files.
// V2.2 by Marc Pomplun on 10/19/2013
#define _CRT_SECURE_NO_WARNINGS

#include "netpbm.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>

// Create a new image of the given size and fill it with white pixels.
// When you don't need the image anymore, don't forget to free its memory using deleteImage.
Image createImage(int height, int width)
{
	int i, j;
	Image img;

	img.map = (Pixel **) malloc(sizeof(Pixel *)*height);
	for (i = 0; i < height; i++)
	{
		img.map[i] = (Pixel *) malloc(sizeof(Pixel)*width);
		for (j = 0; j < width; j++)
		{
			img.map[i][j].r = 255;
			img.map[i][j].g = 255;
			img.map[i][j].b = 255;
		}
	}
	img.height = height;
	img.width = width;
	return img;
}

// Delete a previously created image and free its allocated memory on the heap. 
void deleteImage(Image img)
{
	int i;
	
	for (i = 0; i < img.height; i++)
		free(img.map[i]);
	free(img.map);
}

// Read an image from a file and allocate the required heap memory for it.
// Notice that only binary Netpbm files are supported. Regardless of the
// file type, all fields r, g, b, and i are filled in, with values from 0 to 255. 
Image readImage(const char *filename)
{
	FILE *f;
	int i, j, width, height, imax, bitsPerPixel, filesize, mapsize, mempos;
	char type[200], line[200];
	unsigned char *temp, output;
	Image img;
	Format filetype;

	f = fopen(filename, "rb");
	if (!f)
	{
		fprintf(stderr, "Can't open input file %s.\n", filename);
		exit(1);
	}

	fscanf(f, "%s", type);
	//if (type[0] != 'P' || type[1] < '4' || type[1] > '6')
	if (type[0] != 'P') //??
	{
		fprintf(stderr, "Error in %s: Only binary PPM files are supported.\n", filename);
		exit(1);
	}
 
	filetype = PPM;
	bitsPerPixel = 24;


	line[0] = '#';
	while (line[0] == '#' || line[0] == 10 || line[0] == 13)
		fgets(line, 200, f); 
	sscanf(line, "%d %d", &width, &height);

	fgets(line, 200, f); 
	sscanf(line, "%d", &imax);

	if (width <= 0 || height <= 0)
	{
		fprintf(stderr, "Invalid image size in input file %s.\n", filename);
		exit(1);
	}

	// Notice: In PBM files, every row starts with a new byte.
	mapsize = (bitsPerPixel*width + 7)/8*height;
	temp = (unsigned char *) malloc(mapsize);
	// Using fread is much faster than reading byte-by-byte. 
	filesize = fread((void *) temp, 1, mapsize, f);
	fclose(f);
	if (filesize != mapsize)
	{
		fprintf(stderr, "Data missing in file %s.\n", filename);
		exit(1);
	}

	img = createImage(height, width);
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			mempos = (bitsPerPixel*width + 7)/8*i + (bitsPerPixel*j)/8;
			img.map[i][j].r = (unsigned char) ((int) temp[mempos]*255/imax);
			img.map[i][j].g = (unsigned char) ((int) temp[mempos + 1]*255/imax);
			img.map[i][j].b = (unsigned char) ((int) temp[mempos + 2]*255/imax);
		}
	free(temp);
	return img;
}

Image readImage(const std::string filename)
{
	return readImage(filename.c_str());
}

// Write an image to a file. The file format (binary PBM, PGM, or PPM) is automatically
// chosen based on the given file name. For PBM and PGM files, only the intensity
// (i) information is used, and for PPM files, only r, g, and b are relevant.
void writeImage(Image img, const char *filename)
{
	Format filetype;
	FILE *f;
	int i, j, mapsize, bitsPerPixel, bitsum, mempos;
	unsigned char *temp;

	switch (filename[strlen(filename) - 2])
	{
	case 'p':
	case 'P': 
		filetype = PPM;
		bitsPerPixel = 24;
		break;
	default:  
		fprintf(stderr, "Invalid output file name: %s.\n", filename);
		exit(1);
	}

	if (img.width <= 0 || img.height <= 0)
	{
		fprintf(stderr, "Invalid image size in output file %s.\n", filename);
		exit(1);
	}

	// Notice: In PBM files, every row starts with a new byte.
	mapsize = (bitsPerPixel*img.width + 7)/8*img.height;
	// Creating linear file data in memory and then using fwrite is much faster than writing byte-by-byte. 
	temp = (unsigned char *) malloc(mapsize);
	mempos = 0;
	bitsum = 0;
	for (i = 0; i < img.height; i++){
		for (j = 0; j < img.width; j++){
			temp[mempos++] = img.map[i][j].r;
			temp[mempos++] = img.map[i][j].g;
			temp[mempos++] = img.map[i][j].b;
		}
	}


	f = fopen(filename, "wb");
	if (!f)
	{
		fprintf(stderr, "Can't open output file %s.\n", filename);
		exit(1);
	}

	fprintf(f, "P6\n# Created by netpbm.c\n%d %d\n255\n", img.width, img.height);

	fwrite((void *) temp, 1, mapsize, f);
	fclose(f);
	free(temp);
}

void writeImage(Image img, const std::string filename)
{
	return writeImage(img, filename.c_str());
}

// Set color for pixel (vPos, hPos) in image img.
// If r, g, b, or i are set to NO_CHANGE, the corresponding color channels are left unchanged in img.
// If they are set to INVERT, the corresponding channels are inverted, i.e., set to 255 minus their original value
void setPixel(Image img, int vPos, int hPos, int r, int g, int b)
{
	if (vPos >= 0 && vPos < img.height && hPos >= 0 && hPos < img.width && img.map != NULL)
	{
		if (r == INVERT)
			img.map[vPos][hPos].r = 255 - img.map[vPos][hPos].r;
		else if (r >= 0 && r <= 255)
			img.map[vPos][hPos].r = r;
		
		if (g == INVERT)
			img.map[vPos][hPos].g = 255 - img.map[vPos][hPos].g;
		else if (g >= 0 && g <= 255)
			img.map[vPos][hPos].g = g;

		if (b == INVERT)
			img.map[vPos][hPos].b = 255 - img.map[vPos][hPos].b;
		else if (b >= 0 && b <= 255)
			img.map[vPos][hPos].b = b;
	}
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

// Helper function for resampleBicubic
double c(double s, int n)
{
  switch (n) {
    case 0: return - 1.0/6.0 * s * s * s + s * s - 11.0/6.0 * s + 1.0;
    case 1: return   1.0/2.0 * s * s * s - 5.0/2.0 * s * s + 3.0 * s;
    case 2: return - 1.0/2.0 * s * s * s + 2.0 * s * s - 3.0/2.0 * s;
    case 3: return   1.0/6.0 * s * s * s -1.0/2.0 * s * s + 1.0/3.0 * s;
  }
}

// Rescale a color image using bicubic interpolation so that the new image size is vTarget by hTarget pixels.
Image resampleBicubic(Image inImage, int vTarget, int hTarget)
{
    int i, j, k, l, i_offset, j_offset, k_pixel, l_pixel;
    double rValue, gValue, bValue, i_orig, j_orig, i_relpos, j_relpos;
    double intensity;
    double i_factor = (double) inImage.height/(double) vTarget;
    double j_factor = (double) inImage.width/(double) hTarget;
    Image outImage = createImage(vTarget, hTarget);

    for (i = 0; i < vTarget; i++){
        for (j = 0; j < hTarget; j++){
            i_orig = (double) i*i_factor;
            j_orig = (double) j*j_factor;
            i_offset = (int) i_orig - 1;
            j_offset = (int) j_orig - 1;
            i_relpos = i_orig - (double) i_offset;
            j_relpos = j_orig - (double) j_offset;

            rValue = 0.0;
            gValue = 0.0;
            bValue = 0.0;
            for (k = 0; k < 4; k++)
                for (l = 0; l < 4; l++)
                {
                    k_pixel = MIN(inImage.height - 1, MAX(0, i_offset + k));
                    l_pixel = MIN(inImage.width - 1, MAX(0, j_offset + l));
                    rValue += (double) inImage.map[k_pixel][l_pixel].r*c(i_relpos, k)*c(j_relpos, l);
                    gValue += (double) inImage.map[k_pixel][l_pixel].g*c(i_relpos, k)*c(j_relpos, l);
                    bValue += (double) inImage.map[k_pixel][l_pixel].b*c(i_relpos, k)*c(j_relpos, l);
                    outImage.map[i][j].r = CLAMP((int) (rValue + 0.5));
                    outImage.map[i][j].g = CLAMP((int) (gValue + 0.5));
                    outImage.map[i][j].b = CLAMP((int) (bValue + 0.5));
                }
        }
    }
    return outImage;
}

//Read in the height and width information only and return it in a pair, with height first, width second.
std::pair<int, int> readHeightAndWidth(const std::string filename){
	FILE *f;
	int width, height, imax;
	char type[200], line[200];

	Format filetype;

	f = fopen(filename.c_str(), "rb");
	if (!f)
	{
		fprintf(stderr, "Can't open input file %s.\n", filename.c_str());
		exit(1);
	}

	fscanf(f, "%s", type);
	if (type[0] != 'P')
	{
		fprintf(stderr, "Error in %s: Only binary PPM files are supported.\n", filename.c_str());
		exit(1);
	}
 
	line[0] = '#';
	while (line[0] == '#' || line[0] == 10 || line[0] == 13){
		fgets(line, 200, f); 
	}
	sscanf(line, "%d %d", &width, &height);

	fgets(line, 200, f); 
	sscanf(line, "%d", &imax);

	if (width <= 0 || height <= 0)
	{
		fprintf(stderr, "Invalid image size in input file %s.\n", filename.c_str());
		exit(1);
	}
	fclose(f);
	
	return std::make_pair(height, width);
}