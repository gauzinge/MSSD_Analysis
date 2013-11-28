//
//  langaus.h
//  
//
//  Created by Georg Auzinger on 04.10.12.
//
//

#ifndef _langaus_h
#define _langaus_h
#include "TH1.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TMatrixT.h"
#include "TFitResult.h"

double langaufun(double*, double*);

TF1 *langaufit(TH1F*, double*, double*, double*, double*, double*, double*, double*, double*, double);

TF1* langaus(TH1F*, double&, double&, double&, double&, double);


#endif
