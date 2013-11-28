//
//  Settings.h
//  
//
//  Created by Georg Auzinger on 16.11.12.
//
//

#ifndef ____Settings__
#define ____Settings__

#include <iostream>

//C++ includes
#include <string>
#include <vector>
#include <set>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

//ROOT
#include <TColor.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TLatex.h>

enum unit {ELECTRONS_TICKS = 0, ELECTRONS_THICK, ELECTRONS_REF, ADC};
enum collection {ONTRACKCLUSTERS = 0, TRACKHITS, PEAKDISTANCE};

class Settings{
private:
    int nfiles;
    int case_switch;
    //0... names and regions provided
    //1... names but no specific regions provided
    //2... neither names nor specific regions provided
public:
    //PROCESS settings
    bool pedenoise;
    bool clusteralign;
    bool trackhits;
    bool ontrackclusters;
    
    //PLOT settings
    int unit; // chargeunit;
    int collection;
        //Trackhit plots
    bool interstrip;
    bool efficiencymap;
    bool chargemap;
    bool resolution;
    bool clusterwidth;
        //Data plots
    bool signalfluct;
    bool noisereg;
    bool noiseprofile;
    bool datafluct;
    bool efficiency;
    
    //COMPARE settings
    std::set<std::string> measurements;
    std::set<std::string> names;
    std::set<std::string> doping;
    std::set<std::string> materials;
    std::set<int> thickness;
    std::set<std::string> particle;
    std::set<float> fluence;
    std::map<std::string,std::vector<int> > regions_for_names;
    int xRange_low;
    int xRange_high;
    std::map<std::string,float> scaling;
    std::vector<std::string> filelist;
	
	//Binary settings
	std::set<std::string> algorithm;
	
	//Common settings
    std::set<int> regions;
	    
    //FUNCTIONS
    void read_settings_compare(std::string filename="comparisons/settings.txt");
    void read_settings_plot(std::string);
    void read_settings_process(std::string);
    void read_settings_binary(std::string);

    void read_scales(std::string scalefilename="preferences/scalefactors.txt");
    void read_directory(std::string);
};

#endif /* defined(____Settings__) */
