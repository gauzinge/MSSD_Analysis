//
//  on_track_signal.h
//  
//
//  Created by Georg Auzinger on 14.12.12.
//
//

#ifndef ____on_track_signal__
#define ____on_track_signal__

//Standard Includes
#include <iostream>
#include <vector>

//Custom Includes
#include "Data.h"
#include "ConfigReader.h"
#include "Track.h"
#include "trackfunctions.h"
#include "langaus.h"
#include "Alignment.h"
#include "TProfile.h"
#include "TF1.h"
#include "hitCollection.h"

//ROOT Includes
#include <TH1.h>
#include <TLine.h>


class onTrackCluster : public hitCollection {
private:
    Data mydata;
    float charge_array[5];
    float charge_5strip_array[5];

public:
    void make_tree(std::string, std::string, std::vector<float>);
    
    void readtree(std::string);
        
    void get_entry(int i_entry);
    
    static const int maximumClusterNeighbours;
};

std::vector<float> get_cut(Config&, float treshold = 0.4, float ycut=5);

#endif /* defined(____on_track_signal__) */
