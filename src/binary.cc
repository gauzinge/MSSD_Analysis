//
//  binary.cc
//  
//
//  Created by Georg Auzinger on 01.11.12.
//
//


#include <stdio.h>
#include <iostream>

//Custom
#include "Functions.h"
#include "Alignment.h"
#include "Trackhit.h"
#include "Functions.h"
#include "CollectionFunctions.h"
#include "Data.h"
#include "ConfigReader.h"
#include "style.h"
#include "BinaryCluster.h"
#include "Settings.h"

//ROOT
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"


const int adc_max_threshold = 20;

void syntax(char* progname) {
    std::cout << "Syntax: " << progname << " Configfile" << std::endl;
}

void binary_clustering(std::string config) 
{
    set_plotstyle();
	
	Config myconfig;
    myconfig.readfile(config);
    myconfig.init_resultfile();
	
	Settings mysettings;
	mysettings.read_settings_binary(myconfig.directory + "preferences/binary_settings.txt");
	
    get_y_cuts(myconfig);
	
    std::cout << "[GENERAL] Running binary Clustering on " << myconfig.detname << " with Treshold " << adc_max_threshold << " ... " << std::endl;


    draw_result(myconfig, mysettings, adc_max_threshold);
    myconfig.close_resultfile();
}

void StandaloneApplication(int argc, char** argv) {
    if (argc<2) {
        syntax(argv[0]);
    }
	char* config=argv[1];
	binary_clustering(config);
}

int main(int argc, char** argv) {
    gROOT->Reset();
    TApplication binaryapp("Root Application", &argc, argv);
    StandaloneApplication(binaryapp.Argc(), binaryapp.Argv());
    binaryapp.Run();
    return 0;
}