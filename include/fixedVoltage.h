//
//  fixedVoltage.h
//  
//
//  Created by Georg Auzinger on 14.01.13.
//
//

#ifndef ____fixedVoltage__
#define ____fixedVoltage__

#include <iostream>

//ROOT includes
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TMath.h"
#include "TStyle.h"

//Custom includes
#include "Settings.h"
#include "DetCurve.h"
#include "Detector.h"
#include "trackfunctions.h"

class fixedVoltage{
private:
    std::map<DetCurve, TGraphErrors*> curve_map;
    float xmin, xmax, ymin, ymax;
    std::string measurement;
    std::string collection;
    int Voltage;
public:
    fixedVoltage(std::string, std::string, Settings, double chi2cut);
    ~fixedVoltage();
    void make_historgram(std::string settingfilename, int voltage);
	void set_properties_histo(DetCurve curve, TH1D*& graph);
};

Color_t LibreColor(const unsigned int& plotIndex);


#endif /* defined(____FluencePlotter__) */
