# LeastAverageImage
Given a set of images, LeastAverageImage creates the least average -- the opposite of the average.

![alt text](https://www.AndrewEckel.com/LeastAverageImage/airplane009_aaaa.jpg)

More information about the program is here: https://www.AndrewEckel.com/LeastAverageImage/

## Requirements
To use LeastAverageImage, you will need
- [ImageMagick](https://imagemagick.org/): a free command line program you'll need to convert image files to and from the PPM format, the only format LeastAverageImage recognizes

You will also "pretty much need"
- A text editor that automatically color-codes INI files to make them easier to read, such as [Notepad++](https://notepad-plus-plus.org/downloads/) or [Sublime Text](https://www.sublimetext.com/)
- A general image processing program to adjust the saturation, brightness, contrast, color balance, etc in your result images.  The more variation there is your input images, the more grey-looking your raw results will be.

## Setting up on Windows

### Not compiling

Windows users can download a zip file with a 64 bit executable here:
https://github.com/AndrewEckel/LeastAverageImage/releases

### Compiling!

Those who want to compile the code themselves should start by [cloning](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository) the entire git repo.

LeastAverageImage does not have complex dependencies, so if you have a preferred Windows C++ compiler, use that.  Otherwise...

Install MinGW-w64 and add it to your Windows PATH by following the instructions here:

[Setting up MinGW-w64](https://www.eclipse.org/4diac/documentation/html/installation/minGW.html) (just the first two sections -- Install MinGW-w64 and Test MinGW-w64)

Using the Windows command line, navigate to the project directory, where the Makefile is stored, and execute this command:
```
mingw32-make
```
## Testing on Windows

After unzipping the program or compiling from the source code, navigate to the `program` directory using the Windows command line, and try running these two commands:

```
lai
lai ../input/list_mode_test.ini
```

The first command should create 3 PPM files in the output directory, and the second should create an additional 33.

## Compiling and testing on a Mac or other UNIX-based OS

Start by [cloning](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository) the entire git repo.

Open a Terminal window in the directory that holds the Makefile, and execute this command:
```
make
```
On a Mac, if you do not have `g++` and/or `make` installed, you will be asked if you wish to install Xcode. (Yes, you do!)

After compiling, navigate to the `program` directory and try running these two commands:
```
./lai
./lai ../input/list_mode_test.ini
```
The first command should create 3 PPM files in the output directory, and the second should create an additional 33.

## Running LeastAverageImage

LeastAverageImage accepts one command line argument, the name of a single INI settings file.  Without an argument, the program will default to `../input/settings.ini`.

The included sample INI files have notes on the meaning of all the options.

LeastAverageImage is a single-threaded program.  For processing multiple sets of images on a computer with "n" logical cores, you may wish to run up to n instances of LeastAverageImage at a time.

The amount of time each run takes depends on the number of input images, their dimensions, the number of difference functions included, and the highest value chosen for `rankings_to_save`. Remember, the area of an image is its width times its height, so if an image's dimensions are doubled, that makes it four times as big, meaning it would take LeastAverageImage four times as long to process!

For Windows users, batch files are included in the input and output folders for converting to and from PPM files using ImageMagick:
- `ImagestoPPM_without_delete.bat` converts all JPG/JPEG, TIF, HEIC, PNG, and WEBP files in the current folder to PPM.
- `PPMtoTIF.bat` converts all PPM files in the current folder to TIF and also deletes the original PPM files unless their filenames end in `avg.ppm`.