//
//  CollectionFunctions.cc
//  
//
//  Created by Georg Auzinger on 10.07.12.

#include "CollectionFunctions.h"

std::string trackhit = "Trackhit_";
std::string ontrackcluster = "onTrackCluster_";


float get_fitrange(TH1F* h) {
    float maxval = h->GetMaximum();
    float firstbin = h->GetBinCenter(h->FindFirstBinAbove(maxval/2));
    float lastbin = h->GetBinCenter(h->FindLastBinAbove(maxval/2));
    float fwhm = (lastbin-firstbin);
    return (5*fwhm/2);
}


TLatex* draw_peak(double mpv, double mpv_error) {
    // DETNAME STRING FOR PLOTS
    std::string s;
    std::stringstream namestream;
    namestream.precision(1);
    namestream << "MPV " << std::fixed << mpv << " (+-" << mpv_error << ")"  << std::endl;
    
    s=namestream.str();
    TLatex* name=new TLatex(.58,.8,s.c_str());
    //        name->SetTextAlign(33);
    name->SetNDC(kTRUE); // <- use NDC coordinate
    name->SetTextFont(62);
    name->SetTextSize(0.055);
    return name;
}

//######
//#     # ######  ####   ####  #      #    # ##### #  ####  #    #
//#     # #      #      #    # #      #    #   #   # #    # ##   #
//######  #####   ####  #    # #      #    #   #   # #    # # #  #
//#   #   #           # #    # #      #    #   #   # #    # #  # #
//#    #  #      #    # #    # #      #    #   #   # #    # #   ##
//#     # ######  ####   ####  ######  ####    #   #  ####  #    #

std::vector<float> get_resolution(Config& myconfig, int collection){
	gStyle->SetOptStat(111111);
	std::vector<float> resolution_vector (12,0);

	TCanvas* Residuals = new TCanvas("Residuals", "Resolution of Regions");
	Residuals->Divide(4,3);
	TCanvas* Resolution = new TCanvas("ResolutionRegion", "RMS of Residuals for Regions");
	TLegend* res_legend = new TLegend(0.71,0.82,0.89,0.89,"","NDC");
      
	TGraphErrors* graph1 = new TGraphErrors();
	TGraphErrors* graph2 = new TGraphErrors();
	TGraphErrors* graph3 = new TGraphErrors();
    
	TGraphErrors* graph1corr = new TGraphErrors();
	TGraphErrors* graph2corr = new TGraphErrors();
	TGraphErrors* graph3corr = new TGraphErrors();

	std::vector<TH1D*> resolution_histo_dummy;
	std::vector<TH1D*> resolution_histo;
	std::cout << "[TRACKHIT_FUNCTIONS] Generating Resolution Plots!" << std::endl;
	for (unsigned int index = 1; index<=12 ; index++) 
	{
		std::string title="Resolution Dummy Region ";
		char buffer[3];
        
		sprintf(buffer,"%i", index);
		title+=buffer;
		TH1D* resolution = new TH1D(title.c_str(),title.c_str(), 1000,-.5,.5);
		resolution->GetXaxis()->SetTitle("Residuals [mm]");
		resolution_histo_dummy.push_back(resolution);
        
		// TF1* fit =  new TF1 (buffer, "gaus", -0.5,0.5);
		// 	resolution_fits.push_back(fit);
	}
    
	int counter=0;
    
	for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
	{
		hitCollection* myhitcollection;
		if (collection == 0)
		{
			onTrackCluster* mytrackhit = new onTrackCluster;
			myhitcollection = mytrackhit;
			mytrackhit->readtree(filenames->datafilename);
		}
		else
		{
			Trackhit* mytrackhit = new Trackhit;
			myhitcollection = mytrackhit;
			mytrackhit->readtree(filenames->datafilename);
		}
                       
		for (unsigned int i=0; i<myhitcollection->get_n_entries(); i++) 
		{
			myhitcollection->get_entry(i);
			
			resolution_histo_dummy[myhitcollection->region-1]->Fill(myhitcollection->xPosition-myhitcollection->xTrack);
			counter++;
		}
	}
	
	//fit and create new histogram with mean+-5sigma
	//getRMS of that
	for (unsigned int index = 0; index<12 ; index++) 
	{
		std::string title="Resolution Region ";
		
		char buffer[3];
		sprintf(buffer,"%i", index);
		title+=buffer;
		
		TF1* fit =  new TF1 (buffer, "gaus", -0.5,0.5);
		resolution_histo_dummy.at(index)->Fit(fit,"RNQ");
		int nsigma = 5;
		double binwidth = 0.001;
		int nbins = 2*nsigma*fit->GetParameter(2)*(1/binwidth);
		double lower = 	fit->GetParameter(1)-nsigma*fit->GetParameter(2);
		double upper = fit->GetParameter(1)+nsigma*fit->GetParameter(2);
		
		std::cout << "Range " << lower << " " << upper << " " << nbins << " " << binwidth << std::endl; 
		TH1D* resolution_dummy = new TH1D(title.c_str(),title.c_str(), nbins,lower,upper);
		// resolution_dummy->GetXaxis()->SetRange(-.5,.5);
		resolution_histo.push_back(resolution_dummy);
		delete fit;
	}
	
	//fill again with the right ranges
	for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
	{
		hitCollection* myhitcollection;
		if (collection == 0)
		{
			onTrackCluster* mytrackhit = new onTrackCluster;
			myhitcollection = mytrackhit;
			mytrackhit->readtree(filenames->datafilename);
		}
		else
		{
			Trackhit* mytrackhit = new Trackhit;
			myhitcollection = mytrackhit;
			mytrackhit->readtree(filenames->datafilename);
		}
                       
		for (unsigned int i=0; i<myhitcollection->get_n_entries(); i++) 
		{
			myhitcollection->get_entry(i);
			
			resolution_histo[myhitcollection->region-1]->Fill(myhitcollection->xPosition-myhitcollection->xTrack);
			counter++;
		}
	}
	
	//Draw
    
	for (unsigned int index = 0; index<12 ; index++) 
	{
		if (resolution_histo.at(index)->GetEntries() > 100)
		{
			if (resolution_histo.at(index)->GetEntries() > 1500)
			{
				// resolution_histo.at(index)->Fit(resolution_fits.at(index), "RNQ");
				Residuals->cd(index+1);
				gStyle->SetPadGridX(false);
				gStyle->SetPadGridY(false);
				gStyle->SetOptStat("e");
				resolution_histo.at(index)->Draw();
				// resolution_fits.at(index)->Draw("same");
				myconfig.name->Draw();
			}
			// double sq_subracted_value = sqrt(pow(resolution_fits.at(index)->GetParameter(2)*1000,2) - pow(myconfig.myresolution.sigma_dut[myconfig.DUT],2));
			double sq_subracted_value = sqrt(pow(resolution_histo.at(index)->GetRMS()*1000,2) - pow(myconfig.myresolution.sigma_dut[myconfig.DUT],2));
			// std::cout<<"DEBUG " << myconfig.DUT << " " << myconfig.myresolution.sigma_dut[myconfig.DUT] << " " << sq_subracted_value << std::endl;

			std::cout << "Region " << index+1 << " " << resolution_histo.at(index)->GetRMS()*1000 << " corrected: " << sq_subracted_value << std::endl;
			if (resolution_histo.at(index)->GetEntries() > 1500)
			{
				if (index <= 3 && resolution_histo.at(index)->GetRMS()*1000 > 0 && resolution_histo.at(index)->GetRMS()*1000 < 100) {
					int np = graph1->GetN();
					graph1->SetPoint(np, get_pitch_region(index+1), resolution_histo.at(index)->GetRMS()*1000);
					graph1->SetPointError(np, 0, resolution_histo.at(index)->GetRMSError()*1000);
				}
				else if (index > 3 && index <= 7 && resolution_histo.at(index)->GetRMS()*1000 > 0 && resolution_histo.at(index)->GetRMS()*1000 < 100) {
					int np = graph2->GetN();
					graph2->SetPoint(np, get_pitch_region(index+1), resolution_histo.at(index)->GetRMS()*1000);
					graph2->SetPointError(np, 0, resolution_histo.at(index)->GetRMSError()*1000);
				}
				else if (index > 7 && index<=11 && resolution_histo.at(index)->GetRMS()*1000 > 0 && resolution_histo.at(index)->GetRMS()*1000 < 100)  {
					int np = graph3->GetN();
					graph3->SetPoint(np, get_pitch_region(index+1), resolution_histo.at(index)->GetRMS()*1000);
					graph3->SetPointError(np, 0, resolution_histo.at(index)->GetRMSError()*1000);
				}
        
				if (index <= 3 && sq_subracted_value > 0 && sq_subracted_value < 100) {
					int np = graph1corr->GetN();
					graph1corr->SetPoint(np, get_pitch_region(index+1), sq_subracted_value);
					graph1corr->SetPointError(np, 0, resolution_histo.at(index)->GetRMSError()*1000);
				}
				else if (index > 3 && index <= 7 && sq_subracted_value > 0 && sq_subracted_value < 100) {
					int np = graph2corr->GetN();
					graph2corr->SetPoint(np, get_pitch_region(index+1), sq_subracted_value);
					graph2corr->SetPointError(np, 0, resolution_histo.at(index)->GetRMSError()*1000);
				}
				else if (index > 7 && index<=11 && sq_subracted_value > 0 && sq_subracted_value < 100)  {
					int np = graph3corr->GetN();
					graph3corr->SetPoint(np, get_pitch_region(index+1), sq_subracted_value);
					graph3corr->SetPointError(np, 0, resolution_histo.at(index)->GetRMSError()*1000);
				}
			}
			// if (collection == 0) myconfig.mydetector.onTrackCluster_resolution.at(index) = resolution_fits[index]->GetParameter(2)*1000;
			if (collection == 0)
			{
				resolution_vector.at(index)= static_cast<float>(sq_subracted_value);
			}
		
			// else myconfig.mydetector.Trackhit_resolution.at(index) = resolution_fits[index]->GetParameter(2)*1000;
			else
			{
				resolution_vector.at(index)= static_cast<float>(sq_subracted_value);
			}
		
			if (resolution_histo.at(index)->GetEntries() > 1500)
			{
				if (collection == 0)
				{
					myconfig.mydetector.onTrackCluster_resolution.at(index) = sq_subracted_value;
				}
		
				// else myconfig.mydetector.Trackhit_resolution.at(index) = resolution_fits[index]->GetParameter(2)*1000;
				else
				{
					myconfig.mydetector.Trackhit_resolution.at(index) = sq_subracted_value;
				}
			}
			else 
			{
				std::cout << "Too few entries in the Residual distribution, not extracting an RMS!" << std::endl;
				// if (collection == 0) myconfig.mydetector.onTrackCluster_resolution.at(index) = resolution_fits[index]->GetParameter(2)*1000;
				if (collection == 0) myconfig.mydetector.onTrackCluster_resolution.at(index) = -999;
		
				// else myconfig.mydetector.Trackhit_resolution.at(index) = resolution_fits[index]->GetParameter(2)*1000;
				else myconfig.mydetector.Trackhit_resolution.at(index) = -999;
			}
		}
	}

	std::string title1 = myconfig.foldername;
	title1 += "/Residuals_";
	if (collection == 1)title1+=trackhit;
	else title1 += ontrackcluster;
	title1+=myconfig.detname;
	title1+=".png";
	Residuals->SaveAs(title1.c_str());
    
	myconfig.plotfile->WriteTObject(Residuals,"","Overwrite");
    
	TF1* function = new TF1("function","x*(1/sqrt(12))",1,250);
	function->SetLineColor(3);

	Resolution->cd();
	gStyle->SetPadGridX(true);
	gStyle->SetPadGridY(true);
	graph1->GetXaxis()->SetTitle("Pitch [#mum]");
	graph1->GetYaxis()->SetTitle("Resolution [#mum]");
    
	//    res_legend->AddEntry(function,"x * 1/#sqrt{12}", "lp");
	res_legend->AddEntry(graph1,"w/p = 0.15", "p");
	res_legend->AddEntry(graph2,"w/p = 0.25", "p");
	res_legend->AddEntry(graph3,"w/p = 0.35", "p");
	res_legend->AddEntry(graph1corr,"w/p = 0.15 corr.", "p");
	res_legend->AddEntry(graph2corr,"w/p = 0.25 corr.", "p");
	res_legend->AddEntry(graph3corr,"w/p = 0.35 corr.", "p");
	res_legend->SetNColumns(2);
    
	graph1->SetMarkerColor(1);
	graph2->SetMarkerColor(2);
	graph3->SetMarkerColor(4);
    
	graph1->SetMarkerStyle(4);
	graph2->SetMarkerStyle(4);
	graph3->SetMarkerStyle(4);
	graph1->GetYaxis()->SetRangeUser(0,100);
	graph2->GetYaxis()->SetRangeUser(0,100);
	graph3->GetYaxis()->SetRangeUser(0,100);
	graph1->GetXaxis()->SetLimits(0,250);
	graph2->GetXaxis()->SetLimits(0,250);
	graph3->GetXaxis()->SetLimits(0,250);
    
	graph1corr->SetMarkerColor(1);
	graph2corr->SetMarkerColor(2);
	graph3corr->SetMarkerColor(4);

	graph1corr->SetMarkerStyle(26);
	graph2corr->SetMarkerStyle(26);
	graph3corr->SetMarkerStyle(26);
	graph1corr->GetYaxis()->SetRangeUser(0,100);
	graph2corr->GetYaxis()->SetRangeUser(0,100);
	graph3corr->GetYaxis()->SetRangeUser(0,100);
	graph1corr->GetXaxis()->SetLimits(0,250);
	graph2corr->GetXaxis()->SetLimits(0,250);
	graph3corr->GetXaxis()->SetLimits(0,250);

	graph1->Draw("AP");
	graph2->Draw("Psame");
	graph3->Draw("Psame");
	//    function->Draw("Lsame");
	res_legend->Draw();
	myconfig.name->Draw("same");
    
	//    TF1* resolutionfunc1 = new TF1("resolutionfunc1","sqrt(sq([0])*sq(x)+sq([1]))",0,250);
	//    graph1->Fit("resolutionfunc1","R");
	//    resolutionfunc1->SetLineColor(1);
	//    resolutionfunc1->Draw("Lsame");
	TF1* resolutionfunc2 = new TF1("resolutionfunc2","sqrt(sq([0])*sq(x)+sq([1]))",0,250);
	graph2->Fit("resolutionfunc2","R");
	resolutionfunc2->SetLineColor(2);
	resolutionfunc2->Draw("Lsame");
	//    TF1* resolutionfunc3 = new TF1("resolutionfunc3","sqrt(sq([0])*sq(x)+sq([1]))",0,250);
	//    graph3->Fit("resolutionfunc3","R");
	//    resolutionfunc3->SetLineColor(4);
	//    resolutionfunc3->Draw("Lsame");
	//    res_legend->AddEntry(resolutionfunc2,"sqrt(#kappa^{2} p^{2} + #sigma^{2}_{Telescope})", "lp");

	graph1corr->Draw("P same");
	graph2corr->Draw("P same");
	graph3corr->Draw("P same");
	res_legend->Draw("same");


	std::string title = myconfig.foldername;
	title += "/Resolution_";
	if (collection == 1)title+=trackhit;
	else title += ontrackcluster;
	title+=myconfig.detname;
	title+=".png";
	Resolution->SaveAs(title.c_str());
	myconfig.plotfile->WriteTObject(Resolution,"","Overwrite");
	return resolution_vector;
}


//#     #
//##   ##   ##   #####   ####
//# # # #  #  #  #    # #
//#  #  # #    # #    #  ####
//#     # ###### #####       #
//#     # #    # #      #    #
//#     # #    # #       ####

void get_hitmap(Config& myconfig, int collection){
    
    TCanvas* hitmapcanvas = new TCanvas("Hitmap", "Hitmap");
    TH2D* hitmap = new TH2D("HitMap", "Map of MSSD Hits", 1000,0,65.74,500,-2,32);
    hitmap->GetXaxis()->SetTitle("x Position [mm]");
    hitmap->GetYaxis()->SetTitle("y Position [mm]");
    
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        
        hitCollection* myhitcollection;
        if (collection == 0)
        {
            onTrackCluster* mytrackhit = new onTrackCluster;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        else
        {
            Trackhit* mytrackhit = new Trackhit;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        
        for (unsigned int i=0; i<myhitcollection->get_n_entries(); i++) {
            myhitcollection->get_entry(i);
            hitmap->Fill(myhitcollection->xTrack, myhitcollection->yTrack);
            
        }
    }
    
    std::cout << "[TRACKHIT_FUNCTIONS] Generating Hit Map!" << std::endl;
    
    //Drawing
    gStyle->SetOptStat("e");
    hitmapcanvas->cd();
    hitmap->Draw("scat");
    myconfig.name->Draw();
    
    std::string title = myconfig.foldername;
    title += "/Hitmap_";
    title+=myconfig.detname;
    title+=".png";
    
    myconfig.plotfile->WriteTObject(hitmapcanvas,"","Overwrite");
    
    hitmapcanvas->SaveAs(title.c_str());
}



void get_chargemap(Config& myconfig, int collection, int unit){
    TCanvas* chargemapcanvas = new TCanvas("ChargeMap", "ChargeMap");
    TH2D* chargemap = new TH2D("ChargeMap", "Map of MSSD Hits Charge", 1000,0,65.74,500,-2,32);
    TH2D* hitcount = new TH2D("HitMap", "Map of MSSD Hits", 1000,0,65.74,500,0,32.79);
    chargemap->GetXaxis()->SetTitle("x Position [mm]");
    chargemap->GetYaxis()->SetTitle("y Position [mm]");
    if ( unit >=0 && unit < 3 )chargemap->GetZaxis()->SetTitle("Collected Charge [Electrons]");
    else if ( unit == 3 )chargemap->GetZaxis()->SetTitle("Collected Charge [ADC]");
    
    std::cout << "[TRACKHIT_FUNCITONS] Generating Charge Map!" << std::endl;

    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        hitCollection* myhitcollection;
        if (collection == 0)
        {
            onTrackCluster* mytrackhit = new onTrackCluster;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        else
        {
            Trackhit* mytrackhit = new Trackhit;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        
        for (unsigned int i=0; i<myhitcollection->get_n_entries(); i++) {
            myhitcollection->get_entry(i);
            chargemap->Fill(myhitcollection->xTrack, myhitcollection->yTrack, myconfig.adc_to_electrons(*filenames,myhitcollection->charge, myconfig.strip_to_chip(myhitcollection->first_strip), unit));
            hitcount->Fill(myhitcollection->xTrack, myhitcollection->yTrack);
        }
    }
    
    double binContent;
    double binCount;
    for (int i=1; i<=hitcount->GetNbinsX(); i++) {
        for (int j=1; j<hitcount->GetNbinsY(); j++) {
            binCount=hitcount->GetBinContent(i,j);
            if (binCount>0) {
                binContent=chargemap->GetBinContent(i,j);
                binContent/=binCount;
                chargemap->SetBinContent(i,j,binContent);
            }
        }
    }
    
    //Drawing
    gStyle->SetOptStat("e");
    chargemapcanvas->cd();
    if ( unit >=0 && unit < 3 ) chargemap->SetMaximum(32000);
    else if ( unit == 3 ) chargemap->SetMaximum(50);
    chargemap->Draw("contz");
    myconfig.name->Draw();
    
    std::string title = myconfig.foldername;
    title += "/Chargemap_";
    title+=myconfig.detname;
    title+=".png";
    chargemapcanvas->SaveAs(title.c_str());
    
//    myconfig.plotfile->WriteTObject(chargemapcanvas);
}


//#
//#         ##   #    # #####    ##   #    #  ####
//#        #  #  ##   # #    #  #  #  #    # #
//#       #    # # #  # #    # #    # #    #  ####
//#       ###### #  # # #    # ###### #    #      #
//#       #    # #   ## #    # #    # #    # #    #
//####### #    # #    # #####  #    #  ####   ####


void get_landau_region(Config& myconfig, int collection, int unit){
    std::vector<TH1F*> landau_vec;
    for (int i=0; i<12; i++) {
        std::string title = "Cluster Charge Distribution  for Region ";
        char buffer[5];
        sprintf(buffer,"%i", i+1);
        title += buffer;
        TH1F* landau=new TH1F(title.c_str(), title.c_str(),110,-10,100);
        landau_vec.push_back(landau);
    }
    
    std::cout << "[TRACKHIT_FUNCTIONS] Generating Charge Distribution!" << std::endl;

    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        
        hitCollection* myhitcollection;
        if (collection == 0)
        {
            onTrackCluster* mytrackhit = new onTrackCluster;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        else
        {
            Trackhit* mytrackhit = new Trackhit;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        
        for (unsigned int i=0; i<myhitcollection->get_n_entries(); i++) {
            myhitcollection->get_entry(i);
            landau_vec.at(myhitcollection->region-1)->Fill(myconfig.adc_to_electrons(*filenames,myhitcollection->charge, myconfig.strip_to_chip(myhitcollection->first_strip),2)/1000); //use electrons ref calibration
        }
    }
    
    TCanvas* Signal = new TCanvas("Signal","Cluster Total Charge ");
    gStyle->SetOptStat("e");
    Signal->Divide(4,3);
    
    for (int i=0;i<12;i++) {
        Signal->cd(i+1);
        //Dont show 0 Bin content for convenience
        landau_vec.at(i)->SetBinContent(landau_vec.at(i)->FindFirstBinAbove(0),0);
        if ( unit >=0 && unit <3 ) landau_vec.at(i)->GetXaxis()->SetTitle("[Electrons]");
        else if ( unit == 3 ) landau_vec.at(i)->GetXaxis()->SetTitle("[ADC]");
        if (collection == 1 && myconfig.myscale.noise_cluster_signal.at(i) != NULL) {
            TH1F* subtracted_landaus = new TH1F(*landau_vec.at(i));
            subtracted_landaus->Add(myconfig.myscale.noise_cluster_signal.at(i),-1);
            landau_vec.at(i)->SetFillColor(38);
            subtracted_landaus->SetFillColor(30);
            myconfig.myscale.noise_cluster_signal.at(i)->SetFillColor(46);
            
            double mpv, mpv_error, landau_mpv, landau_mpv_error;
            TF1* fit = langaus(subtracted_landaus, mpv, mpv_error, landau_mpv, landau_mpv_error,0);
                if (mpv <= 1) {
                    mpv = -999;
                    mpv_error = -999;
                }
                            
            myconfig.mydetector.Trackhit_signal.at(i) = mpv;
            myconfig.mydetector.Trackhit_signal_error.at(i) = mpv_error;
            myconfig.mydetector.Trackhit_signal_chisq.at(i) = fit->GetChisquare();
            myconfig.mydetector.Trackhit_signal_ndf.at(i) = fit->GetNDF();
            landau_vec.at(i)->Draw();
            subtracted_landaus->Draw("same");
            fit->Draw("Lsame");
            myconfig.myscale.noise_cluster_signal.at(i)->Draw("same");
            
            myconfig.name->Draw();
            TLatex* peak = draw_peak(mpv, mpv_error);
            peak->Draw("same");
            myconfig.warningstream << "[TRACKHIT SINGNAL] Region " << i+1 << " Peak " << mpv << " Peak ERROR " << mpv_error << " LANDAU MPV " << landau_mpv << " LANDAU MPV ERROR " << landau_mpv_error << std::endl;
        }
        else if (collection == 0 /*&& landau_vec.at(i)->GetEntries() > 0*/ ) { // no noise estimate possible or desired du to cluster algorithm
            landau_vec.at(i)->SetFillColor(30);
            
            double mpv, mpv_error, landau_mpv, landau_mpv_error;
            TF1* fit = langaus(landau_vec.at(i), mpv, mpv_error, landau_mpv, landau_mpv_error,0);
                if (mpv <= 1) {
                    mpv = -999;
                    mpv_error = -999;
                }
           
            landau_vec.at(i)->Draw();
			// fit->Dump();
			std::cout << fit->GetChisquare() << "  " << fit->GetNDF() << std::endl;
            myconfig.mydetector.onTrackCluster_signal.at(i) = mpv;
            myconfig.mydetector.onTrackCluster_signal_error.at(i) = mpv_error;
            myconfig.mydetector.onTrackCluster_signal_chisq.at(i) = fit->GetChisquare();
            myconfig.mydetector.onTrackCluster_signal_ndf.at(i) = fit->GetNDF();
			std::cout << myconfig.mydetector.onTrackCluster_signal_chisq.at(i) << "		" << myconfig.mydetector.onTrackCluster_signal_ndf.at(i) << std::endl;
				
            fit->SetLineColor(2);
            fit->Draw("Lsame");
            myconfig.name->Draw();
            TLatex* peak = draw_peak(mpv, mpv_error);
            peak->Draw("same");
            myconfig.warningstream << "[ON_TRACK SINGNAL] Region " << i+1 << " PEAK " << mpv << " PEAK ERROR " << mpv_error << " LANDAU MPV " << landau_mpv << " LANDAU MPV ERROR " << landau_mpv_error << std::endl;

        }
    }

    std::string title = myconfig.foldername;
    title += "/Signal_";
    if (collection == 1)title+=trackhit;
    else title += ontrackcluster;
    title+=myconfig.detname;
    title+=".png";
    Signal->SaveAs(title.c_str());
    
    myconfig.plotfile->WriteTObject(Signal,"","Overwrite");
}

//CLUSTERWIDTH
void clusterwidth(Config& myconfig, int collection){
    
    std::vector<TH1F*> width_vector;
    
    for (unsigned int index = 1; index<=12 ; index++) {
        std::string title="Clusterwidth Region ";
        char buffer[3];
        sprintf(buffer,"%i", index);
        title+=buffer;
        TH1F* width = new TH1F(title.c_str(),title.c_str(), 10,-.5,9.5);
        width->GetXaxis()->SetTitle("Clusterwidth");
        width_vector.push_back(width);
    }
    
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        hitCollection* myhitcollection;
        if (collection == 0)
        {
            onTrackCluster* mytrackhit = new onTrackCluster;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        else
        {
            Trackhit* mytrackhit = new Trackhit;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        
        for (unsigned int i=0; i<myhitcollection->get_n_entries(); i++) {
            
            myhitcollection->get_entry(i);
            
            if (myhitcollection->width != 0) width_vector.at(myhitcollection->region-1)->Fill(myhitcollection->width);
        }
    }
    TCanvas* Clusterwidth = new TCanvas("Clusterwidth", "Distribution of Clusterwidth");
    gStyle->SetOptStat("e");
    Clusterwidth->Divide(4,3);
    for (int i=0;i<12;i++) {
        Clusterwidth->cd(i+1);
        width_vector.at(i)->GetXaxis()->SetTitle("Clusterwidth");
        width_vector.at(i)->SetFillColor(38);
        width_vector.at(i)->Draw();
        myconfig.name->Draw();
        
        if (collection == 0) myconfig.mydetector.onTrackCluster_width.at(i) = width_vector.at(i)->GetMean();
        else  myconfig.mydetector.Trackhit_width.at(i) = width_vector.at(i)->GetMean();
    }
    std::string title = myconfig.foldername;
    title += "/Clusterwidth_";
    if (collection == 1)title+=trackhit;
    else title += ontrackcluster;
    title+=myconfig.detname;
    title+=".png";
    Clusterwidth->SaveAs(title.c_str());
    
    myconfig.plotfile->WriteTObject(Clusterwidth,"","Overwrite");
}




//####### #######    #
//#          #      # #
//#          #     #   #
//#####      #    #     #
//#          #    #######
//#          #    #     #
//#######    #    #     #

void get_eta_trackhit (Config& myconfig, int collection) {
    TCanvas* EtaDistribution = new TCanvas("EtaDistribution","Eta Canvas ");
    std::vector<TH1F*> eta_vector_trackhit;

    for (int i =0; i<12; i++) {
        std::string title = "#eta Distribution  for Region ";
        char buffer[5];
        sprintf(buffer,"%i", i+1);
        title += buffer;
        TH1F* etaf = new TH1F(title.c_str(), title.c_str(),50,0,1);
        std::string title1 = "Integrated #eta  for Region ";
        title1 += buffer;
        eta_vector_trackhit.push_back(etaf);
}
    
    float eta=-99;
    
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        hitCollection* myhitcollection;
        if (collection == 0)
        {
            onTrackCluster* mytrackhit = new onTrackCluster;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        else
        {
            Trackhit* mytrackhit = new Trackhit;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        
        for (unsigned int i=0; i<myhitcollection->get_n_entries(); i++) {
            myhitcollection->get_entry(i);
            
            if (myhitcollection->width == 2)
            {
                eta = myhitcollection->charge_vector.at(0)/(myhitcollection->charge_vector.at(0)+myhitcollection->charge_vector.at(1));
            }
            else if (myhitcollection->width > 2 )
            {
                float maxcharge=0;
                float secondmax=0;
                int index_max=0;
                int index_second=0;
                for (size_t j=0; j<myhitcollection->charge_vector.size(); j++) {
                    if (myhitcollection->charge_vector.at(j) > maxcharge) 
                    {
                        secondmax = maxcharge;
                        maxcharge = myhitcollection->charge_vector.at(j);
                        index_max = j;
                    } else if ((myhitcollection->charge_vector.at(j) > secondmax) && (myhitcollection->charge_vector.at(j) != maxcharge)) {
                        secondmax=myhitcollection->charge_vector.at(j);
                        index_second = j;
                    }
                }
                
                if ((fabs(index_max-index_second) == 1) && (index_max < index_second)) {
                    eta = maxcharge/(maxcharge+secondmax);
                }
                if ((fabs(index_max-index_second) == 1) && (index_max > index_second)) {
                    eta = secondmax/(maxcharge+secondmax);
                }
            }
            if( eta != -99 ) eta_vector_trackhit.at((myhitcollection->region)-1)->Fill(eta);
        }
    }
    gStyle->SetOptStat("e");
    EtaDistribution->Divide(4,3);
    int counter=1;
    for (std::vector<TH1F*>::iterator it = eta_vector_trackhit.begin(); it != eta_vector_trackhit.end(); it++) {
        EtaDistribution->cd(counter);
        (*it)->SetFillColor(38);
        (*it)->Draw();
        myconfig.name->Draw();
        counter++;
    }
    
    std::string title = myconfig.foldername;
    title += "/Eta_";
    title+=myconfig.detname;
    
    title+=".png";
    EtaDistribution->SaveAs(title.c_str());
    
    myconfig.plotfile->WriteTObject(EtaDistribution,"","Overwrite");
}



void interstrip_position(Config& myconfig, int collection) {
	//introduce reverse hitmaker that returns double
    TCanvas* InterstripPosition = new TCanvas("InterstripPosition", "InterstripPosition");
    InterstripPosition->Divide(4,3);
    std::vector<TProfile*> _1_strip_vector;
    std::vector<TProfile*> _2_strip_vector;
    std::vector<TProfile*> _more_strip_vector;

    for (int i =0; i<12; i++) {
        std::string title = "Interstrip Position Region ";
        char buffer[5];
        sprintf(buffer,"%i", i+1);
        title += buffer;
        
        TProfile* _1strip_profile = new TProfile("1strip",title.c_str(),50,0,1);
        _1strip_profile->SetLineColor(1);
        _1strip_profile->SetMinimum(0);
        
        _1strip_profile->GetXaxis()->SetTitle("Interstrip Position");

        TProfile* _2strip_profile = new TProfile("2strip",title.c_str(),50,0,1);
        _2strip_profile->SetLineColor(2);
        _2strip_profile->SetMinimum(0);
        TProfile* _morestrip_profile = new TProfile("morestrip",title.c_str(),50,0,1);
        _morestrip_profile->SetMinimum(0);

        _1_strip_vector.push_back(_1strip_profile);
        _2_strip_vector.push_back(_2strip_profile);
        _more_strip_vector.push_back(_morestrip_profile);
    }
    
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        hitCollection* myhitcollection;
        if (collection == 0)
        {
            onTrackCluster* mytrackhit = new onTrackCluster;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        else
        {
            Trackhit* mytrackhit = new Trackhit;
            myhitcollection = mytrackhit;
            mytrackhit->readtree(filenames->datafilename);
        }
        
        for (unsigned int i=0; i<myhitcollection->get_n_entries(); i++) {
            myhitcollection->get_entry(i);
            
            double interstrip = (reverse_hitmaker(myhitcollection->xTrack));
            int int_interstrip = reverse_hitmaker(myhitcollection->xTrack);
            interstrip -= int_interstrip;
            
            if (myhitcollection->width == 1) {
                _1_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,1);
                _2_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,0);
                _more_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,0);
            }
            else if (myhitcollection->width == 2) {
                _1_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,0);
                _2_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,1);
                _more_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,0);
            }
            else if (myhitcollection->width >= 3) {
                _1_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,0);
                _2_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,0);
                _more_strip_vector.at(myhitcollection->region-1)->Fill(interstrip,1);
            }
        }
    }
    
    for (int i = 0; i<12; i++) {
        InterstripPosition->cd(i+1);
        _1_strip_vector.at(i)->Draw();
        _2_strip_vector.at(i)->Draw("same");
        _more_strip_vector.at(i)->Draw("same");
        myconfig.name->Draw();
    }
  

std::string title = myconfig.foldername;
    title += "/InterstripPosition_";
    if (collection == 1)title += trackhit;
    else title += ontrackcluster;
    title += myconfig.detname;
    title += ".png";
    InterstripPosition->SaveAs(title.c_str());
    myconfig.plotfile->WriteTObject(InterstripPosition,"","Overwrite");
}


