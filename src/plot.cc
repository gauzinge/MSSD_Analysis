//
//  plot.cc
//  
//
//  Created by Georg Auzinger on 04.10.12.
//
//
//Standard includes
#include <stdio.h>

//ROOT includes
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"

#include <stdio.h>
#include <iostream>
#include "Alignment.h"
#include "Trackhit.h"
#include "Functions.h"
#include "CollectionFunctions.h"
#include "Data.h"
#include "ConfigReader.h"
#include "Noise_estimate.h"
#include "onTrackCluster.h"
#include "Settings.h"
#include "efficiency.h"
#include <TStyle.h>
#include <style.h>
#include <TROOT.h>

const int ycut = 5; //distance close to the sensor edges of the MSSD to exclude in mm

void syntax(char* progname) {
    std::cout << "Syntax: " << progname << " Configfile" << std::endl;
}

int main(int argc, char* argv[]){
    if (argc!=2) {
        syntax(argv[0]);
        return -1;
    }
    set_plotstyle();
    char* config=argv[1];
    Config myconfig;
    myconfig.readfile(config);
    myconfig.init_resultfile();
    myconfig.init_plotfile();
    
    gStyle->SetCanvasDefH(800);
    gStyle->SetCanvasDefW(1280);
    gStyle->SetOptFit(1111);
    
    Settings settings;
    settings.read_settings_plot(myconfig.directory + "preferences/plot_settings.txt");
    get_y_cuts(myconfig);
    // if (settings.collection == 1) {  //  ESTIMATE NOISE
//         std::cout << std::endl << "[PLOT] Estimating Fraction of Noise Clusters" << std::endl;
//         estimate_noise(myconfig, settings.unit);
//         get_eta_trackhit(myconfig, settings.collection);
//     }
    std::cout << "[PLOT] Building Signal and Noise Distributions for PEAKDISTANCE" << std::endl;
    get_signal_snr(myconfig, 2, settings.unit); //  GET SIGNAL SNR from PEAKDISTANCE
    
    // if (settings.collection != 2) {
 //        std::cout << "[PLOT] Building Signal and SNR Distributions with Track-based Collections" << std::endl;
 //        get_landau_region(myconfig, settings.collection, settings.unit);
 //        get_snr_trackhit(myconfig, settings.collection);
 //    }
    
    //TRACKHITPLOTS
    std::cout << std::endl << "[PLOT] Creating Plots based on Tracks!" << std::endl;
	
    // if (settings.interstrip) interstrip_position(myconfig, settings.collection);
    // if (settings.chargemap) get_chargemap(myconfig, settings.collection, settings.unit);
    std::vector<float> resolution_vector = get_resolution(myconfig, settings.collection);
    if (settings.clusterwidth) clusterwidth(myconfig, settings.collection);
	
	calculate_efficiency(myconfig, resolution_vector);
    // calculate_efficiency_alternative(myconfig);
	
    //DATAPLOTS
    std::cout << std::endl << "[GENERAL] Creating Plots based on Data!" << std::endl;
	
    if (settings.noisereg) get_noise(myconfig, settings.unit);
    if (settings.noiseprofile) get_noiseprofile(myconfig, settings.unit);
    if (settings.datafluct) check_data(myconfig, settings.unit);
    
    myconfig.close_plotfile();
    myconfig.close_resultfile();
}
