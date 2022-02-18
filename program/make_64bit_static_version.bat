rem This is a batch file for a 64bit Windows machine. It compiles a 64 bit static executable, without using MinGW or the makefile at all.
del lai.exe
g++ ../src/* -std=c++11 -static -flto -o lai.exe