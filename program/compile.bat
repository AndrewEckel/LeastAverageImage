rem This is a Windows batch file for PCs with MinGW-w64 already installed and on the PATH.
rem It just goes up a level, calls make, and then comes back.
cd ..
mingw32-make
cd program