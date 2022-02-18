rem This is a Windows batch file. It compiles a 32 bit static executable, without using MinGW or the makefile at all.
del lai32.exe
g++ ../src/* -std=c++11 -static -m32 -flto -o lai32.exe