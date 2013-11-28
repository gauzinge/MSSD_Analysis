//
//  chi2dist.h
//  
//
//  Created by Georg Auzinger on 16.11.12.
//
//

#ifndef ____chi2dist__
#define ____chi2dist__

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
#include "TH1.h"

//Custom includes
#include "Settings.h"
#include "DetCurve.h"
#include "Detector.h"
#include "trackfunctions.h"

double get_chi2(std::string, std::string, Settings, double);
   

#endif /* defined(____chi2dist__) */
