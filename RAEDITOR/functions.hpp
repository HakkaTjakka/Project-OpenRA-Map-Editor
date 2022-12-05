#ifndef FUNCTIONS_HPP_INCLUDED
#define FUNCTIONS_HPP_INCLUDED


#include <iostream>
#include <algorithm>
//#include <conio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <sys/stat.h>
#include <filesystem>

#include <SFML/System/Thread.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>



using namespace std;



long int read_bin_file( string filename, unsigned char* &mem);
long main_readbin(int argc, char ** argv, unsigned char* &bin);

int main2();
bool file_exists(const char * filename);
int edit_bin(unsigned char* bin, long size);
int make_bin(unsigned char* bin, long size);
int save_bin(unsigned char* bin, long size);





#endif // FUNCTIONS_HPP_INCLUDED
