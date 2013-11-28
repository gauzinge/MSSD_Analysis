//
//  annealing.cc
//  
//
//  Created by Georg Auzinger on 04.12.12.
//
//

#include <stdio.h>
#include <stdlib.h>
#include "TROOT.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLegend.h"
#include "TMath.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include <iostream>
#include <fstream>

double tauI(double T){ // min
    return 1./TMath::Exp(34.1-12.9e3/T); // fit to data in table 5.2
}

double beta(){ // A/cm
    return 3.07e-18;
}

double alphaI(){ // A/cm
    return 1.23e-17;
}

double alpha0(double T){ // A/cm
    return (-8.9e-17 + 4.6e-14/T); // fit to data in table 5.2
}

Double_t alphaFunction(Double_t* x, Double_t* par){ // Eq. 5.4 from M.Moll PhD Thesis
    Float_t t = x[0];
    double a = alphaI() * TMath::Exp( -t / tauI(par[0]) ) + alpha0(par[0]) - beta() * TMath::Log(t/1.);
    return a;
}

double eq_at_RT(double time, double T) {
    double alpha=0;
    
    TF1 *f1 = new TF1("alpha",alphaFunction,0,1e7,1);
    f1->SetParameter(0, T+273.);
    f1->SetNpx(100000);
    f1->Update();
//    std::cout << "The input time is " << time << " and the temperature is: " << T << std::endl;
    
    alpha = f1->Eval(time);
    time = f1->GetX(alpha);
//    std::cout << "Time at " << T << " to reach alpha=" << alpha << " is " << time << " min." << std::endl;
    f1->SetParameter(0, 294.);
    f1->SetNpx(100000);
    f1->Update();
    
    double time21 = f1->GetX(alpha,0.,0.,1e-10,1000);
//    double alpha21 = f1->Eval(time21);
//    std::cout << "Eq. time at 21 degrees to reach " << alpha21 << " is " << time21 << std::endl;
    return time21;
}