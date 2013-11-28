//
//  Detector.h
//  
//
//  Created by Georg Auzinger on 13.11.12.
//
//

#ifndef ____Detector__
#define ____Detector__

#include <iostream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObject.h>

class Detector{
private:
    TFile* detectorfile;
    TTree* detectortree;
public:
    std::string name;
    std::string material;
    std::string doping;
    int thickness;
    float fluence;
    std::string particle;
    int annealing;
    int voltage;
    
    std::vector<float> onTrackCluster_resolution;
    std::vector<float> Trackhit_resolution;
    
    std::vector<float> onTrackCluster_snr;
    std::vector<float> onTrackCluster_snr_error;
    std::vector<float> onTrackCluster_signal;
    std::vector<float> onTrackCluster_signal_error;
	std::vector<float> onTrackCluster_signal_chisq;
	std::vector<float> onTrackCluster_signal_ndf;
    std::vector<float> onTrackCluster_width;

    std::vector<float> Trackhit_snr;
    std::vector<float> Trackhit_snr_error;
    std::vector<float> Trackhit_signal;
    std::vector<float> Trackhit_signal_error;
	std::vector<float> Trackhit_signal_chisq;
    std::vector<float> Trackhit_signal_ndf;
	std::vector<float> Trackhit_width;

    std::vector<float> PeakDistance_snr;
    std::vector<float> PeakDistance_snr_error;
    std::vector<float> PeakDistance_signal;
    std::vector<float> PeakDistance_signal_error;
	std::vector<float> PeakDistance_signal_chisq;
	std::vector<float> PeakDistance_signal_ndf;

    std::vector<float> noise;
    std::vector<float> efficiency;
    std::vector<float> occupancy;
	
    Detector();
    ~Detector();
    
    void write_file(std::string);
    void read_file(std::string);
};
#endif /* defined(____Detector__) */
