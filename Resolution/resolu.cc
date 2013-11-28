//
//  resolu.cc
//  
//
//  Created by Georg Auzinger on 15.03.13.
//
//
#include <math.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <dirent.h>
//ROOT
#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TH1D.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>

const double PI  =3.141592653589793238462;

double get_sigma(int position, TFitResultPtr res) {
    return TMath::Sqrt(res.Get()->ParError(0)*res.Get()->ParError(0)+res.Get()->ParError(1)*res.Get()->ParError(1)*position*position+2*(res.Get()->GetCovarianceMatrix()(0,1)*position));
}

struct event{
    TFile* file;
    TTree* hits;
    int eventno;
    double x[8];
    double y[8];
    double z[8];
    void readtree(std::string);
    unsigned int get_n_entries();
    void get_entry(int);
};

double get_fitrange(TH1D* h) {
    double maxval = h->GetMaximum();
    double firstbin = h->GetBinCenter(h->FindFirstBinAbove(maxval/2));
    double lastbin = h->GetBinCenter(h->FindLastBinAbove(maxval/2));
    double fwhm = (lastbin-firstbin);
    return (2*fwhm/2);
}

std::vector<std::string> read_directory(std::string folder) {
    std::cout << "Reading Directory: " << folder << std::endl;
    std::vector<std::string> filelist;
    std::string foldername = folder;
    DIR           *d;
    struct dirent *dir;
    //    std::vector<std::string> filelist;
    int counter=0;
    d = opendir(foldername.c_str());
    if (d) {
        //        std::cout << "The Foldername is: " << foldername << "/" << std::endl;
        while ((dir = readdir(d)) != NULL)
        {
            if ( strlen(dir->d_name) > 20 ) {
                std::string name = folder;  //used to be "Exported"
                name += "/";
                name += dir->d_name;
                std::string extension = ".root";
                if (name.find(extension) != std::string::npos) {
                    filelist.push_back(name);
                    counter++;
                    std::cout<< "Found file: " << name << std::endl;
                }
            }
        }
        closedir(d);
    } else std::cout << "[ERROR] The directory " << foldername << " does not exist!" << std::endl;
    return filelist;
}

void event::readtree(std::string filename) {
    file = new TFile(filename.c_str());
//    if (file!-NULL) {
//        file->Close();
//        delete file;
//    }
    hits = new TTree();
    file->GetObject("TelescopeHit",hits);
    
    TBranch* event_no = hits->GetBranch("EventNo");
    
    TBranch* x0 = hits->GetBranch("x0");
    TBranch* y0 = hits->GetBranch("y0");
    TBranch* z0 = hits->GetBranch("z0");
    
    TBranch* x1 = hits->GetBranch("x1");
    TBranch* y1 = hits->GetBranch("y1");
    TBranch* z1 = hits->GetBranch("z1");
    
    TBranch* x2 = hits->GetBranch("x2");
    TBranch* y2 = hits->GetBranch("y2");
    TBranch* z2 = hits->GetBranch("z2");

    TBranch* x3 = hits->GetBranch("x3");
    TBranch* y3 = hits->GetBranch("y3");
    TBranch* z3 = hits->GetBranch("z3");
    
    TBranch* x4 = hits->GetBranch("x4");
    TBranch* y4 = hits->GetBranch("y4");
    TBranch* z4 = hits->GetBranch("z4");

    TBranch* x5 = hits->GetBranch("x5");
    TBranch* y5 = hits->GetBranch("y5");
    TBranch* z5 = hits->GetBranch("z5");

    TBranch* x6 = hits->GetBranch("x6");
    TBranch* y6 = hits->GetBranch("y6");
    TBranch* z6 = hits->GetBranch("z6");

    TBranch* x7 = hits->GetBranch("x7");
    TBranch* y7 = hits->GetBranch("y7");
    TBranch* z7 = hits->GetBranch("z7");
    
    event_no->SetAddress(&this->eventno);
    
    x0->SetAddress(&this->x[0]);
    y0->SetAddress(&this->y[0]);
    z0->SetAddress(&this->z[0]);

    x1->SetAddress(&this->x[1]);
    y1->SetAddress(&this->y[1]);
    z1->SetAddress(&this->z[1]);
    
    x2->SetAddress(&this->x[2]);
    y2->SetAddress(&this->y[2]);
    z2->SetAddress(&this->z[2]);

    x3->SetAddress(&this->x[3]);
    y3->SetAddress(&this->y[3]);
    z3->SetAddress(&this->z[3]);
    
    x4->SetAddress(&this->x[4]);
    y4->SetAddress(&this->y[4]);
    z4->SetAddress(&this->z[4]);
    
    x5->SetAddress(&this->x[5]);
    y5->SetAddress(&this->y[5]);
    z5->SetAddress(&this->z[5]);
    
    x6->SetAddress(&this->x[6]);
    y6->SetAddress(&this->y[6]);
    z6->SetAddress(&this->z[6]);
    
    x7->SetAddress(&this->x[7]);
    y7->SetAddress(&this->y[7]);
    z7->SetAddress(&this->z[7]);
}

unsigned int event::get_n_entries() {
    return static_cast<unsigned int>(hits->GetEntries());
}

void event::get_entry(int i_entry){
    hits->GetEntry(i_entry);
//    std::cout << "Debug Output: x0" << this->x[0] << " y0 " << this->y[0] << " z0 " << this->z[0] << std::endl;
//    if (i_entry%1000 == 0) std::cout << i_entry << " Events read! " << std::endl;
}

double* compute(double* errors) {
    
    for (int i =0; i<8; i++) {
        std::cout << "The a-priori Error is " << errors[i] *1000 << " micron" << std::endl;
    }

    int nbins=200;
    double minbin=-.20;
    double maxbin=.20;
    
    std::vector<TH1D*> sigmahistoX;
    std::vector<TH1D*> sigmahistoY;
    std::vector<TH1D*> residualhistoX;
    std::vector<TH1D*> residualhistoY;
	std::vector<TH1D*> sigmaDUTX;
	std::vector<TH1D*> sigmaDUTY;
	
    
    int xplanes[4]={0,2,5,7};
    int yplanes[4]={1,3,4,6};
	int xpositions[4] = {-300,-220,220,300};
	int ypositions[4] = {-260,-180,180,260};
    
    for (int i =0; i<4; i++) {
        std::stringstream ssx;
        ssx << xplanes[i];
        std::string sigmaHistoNameX="XSigma_plane_" + ssx.str();
        std::string residualHistoNameX="XResiduals_plane_"+ssx.str();
        std::stringstream ssy;
        ssy << yplanes[i];
        std::string sigmaHistoNameY="YSigma_plane_"+ssy.str();
        std::string residualHistoNameY="YResiduals_plane_"+ssy.str();
        TH1D* dummy1 = new TH1D(sigmaHistoNameX.c_str(),sigmaHistoNameX.c_str(),nbins,-0.1,0.1);
        TH1D* dummy3 = new TH1D(sigmaHistoNameY.c_str(),sigmaHistoNameY.c_str(),nbins,-0.1,0.1);
        TH1D* dummy2 = new TH1D(residualHistoNameX.c_str(),residualHistoNameX.c_str(),nbins,minbin,maxbin);
        TH1D* dummy4 = new TH1D(residualHistoNameY.c_str(),residualHistoNameY.c_str(),nbins,minbin,maxbin);
        sigmahistoX.push_back(dummy1);
        sigmahistoY.push_back(dummy3);
        residualhistoX.push_back(dummy2);
        residualhistoY.push_back(dummy4);
    }
	
    for (int i =0; i<4; i++) {
        std::stringstream ssx;
        ssx << i;
        std::string sigmaHistoNameX="sigmaX_DUT_" + ssx.str();
        std::stringstream ssy;
        ssy << i;
        std::string sigmaHistoNameY="sigmaY_DUT_"+ssy.str();
        TH1D* dummy1 = new TH1D(sigmaHistoNameX.c_str(),sigmaHistoNameX.c_str(),nbins,-0.1,0.1);
        TH1D* dummy3 = new TH1D(sigmaHistoNameY.c_str(),sigmaHistoNameY.c_str(),nbins,-0.1,0.1);
        sigmaDUTX.push_back(dummy1);
        sigmaDUTY.push_back(dummy3);
    }
	
    double sigma_dut_x[4];
    double sigma_dut_y[4];
	double dut_zposition[4] = {140,60,-20,-100};
	// double dut_zposition[4] = {20,0,0,0};
	
	TGraph* resolution_x = new TGraph();
	TGraph* resolution_y = new TGraph();
	
    std::vector<std::string> filelist = read_directory("Hits");
    
    for (std::vector<std::string>::iterator files = filelist.begin(); files != filelist.end(); files++) {
        event myevent;
        myevent.readtree(*files);
        std::cout << "Loading file " << *files << std::endl;
        for (int exc=0; exc<4; exc++) {  //loop over excluded planes
            
            int excludedX=xplanes[exc];
            int excludedY=yplanes[exc];
            
            std::cout << "Excluding plane " << excludedX << " in X direction and " << excludedY << " in Y direction!" << std::endl;
            
            for (unsigned int evt=0; evt<myevent.get_n_entries(); evt++) { //loop over events
                myevent.get_entry(evt);
                double residualX=0;
                double residualY=0;
                
                //here i have all hits per event
                TGraphErrors* grx = new TGraphErrors();
                TGraphErrors* gry = new TGraphErrors();
                //fill graphs with all planes but excluded
                for (int j=0; j<4; j++) {
                    if (myevent.x[xplanes[j]]!=0 && myevent.y[xplanes[j]]!=0 && myevent.z[xplanes[j]]!=0) {
                        if (xplanes[j]!=excludedX){
                            int np = grx->GetN();
                            grx->SetPoint(np,myevent.z[xplanes[j]],myevent.x[xplanes[j]]);
                            grx->SetPointError(np,0,errors[xplanes[j]]);
                        } else if (xplanes[j] == excludedX ){
                            residualX=myevent.x[xplanes[j]];
                        }
                    }
                    if (myevent.x[yplanes[j]]!=0 && myevent.y[yplanes[j]]!=0 && myevent.z[yplanes[j]]!=0) {
                        if (yplanes[j]!=excludedY) {
                            int np = gry->GetN();
                            gry->SetPoint(np,myevent.z[yplanes[j]],myevent.y[yplanes[j]]);
                            gry->SetPointError(np,0,errors[yplanes[j]]);
                        } else if (yplanes[j] == excludedY ){
                            residualY=myevent.y[yplanes[j]];
                        }
                    }
                }
                //Here the graphs are filled for each event with 1 plane excluded
                if (grx->GetN() > 1 && residualX != 0) 
				{
                TFitResultPtr resx = grx->Fit("pol1","SQ");
                    double x = resx.Get()->Parameter(0)+resx.Get()->Parameter(1)*myevent.z[excludedX];

                    double sigma_x = TMath::Sqrt(resx.Get()->ParError(0)*resx.Get()->ParError(0)+resx.Get()->ParError(1)*resx.Get()->ParError(1)*myevent.z[excludedX]*myevent.z[excludedX]+2*(resx.Get()->GetCovarianceMatrix()(0,1)*myevent.z[excludedX]));
                    sigmahistoX.at(exc)->Fill(sigma_x);
                    residualhistoX.at(exc)->Fill(residualX-x);
				}
                if (gry->GetN()>1 && residualY != 0)
				{
                TFitResultPtr resy = gry->Fit("pol1","SQ");
                double y = resy.Get()->Parameter(0)+resy.Get()->Parameter(1)*myevent.z[excludedY];

                    double sigma_y = TMath::Sqrt(resy.Get()->ParError(0)*resy.Get()->ParError(0)+resy.Get()->ParError(1)*resy.Get()->ParError(1)*myevent.z[excludedY]*myevent.z[excludedY]+2*(resy.Get()->GetCovarianceMatrix()(0,1)*myevent.z[excludedY]));
                    sigmahistoY.at(exc)->Fill(sigma_y);
                    residualhistoY.at(exc)->Fill(residualY-y);
					//here i have the y of the track, the error on that y for each event on each exclude plane
                }
            }  // end of event loop
        }  // end of exclusion loop
		
    }  // end of files loop

    //Draw

    TCanvas* residualcanvasX = new TCanvas("ResidualX","ResidualX");
    residualcanvasX->Divide(2,2);

    TCanvas* residualcanvasY = new TCanvas("ResidualY","ResidualY");
    residualcanvasY->Divide(2,2);
	
    double new_error[8];//={0,0,0,0,0,0,0,0};
	TGraph* resolutionX = new TGraph();
	TGraph* resolutionY = new TGraph();
    for (int i=0; i<4; i++) 
	{
        residualcanvasX->cd(i+1);
        residualhistoX.at(i)->Draw();

        residualcanvasY->cd(i+1);
        residualhistoY.at(i)->Draw();
        
        new_error[xplanes[i]] = residualhistoX.at(i)->GetRMS()/(TMath::Sqrt(1+(sigmahistoX.at(i)->GetMean()*sigmahistoX.at(i)->GetMean()/errors[xplanes[i]]*errors[xplanes[i]])));
		
        std::cout << "X Direction: Hit Resolution for plane " << xplanes[i] << " is " << (new_error[xplanes[i]])*1000 << " micron" << std::endl;
		
        new_error[yplanes[i]] = residualhistoY.at(i)->GetRMS()/(TMath::Sqrt(1+(sigmahistoY.at(i)->GetMean()*sigmahistoY.at(i)->GetMean()/errors[yplanes[i]]*errors[yplanes[i]])));
					
        std::cout << "Y Direction: Hit Resolution for plane " << yplanes[i] << " is " << (new_error[yplanes[i]])*1000 << " micron" << std::endl;
    }
	//Here I basically have the intrinsic sigma of all planes by saying sigma_plane = sqrt(sigma_residual^2 - sigma_track_2)
	
	
	
	std::cout << "2nd loop to calculate sigma_track at DUT positions!" << std::endl;
    for (std::vector<std::string>::iterator files = filelist.begin(); files != filelist.end(); files++) {
        event myevent;
        myevent.readtree(*files);
		
        for (unsigned int evt=0; evt<myevent.get_n_entries(); evt++) { //loop over events
            myevent.get_entry(evt);
            
            //here i have all hits per event
            TGraphErrors* grxdut = new TGraphErrors();
            TGraphErrors* grydut = new TGraphErrors();
			
            for (int j=0; j<4; j++) {
                if (myevent.x[xplanes[j]]!=0 && myevent.y[xplanes[j]]!=0 && myevent.z[xplanes[j]]!=0) {
                        int np = grxdut->GetN();
                        grxdut->SetPoint(np,myevent.z[xplanes[j]],myevent.x[xplanes[j]]);
                        grxdut->SetPointError(np,0,new_error[xplanes[j]]);
                }
                if (myevent.x[yplanes[j]]!=0 && myevent.y[yplanes[j]]!=0 && myevent.z[yplanes[j]]!=0) {
                        int np = grydut->GetN();
                        grydut->SetPoint(np,myevent.z[yplanes[j]],myevent.y[yplanes[j]]);
                        grydut->SetPointError(np,0,new_error[yplanes[j]]);
                }
            }
			//Graph with all 4 datapoints and the plane_error as from above calculation
			//now FIT
            if (grxdut->GetN() == 4) 
			{
				TFitResultPtr resxdut = grxdut->Fit("pol1","SQ");
				for (int i = 0 ; i <4 ; i++)
				{
					double sigma_x_dut = TMath::Sqrt(resxdut.Get()->ParError(0)*resxdut.Get()->ParError(0)+resxdut.Get()->ParError(1)*resxdut.Get()->ParError(1)*dut_zposition[i]*dut_zposition[i]+2*(resxdut.Get()->GetCovarianceMatrix()(0,1)*dut_zposition[i]));
					sigmaDUTX.at(i)->Fill(sigma_x_dut);
				}
			}
            if (grydut->GetN()==4)
			{
	            TFitResultPtr resydut = grydut->Fit("pol1","SQ");
				for (int i = 0 ; i <4 ; i++)
				{
	                double sigma_y_dut = TMath::Sqrt(resydut.Get()->ParError(0)*resydut.Get()->ParError(0)+resydut.Get()->ParError(1)*resydut.Get()->ParError(1)*dut_zposition[i]*dut_zposition[i]+2*(resydut.Get()->GetCovarianceMatrix()(0,1)*dut_zposition[i]));
					sigmaDUTY.at(i)->Fill(sigma_y_dut);
				}
	        }
			//Fitted and now I have sigma_x,y at the dut positions[i]
		} // event loop
		
		
		// to have sigma dut x,y as function of z
		TGraphErrors* graphx = new TGraphErrors();
		TGraphErrors* graphy = new TGraphErrors();
		
        for (int j=0; j<4; j++) {
                    int np = graphx->GetN();
                    graphx->SetPoint(np,myevent.z[xplanes[j]],0);
                    graphx->SetPointError(np,0,new_error[xplanes[j]]);
                    np = graphy->GetN();
                    graphy->SetPoint(np,myevent.z[yplanes[j]],0);
                    graphy->SetPointError(np,0,new_error[yplanes[j]]);
            }
			TFitResultPtr graphxres = graphx->Fit("pol1","SQ");
			TFitResultPtr graphyres = graphy->Fit("pol1","SQ");
			for (int i = -300 ; i <301 ; i++)
			{
				double sigma_x_dut = TMath::Sqrt(graphxres.Get()->ParError(0)*graphxres.Get()->ParError(0)+graphxres.Get()->ParError(1)*graphxres.Get()->ParError(1)*i*i+2*(graphxres.Get()->GetCovarianceMatrix()(0,1)*i));
				double sigma_y_dut = TMath::Sqrt(graphyres.Get()->ParError(0)*graphyres.Get()->ParError(0)+graphyres.Get()->ParError(1)*graphyres.Get()->ParError(1)*i*i+2*(graphyres.Get()->GetCovarianceMatrix()(0,1)*i));
				resolution_x->SetPoint(resolution_x->GetN(),i,sigma_x_dut*1000);
				resolution_y->SetPoint(resolution_y->GetN(),i,sigma_y_dut*1000);
			}
		
	}//File loop	
	
	TCanvas* dutcanvasX = new TCanvas("sigma_dut_x","sigma_dut_x");
	dutcanvasX->Divide(2,2);
	TCanvas* dutcanvasY = new TCanvas("sigma_dut_y","sigma_dut_y");
	dutcanvasY->Divide(2,2);
	
    for (int i=0; i<4; i++) 
	{
        dutcanvasX->cd(i+1);
        sigmaDUTX.at(i)->Draw();

        dutcanvasY->cd(i+1);
        sigmaDUTY.at(i)->Draw();
		
		double sigma_xPrime=sqrt(sigmaDUTX.at(i)->GetMean()*sigmaDUTX.at(i)->GetMean()*cos(45 * PI / 180)*cos(45 * PI / 180)+sigmaDUTY.at(i)->GetMean()*sigmaDUTY.at(i)->GetMean()*sin(45 * PI / 180)*sin(45 * PI / 180));
		
		std::cout << "The sigma of the Track at the DUT position " << i << " = " << dut_zposition[i] << " is: " << std::endl << "X = " << sigmaDUTX.at(i)->GetMean()*1000 << "	Y = " << sigmaDUTY.at(i)->GetMean()*1000 <<  "	xPrime = " << sigma_xPrime << std::endl;
	}
	
	TGraph* resolution_xPrime = new TGraph();
	// TLegend* noise_legend = new TLegend(0.590517,0.641949,0.900862,0.900424,"","NDC");
	TLegend* legend = new TLegend(0.590517,0.641949,0.900862,0.900424,"","NDC");
	legend->AddEntry(resolution_x,"Telescope Resolution in x Direction","l");
	legend->AddEntry(resolution_y,"Telescope Resolution in y Direction","l");
	legend->AddEntry(resolution_xPrime,"Telescope Resolution in MSSD x Direction","l");
	
	for (int i = 0 ; i < resolution_x->GetN() ; i++)
	{
		double x1,x2,y1,y2;
		resolution_x->GetPoint(i,x1,x2);
		resolution_y->GetPoint(i,y1,y2);
		
		resolution_xPrime->SetPoint(i,i-300,sqrt(x2*x2*cos(45 * PI / 180)*cos(45 * PI / 180) + y2*y2*sin(45 * PI / 180)*sin(45 * PI / 180)));
			
	}
	resolution_x->SetMarkerColor(1);
	resolution_y->SetMarkerColor(2);
	resolution_xPrime->SetMarkerColor(3);
	
	TCanvas* telescope_res =  new TCanvas("TelescopeResolution","Telescope Resolution");
	telescope_res->cd();
	resolution_x->Draw("apl");
	resolution_y->Draw("pl same");
	resolution_xPrime->Draw("pl_same");
	legend->Draw("same");
	
    double *new_e =  new_error;
    return new_e;
}

void resolu() {
    int niterations = 3;
    double error[8]={0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02};
    TGraph* plane1 = new TGraph();
    TGraph* plane2 = new TGraph();
    for (int i = 0; i<niterations; i++) {
        double *new_error = compute(error);
        for (int j =0 ; j < 8; j++) {
            error[j]=new_error[j];
        }
        plane1->SetPoint(i+1,i,error[0]);
        plane2->SetPoint(i+1,i,error[1]);
//        error =  new_error;
    }
    // TCanvas* c1 = new TCanvas("c1","Canvas1");
//     c1->cd();
//     plane1->Draw("APL");
//     plane2->Draw("PLsame");
}
