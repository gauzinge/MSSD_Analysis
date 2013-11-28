//
//  pedenoise.h
//  
//
//  Created by Georg Auzinger on 07.06.13.
//
//

#ifndef _pedenoise_h
#define _pedenoise_h

#include <stdio.h>
#include "Data.h"
//ROOT
#include "TGraph.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TMath.h"

#include <string>
#include <vector>


void get_pede_noise(std::string datafilename, bool cm, float hitrejectcut, std::string algorithm, int n_cm_loops, bool write);

void calibrate_event(std::string datafilename, bool cm, float hitrejectcut, std::string algorithm);


#endif
