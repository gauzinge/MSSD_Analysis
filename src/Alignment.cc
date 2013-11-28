#include "Alignment.h"

Alignment::Alignment(std::string dataf, std::string trackf){
    this->datafilename = dataf;
    this->trackfilename = trackf;
    this->DUT_zPosition = 0;
    this->AlignmentFile=NULL;
    this->AlignmentData=NULL;
    this->x_shift=0, this->angle=0, this->y_shift=0;
}

Alignment::~Alignment(){
    if (AlignmentFile!=NULL){
        AlignmentFile->Close();
        delete AlignmentFile;
    }
}

double Alignment::get_fitrange(TH1D* h) {
    double maxval = h->GetMaximum();
    double firstbin = h->GetBinCenter(h->FindFirstBinAbove(maxval/2));
    double lastbin = h->GetBinCenter(h->FindLastBinAbove(maxval/2));
    double fwhm = (lastbin-firstbin);
    return (5*fwhm/2);
}

void Alignment::do_alignment_angular(Config& myconfig, int n_of_events, bool draw, double z_position){
    TGraph* RMSGraph_angular = new TGraph();
    
    RMSGraph_angular->SetTitle("Angular Alignment RMS ; Angle; RMS of Residuals");
    
    EventCollection Aligncollection;
    Aligncluster.readtree(datafilename.c_str());
    Aligntrack.readtree(trackfilename.c_str());
    for (unsigned int i=0; i<Aligncluster.get_n_entries(); i++) {
        Aligncluster.get_entry(i);
        Aligncollection.addHit(i, Aligncluster.EventNumber);
    }
    
    std::string matchfilename = find_matchfiledir(datafilename);
    size_t last_int= datafilename.find("_MSSD");
    matchfilename += datafilename.substr(last_int-4,4);
	std::string run = datafilename.substr(last_int-4,4);
    matchfilename += "_Alignment.txt";
    // std::cout << "Matchfilename " << matchfilename << std::endl;

    std::ofstream clusterMatchFile(matchfilename.c_str());
    Aligncollection.Serialize(clusterMatchFile);
    
    std::ifstream inFile(matchfilename.c_str());
    Aligncollection.DeSerialize(inFile);
    
    //        int counter_good=0, counter_bad=0;
    
    Aligncluster.get_entry(1); //Could use an arbitrary entry here, its just about the z
    if (z_position == 0) 
	{
		this->DUT_zPosition = Aligncluster.zPosition;
    	std::cout << "[ALIGNMENT] The a-priori z -position is " << DUT_zPosition << " Propataging Track Coordinates there!" << std::endl;
	}
	else 
	{
		DUT_zPosition = z_position;
    	std::cout << "[ALIGNMENT] The true z -position is " << DUT_zPosition << " Propataging Track Coordinates there!" << std::endl;
	}
	
    double min_angle=0;
    double old_rms=1;
    for (int index=0; index<=100; index++) {
        double fit_rms = 0;
        double actual_angle = 42+(index*.05);
        int counter=0;
        std::string title1="offsets";
        
        char buffer[4];
        sprintf(buffer,"%i", index);
        title1+=buffer;
        
        TH1D* residuals = new TH1D( title1.c_str(), "Track to hit x offset;Hit - Track x [mm]",10000, 0., 55.);
        
        
        for (unsigned int j=0; j<=Aligntrack.get_n_entries(); j++) {
            Aligntrack.get_entry(j);
            Aligntrack.get_track_DUTPos(DUT_zPosition);
            Aligntrack.rotate_hit_coordinates(0, 0, actual_angle);
            
            const std::vector<int>& eventHits = Aligncollection.GetHitLines(Aligntrack.EventNumber);
            std::vector<int>::const_iterator it;
            for(it = eventHits.begin() ; it!= eventHits.end(); ++it) {
                const int& hitLine = *it;
                Aligncluster.get_entry(hitLine);
                residuals->Fill(Aligncluster.xPosition-Aligntrack.xPrime);
            }
            counter++;
            if (n_of_events != -1){
                if (counter > n_of_events) break;
            }
        }
        double center = residuals->GetBinCenter(residuals->GetMaximumBin());
        double fitrange = get_fitrange(residuals);
        TF1* residuals_fit = new TF1("gausFit", "gaus", center-fitrange, center+fitrange);
        
        residuals->Fit("gausFit", "RQO");
//        TCanvas* tempcanvas = new TCanvas("","");
//        tempcanvas->cd();
//        residuals->Draw();
//        tempcanvas->SaveAs("Alignemnt_tmp.png");
        
        
        fit_rms = residuals_fit->GetParameter(2);
        
        delete residuals;
        delete residuals_fit;
        //            if (fit_rms<.1) {
        if (fit_rms < old_rms  ) {
            old_rms = fit_rms;
            min_angle = actual_angle;
        }
        RMSGraph_angular->SetPoint(index, actual_angle, fit_rms);
        //                std::cout << "[ALIGNMENT] Angle " << actual_angle << " with RMS of Gausian Fit (" << fit_rms << ") processed!" << std::endl;
        //            }
    }
    
//    std::cout << "Temporary Min = " << min_angle << std::endl;
    TF1* fit_angular = new TF1("fit_angular","pol2", min_angle-.2, min_angle+.2);
    
    RMSGraph_angular->Fit(fit_angular,"RQO");
    
    angle = fit_angular->GetMinimumX();
    
    //Draw the whole thing!
    if (draw == true) {
	    TCanvas* angularcanvas = new TCanvas("angularcanvas", "Alignment");
        angularcanvas->cd();
        gStyle->SetOptStat("");
        gStyle->SetOptFit(0000);
        RMSGraph_angular->SetMarkerStyle(7);
        RMSGraph_angular->GetXaxis()->SetRangeUser(40,50);
        RMSGraph_angular->Draw("AP");
        fit_angular->Draw("same");
        angularcanvas->Update();
//        angularcanvas->SaveAs("angularalignment.root");
		std::string title = myconfig.foldername;
		title += "/AlignmentAngular_";
		title+=run;
		title+=".png";
		angularcanvas->SaveAs(title.c_str());
		// myconfig.plotfile->WriteTObject(angularcanvas,"","Overwrite");
    }
    delete RMSGraph_angular;
	delete fit_angular;
    std::cout << "[ALIGNMENT] ****************************************************** " << std::endl;
    std::cout << "[ALIGNMENT RESULT] The Correct Angle is " << angle << " Degrees!" << std::endl;
    std::cout << "[ALIGNMENT] ****************************************************** " <<std::endl;
    
}


void Alignment::do_alignment_x(Config& myconfig, int n_of_events, bool draw) {
    
    EventCollection Aligncollection;
    Aligncluster.readtree(datafilename.c_str());
    Aligntrack.readtree(trackfilename.c_str());
    
    std::string matchfilename = find_matchfiledir(datafilename);
    size_t last_int= datafilename.find("_MSSD");
    matchfilename += datafilename.substr(last_int-4,4);
	std::string run = datafilename.substr(last_int-4,4);
    matchfilename += "_Alignment.txt";
    
    std::ifstream inFile(matchfilename.c_str());
    Aligncollection.DeSerialize(inFile);
    
    int counter = 0;
    TH1D* xalignment = new TH1D( "Shift in x Direction", "Hit x - Track x [mm]",10000, 10., 50.);
    
    for (unsigned int j=0; j<=Aligntrack.get_n_entries(); j++) {
        Aligntrack.get_entry(j);
        Aligntrack.get_track_DUTPos(this->DUT_zPosition);
        Aligntrack.rotate_hit_coordinates(0, 0, angle);
        
        const std::vector<int>& eventHits = Aligncollection.GetHitLines(Aligntrack.EventNumber);
        std::vector<int>::const_iterator it;
        for(it = eventHits.begin() ; it!= eventHits.end(); ++it) {
            const int& hitLine = *it;
            Aligncluster.get_entry(hitLine);
            
            xalignment->Fill(Aligncluster.xPosition-Aligntrack.xPrime);
            counter++;
            
        }
        //            if (counter%1000 == 0) std::cout << "[ALIGNMENT] " << counter  <<" Events for x Shift processed!" << std::endl;
        n_events = counter;
        if (n_of_events != -1){
            if (counter > n_of_events) break;
        }
    }
    double center = xalignment->GetBinCenter(xalignment->GetMaximumBin());
    double fitrange = get_fitrange(xalignment);
    TF1 *fit_low = new TF1("fit_low","gaus",center-fitrange,center+fitrange);
    xalignment->Fit("fit_low", "RQO"/*, "QN"*/);
    x_shift = fit_low->GetParameter(1);
    
    
    //Draw the whole thing!
    if (draw == true) {
	    TCanvas* xcanvas = new TCanvas("xcanvas","Alignment in x Direction");
		
        gStyle->SetOptStat("em");
        gStyle->SetOptFit(0001);
        xcanvas->cd();
        xalignment->DrawCopy();
        fit_low->Draw("same");
        xcanvas->Update();

		std::string title = myconfig.foldername;
		title += "/AlignmentX_";
		title+=run;
		title+=".png";
		xcanvas->SaveAs(title.c_str());
		// myconfig.plotfile->WriteTObject(xcanvas,"","Overwrite");
    }
    
	delete xalignment;
	delete fit_low;
    std::cout << "[ALIGNMENT] *************************************************************** " << std::endl;
    std::cout << "[ALIGNMENT RESULT] The Alignment Shift in x Direction is: " << x_shift <<  std::endl;
    std::cout << "[ALIGNMENT] *************************************************************** " << std::endl;
    
}

void Alignment::do_alignment_z(Config& myconfig, int n_of_events, bool draw){
    TGraph* RMSGraph_z = new TGraph();
    
    RMSGraph_z->SetTitle("Z Alignment RMS ;z [mm]; RMS of Residuals");
    
    EventCollection Aligncollection;
    Aligncluster.readtree(datafilename.c_str());
    Aligntrack.readtree(trackfilename.c_str());
    
    std::string matchfilename = find_matchfiledir(datafilename);
    size_t last_int= datafilename.find("_MSSD");
    matchfilename += datafilename.substr(last_int-4,4);
	std::string run = datafilename.substr(last_int-4,4);
    matchfilename += "_Alignment.txt";
    
    std::ifstream inFile(matchfilename.c_str());
    Aligncollection.DeSerialize(inFile);
    
    
    std::cout << "[ALIGNMENT] Running z Alignment!" << std::endl;
    double min_z=-200;
    // double old_rms=1;
    for (int index=0; index<=200; index++) 
	{
		double fit_rms = 0;
		double actual_z = min_z + index * 2;
		int counter=0;
		std::string title1="offsets";
        
		char buffer[4];
		sprintf(buffer,"%i", index);
		title1+=buffer;
        
		TH1D* residuals = new TH1D( title1.c_str(), "Track to hit x offset;Hit - Track x [mm]",10000, 0., 55.);
        
        
		for (unsigned int j=0; j<=Aligntrack.get_n_entries(); j++) {
			Aligntrack.get_entry(j);
			Aligntrack.get_track_DUTPos(actual_z);
			Aligntrack.rotate_hit_coordinates(0, 0, this->angle);
            
			const std::vector<int>& eventHits = Aligncollection.GetHitLines(Aligntrack.EventNumber);
			std::vector<int>::const_iterator it;
			for(it = eventHits.begin() ; it!= eventHits.end(); ++it) {
				const int& hitLine = *it;
				Aligncluster.get_entry(hitLine);
				residuals->Fill(Aligncluster.xPosition-Aligntrack.xPrime);
			}
			counter++;
			if (n_of_events != -1){
				if (counter > n_of_events) break;
			}
		}
		double center = residuals->GetBinCenter(residuals->GetMaximumBin());
		double fitrange = get_fitrange(residuals);
		TF1* residuals_fit = new TF1("gausFit", "gaus", center-fitrange, center+fitrange);
        
		residuals->Fit("gausFit", "RQO");      
        
		fit_rms = residuals_fit->GetParameter(2);
		delete residuals;
		delete residuals_fit;
		
		RMSGraph_z->SetPoint(index, actual_z, fit_rms);
	}
    
    int n = RMSGraph_z->GetN();
     double *x = RMSGraph_z->GetX();
     double *y = RMSGraph_z->GetY();
     int locmin = TMath::LocMin(n,y);
     double xmin = x[locmin];
	 
     TF1* fit_z = new TF1("fit_z","pol2", xmin-30, xmin+30);
    
     RMSGraph_z->Fit(fit_z,"RQO");
     this->DUT_zPosition = fit_z->GetMinimumX();
	 
	 std::cout << "The z value is: " << xmin << " fig " << fit_z->GetMinimumX() << std::endl;
    //Draw the whole thing!
    if (draw == true) {
	    TCanvas* zcanvas = new TCanvas("zcanvas", "z Alignment");
        zcanvas->cd();
        gStyle->SetOptStat("");
        gStyle->SetOptFit(0000);
        RMSGraph_z->SetMarkerStyle(7);
        // RMSGraph_z->GetXaxis()->SetRangeUser(40,50);
        RMSGraph_z->Draw("AP");
        // fit_angular->Draw("same");
        zcanvas->Update();
//        angularcanvas->SaveAs("angularalignment.root");
		std::string title = myconfig.foldername;
		title += "/AlignmentZ_";
		title+=run;
		title+=".png";
		zcanvas->SaveAs(title.c_str());
		// myconfig.plotfile->WriteTObject(angularcanvas,"","Overwrite");
    }
    delete RMSGraph_z;
	delete fit_z;
    std::cout << "[ALIGNMENT] *************************************************************** " << std::endl;
    std::cout << "[ALIGNMENT RESULT] The Correct z Position is: " << this->DUT_zPosition <<  std::endl;
    std::cout << "[ALIGNMENT] *************************************************************** " << std::endl;
	
    
}

// void Alignment::do_alignment_y(int n_of_events, bool draw){
    
    // EventCollection Aligncollection;
//     Aligncluster.readtree(datafilename.c_str());
//     Aligntrack.readtree(trackfilename.c_str());
//     
//     std::string matchfilename = find_matchfiledir(datafilename);
//     size_t last_int= datafilename.find("_MSSD");
//     matchfilename += datafilename.substr(last_int-4,4);
//     matchfilename += "_Alignment.txt";
//     
//     std::ifstream inFile(matchfilename.c_str());
//     Aligncollection.DeSerialize(inFile);
    
	// int nbins = 200;
//     TProfile* yalignment = new TProfile("YHitProfile","y Profile",nbins, -35, 35);
//     yalignment->SetErrorOption("");
//     int counter = 0;
// 	
//     for (unsigned int j=0; j<=Aligntrack.get_n_entries(); j++) {
//         Aligntrack.get_entry(j);
//         Aligntrack.get_track_DUTPos(DUT_zPosition);
//         Aligntrack.rotate_hit_coordinates(this->x_shift, 0, this->angle);
//         
//         
//         const std::vector<int>& eventHits = Aligncollection.GetHitLines(Aligntrack.EventNumber);
//         std::vector<int>::const_iterator it;
//         for(it = eventHits.begin() ; it!= eventHits.end(); ++it) {
//             const int& hitLine = *it;
//             Aligncluster.get_entry(hitLine);
//             int value = 0;
//             if ((fabs(Aligncluster.xPosition - Aligntrack.xPrime) < get_pitch_region(get_region(Aligntrack.xPrime))*0.001)) 
// 			{	//cluster found
// 				value = 1;
//             }
//            	
// 			yalignment->Fill(Aligntrack.yPrime,value);
//             counter++;
// 			
//         }
//         n_events = counter;
//         if (n_of_events != -1){
//             if (counter > n_of_events) break;
//         }
//     }
//     TF1 *fit = new TF1("fit","pol0",yalignment->GetMean()-15,yalignment->GetMean()+15);
//     yalignment->Fit("fit", "RQO");
// 	double maximum = fit->GetParameter(0);
// 	// double maximum = yalignment->GetMaximum();
// 	double minimum = yalignment->GetMinimum();
// 	
// 	// double maxvalue = (maximum - minimum)*0.7;
// 	double maxvalue = (maximum - minimum)*0.85;
// 	
// 	//find min and max bin above threshold
// 	int minbin, maxbin;
// 	for (int i = 0 ; i < nbins ; i++) 
// 	{
// 		if (yalignment->GetBinContent(i) > maxvalue) 
// 		{
// 			minbin = i;
// 			break;
// 		}	
// 	}
// 	for (int i = nbins-1 ; i > 0 ; i--) 
// 	{
// 		if (yalignment->GetBinContent(i) > maxvalue) 
// 		{
// 			maxbin = i;
// 			break;
// 		}	
// 	} 
// 	double y_min = yalignment->GetBinCenter(minbin);
// 	double y_max = yalignment->GetBinCenter(maxbin);
// 	// double y_min = yalignment->GetBinCenter(yalignment->FindFirstBinAbove(maxvalue/2));
// // 	double y_max = yalignment->GetBinCenter(yalignment->FindLastBinAbove(maxvalue/2));
// 	double shift;
// 	if (y_max-y_min > 32 || y_max-y_min < 25) shift = 0;
// 	else shift = (y_min+(y_max-y_min)/2);
	// if (15.2 - shift > 17 || 15.2 - shift < 14 )
// 	{
// 		this->y_shift = 15.04;
// 	}
// 	else this->y_shift = 15.2 - shift;
	// 
	//     std::cout << "[ALIGNMENT] *************************************************************** " << std::endl;
	//     std::cout << "[ALIGNMENT RESULT] The Width of the Beam in y Direction is " << y_max - y_min <<  std::endl;
	// std::cout << "[ALIGNMENT RESULT] Centering on Sensor by applying a y Shift of " << this->y_shift  << std::endl;
	//     std::cout << "[ALIGNMENT] *************************************************************** " << std::endl;
	
	//sensor is 15.2 wide (active)
	// this->y_shift = 15.04;
	// if (draw)
// 	{
// 		TLine* line = new TLine(y_min,maxvalue,y_max,maxvalue);
// 		line->SetLineWidth(3);
// 		line->SetLineColor(2);
// 	    TCanvas* yaligncanvas= new TCanvas("yaligncanvas","Alignment in y Direction");
// 	    yaligncanvas->cd();
// 	    yalignment->DrawCopy();
// 		line->Draw("same");
// 	    yaligncanvas->SaveAs("AlignmentY.png");
// 	}
// 	delete yalignment;
// }

void Alignment::save_alignment() {
    AlignmentFile = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"Alignment").c_str(), "RECREATE"));
    if (!AlignmentFile)std::cerr << "[ERROR] Cannot create Alignment File!" << std::endl;
    else if (AlignmentFile->IsZombie()) std::cerr << "[ERROR] Error opening Alignment File!" << std::endl;
    
    else{
        if (AlignmentData!=NULL) {
            AlignmentData->Delete();
            std::cout << "[ALIGNMENT] Overwriting old Alignment Tree!" << std::endl;
        }
        else if (!AlignmentData){
            AlignmentData = new TTree ("AlignmentConstants","Alignment constants");
            AlignmentData->Branch("RotationAngle", &angle, "Angle/D");
            AlignmentData->Branch("x_shift", &x_shift, "xshift/D");
            AlignmentData->Branch("y_shift", &y_shift, "yshift/D");
            AlignmentData->Branch("DUT_zPosition", &DUT_zPosition, "DUT_zPosition/D");
            AlignmentData->Branch("No_of_events", &n_events, "No_of_events/I");
            
            AlignmentData->Fill();
            AlignmentFile->Write();
            AlignmentFile->Close();
//            std::cout << "[ALIGNMENT] Alignment Data successfully written to Datafile!" << std::endl;
        }
        
    }
}


void Alignment::get_alignment(std::string filename){
    if (AlignmentFile!=NULL){
        AlignmentFile->Close();
        delete AlignmentFile;
    }
    
    AlignmentFile = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"Alignment").c_str()));
    if (!AlignmentFile) std::cerr << "[ERROR] Cannot open Alignment File!" << std::endl;
    else {
//        std::cout << "[ALIGNMENT] Alignment File successfully opened!" << std::endl;
        
        
        AlignmentFile->GetObject("AlignmentConstants", AlignmentData);
        
        TBranch* RotationAngle =AlignmentData->GetBranch("RotationAngle");
        TBranch* x_shift_br = AlignmentData->GetBranch("x_shift");
        TBranch* y_shift_br = AlignmentData->GetBranch("y_shift");
        TBranch* DUT_zPosition_br = AlignmentData->GetBranch("DUT_zPosition");
        TBranch* No_of_events = AlignmentData->GetBranch("No_of_events");
        
        
        RotationAngle->SetAddress(&angle);
        x_shift_br->SetAddress(&x_shift);
        y_shift_br->SetAddress(&y_shift);
        DUT_zPosition_br->SetAddress(&DUT_zPosition);
        No_of_events->SetAddress(&n_events);
        
        AlignmentData->GetEntry(0);
//        std::cout << "[ALIGNMENT] Alignment File successfully read!" << std::endl;
    }
}

void Alignment::align_sensor(Config& myconfig, int howmanyevents, bool draw) {
    Alignment::do_alignment_angular(myconfig, howmanyevents, draw);
	// do_alignment_z(myconfig, howmanyevents, draw);
    Alignment::do_alignment_x(myconfig, howmanyevents, draw);
    // Alignment::do_alignment_y(howmanyevents, draw);
	this->y_shift = 15.04;
    Alignment::save_alignment();
	std::cout << "The alignment Constants are: " << std::endl
		<< "Angle: " << this->angle << std::endl
			<< "Z Position: " << this->DUT_zPosition << std::endl
				<< "X Position: " << this->x_shift << std::endl
					<< "Y Position: " << this->y_shift << std::endl;
}

