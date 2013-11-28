//
//  hitCollection.h
//  
//
//  Created by Georg Auzinger on 17.12.12.
//
//

#ifndef ____hitCollection__
#define ____hitCollection__
// my includes
#include "Track.h"
#include "Cluster.h"
#include "Data.h"
#include "Alignment.h"

//ROOT includes
#include <TFile.h>
#include <TTree.h>

#include <iostream>

class hitCollection {
protected:
    std::string datafilename;
    std::string trackfilename;
    Track mytrack;
//    Cluster mycluster;
    
    TFile* file;
    TTree* tree;
    
    float cut[13];
    
public:
    std::vector<float> cut_vector; //in ADCs
    int first_strip;
    int width;
    float charge;
    float noise;
    float snr;
    double xPosition;
    double zPosition;
    double xTrack;
    double yTrack;
    int EventNumber;
    int region;
    std::vector<float> charge_vector;
    std::vector<float> charge_5strips;
    
    hitCollection();
    
    virtual ~hitCollection();
    
    unsigned int get_n_entries();

    virtual void make_tree(std::string dataf, std::string trackf, std::vector<float>) { std::cout << "ERROR: this should never get executed!" << std::endl;} ;
    
    virtual void readtree(std::string)  { std::cout << "ERROR: this should never get executed!" << std::endl;} ;
        
    virtual void get_entry(int i_entry) { std::cout << "ERROR: this should never get executed!" << std::endl;} ;
    
};

#endif /* defined(____hitCollection__) */
