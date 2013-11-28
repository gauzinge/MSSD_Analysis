#include "efficiency.h"


void calculate_efficiency(Config& myconfig, std::vector<float> resolution_vector)
{
	std::vector<int> Neighbours (12,0);
	std::cout << "Calculating number of Neighbors ..." << std::endl;
	//both in micron
	for (int i = 0; i < 12; i++)
	{
		Neighbours.at(i) = ceil(resolution_vector.at(i)*sqrt(12)/get_pitch_region(i+1))+1;
		std::cout << "Region " << i+1 << " Resolution [micron] " << resolution_vector.at(i) << " pitch " << get_pitch_region(i+1) << " n Neighbors " << Neighbours.at(i) << std::endl;
	}
	std::cout << "Calculating Efficiency ..." << std::endl;
	
	TGraph* efficiencygraph =  new TGraph();
	TGraph* occupancygraph =  new TGraph();
	
	TLegend* legend = new TLegend(0.734326,0.799223 ,0.90047,0.898964,"","NDC");
	legend->AddEntry(efficiencygraph,"Efficiency","p");
	legend->AddEntry(occupancygraph,"Occupancy","p");
	
	//Histograms for not found Tracks
	
	TH1D* xintercept = new TH1D("xintercept", "xintercept", 50,-20,20);
	TH1D* xintercept_fake = new TH1D("xintercept_fake", "xintercept_fake", 50,-20,20);
	xintercept_fake->SetLineColor(2);
	
	TH1D* yintercept = new TH1D("yintercept", "yintercept", 50,-20,20);
	TH1D* yintercept_fake = new TH1D("yintercept_fake", "yintercept_fake", 50,-20,20);
	yintercept_fake->SetLineColor(2);
	
	TH1D* xslope = new TH1D("xslope", "xslope", 50,-0.05,0.05);
	TH1D* xslope_fake = new TH1D("xslope_fake", "xslope_fake", 50,-0.05,0.05);
	xslope_fake->SetLineColor(2);
	
	TH1D* yslope = new TH1D("yslope", "yslope", 50,-0.05,0.05);
	TH1D* yslope_fake = new TH1D("yslope_fake", "yslope_fake", 50,-0.05,0.05);
	yslope_fake->SetLineColor(2);
	
	std::vector<int> trackcounter (12,0);
	std::vector<int> foundcounter (12,0);
	std::vector<int> notfoundcounter (12,0);
	std::vector<int> fakefoundcounter (12,0);
	std::vector<double> noiseoccupancy (12,0);
	std::vector<int> countvector (12,0);
	
	//2D Histogram to illustrate inefficient regions or noise clusters
	TH2D* badregions = new TH2D("inefficient_regions", "Inefficient Regions", 400,0,65,400,-20,40);
	badregions->GetXaxis()->SetTitle("x [mm]");
	badregions->GetYaxis()->SetTitle("y [mm]");
	badregions->SetMarkerColor(1);
	TH2D* noisehits = new TH2D("Noisehits", "Noisehits", 400,0,65,400,-20,40);
	noisehits->GetXaxis()->SetTitle("x [mm]");
	noisehits->GetYaxis()->SetTitle("y [mm]");
	noisehits->SetMarkerColor(2);
	
	//finished initializing histograms, now create matchfile
	
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        Track mytrack;
        mytrack.readtree(filenames->trackfilename);
        Data mydata;
        mydata.readtree(filenames->datafilename);
        Alignment myalignment(filenames->datafilename, filenames->trackfilename);
        myalignment.get_alignment(filenames->datafilename);
        EventCollection mycollection;
		
		onTrackCluster myontrackcluster; //for cuts
        myontrackcluster.readtree(filenames->datafilename);
		myontrackcluster.get_entry(0); 
		std::vector<float> cuts = myontrackcluster.cut_vector;
		
        for (unsigned int i=0; i<mydata.get_n_entries(); i++) 
		{
            mydata.get_entry(i);
            mycollection.addHit(i, mydata.EventNumber);
        }
        std::string matchfilename = myconfig.directory + "Matchfiles/";
        size_t last_int= filenames->datafilename.find("_MSSD");
        matchfilename += filenames->datafilename.substr(last_int-4,4);
        matchfilename += "_EfficiencyNew.txt";
        
        std::ofstream binaryMatchFile(matchfilename.c_str());
        mycollection.Serialize(binaryMatchFile);
        
        std::ifstream inFile(matchfilename.c_str());
        mycollection.DeSerialize(inFile);
		
		//now I have the matchfile
		
        for (unsigned int ntrack=0; ntrack<mytrack.get_n_entries(); ntrack++)
        {
            mytrack.get_entry(ntrack);
            mytrack.apply_alignment(myalignment.angle, myalignment.x_shift, myalignment.y_shift, myalignment.DUT_zPosition);
            
            const std::vector<int>& eventTracks = mycollection.GetHitLines(mytrack.EventNumber);
            std::vector<int>::const_iterator it;
            
            for(it = eventTracks.begin() ; it!= eventTracks.end(); ++it)
            {
                const int& hitLine = *it;
				
                int nearest_strip = reverse_hitmaker(mytrack.xPrime); 
                int region = get_region(mytrack.xPrime)-1;
				//here I have all relevant parameters
				//check if the track is in the allowed window
                if (track_good_strips(nearest_strip) && (track_good(mytrack.xPrime, mytrack.yPrime,0) == 1) && track_good_y(mytrack.yPrime,myconfig.y_min,myconfig.y_max))
                {				
						
					//increment good track counter
					trackcounter.at(region)++;
	                mydata.get_entry(hitLine);
					
					//flag for track detected or not
					bool found = false;
					// find a cluster object first in up direction
                    for (int nstrip = nearest_strip; nstrip<=nearest_strip+Neighbours.at(region); nstrip++) 
					{
                      if (mydata.signal[nstrip] > cuts.at(region))found = true;
					  //this means that I have found one strip in the window that exceed the threshold and therefore i detected the track
                    }
                    // ... and then down
                    for (int nstrip = nearest_strip-1; nstrip>=nearest_strip-Neighbours.at(region); nstrip--) 
					{
                      if (mydata.signal[nstrip] > cuts.at(region)) found = true;
                    }
					
					//if found is still false here it means that i did not find a track
					if (found)
					{
						foundcounter.at(region)++; 
						xintercept->Fill(mytrack.xIntercept_local);
						yintercept->Fill(mytrack.yIntercept_local);
						
						xslope->Fill(mytrack.xSlope_local);
						yslope->Fill(mytrack.ySlope_local);
					}
					else
					{
						notfoundcounter.at(region)++;
						badregions->Fill(mytrack.xPrime,mytrack.yPrime);
						
						xintercept_fake->Fill(mytrack.xIntercept_local);
						yintercept_fake->Fill(mytrack.yIntercept_local);
						
						xslope_fake->Fill(mytrack.xSlope_local);
						yslope_fake->Fill(mytrack.ySlope_local);
					}
					
					//now the occupancy as cross check
					int anti_strip =  get_antistrip(nearest_strip);
					
					//flag for fake cluster found
					bool fakefound = false;
                    for (int nstrip = anti_strip; nstrip<=anti_strip+Neighbours.at(region); nstrip++) 
					{
                      if (mydata.signal[nstrip] > cuts.at(region))fakefound = true;
					  //this means that I have found one strip in the window that exceed the threshold and therefore i detected a fake cluster
                    }
                    // ... and then down
                    for (int nstrip = anti_strip-1; nstrip>=anti_strip-Neighbours.at(region); nstrip--) 
					{
                      if (mydata.signal[nstrip] > cuts.at(region)) fakefound = true;
                    }
					
					if (fakefound)
					{
						fakefoundcounter.at(region)++;
						noisehits->Fill(mytrack.xPrime,mytrack.yPrime);
					}
					//additionally count the number of noisehits in the region
					int first_strip = (12-(region+1)) * 32;
					for (int strip = first_strip + 2; strip < first_strip + 30; strip++)
					{
						if (fabs(nearest_strip - strip) > Neighbours.at(region)) //strips not close to track
						{
							if (mydata.signal[strip] > cuts.at(region)) //found fake track
							{
								noiseoccupancy.at(region)++;
							}
							countvector.at(region)++; 
						} 						
						else strip++;

						// std::cout << "Region " << region +1 << " Counter " << counter << " fake " << noiseoccupancy.at(region) << std::endl;
					}
					// if (counter != 28-1-2*Neighbours) std::cout << counter << " WARNING! Something is wrong with the strip counter!" << std::endl;
					// noiseoccupancy.at(region) /= static_cast<double>(counter);
				}
				
			}
		}
	}
	//here everything is filled and I can plot and make my calculations with the vectors
	//first calculate efficiency etc.
	for (int i = 0; i < 12; i++)
	{
		if (trackcounter.at(i) != 0)
		{
			int window = 1 + 2*Neighbours.at(i);
			//probability of fake hit
			double prob_hit = 1-(noiseoccupancy.at(i)/static_cast<double>(countvector.at(i))); // prob of a true hit = 1-fake/strips
			double prob_fake = 1 - pow(prob_hit,window); // this is the probability for a fake hit in w strips, equivalent to a hit in the antiwindow, but more general
			double efficiency = ((static_cast<double>(foundcounter.at(i))/static_cast<double>(trackcounter.at(i))) - prob_fake) / (1-prob_fake);
			double occupancy = (noiseoccupancy.at(i)/static_cast<double>(countvector.at(i)));
			myconfig.mydetector.efficiency.at(i) = efficiency * 100;
			myconfig.mydetector.occupancy.at(i) = occupancy * 100;
			
			efficiencygraph->SetPoint(efficiencygraph->GetN(), i+1, efficiency *100);
			occupancygraph->SetPoint(occupancygraph->GetN(), i+1, occupancy *100);
		
			if (fabs(trackcounter.at(i) - (foundcounter.at(i) + notfoundcounter.at(i))) > 2) std::cout << "WARNING! Efficinecy and Inefficiency do not add up to 100%!" << std::endl;
			std::cout << "Region " << i+1 << " Efficiency " << efficiency*100 << " Occupancy " <<  occupancy * 100 << " N Tracks " << trackcounter.at(i) << " N found " << foundcounter.at(i) << " not fournd " << notfoundcounter.at(i) << " fake (all other strips) " << noiseoccupancy.at(i) << std::endl; 
		}
		else
		{
			myconfig.mydetector.efficiency.at(i) = -999;
			myconfig.mydetector.occupancy.at(i) = -999;
			std::cout << "Region " << i+1 << " NO TRACKS!" << std::endl; 
		}
	}
	//Draw the bad regions
	TCanvas* badmap = new TCanvas("badmap","Inefficient regions and noiseclusters");
	badmap->cd();
	badregions->Draw();
	// noisehits->Draw("same");
	
	std::string title = myconfig.foldername;
	title += "/BadStrips_";
	title+=myconfig.detname;
	title+=".png";
	badmap->SaveAs(title.c_str());
	
	//bad track parameters
	TCanvas* trackparams = new TCanvas("trackparams","Track Parameters");
	trackparams->Divide(2,2);
	trackparams->cd(1);
	xintercept->Draw();
	xintercept_fake->Draw("same");
	trackparams->cd(2);
	xslope->Draw();
	xslope_fake->Draw("same");
	trackparams->cd(3);
	yintercept->Draw();
	yintercept_fake->Draw("same");
	trackparams->cd(4);
	yslope->Draw();
	yslope_fake->Draw("same");
	
	title = myconfig.foldername;
	title += "/BadTrackParameters.png";
	trackparams->SaveAs(title.c_str());
    
	//efficiencygraph
	TCanvas* Efficiency = new TCanvas("Efficiency", "Efficiency");
	
	efficiencygraph->SetMarkerSize(2);
	efficiencygraph->SetMarkerStyle(33);
	efficiencygraph->SetMarkerColor(38);
	
	occupancygraph->SetMarkerSize(2);
	occupancygraph->SetMarkerStyle(33);
	occupancygraph->SetMarkerColor(2);
	
	TMultiGraph* effocc = new TMultiGraph();
	effocc->SetTitle("Efficiency/Occupancy;Region;Efficiency/Occupancy [%]");
	effocc->Add(efficiencygraph,"p");
	effocc->Add(occupancygraph,"p");
	Efficiency->cd();
	effocc->Draw("A");
	legend->Draw("same");
	
	title = myconfig.foldername;
	title += "/Efficiency_";
	title += myconfig.detname;
	title += ".png";
	Efficiency->SaveAs(title.c_str());
	
	myconfig.plotfile->WriteTObject(Efficiency,"","Overwrite");
	myconfig.plotfile->WriteTObject(badmap,"Map of bad Regions","Overwrite");
}



void calculate_efficiency_alternative(Config& myconfig)
{
	gStyle->SetOptStat(111111111);
	//match clusters and tracks
	//fit dummy with gauss and extract sigma
	//2nd loop:
	//fill actual residual distribution
	//with condition that cluster and track are within three gaussian sigmas <-> range +-3 sigma
	//this is n_A
	//calculate antistrip, hitmaker antistrip
	//fill second distribution with noise clusters within 3 sigma of the fake track
	//this is n_B
	
	std::vector<TH1D*> resolution_histo_dummy;
	std::vector<TH1D*> resolution_histo;
	std::vector<TH1D*> off_track_histo;
	std::vector<int> trackcounter (12,0);
	
	for (unsigned int index = 1; index<=12 ; index++) 
	{
		std::string title="Resolution Dummy Region ";
		char buffer[3];
        
		sprintf(buffer,"%i", index);
		title+=buffer;
		TH1D* resolution = new TH1D(title.c_str(),title.c_str(), 1000,-.5,.5);
		resolution->GetXaxis()->SetTitle("Residuals [mm]");
		resolution_histo_dummy.push_back(resolution);
	}
	
	for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
	{
		Track mytrack;
		Cluster mycluster;
		Alignment myalignment(filenames->datafilename, filenames->trackfilename);
		mycluster.readtree(filenames->datafilename);
		mytrack.readtree(filenames->trackfilename);
		myalignment.get_alignment(filenames->datafilename);
				
		//Matchfile & Event Collection should alrady exist, nevertheless, recreate
		EventCollection mycollection;
		//Fill with Cluster Data
		for (unsigned int i=0; i<mycluster.get_n_entries(); i++) {
			mycluster.get_entry(i);
			mycollection.addHit(i, mycluster.EventNumber);
		}
        
		std::string matchfilename = myconfig.directory + "Matchfiles/";
		size_t last_int= filenames->datafilename.find("_MSSD");
		matchfilename += filenames->datafilename.substr(last_int-4,4);
		matchfilename += "_Efficiency.txt";
        
        
		std::ofstream clusterMatchFile(matchfilename.c_str());
		mycollection.Serialize(clusterMatchFile);
        
		std::ifstream inFile(matchfilename.c_str());
		mycollection.DeSerialize(inFile);
			
		std::cout << "Filling a-priori residual distribution to find the Gaussian Sigma!" << std::endl;
		
		for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
		{
			mytrack.get_entry(j);
			mytrack.apply_alignment(myalignment.angle,myalignment.x_shift, myalignment.y_shift, myalignment.DUT_zPosition);
			
			//check if Track OK
			if ( get_region(mytrack.xPrime)-1 >= 0 && track_good(mytrack.xPrime, mytrack.yPrime, 2) == 1 && track_good_y(mytrack.yPrime, myconfig.y_min, myconfig.y_max)) 
			{
				int region = get_region(mytrack.xPrime) -1;
				//yes, so lets find the clusters from this event
				const std::vector<int>& eventHits = mycollection.GetHitLines(mytrack.EventNumber);
				std::vector<int>::const_iterator it;
            
				for(it = eventHits.begin() ; it!= eventHits.end(); ++it)
				{
					const int& hitLine = *it;
					mycluster.get_entry(hitLine);
					
					resolution_histo_dummy.at(region)->Fill(mytrack.xPrime-mycluster.xPosition);
				}
			
			}
		}
	} // done with the initial loop to fill the histograms
	//now fit and create new histograms with the appropriate range
	for (unsigned int index = 0; index<12 ; index++) 
	{
		std::string title="OnTrackResiduals_";
		std::string title1="OffTrackResiduals_";
		char buffer[3];
		sprintf(buffer,"%i", index);
		title+=buffer;
		title1+=buffer;
		
		TF1* fit =  new TF1 (buffer, "gaus", -0.5,0.5);
		resolution_histo_dummy.at(index)->Fit(fit,"RNQ");
		int nsigma = 5;
		double binwidth = 0.001;
		int nbins = 2*nsigma*fit->GetParameter(2)*(1/binwidth);
		double lower = 	fit->GetParameter(1)-nsigma*fit->GetParameter(2);
		double upper = fit->GetParameter(1)+nsigma*fit->GetParameter(2);
		
		std::cout << "Range " << lower << " " << upper << " " << nbins << " " << binwidth << std::endl; 
		TH1D* ontrack_dummy = new TH1D(title.c_str(),title.c_str(), nbins,lower,upper);
		TH1D* offtrack_dummy = new TH1D(title1.c_str(),title1.c_str(), nbins,lower,upper);
		
		// std::cout << index+1 << " " << resolution_histo_dummy.at(index)->DoIntegral(-5*fit->GetParameter(2),5*fit->GetParameter(2)) << std::endl;
		resolution_histo.push_back(ontrack_dummy);
		off_track_histo.push_back(offtrack_dummy);
		delete fit;
	}
	
	//now fill the new distribution with the right residuals in the right range
	for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
	{
		Track mytrack;
		Cluster mycluster;
		Alignment myalignment(filenames->datafilename, filenames->trackfilename);
		mycluster.readtree(filenames->datafilename);
		mytrack.readtree(filenames->trackfilename);
		myalignment.get_alignment(filenames->datafilename);
				
		//Matchfile & Event Collection should alrady exist, nevertheless, recreate
		EventCollection mycollection;
		//Fill with Cluster Data
		// for (unsigned int i=0; i<mycluster.get_n_entries(); i++) {
// 			mycluster.get_entry(i);
// 			mycollection.addHit(i, mycluster.EventNumber);
// 		}
        
		std::string matchfilename = myconfig.directory + "Matchfiles/";
		size_t last_int= filenames->datafilename.find("_MSSD");
		matchfilename += filenames->datafilename.substr(last_int-4,4);
		matchfilename += "_Efficiency.txt";
        
		// std::ofstream clusterMatchFile(matchfilename.c_str());
	// 	mycollection.Serialize(clusterMatchFile);
        
		std::ifstream inFile(matchfilename.c_str());
		mycollection.DeSerialize(inFile);
					
		std::cout << "Filling residual distribution!" << std::endl;
		for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
		{
			mytrack.get_entry(j);
			mytrack.apply_alignment(myalignment.angle,myalignment.x_shift, myalignment.y_shift, myalignment.DUT_zPosition);
			
			//check if Track OK
			if ( get_region(mytrack.xPrime)-1 >= 0 && track_good(mytrack.xPrime, mytrack.yPrime, 0) == 1 && track_good_y(mytrack.yPrime, myconfig.y_min, myconfig.y_max)) 
			{
					
				int region = get_region(mytrack.xPrime) -1;
				int nearest_strip = reverse_hitmaker(mytrack.xPrime); 
				int anti_strip =  get_antistrip(nearest_strip);
				double anti_track = hitmaker(anti_strip);
				if (track_good_strips(nearest_strip))
				{
					trackcounter.at(region)++;
				
					//yes, so lets find the clusters from this event
					const std::vector<int>& eventHits = mycollection.GetHitLines(mytrack.EventNumber);
					std::vector<int>::const_iterator it;
					int counter = 0; 
					for(it = eventHits.begin() ; it!= eventHits.end(); ++it)
					{
						const int& hitLine = *it;
						mycluster.get_entry(hitLine);
						if (mycluster.region == region+1){
							counter++;
							resolution_histo.at(region)->Fill(mytrack.xPrime-mycluster.xPosition); // this are all clusters in the range
							off_track_histo.at(region)->Fill(anti_track-mycluster.xPosition); // this gives all noise clusters in the range
						}
					}
				}
				// std::cout << counter << std::endl;
			}
		}
	}
	//here I am done with the procedure, now extract the efficiencey from on and off track distributions
	TCanvas* effcanvas = new TCanvas("effcanvas","effcanvas");
	effcanvas->Divide(4,3);
	for (int i=0; i<12; i++)
	{
		
		double efficiency = (resolution_histo.at(i)->Integral()/static_cast<double>(trackcounter.at(i)));
		double occupancy = off_track_histo.at(i)->Integral()/static_cast<double>(trackcounter.at(i));
		std::cout << "Region " << i+1 << " Efficiency " << efficiency * 100 << " Occupancy " << occupancy * 100 << std::endl;
		// std::cout << "Region " << i+1 << " Integral " << resolution_histo.at(i)->Integral() << " Tracks " << trackcounter.at(i) <<  " Integral Off " << off_track_histo.at(i)->Integral() << std::endl;
		
		effcanvas->cd(i+1);
		resolution_histo.at(i)->SetLineWidth(2);
		resolution_histo.at(i)->SetLineColor(4);
		
		off_track_histo.at(i)->SetLineWidth(2);
		off_track_histo.at(i)->SetLineColor(2);
		
		resolution_histo.at(i)->Draw();
		off_track_histo.at(i)->Draw("same");
	}
}