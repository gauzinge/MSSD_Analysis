//
//  CollectionFunctions.cc
//  
//
//  Created by Georg Auzinger on 10.07.12.

#ifndef CollectionFunctions_h
#define CollectionFunctions_h

//STANDARD
#include <stdio.h>
#include <iostream>
#include <string>
#include <cmath>
//CUSTOM
#include "Trackhit.h"
#include "ConfigReader.h"
#include "langaus.h"
#include "onTrackCluster.h"
#include "hitCollection.h"

//ROOT
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TFile.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TROOT.h>
#include <TLegend.h>
#include "TStyle.h"
#include <TLatex.h>
#include <TF1.h>
#include <TGraphErrors.h>

double get_fitrange(TH1D* h);

TLatex* draw_peak(double, double);

std::vector<float> get_resolution(Config&, int);

void get_hitmap(Config&, int);

void get_chargemap(Config&, int, int);

void get_landau_region(Config&, int, int);

void clusterwidth(Config&, int);

void get_eta_trackhit (Config&, int);

void interstrip_position(Config&, int);

#endif


