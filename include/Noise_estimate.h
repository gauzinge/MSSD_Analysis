//
//  Noise_estimate.h
//  
//
//  Created by Georg Auzinger on 11.12.12.
//
//

#ifndef ____Noise_estimate__
#define ____Noise_estimate__

#include <iostream>
#include "Cluster.h"
#include "Track.h"
#include "Trackhit.h"
#include "Alignment.h"
#include "hitMatch.h"
#include "ConfigReader.h"
#include <TMath.h>
#include <vector>
#include <TH1D.h>



std::vector<TH1D*> fill_residuals(Config&);

void fit_residuals(Config&, std::vector<TH1D*>);

std::vector<TH1F*> fill_bad(Config&, std::string, int);

void scale_distribution(Config&, std::vector<TH1F*>, std::string);

void estimate_noise(Config&, int);

#endif /* defined(____Noise_estimate__) */
