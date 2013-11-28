//
//  Track.cc
//  
//
//  Created by Georg Auzinger on 19.06.12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
#ifndef Track_h
#define Track_h

#include <stdio.h>
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include <string.h>
#include <math.h>
#define PI 3.14159265

class Track{
private:  
    TFile* TrackFile;
public:
    //All coordinates still in Telescope Frame of Reference called x, y, z!
    TTree* Tracks;
    int EventNumber;
    double x, xPrime;
    double y, yPrime;
    double xSlope_local, ySlope_local;
    double xIntercept_local, yIntercept_local;
    
    Track();
    
    ~Track();
    
    void readtree(std::string TrackFile_Name); 
    
    unsigned int get_n_entries();
    
    void get_entry(int i_entry);
    
    void rotate_hit_coordinates (double x_shift, double y_shift, double angle); 
    
    void get_track_DUTPos(double z);
    
    void apply_alignment(double angle, double xshift, double yshift, double zPos);
    
};

#endif


