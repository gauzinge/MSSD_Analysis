//
//  trackfunctions.cc
//  
//
//  Created by Georg Auzinger on 22.08.12.
//
//
#ifndef trackfunctions_h
#define trackfunctions_h

#include <string>
#include <string.h>

int get_pitch_region(int Region); 

int track_good(double xCoordinate, double yCoordinate, int npitches); 

bool track_good_strips(int);

bool track_good_y(double, double, double);

int get_region(double xCoordinate);

int get_antistrip(int);

int reverse_hitmaker(double);

double hitmaker(double);

std::string find_matchfiledir(std::string datafilename);

std::string change_filetype(std::string filename, std::string filetype);

#endif

