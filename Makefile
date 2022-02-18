# Makefile for LeastAverageImage
# Andrew Eckel

#The compiler is g++, the code requies C++11, and, I don't know, this flto thing may or may not help.
CC=g++
FLAGS=-std=c++11 -flto
#The subdirectories for the object files and the program file
FOLDER_OBJ=object_files
FOLDER_PROGRAM=program
#The objects that will be built from C++ style code
OBJ_CPP=$(FOLDER_OBJ)/differencefunctions.o $(FOLDER_OBJ)/iniparser.o $(FOLDER_OBJ)/main.o $(FOLDER_OBJ)/utility.o $(FOLDER_OBJ)/ppm_functions.o
#The objects that will be built from C style code.
OBJ_C=$(FOLDER_OBJ)/ini.o

$(FOLDER_PROGRAM)/lai : $(OBJ_CPP) $(OBJ_C)
		$(CC) -o $(FOLDER_PROGRAM)/lai $(FLAGS) $(OBJ_CPP) $(OBJ_C)

$(OBJ_CPP): $(FOLDER_OBJ)/%.o: src/%.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(OBJ_C): $(FOLDER_OBJ)/%.o: src/%.c
	$(CC) $(FLAGS) -c $< -o $@

#This is the clean function for UNIX based operating systems.
clean :
	rm $(FOLDER_PROGRAM)/lai $(OBJ_CPP) $(OBJ_C)

#This is the clean function for Windows.
clean_win :
	del $(FOLDER_PROGRAM)\lai.exe
	del $(FOLDER_OBJ)\*.o