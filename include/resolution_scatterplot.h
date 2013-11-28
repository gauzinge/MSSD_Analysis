//
//  ResultPlotter.h
//  
//
//  Created by Georg Auzinger on 16.11.12.
//
//

#ifndef ____resolution_scatterplot__
#define ____resolution_scatterplot__

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


class resolution_scatterplot{
private:	
    std::string measurement;
    std::string collection;
public:
    resolution_scatterplot(std::string, std::string, Settings);
    ~resolution_scatterplot();
    void draw_graphs(std::string settingfilename);
	TMultiGraph* multigraph;
};

#endif /* defined(____ResultPlotter__) */
