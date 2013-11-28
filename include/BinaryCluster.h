//
//  BinaryCluster.h
//  
//
//  Created by Georg Auzinger on 17.01.13.
//
//

#ifndef ____BinaryCluster__
#define ____BinaryCluster__

#include <sys/stat.h>
#include <map>
#include <iostream>
#include "Track.h"
#include "Data.h"
#include "hitCollection.h"
#include "onTrackCluster.h"
#include "ConfigReader.h"
#include "trackfunctions.h"
#include "Alignment.h"
#include "Detector.h"

#include <TROOT.h>
#include "TStyle.h"
#include <TColor.h>
#include <TLegend.h>
#include <TLine.h>
#include <TGraph.h>
#include <vector>
#include <TFile.h>
#include <TProfile.h>
#include <TH1D.h>
#include <TH1I.h>
#include <TF1.h>
#include <TGaxis.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include "Settings.h"

struct Curve{
	int region;
	std::string algorithm;
	Curve(int, std::string);
	~Curve();
    bool operator<(const Curve& ) const;
	void set_properties(TGraph*&);
};

struct Result{
    std::vector<double> resolution_bary;
    std::vector<double> resolution_binary12;
    std::vector<double> resolution_binary1;
    std::vector<double> resolution_leading;
    
    std::vector<double> clusterwidth;
    std::vector<double> global_efficiency;
    std::vector<double> noise_occupancy;
};

struct resolution_noise {
	std::vector<float> resolution;
	std::vector<float> noise;
};
float get_calibfactor(Config&);

resolution_noise read_resolution_noise(Config&);
	
Result find_cluster_raw(Config&, float);

void draw_result(Config&, Settings mysettings, int max_threshold);

Color_t LibreColor(const unsigned int&);
#endif /* defined(____BinaryCluster__) */


