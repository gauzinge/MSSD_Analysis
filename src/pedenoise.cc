//
//  pedenoise.cc
//  
//
//  Created by Georg Auzinger on 06.06.13.
//
//

#include "pedenoise.h"

void get_pede_noise (std::string datafilename, bool cm, float hitrejectcut, std::string algorithm, int n_cm_loops, bool write)
{
    
	Data mydata;
	mydata.readtree_raw(datafilename);
    
	TProfile* prelimiprofile = new TProfile("preliminaryProfile","preliminaryProfile",384,0.5,384.5);
	prelimiprofile->SetErrorOption("s");
	TProfile* pedeprofile = new TProfile("pedeProfile","pedeProfile",384,0.5,384.5);
	pedeprofile->SetErrorOption("s");
	std::cout << "[PEDENOISE] Running preliminary loop to estimate noise for hit rejection!" << std::endl;
    
	//prelim Noise loop
	for (unsigned int i=0; i<mydata.get_n_entries();i++)
	{
		mydata.get_raw(i);
        
		for (int strip=0; strip<384;strip++) {
            
			prelimiprofile->Fill(strip+1,mydata.adc[strip]);
            
		}
	}
    
	std::vector<float> prelimnoise(384,0);
	std::vector<float> prelimpede(384,0);
    
	for (int strip=0;strip<384;strip++)
	{
        
		prelimnoise.at(strip) = prelimiprofile->GetBinError(strip+1);
		prelimpede.at(strip) = prelimiprofile->GetBinContent(strip+1);
        
	}
	delete prelimiprofile;
	std::cout << "[PEDENOISE] Done estimating Noise. The hit rejection cut is " << hitrejectcut << " x noise!" << std::endl;
    
    
    
	//Pedestal & Noise Loop
	std::cout << "[PEDENOISE] Running loop to calculate Pedestal and Noise!" << std::endl;
    
	for (unsigned int i=0; i<mydata.get_n_entries();i++) //event loop
	{
		mydata.get_raw(i);
        
		for (int strip=0; strip<384;strip++) // strip loop
		{     
			if (fabs(mydata.adc[strip] - prelimpede.at(strip)) < hitrejectcut * prelimnoise.at(strip)) //not hit (in both "directions")
			{
				pedeprofile->Fill(strip+1, mydata.adc[strip]);
			}
		}
	}
    
	std::vector<float> pedestal(384,0);
	std::vector<float> noise(384,0);
    
	for (int strip=0;strip<384;strip++)
	{
		noise.at(strip) = pedeprofile->GetBinError(strip+1);
		pedestal.at(strip) = pedeprofile->GetBinContent(strip+1);
	}
	delete pedeprofile;
	 
	std::cout << "[PEDENOISE] Done with calculating Pedestal and Noise!" << std::endl;
    
	//IF COMMON MODE CORRECTION
    
	if (cm) {
        
		for (int cm_iter = 0; cm_iter < n_cm_loops; cm_iter++)
		{
			std::cout << "[PEDENOISE] Running loop Number " << cm_iter+1 << " for Common Mode Correction!" << std::endl;
        
			TProfile* cm_pedeprofile = new TProfile("cm_pedestalProfile","Common Mode Pedestal Profile",384,0.5,384.5);
			cm_pedeprofile->SetErrorOption("s");
        
			//Fill for pedestal calculation
			for (unsigned int i=0; i<mydata.get_n_entries();i++) //event loop
			{
				mydata.get_raw(i);
        
				std::vector<float> cm_region(12,0);
				std::vector <int> count(12,0);
        
				// for MEDIAN algorithm
				std::vector<std::vector<float> > cm_region_median;
				
				for (int help=0; help<12; help++)
				{
					std::vector<float> dummy;
					cm_region_median.push_back(dummy);
				}
				// now I hava a std::vector of std::vectors of float for the common mode
        
				for (int strip=0; strip<384;strip++) // strip loop
				{     
					if (fabs(mydata.adc[strip] - pedestal.at(strip)) < hitrejectcut * noise.at(strip)) // not hit
					{     
						int region = strip/32;
						if (region >= 0 && region < 12 && strip%32 > 1 && strip%32 < 30) //strip for common mode calculation
						{
							if (algorithm == "MEAN")
							{
								cm_region.at(region) += mydata.adc[strip] - pedestal.at(strip);
								count.at(region)++;
							}
							else if (algorithm == "MEDIAN")
							{
								cm_region_median.at(region).push_back(mydata.adc[strip] - pedestal.at(strip));
							}
							else std::cerr << "[PEDENOISE] Not a valid Algorithm!" << std::endl;
                        
						}
					}
                
				}
				// cm_region_median filled with 28 values per region, now sort
				
				for (int region=0;region<12;region++)
				{
					if (algorithm == "MEAN") cm_region.at(region) /= count[region];
                
					else if (algorithm == "MEDIAN")
					{
						std::sort(cm_region_median.at(region).begin(), cm_region_median.at(region).end());
						int half = cm_region_median.at(region).size()/2;
						if (half != 0) cm_region.at(region) = cm_region_median.at(region).at(half); // this is the MEDIAN per definition!!
						else cm_region.at(region) = 0;
					}
				}

				//here I have calculated the common mode correction for this specific event, now need to fill pedestal histo with common mode corrected adc values
				for (int strip=0; strip<384;strip++) // strip loop
				{ 
					float cm_corrected = mydata.adc[strip]-cm_region.at(strip/32);
                
					if (fabs(cm_corrected - pedestal.at(strip)) < hitrejectcut * noise.at(strip))
					{ // not hit
						cm_pedeprofile->Fill(strip+1,cm_corrected);
					}
				}
			}
			//now I have a 2D histogram with the common mode corrected, hit rejected adc values
        
			for (int strip=0;strip<384;strip++)
			{
				noise.at(strip)       = cm_pedeprofile->GetBinError(strip+1);
				pedestal.at(strip)    = cm_pedeprofile->GetBinContent(strip+1);
			}
			delete cm_pedeprofile;
		}

		//and finally 2 std::vector <float> with the good pedestal and noise values
		std::cout << "[PEDENOISE] Im done calculating the common mode!" << std::endl;
	}
	
	if (write)
	{
		TFile* pedenoisefile = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"PedeNoise").c_str(),"RECREATE"));
		if (!pedenoisefile) std::cerr << "[ERROR] Cannot create PedeNoise File!" << std::endl;
		else 
		{
			float DUTpedestals[390];
			float DUTnoise[390];

			// TTree* pedenoisetree = (TTree*) datafile->Get("DUTPedeNoise");
			TTree* pedenoisetree = new TTree("DUTPedeNoise","Pedestal and Noise Values of the DUT");
			
			pedenoisetree->Branch("PedestalValues", DUTpedestals, "PedestalValues[390]/F");
			pedenoisetree->Branch("NoiseValues", DUTnoise, "NoiseValues[390]/F");
	
			for (unsigned int i = 0; i < pedestal.size(); i++)
			{
				DUTpedestals[i] = pedestal.at(i);
				DUTnoise[i]     = noise.at(i);
			}
			
			pedenoisetree->Fill();
			pedenoisefile->Write();
			pedenoisefile->Close();
			std::cout << "[PEDENOISE] Pedestal and Noise File successfully created!" << std::endl;
		}
	}
	//  //Draw for Debug
	//  
	//  TGraph* prelimnoisegraph = new TGraph();
	//  prelimnoisegraph->SetMarkerColor(1);
	//  prelimnoisegraph->SetMarkerStyle(20);
	//  prelimnoisegraph->SetMarkerSize(0.5);
	// 
	//  TGraph* prelimpedegraph = new TGraph();
	//  prelimpedegraph->SetMarkerColor(1);
	//  prelimpedegraph->SetMarkerStyle(20);
	//  prelimpedegraph->SetMarkerSize(0.5);
	// 
	//  TGraph* noisegraph = new TGraph();
	//  noisegraph->SetMarkerColor(2);
	//  noisegraph->SetMarkerStyle(20);
	//  noisegraph->SetMarkerSize(0.5);
	// 
	//  TGraph* pedegraph = new TGraph();
	//  pedegraph->SetMarkerColor(2);
	//  pedegraph->SetMarkerStyle(20);
	//  pedegraph->SetMarkerSize(0.5);
	// 
	// 
	//  TLegend* legend = new TLegend(0.590517,0.641949,0.900862,0.900424,"","NDC");
	//  legend->AddEntry(prelimnoisegraph,"no CM","p");  
	//  legend->AddEntry(noisegraph,"TBAnalysis","p");
	// 	
	//  for (int strip=0; strip<384; strip++) {
	//      prelimnoisegraph->SetPoint(strip,strip,prelimnoise.at(strip));
	//      prelimpedegraph->SetPoint(strip,strip, prelimpede.at(strip));
	//      
	//      noisegraph->SetPoint(strip,strip, noise.at(strip));
	//      pedegraph->SetPoint(strip, strip, pedestal.at(strip));
	//  }
	//  
	//  TCanvas* pedenoisecanvas_1 = new TCanvas("pedenoisecanvas_1","pedenoisecanvas_1");
	//  TCanvas* pedenoisecanvas_2 = new TCanvas("pedenoisecanvas_2","pedenoisecanvas_2");
	//  
	//  pedenoisecanvas_1->cd();
	//  prelimnoisegraph->Draw("a p");
	//  noisegraph->Draw("p same");
	//  legend->Draw("same");
	//  
	//  pedenoisecanvas_2->cd();
	//  prelimpedegraph->Draw("a p");
	//  pedegraph->Draw("p same");
	//  legend->Draw("same");
	// 
	// pedenoisecanvas_1->SaveAs("Noise.root");
	// pedenoisecanvas_2->SaveAs("Pedestal.root");
}

void calibrate_event(std::string datafilename, bool cm, float hitrejectcut, std::string algorithm)
{
	float calibrated_data[390];
    Data mydata;
    mydata.readtree_raw(datafilename);
	
	TFile* pedefile = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"PedeNoise").c_str()));	
	
	if (!pedefile) std::cerr << "[ERROR] Missing Pedefile!" << std::endl;
	else  
	{		
		float pedestals[390];
		float noise[390];
		std::cout << "[CALIBRATE EVENT] Reading PedeNoise File!" << std::endl;
		TTree* pedetree = (TTree*) pedefile->Get("DUTPedeNoise");
		TBranch* pedestals_br = pedetree->GetBranch("PedestalValues");
		TBranch* noise_br = pedetree->GetBranch("NoiseValues");
		pedestals_br->SetAddress(pedestals);
		noise_br->SetAddress(noise);
		
		if (pedetree != NULL)
		{
			pedetree->GetEntry(0);

			std::cout << "[CALIBRATE EVENT] Writing Signal file " << change_filetype(datafilename,"Signal") << " with Calibrated Event Data!" << std::endl;
			TFile* signalfile = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"Signal").c_str(),"RECREATE"));
			TTree* signaltree = new TTree("CalibratedEventData","Tree containing calibrated event data");
			signaltree->Branch("Signal",calibrated_data,"Signal[390]/F");
			
			// for (unsigned int i=0; i<datatree->GetEntries();i++)
			for (unsigned int i=0; i<mydata.get_n_entries();i++)
			{
				mydata.get_raw(i);
				
				//MEAN
				std::vector<float> cm_region(12,0);
				std::vector <int> count(12,0);

				//MEDIAN
				std::vector<std::vector<float> > cm_region_median;
	
				for (int help=0; help<12; help++)
				{
					std::vector<float> dummy;
					cm_region_median.push_back(dummy);
				}

				for (int strip=0; strip<384;strip++) // strip loop
				{     
					if (fabs(mydata.adc[strip] - pedestals[strip]) < hitrejectcut * noise[strip]) // not hit
					{     
						int region = strip/32;
						if (region >= 0 && region < 12 && strip%32 > 1 && strip%32 < 30) //strip for common mode calculation
						{
							if (algorithm == "MEAN")
							{
								cm_region.at(region) += mydata.adc[strip] - pedestals[strip];
								count.at(region)++;
							}
							else if (algorithm == "MEDIAN")
							{
								cm_region_median.at(region).push_back(mydata.adc[strip] - pedestals[strip]);
							}
							else std::cerr << "[PEDENOISE] Not a valid Algorithm!" << std::endl;
               
						}
					}
       
				}
				// cm_region_median filled with 28 values per region, now sort
	
				for (int region=0;region<12;region++)
				{
					if (algorithm == "MEAN") cm_region.at(region) /= count.at(region);
       
					else if (algorithm == "MEDIAN")
					{
						std::sort(cm_region_median.at(region).begin(), cm_region_median.at(region).end());
						int half = cm_region_median.at(region).size()/2;
						if (half != 0) cm_region.at(region) = cm_region_median.at(region).at(half); 
						else cm_region.at(region) = 0;
					}
				}

				for (int strip=0; strip<384;strip++) // strip loop
				{ 
					calibrated_data[strip] = mydata.adc[strip] - pedestals[strip] - cm_region.at(strip/32);
					// std::cout << calibrated_data[strip] << std::endl;
				}
				signaltree->Fill();
			}
			signalfile->Write();
			signalfile->Close();
			pedefile->Close();
			// TFile::AsyncOpen(change_filetype(datafilename,"PedeNoise"));
			// pedefile->Delete();
			// signalfile->Delete();
			std::cout << "[CALIBRATE EVENT] Signal File successfully written!" << std::endl;
		}
	}		
}