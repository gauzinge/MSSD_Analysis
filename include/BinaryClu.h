//
//  BinaryClu.h
//  
//
//  Created by Georg Auzinger on 01.11.12.
//
//

#ifndef ____BinaryClu__
#define ____BinaryClu__
#include "Data.h"
#include "Alignment.h"
#include "Track.h"
#include "ConfigReader.h"
#include "trackfunctions.h"

#include <TROOT.h>
#include "TStyle.h"
#include <TLegend.h>
#include <TGraph.h>
#include <vector>
#include <TFile.h>
#include <TProfile.h>

void binary_cluster(Config&, int max_treshold, std::vector<int>, int calibration = 1);


#endif /* defined(____BinaryClu__) */
