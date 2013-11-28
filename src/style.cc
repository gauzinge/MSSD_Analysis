//
//  style.cpp
//  
//
//  Created by Georg Auzinger on 29.04.13.
//
//

#include "style.h"

void set_plotstyle() {
    
	// gStyle->SetPadLeftMargin(0.15);
// 	gStyle->SetPadBottomMargin(0.15);
// 	gStyle->SetPadRightMargin(0.15);
	
    gStyle->SetOptStat(1000001110);
    gStyle->SetOptFit(0011);
    
    gStyle->SetFrameLineWidth(3);
    gStyle->SetLabelFont(42,"xyz");
    gStyle->SetLabelSize(0.05,"xyz");
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetTitleSize(0.05,"xyz");
    gStyle->SetTitleFont(62,"xyz");
    gStyle->SetTitleOffset(1,"yz");
    gStyle->SetFuncColor(2);
    gStyle->SetFuncWidth(3);
    
    gStyle->SetFillStyle(0);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetMarkerSize(2);
    gStyle->SetCanvasColor(0);
//    gStyle->cd();
}