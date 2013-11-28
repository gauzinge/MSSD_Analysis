//
//  Alignment.cc
//  
//
//  Created by Georg Auzinger on 19.06.12.
//  Copyright 2012 __AlignCompanyName__. All rights reserved.
//
#ifndef Alignment_h
#define Alignment_h


#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH1F.h"
#include <TH2D.h>
#include "TF1.h"
#include "TGraph.h"
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TMath.h>
#include <TProfile.h>
#include <TLine.h>
#include <string.h>

#include "ConfigReader.h"
#include "Cluster.h"
#include "Track.h"
#include "hitMatch.h"
#include "trackfunctions.h"

class Alignment {
private:
    TFile* AlignmentFile;
    TTree* AlignmentData;
    Cluster Aligncluster;
    Track Aligntrack;
    std::string datafilename;
    std::string trackfilename;
    
public:
    
    double angle;
    double x_shift;
	double y_shift;
    double DUT_zPosition;
    int n_events;
    
    Alignment(std::string dataf, std::string trackf);
    
    ~Alignment();
    
    double get_fitrange(TH1D* h);
    
    void do_alignment_angular(Config& myconfig, int n_of_events, bool draw, double z_position=0);

    void do_alignment_x(Config& myconfig, int n_of_events, bool draw);
    
	void do_alignment_z(Config& myconfig, int n_of_events, bool draw);
	
    // void do_alignment_y(int n_of_events, bool draw);
             
    void save_alignment();

    void get_alignment(std::string filename);
      
    void align_sensor(Config& myconfig, int howmanyevents, bool draw = false); 
};

#endif

