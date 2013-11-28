//
//  compare.cc
//  
//
//  Created by Georg Auzinger on 14.11.12.
//
//
#ifndef __CINT__
//Standard includes
#include <stdio.h>

//ROOT includes
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#endif

//Custom includes
#include "Settings.h"
#include "Detector.h"
#include "DetCurve.h"
#include "style.h"
#include "ResultPlotter.h"
#include "resolution_scatterplot.h"
#include "FluencePlotter.h"
#include "fixedVoltage.h"
#include "chi2dist.h"

std::string collection = "PeakDistance";
// std::string collection = "onTrackCluster";
// std::string collection = "Trackhit";

void compare(std::string settingfile, std::string collection = "PeakDistance") {

    //actual Code
    set_plotstyle();
	
    Settings mysettings;
    mysettings.read_settings_compare(settingfile.c_str());
//    mysettings.read_scales("preferences/scalefile.txt");

    mysettings.read_directory("Resultfiles/032011");
    mysettings.read_directory("Resultfiles/052012");
    mysettings.read_directory("Resultfiles/092011");
    mysettings.read_directory("Resultfiles/102012");
	
	//get chi2 of all affected Data
    std::cout << "Building Chi2 distribution!" << std::endl;
	double chi2multiple = 3;
	double chi2cut = get_chi2(collection.c_str(),"Signal", mysettings, chi2multiple);
	std::cout << "The Chi2 Cut is " << chi2cut << std::endl;
	std::cout << "The Collection is " << collection << std::endl;
	
    if (mysettings.measurements.find("SNR") != mysettings.measurements.end()) {
        std::cout << "Plotting SNR" << std::endl;
        ResultPlotter snrplotter(collection.c_str(),"SNR", mysettings, 2*chi2cut);
        snrplotter.draw_graphs(settingfile);
    }
    if (mysettings.measurements.find("Signal") != mysettings.measurements.end()) {
        		
        std::cout << "Plotting Signal" << std::endl;
        ResultPlotter snrplotter(collection.c_str(),"Signal", mysettings, 2*chi2cut);
        snrplotter.draw_graphs(settingfile);
		
  		fixedVoltage myvoltage(collection.c_str(),"Signal", mysettings, 2*chi2cut);
		myvoltage.make_historgram(settingfile,500);
		
//        FluencePlotter snrfluenceplotter(600, collection.c_str(),"Signal", mysettings, chi2cut);
//        snrfluenceplotter.draw_graphs(settingfile);
    }
    if (mysettings.measurements.find("Resolution") != mysettings.measurements.end()) {
        std::cout << "Plotting Resolution" << std::endl;
        ResultPlotter snrplotter("Trackhit","Resolution", mysettings,10*chi2cut);
        snrplotter.draw_graphs(settingfile);
		
		resolution_scatterplot myscatterplot("Trackhit","Resolution",mysettings);
		myscatterplot.draw_graphs(settingfile);
    }
    if (mysettings.measurements.find("Noise") != mysettings.measurements.end()) {
        std::cout << "Plotting Noise" << std::endl;
        ResultPlotter snrplotter(collection.c_str(),"Noise", mysettings, 10*chi2cut);
        snrplotter.draw_graphs(settingfile);
    }
    if (mysettings.measurements.find("Width") != mysettings.measurements.end()) {
        std::cout << "Plotting Width" << std::endl;
        ResultPlotter snrplotter(collection.c_str(),"Width", mysettings, 2*chi2cut);
        snrplotter.draw_graphs(settingfile);
    }
    if (mysettings.measurements.find("Efficiency") != mysettings.measurements.end()) {
        std::cout << "Plotting Efficiency" << std::endl;
        ResultPlotter snrplotter(collection.c_str(),"Efficiency", mysettings, 10*chi2cut);
        snrplotter.draw_graphs(settingfile);
    }
}

void syntax(char* progname) {
     std::cout << "Syntax: " << progname << " Configfile Collection (Default is onTrackCluster)" << std::endl;
}

//Standalone APP
#ifndef __CINT__
void StandaloneApplication(int argc, char** argv) {
    if (argc!=2) {
        syntax(argv[0]);
    }
    char* config=argv[1];
    compare(config, collection);
}

int main(int argc, char** argv) {
    gROOT->Reset();
    TApplication app("Root Application", &argc, argv);
    StandaloneApplication(app.Argc(), app.Argv());
    app.Run();
    return 0;
}
#endif
