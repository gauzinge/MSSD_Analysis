//
//  ResultPlotter.h
//  
//
//  Created by Georg Auzinger on 16.11.12.
//
//

#ifndef ____ResultPlotter__
#define ____ResultPlotter__

#include <iostream>
#include <fstream>
#include <math.h>

//ROOT includes
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TStyle.h"

//Custom includes
#include "Settings.h"
#include "DetCurve.h"
#include "Detector.h"
#include "trackfunctions.h"


class ResultPlotter{
private:
    std::map<DetCurve, TGraphErrors*> curve_map;
	
    float xmin, xmax, ymin, ymax;
    std::string measurement;
    std::string collection;
public:
    ResultPlotter(std::string, std::string, Settings, double chi2cut);
    ~ResultPlotter();
    void draw_graphs(std::string settingfilename);
    void export_data(std::string settingfilename);
	
	std::map<DetCurve, TGraphAsymmErrors*> make_bandgraphs(std::map <DetCurve, TGraphErrors*>& curves, bool);
};

#endif /* defined(____ResultPlotter__) */
