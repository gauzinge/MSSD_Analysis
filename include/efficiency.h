#ifndef efficiency_h_
#define efficiency_h_

#include "Cluster.h"
#include "hitMatch.h"
#include "hitCollection.h"
#include "Data.h"
#include "Alignment.h"
#include "Track.h"
#include "ConfigReader.h"
#include "trackfunctions.h"
#include "langaus.h"
#include "onTrackCluster.h"

#include <TROOT.h>
#include "TStyle.h"
#include <TLegend.h>
#include <TGraph.h>
#include <vector>
#include <TFile.h>
#include <TProfile2D.h>
#include <TH2D.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <math.h>

void calculate_efficiency(Config& myconfig, std::vector<float> resolution_vector);

void calculate_efficiency_alternative(Config& myconfig);
	
#endif