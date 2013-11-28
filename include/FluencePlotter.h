//
//  FluencePlotter.h
//  
//
//  Created by Georg Auzinger on 14.01.13.
//
//

#ifndef ____FluencePlotter__
#define ____FluencePlotter__

#include <iostream>

//ROOT includes
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TLegend.h"

//Custom includes
#include "Settings.h"
#include "DetCurve.h"
#include "Detector.h"
#include "trackfunctions.h"

class FluencePlotter{
private:
    std::map<DetCurve, TGraphErrors*> curve_map;
    float xmin, xmax, ymin, ymax;
    std::string measurement;
    std::string collection;
    int Voltage;
public:
    FluencePlotter(int, std::string, std::string, Settings, double chi2cut);
    ~FluencePlotter();
    void draw_graphs(std::string settingfilename);
};


#endif /* defined(____FluencePlotter__) */
