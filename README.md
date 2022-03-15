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

A pre-compiled executable of LeastAverageImage for 64 bit Windows version 8 or later is hosted here:
https://www.AndrewEckel.com/LeastAverageImage/

If that doesn't work for you, here's how to compile from the source code.  LeastAverageImage does not have complex dependencies, so if you have a preferred Windows C++ compiler, use that.  Otherwise...

Install MinGW-w64 and add it to your Windows PATH by following the instructions here:

[Setting up MinGW-w64](https://www.eclipse.org/4diac/documentation/html/installation/minGW.html) (just the first two sections -- Install MinGW-w64 and Test MinGW-w64)

Using the Windows command line, navigate to the project directory, where the Makefile is stored, and execute this command:
```
mingw32-make
```
## Testing on Windows

After unzipping the pre-compiled program or compiling from the source code, navigate to the program directory using the Windows command line, and try running these two commands:
```
lai
lai ../input/list_mode_test.ini
```
The first command should create 3 PPM files in the output directory, and the second should create an additional 33.

## Compiling and testing on a Mac or other UNIX-based OS

Open a Terminal window in the directory that holds the Makefile, and execute this command:
```
make
```
If you do not have g++ and/or make installed, you will be asked if you wish to install Xcode. (You do!)

After compiling, navigate to the program directory and try running these two commands:
```
./lai
./lai ../input/list_mode_test.ini
```
The first command should create 3 PPM files in the output directory, and the second should create an additional 33.

## Running LeastAverageImage

LeastAverageImage accepts one command line argument, the name of a single INI settings file.  Without an argument, the program will default to ../input/settings.ini.

The included sample INI files have notes on the meaning of all the options.

LeastAverageImage is a single-threaded program.  For processing multiple sets of images on a computer with "n" logical cores, you may wish to run up to n instances of LeastAverageImage at a time.
