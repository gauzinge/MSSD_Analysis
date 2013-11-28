#ifndef Functions_h
#define Functions_h

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

void get_y_cuts(Config&);

void get_signal_snr(Config&, int, int);

void get_noiseprofile (Config&, int unit);

void get_noise (Config&, int unit);

void check_data(Config&, int unit);

#endif


