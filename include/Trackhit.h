//
//  trackhits.cc
//  
//
//  Created by Georg Auzinger on 05.07.12.
//

#ifndef Trackhit_h
#define Trackhit_h

#include "Track.h"
#include "Cluster.h"
#include "Alignment.h"
#include "hitMatch.h"
#include "trackfunctions.h"
#include "hitCollection.h"



class Trackhit : public hitCollection {
private:
    Cluster mycluster;
    float charge_array[15];

    
public:
//    std::vector<float> cut_vector; //in ADCs
//    int first_strip;
//    int width;
//    float charge;
//    float noise;
//    float snr;
//    double xPosition;
//    double zPosition;
//    double xTrack;
//    double yTrack;
//    int EventNumber;
//    int region;
//    std::vector<float> charge_vector;
    
//    Trackhit ();
//    
//    ~Trackhit ();
    
    void make_tree(std::string dataf, std::string trackf, std::vector<float>);

    void readtree(std::string);
    
//    unsigned int get_n_entries();
    
    void get_entry(int i_entry); 
    
};


#endif

