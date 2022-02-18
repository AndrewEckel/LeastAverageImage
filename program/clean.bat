rem This is a Windows batch files for PCs with MinGW-w64 already installed and on the PATH.
rem It just goes up a level, calls make clean, and then comes back.
cd ..
mingw32-make clean_win
cd program