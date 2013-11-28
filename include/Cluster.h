//
//  Cluster.cc
//  
//
//  Created by Georg Auzinger on 21.06.12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
#ifndef Cluster_h
#define Cluster_h
#include "Data.h"
#include <TCanvas.h>
#include <TGraph.h>
#include <list>
#include <cmath>
#include <vector>
#include "trackfunctions.h"


class Cluster {
private:
    std::vector<bool> found_cluster;
    TFile* HitFile;
    TTree* ClusterData;
    Data mydata;
    int seed; //in SNR Units
    int neighbor;
    int total;
    int w_cut;
    std::vector<int> first_st;
    std::vector<float> clu_charge;
    std::vector<float> clu_noise;
    std::vector<float> clu_snr;
    std::vector<float> clu_significance;
    std::vector<int> clu_width;
    std::vector<double> cog_SeedStrip;
    double cog_strip;
    float charge_array[15];
    std::vector< std::vector<float> >  found_vector;


public:
    int first_strip;
    int width;
    float charge;
    std::vector<float> charge_vector;
    float noise;
    float snr;
    float significance;
    double xPosition;
    double zPosition;
    int EventNumber;
    int region;
    
        
    Cluster();
    
    Cluster(int se, int neig, int to, int w);
    
    ~Cluster();
    

    void make_tree(std::string filename); 
    
    void readtree(std::string filename);
    
    unsigned int get_n_entries();
    
    void get_entry(int i_entry);
    
    
    void find_cluster(int total_cut, int width_cut); 
    
    
    int hitmaker(); 

    
};

#endif
