//
//  ResultPlotter.cc
//  
//
//  Created by Georg Auzinger on 16.11.12.
//
//

#include "ResultPlotter.h"
#include "TLine.h"

ResultPlotter::ResultPlotter(std::string collection, std::string measurement, Settings thesettings, double chi2cut) {
	 
    xmin = 100;
    ymin = 5;
    xmax = ymax = 0;
    this->collection = collection;
    this->measurement = measurement;
    
    for (std::vector<std::string>::iterator files = thesettings.filelist.begin(); files != thesettings.filelist.end(); files++) {
        
        //read resultfiles
        std::string dummy = *files;
        size_t last_int= dummy.find("/");
		std::string dummy2 = dummy.substr(last_int+1/*+7*/,6);
        int dummytestbeam;
		std::istringstream (dummy2) >> dummytestbeam;
				
        Detector mydetector;
        mydetector.read_file(*files);
		
		//some a-posteriori cleanup which is unfortunately necessary
        if (mydetector.particle == "mixed") mydetector.particle = "p+n";
        if (mydetector.name == "FTH200N_24_MSSD_2") mydetector.annealing = 0;
		if (mydetector.name == "FZ200Y_02_MSSD_2") {
			mydetector.fluence = 3e14;
			mydetector.particle = "p";
		}
        if (mydetector.name == "FZ200Y_10_MSSD_2") mydetector.particle = "n", mydetector.fluence = 4.5E14;
        if (mydetector.name == "FZ320N_04_MSSD_1") mydetector.particle = "none", mydetector.fluence = 0;
		
		
        //Scale
        // float scalefactor=1;
        char voltage_buffer[4],fluence_buffer[8], annealing_buffer[8];
        sprintf(voltage_buffer,"%03i",mydetector.voltage);
        sprintf(fluence_buffer,"%1.1e",mydetector.fluence);
        sprintf(annealing_buffer,"%06d",mydetector.annealing);

        std::string detname_long = mydetector.name + "_" + voltage_buffer + "V" + "_" + fluence_buffer + "_" + annealing_buffer;
        if (!thesettings.names.empty()) //names provided
        {
            for (std::set<std::string>::iterator nameit = thesettings.names.begin(); nameit != thesettings.names.end(); nameit++)
            {
                if (*nameit == mydetector.name)
                {
                    std::vector<int> dummyvector = thesettings.regions_for_names[*nameit];
                    for (std::vector<int>::iterator regionit = dummyvector.begin(); regionit != dummyvector.end(); regionit++)
                    {
						
						double chi2;
						if (collection == "onTrackCluster") chi2 = mydetector.onTrackCluster_signal_chisq.at(*regionit-1)/mydetector.onTrackCluster_signal_ndf.at(*regionit-1);
						// else if (collection == "Trackhit") chi2 = mydetector.Trackhit_signal_chisq.at(*regionit-1)/mydetector.Trackhit_signal_ndf.at(*regionit-1);
						else if (collection == "PeakDistance") chi2 = mydetector.PeakDistance_signal_chisq.at(*regionit-1)/mydetector.PeakDistance_signal_ndf.at(*regionit-1);
						else if (collection == "Trackhit") chi2 = 0;												
                        float result;
						float result_2 = 0;
                        float result_error;
                        std::string plottitle;
						std::string plottitle2 = "dummy";
                        if (this->collection == "onTrackCluster") {
                            if (measurement == "SNR") {
                                result=mydetector.onTrackCluster_snr.at(*regionit-1);
                                result_error=mydetector.onTrackCluster_snr_error.at(*regionit-1);
                                plottitle = "SNR ";
                            } else if (measurement == "Resolution") {
                                result=mydetector.onTrackCluster_resolution.at(*regionit-1);
                                result_error=0;
                                plottitle = "Resolution ";
                            } else if (measurement == "Signal") {
                                result=mydetector.onTrackCluster_signal.at(*regionit-1);
                                result_error=mydetector.onTrackCluster_signal_error.at(*regionit-1);
                                plottitle = "Signal ";
                            } else if (measurement == "Width") {
                                result=mydetector.onTrackCluster_width.at(*regionit-1);
                                result_error=0;
                                plottitle = "Clusterwidth ";
                            }
                        }
                        
                        if (this->collection == "Trackhit") {
                            if (measurement == "SNR") {
                                result=mydetector.Trackhit_snr.at(*regionit-1);
                                result_error=mydetector.Trackhit_snr_error.at(*regionit-1);
                                plottitle = "SNR ";
                            } else if (measurement == "Resolution") {
                                result=mydetector.Trackhit_resolution.at(*regionit-1);
                                result_error=0;
                                plottitle = "Resolution ";
                            } else if (measurement == "Signal") {
                                result=mydetector.Trackhit_signal.at(*regionit-1);
                                result_error=mydetector.Trackhit_signal_error.at(*regionit-1);
                                plottitle = "Signal of Region ";
                            } else if (measurement == "Width") {
                                result=mydetector.Trackhit_width.at(*regionit-1);
                                result_error=0;
                                plottitle = "Clusterwidth ";
                            }
                        }
                        
                        if (this->collection == "PeakDistance") {
                            if (measurement == "SNR") {
                                result=mydetector.PeakDistance_snr.at(*regionit-1);
                                result_error=mydetector.PeakDistance_snr_error.at(*regionit-1);
                                plottitle = "SNR ";
                            } else if (measurement == "Signal") {
                                result=mydetector.PeakDistance_signal.at(*regionit-1);
                                result_error=mydetector.PeakDistance_signal_error.at(*regionit-1);
                                plottitle = "Signal ";
                            }
                        }
                        
                        if (measurement == "Noise") {
                            result=mydetector.noise.at(*regionit-1);
                            result_error=0;
                            plottitle = "Average Noise ";
                        } else if (measurement == "Efficiency") {
                            result=mydetector.efficiency.at(*regionit-1);
                            result_error=0;
							result_2 = mydetector.occupancy.at(*regionit-1);
                            plottitle = "Efficiency ";
							plottitle2 = "Occupancy";
                        }
                        char buffer[4];
                        sprintf(buffer,"%i", get_pitch_region(*regionit));
                        plottitle+=buffer;
                        plottitle+=" #mum";
                        
						if (chi2 < chi2cut || chi2 == 0)
						{
	                        DetCurve mycurve(dummytestbeam, mydetector.name, mydetector.material, mydetector.thickness, mydetector.doping, *regionit, mydetector.fluence, mydetector.particle, mydetector.annealing, measurement);
							
	                        DetCurve dummycurve(dummytestbeam, mydetector.name, mydetector.material, mydetector.thickness, mydetector.doping, *regionit, mydetector.fluence, mydetector.particle, mydetector.annealing, "Occupancy");
							
		                    if (this->curve_map.find(mycurve) == this->curve_map.end())//graph does not exist->insert
		                    {
		                        if (result != -999 && result_error != -999 && result > 0) { //sanity check
		                            TGraphErrors* tmp = new TGraphErrors();
		                            tmp->SetTitle(plottitle.c_str());
		                            mycurve.set_properties(tmp);
		                            int np = tmp->GetN();
		                            tmp->SetPoint(np,mydetector.voltage,result);
		                            tmp->SetPointError(np,0,result_error);
		                            this->curve_map[mycurve]=tmp;
									
		                            TGraphErrors* tmp2 = new TGraphErrors();
		                            tmp->SetTitle(plottitle2.c_str());
		                            dummycurve.set_properties(tmp2);
		                            int np2 = tmp2->GetN();
		                            tmp2->SetPoint(np2,mydetector.voltage,result_2);
		                            tmp2->SetPointError(np2,0,0);
		                            this->curve_map[dummycurve]=tmp2;
		                        }
		                        std::cout << "[NEW] Graph with Name " << mydetector.name << " Region " << *regionit << " Fluence " << mydetector.fluence << " Particle " << mydetector.particle << " and Annealing " << mydetector.annealing << " measured in testbeam " << dummytestbeam << " initialized!" << std::endl;
                        
		                        if (mydetector.voltage > xmax) xmax = mydetector.voltage;
		                        if (mydetector.voltage < xmin) xmin = mydetector.voltage;
		                        if (result > ymax) ymax = result;
		                        if (result < ymin) ymin = result;
		                    }
		                    else
		                    {
		                        std::map<DetCurve, TGraphErrors*>::iterator it = this->curve_map.find(mycurve);
		                        if (result != -999 && result_error != -999 && result > 0) {
		                            int np = it->second->GetN();
		                            it->second->SetPoint(np, mydetector.voltage, result);
		                            it->second->SetPointError(np,0,result_error);
		                        }
								
		                        std::map<DetCurve, TGraphErrors*>::iterator it2 = this->curve_map.find(dummycurve);
		                        if (result != -999 && result_error != -999 && result > 0) {
		                            int np2 = it2->second->GetN();
		                            it2->second->SetPoint(np2, mydetector.voltage, result_2);
		                            it2->second->SetPointError(np2,0,0);
		                        }
		                        if (mydetector.voltage > xmax) xmax = mydetector.voltage;
		                        if (mydetector.voltage < xmin) xmin = mydetector.voltage;
		                        if (result > ymax) ymax = result;
		                        if (result < ymin) ymin = result;
		                    }
						}
						else 
						{
		                    std::cout << "[CHI2CUT] Datapoint " << mydetector.name << " Region " << *regionit << " Voltage " << mydetector.voltage << " Testbeam " << dummytestbeam << " not added because the fit exceeded the chi2 cut!" << std::endl;
						}
                    }
                }
            }
        }
        else if (//no names provided: if detector material, doping, fluence, particle matches settings
                 (thesettings.materials.find(mydetector.material) != thesettings.materials.end())
                 && (thesettings.doping.find(mydetector.doping) != thesettings.doping.end())
                 && (thesettings.thickness.find(mydetector.thickness) != thesettings.thickness.end())
                 && (thesettings.fluence.find(mydetector.fluence) != thesettings.fluence.end())
                 && (thesettings.particle.find(mydetector.particle) != thesettings.particle.end())
                 )
        {
            
            for (std::set<int>::iterator regionit = thesettings.regions.begin(); regionit != thesettings.regions.end(); regionit++)
            {
                DetCurve mycurve(dummytestbeam, mydetector.name, mydetector.material, mydetector.thickness, mydetector.doping, *regionit, mydetector.fluence, mydetector.particle, mydetector.annealing, measurement);
				
				double chi2;
				if (collection == "onTrackCluster") chi2 = mydetector.onTrackCluster_signal_chisq.at(*regionit-1)/mydetector.onTrackCluster_signal_ndf.at(*regionit-1);
				// else if (collection == "Trackhit") chi2 = mydetector.Trackhit_signal_chisq.at(*regionit-1)/mydetector.Trackhit_signal_ndf.at(*regionit-1);
				else if (collection == "PeakDistance") chi2 = mydetector.PeakDistance_signal_chisq.at(*regionit-1)/mydetector.PeakDistance_signal_ndf.at(*regionit-1);
				else if (collection == "Trackhit") chi2 = 0;												
				
                float result;
                float result_error;
				float result_2 = 0;
                std::string plottitle;
				std::string plottitle2 = "dummy";
                if (this->collection == "onTrackCluster") {
                    if (measurement == "SNR") {
                        result=mydetector.onTrackCluster_snr.at(*regionit-1);
                        result_error=mydetector.onTrackCluster_snr_error.at(*regionit-1);
                        plottitle = "SNR ";
                    } else if (measurement == "Resolution") {
                        result=mydetector.onTrackCluster_resolution.at(*regionit-1);
                        result_error=0;
                        plottitle = "Resolution ";
                    } else if (measurement == "Signal") {
                        result=mydetector.onTrackCluster_signal.at(*regionit-1);
                        result_error=mydetector.onTrackCluster_signal_error.at(*regionit-1);
                        plottitle = "Signal ";
                    } else if (measurement == "Width") {
                        result=mydetector.onTrackCluster_width.at(*regionit-1);
                        result_error=0;
                        plottitle = "Clusterwidth ";
                    }
                }
                
                if (this->collection == "Trackhit") {
                    if (measurement == "SNR") {
                        result=mydetector.Trackhit_snr.at(*regionit-1);
                        result_error=mydetector.Trackhit_snr_error.at(*regionit-1);
                        plottitle = "SNR ";
                    } else if (measurement == "Resolution") {
                        result=mydetector.Trackhit_resolution.at(*regionit-1);
                        result_error=0;
                        plottitle = "Resolution ";
                    } else if (measurement == "Signal") {
                        result=mydetector.Trackhit_signal.at(*regionit-1);
                        result_error=mydetector.Trackhit_signal_error.at(*regionit-1);
                        plottitle = "Signal ";
                    } else if (measurement == "Width") {
                        result=mydetector.Trackhit_width.at(*regionit-1);
                        result_error=0;
                        plottitle = "Clusterwidth ";
                    }
                }
                
                if (this->collection == "PeakDistance") {
                    if (measurement == "SNR") {
                        result=mydetector.PeakDistance_snr.at(*regionit-1);
                        result_error=mydetector.PeakDistance_snr_error.at(*regionit-1);
                        plottitle = "SNR ";
                    } else if (measurement == "Signal") {
                        result=mydetector.PeakDistance_signal.at(*regionit-1);
                        result_error=mydetector.PeakDistance_signal_error.at(*regionit-1);
                        plottitle = "Signal ";
                    }
                }
                
                if (measurement == "Noise") {
                    result=mydetector.noise.at(*regionit-1);
                    result_error=0;
                    plottitle = "Average Noise ";
                } else if (measurement == "Efficiency") {
                    result=mydetector.efficiency.at(*regionit-1);
                    result_error=0;
					result_2 = mydetector.occupancy.at(*regionit-1);
                    plottitle = "Efficiency ";
					plottitle2 = "Occupancy";
                }
                
                char buffer[3];
                sprintf(buffer,"%i", get_pitch_region(*regionit));
                plottitle+=buffer;
                plottitle+=" ";
                
				if (chi2 < chi2cut || chi2 == 0)
				{
	                DetCurve mycurve(dummytestbeam, mydetector.name, mydetector.material, mydetector.thickness, mydetector.doping, *regionit, mydetector.fluence, mydetector.particle, mydetector.annealing, measurement);
					
	                DetCurve dummycurve(dummytestbeam, mydetector.name, mydetector.material, mydetector.thickness, mydetector.doping, *regionit, mydetector.fluence, mydetector.particle, mydetector.annealing, "Occupancy");
					
	                if (this->curve_map.find(mycurve) == this->curve_map.end())//graph does not exist->insert
	                {
	                    if (result != -999 && result_error != -999 && result > 0) {
	                        TGraphErrors* tmp = new TGraphErrors();
	                        tmp->SetTitle(plottitle.c_str());
	                        mycurve.set_properties(tmp);
	                        int np = tmp->GetN();
	                        tmp->SetPoint(np,mydetector.voltage,result);
	                        tmp->SetPointError(np,0,result_error);
	                        this->curve_map[mycurve]=tmp;
							
	                        TGraphErrors* tmp2 = new TGraphErrors();
	                        tmp2->SetTitle(plottitle2.c_str());
	                        dummycurve.set_properties(tmp2);
	                        int np2 = tmp2->GetN();
	                        tmp2->SetPoint(np2,mydetector.voltage,result_2);
	                        tmp2->SetPointError(np2,0,0);
	                        this->curve_map[dummycurve]=tmp2;
	                    }
                        std::cout << "[NEW] Graph with Name " << mydetector.name << " Region " << *regionit << " Fluence " << mydetector.fluence << " Particle " << mydetector.particle << " and Annealing " << mydetector.annealing << " measured in testbeam " << dummytestbeam << " initialized!" << std::endl;
                    
	                    if (mydetector.voltage > xmax) xmax = mydetector.voltage;
	                    if (mydetector.voltage < xmin) xmin = mydetector.voltage;
	                    if (result > ymax) ymax = result;
	                    if (result < ymin) ymin = result;
	                }
	                else
	                {
	                    std::map<DetCurve, TGraphErrors*>::iterator it = this->curve_map.find(mycurve);
	                    if (result != -999 && result_error != -999 && result > 0) {
	                        int np = it->second->GetN();
	                        it->second->SetPoint(np, mydetector.voltage, result);
	                        it->second->SetPointError(np,0,result_error);
	                    }
	                    std::map<DetCurve, TGraphErrors*>::iterator it2 = this->curve_map.find(dummycurve);
	                    if (result != -999 && result_error != -999 && result > 0) {
	                        int np2 = it2->second->GetN();
	                        it2->second->SetPoint(np2, mydetector.voltage, result_2);
	                        it2->second->SetPointError(np2,0,0);
	                    }
	                    if (mydetector.voltage > xmax) xmax = mydetector.voltage;
	                    if (mydetector.voltage < xmin) xmin = mydetector.voltage;
	                    if (result > ymax) ymax = result;
	                    if (result < ymin) ymin = result;
	                }
				}
				else 
				{
                    std::cout << "[CHI2CUT] Datapoint " << mydetector.name << " Region " << *regionit << " Voltage " << mydetector.voltage << " Testbeam " << dummytestbeam << " not added because the fit exceeded the chi2 cut!" << std::endl;
				}
            }
            
        }
    }
}

ResultPlotter::~ResultPlotter(){//eventually iterate through graph set and delete TGraph*s
}

void ResultPlotter::draw_graphs(std::string settingfilename) {
	std::set<std::string> legend_name_set;
	TMultiGraph* multigraph = new TMultiGraph();
	TMultiGraph* occ_multigraph = new TMultiGraph();
	
	TCanvas* plotcanvas;
	gPad->SetLeftMargin(0.15);
	gPad->SetBottomMargin(0.15);
	gStyle->SetTitleOffset(1,"y");
	//add bands
	// std::map<DetCurve, TGraphAsymmErrors*> bandmap = make_bandgraphs(this->curve_map, false);
	
	for (std::map<DetCurve, TGraphErrors*>::iterator graphit = this->curve_map.begin(); graphit != this->curve_map.end(); graphit++)
	{
		graphit->second->Sort();

		
		if (measurement == "SNR") {
			if (graphit->first.region <= 4)		multigraph->SetTitle("w/p = 0.15;Voltage [V];SNR");
			else if (graphit->first.region > 4 && graphit->first.region <=8) multigraph->SetTitle("w/p = 0.25;Voltage [V];SNR");
			else if (graphit->first.region > 8 && graphit->first.region <=12) multigraph->SetTitle("w/p = 0.35;Voltage [V];SNR");
			
		} else if (measurement == "Signal") {
			if (graphit->first.region <= 4)		multigraph->SetTitle("w/p = 0.15;Voltage [V];Signal [Electrons]");
			else if (graphit->first.region > 4 && graphit->first.region <=8) multigraph->SetTitle("w/p = 0.25;Voltage [V];Signal [Electrons]");
			else if (graphit->first.region > 8 && graphit->first.region <=12) multigraph->SetTitle("w/p = 0.35;Voltage [V];Signal [Electrons]");
			
		} else if (measurement == "Signal_5Strips") {
			if (graphit->first.region <= 4)		multigraph->SetTitle("w/p = 0.15;Voltage [V];Signal [Electrons]");
			else if (graphit->first.region > 4 && graphit->first.region <=8) multigraph->SetTitle("w/p = 0.25;Voltage [V];Signal [Electrons]");
			else if (graphit->first.region > 8 && graphit->first.region <=12) multigraph->SetTitle("w/p = 0.35;Voltage [V];Signal [Electrons]");
			
		} else if (measurement == "Noise") {
			if (graphit->first.region <= 4)		multigraph->SetTitle("w/p = 0.15;Voltage [V];Noise [Electrons]");
			else if (graphit->first.region > 4 && graphit->first.region <=8) multigraph->SetTitle("w/p = 0.25;Voltage [V];Noise [Electrons]");
			else if (graphit->first.region > 8 && graphit->first.region <=12) multigraph->SetTitle("w/p = 0.35;Voltage [V];Noise [Electrons]");
			
		} else if (measurement == "Width") {
			if (graphit->first.region <= 4)		multigraph->SetTitle("w/p = 0.15;Voltage [V];Clusterwidt");
			else if (graphit->first.region > 4 && graphit->first.region <=8) multigraph->SetTitle("w/p = 0.25;Voltage [V];Clusterwidth");
			else if (graphit->first.region > 8 && graphit->first.region <=12) multigraph->SetTitle("w/p = 0.35;Voltage [V];Clusterwidth");
			
		} else if (measurement == "Efficiency") {
			if (graphit->first.measurement == "Efficiency"){
				if (graphit->first.region <= 4)		multigraph->SetTitle("w/p = 0.15;Voltage [V];Efficiency [%]");
				else if (graphit->first.region > 4 && graphit->first.region <=8) multigraph->SetTitle("w/p = 0.25;Voltage [V];Efficiency [%]");
				else if (graphit->first.region > 8 && graphit->first.region <=12) multigraph->SetTitle("w/p = 0.35;Voltage [V];Efficiency [%]");
			}
			if (graphit->first.measurement == "Occupancy"){
				if (graphit->first.region <= 4)		occ_multigraph->SetTitle(";;Noise Occupancy [%]");
				else if (graphit->first.region > 4 && graphit->first.region <=8) occ_multigraph->SetTitle(";;Noise Occupancy [%]");
				else if (graphit->first.region > 8 && graphit->first.region <=12) occ_multigraph->SetTitle(";;Noise Occupancy [%]");
			}
		} else if (measurement == "Resolution") {
			if (graphit->first.region <= 4)		multigraph->SetTitle("w/p = 0.15;Voltage [V];Resolution [#mum]");
			else if (graphit->first.region > 4 && graphit->first.region <=8) multigraph->SetTitle("w/p = 0.25;Voltage [V];Resolution [#mum]");
			else if (graphit->first.region > 8 && graphit->first.region <=12) multigraph->SetTitle("w/p = 0.35;Voltage [V];Resolution [#mum]");
			
		} 		
		// if (graphit->first.annealing == 0)
// 		{
			if (graphit->second->GetN()>0 && graphit->first.measurement != "Occupancy") {
				multigraph->Add(graphit->second,"lp");
			}
			if (graphit->first.measurement == "Occupancy")
			{
				occ_multigraph->Add(graphit->second,"lp");
			}
		// }
	}
	TLine* binary =  new TLine(0,23.09,1100,23.09);
	binary->SetLineWidth(2);
	binary->SetLineColor(2);
	// for (std::map<DetCurve, TGraphAsymmErrors*>::iterator graphit = bandmap.begin(); graphit != bandmap.end(); graphit++)
// 	{
		// if (graphit->second->GetN()>0) 
// 		{
// 			multigraph->Add(graphit->second,"3");
// 		}
	// }
	if (measurement == "Efficiency")
	{
	    plotcanvas = new TCanvas(this->measurement.c_str(), this->measurement.c_str(),200,10,600,800);
		TPad *pad1 = new TPad("pad1", "The pad 80% of the height",0.0,0.3,1.0,1.0);
		TPad *pad2 = new TPad("pad2", "The pad 20% of the height",0.0,0.0,1.0,0.3);
		pad1->Draw();
		pad2->Draw();
		pad1->cd();
		multigraph->Draw("a");
		multigraph->GetXaxis()->SetLimits(0, 1100);
		// multigraph->GetYaxis()->SetRangeUser(0.9*ymin, 1.1*ymax);
		gPad->Modified();		
		pad2->cd();
		occ_multigraph->Draw("a");
		occ_multigraph->GetXaxis()->SetLimits(0, 1100);
		occ_multigraph->GetYaxis()->SetTitleOffset(0.5);
		occ_multigraph->GetYaxis()->SetTitleSize(0.09);
		occ_multigraph->GetXaxis()->SetLabelSize(0.1);
		occ_multigraph->GetYaxis()->SetLabelSize(0.1);
		// SetTitleOffset(1,"y")->SetTitleOffset(0.5);SetRangeUser(0.9*ymin, 1.1*ymax);
		gPad->Modified();	
		std::string filename = "Results/Comparison/";
		settingfilename = settingfilename.substr(12, settingfilename.size()-16);
		filename += measurement + settingfilename + ".root";
		plotcanvas->SaveAs(filename.c_str());	
	}
	else 
	{
	    plotcanvas = new TCanvas(this->measurement.c_str(), this->measurement.c_str());
	   	plotcanvas->cd();
		multigraph->Draw("a");
		if (measurement == "Resolution") binary->Draw("same");
		multigraph->GetXaxis()->SetLimits(0, 1100);
		multigraph->GetYaxis()->SetRangeUser(0.9*ymin, 1.1*ymax);
		gPad->Modified();		
		std::string filename = "Results/Comparison/";
		settingfilename = settingfilename.substr(12, settingfilename.size()-16);
		filename += measurement + settingfilename + ".root";
		plotcanvas->SaveAs(filename.c_str());
	}
}


std::map<DetCurve, TGraphAsymmErrors*> ResultPlotter::make_bandgraphs(std::map <DetCurve, TGraphErrors*>& curves, bool del)
{
	std::cout << "Making Bandgraphs! " << std::endl;
	// go through curves and put every curve that matches criteria in separate map
	// loop through that subset if more than 1 entry and calculate mean and max for each voltage +- 10V
	// feed that information to TGraphAsymErrors and plot with option 'f'
	// remove original graphs from curves and add the TGraphAsymErrors? or return a second map and loop over that as well
	std::map <DetCurve, TGraphErrors*> maps[7];
		
	for (std::map<DetCurve, TGraphErrors*>::iterator graphit = curves.begin(); graphit != curves.end(); graphit++)
	{
		//check for combos and put them in submaps; this does not account for different regions!
		if (graphit->first.material == "FZ" && graphit->first.thickness == 320 && graphit->first.doping == "N" && graphit->first.fluence == 0 && graphit->first.annealing == 0) 
		{
			maps[0][graphit->first] =  graphit->second;
			// std::cout << "NEW ENTRY in submap " << graphit->first.material << graphit->first.thickness << graphit->first.doping << " Fluence " << graphit->first.fluence << " and Annealing " << graphit->first.annealing << std::endl;
		}
		if (graphit->first.material == "FZ" && graphit->first.thickness == 200 && graphit->first.doping == "P" && graphit->first.fluence == 0 && graphit->first.annealing == 0) 
		{
			maps[1][graphit->first] =  graphit->second;
			// std::cout << "NEW ENTRY in submap " << graphit->first.material << graphit->first.thickness << graphit->first.doping << " Fluence " << graphit->first.fluence << " and Annealing " << graphit->first.annealing << std::endl;
		}
			
		if (graphit->first.material == "MCZ" && graphit->first.thickness == 200 && graphit->first.doping == "N" && graphit->first.fluence == 0 && graphit->first.annealing == 0)
		{
			maps[2][graphit->first] =  graphit->second; 
			// std::cout << "NEW ENTRY in submap " << graphit->first.material << graphit->first.thickness << graphit->first.doping << " Fluence " << graphit->first.fluence << " and Annealing " << graphit->first.annealing << std::endl;
		}
			
		if (graphit->first.material == "MCZ" && graphit->first.thickness == 200 && graphit->first.doping == "N" && graphit->first.fluence != 0 && graphit->first.annealing == 0) 
		{
			maps[3][graphit->first] =  graphit->second;
			// std::cout << "NEW ENTRY in submap " << graphit->first.material << graphit->first.thickness << graphit->first.doping << " Fluence " << graphit->first.fluence << " and Annealing " << graphit->first.annealing << std::endl;
		}
			
		if (graphit->first.material == "MCZ" && graphit->first.thickness == 200 && graphit->first.doping == "P" && graphit->first.fluence != 0 && graphit->first.annealing == 0) 
		{
			maps[4][graphit->first] =  graphit->second;
			// std::cout << "NEW ENTRY in submap " << graphit->first.material << graphit->first.thickness << graphit->first.doping << " Fluence " << graphit->first.fluence << " and Annealing " << graphit->first.annealing << std::endl;
		}
			
		if (graphit->first.material == "MCZ" && graphit->first.thickness == 200 && graphit->first.doping == "Y" && graphit->first.fluence != 0 && graphit->first.annealing == 0) 
		{
			maps[5][graphit->first] =  graphit->second;
			// std::cout << "NEW ENTRY in submap " << graphit->first.material << graphit->first.thickness << graphit->first.doping << " Fluence " << graphit->first.fluence << " and Annealing " << graphit->first.annealing << std::endl;
		}
			
		if (graphit->first.material == "FZ" && graphit->first.thickness == 200 && graphit->first.doping == "Y" && graphit->first.fluence == 1.5e15 && graphit->first.annealing == 0) 
		{
			maps[6][graphit->first] =  graphit->second;
			// std::cout << "NEW ENTRY in submap " << graphit->first.material << graphit->first.thickness << graphit->first.doping << " Fluence " << graphit->first.fluence << " and Annealing " << graphit->first.annealing << std::endl;
		}
	}
	
	if (del)
	{
		for (std::map<DetCurve, TGraphErrors*>::iterator graphit = curves.begin(); graphit != curves.end(); graphit++)
		{
			if (graphit->first.material == "FZ" && graphit->first.thickness == 320 && graphit->first.doping == "N" && graphit->first.fluence == 0 && graphit->first.annealing == 0) 
			{
				curves.erase(graphit++);
			}
			if (graphit->first.material == "FZ" && graphit->first.thickness == 200 && graphit->first.doping == "P" && graphit->first.fluence == 0 && graphit->first.annealing == 0) 
			{
				curves.erase(graphit++);
			}
			
			if (graphit->first.material == "MCZ" && graphit->first.thickness == 200 && graphit->first.doping == "N" && graphit->first.fluence == 0 && graphit->first.annealing == 0)
			{
				curves.erase(graphit++);
			}
			
			if (graphit->first.material == "MCZ" && graphit->first.thickness == 200 && graphit->first.doping == "N" && graphit->first.fluence != 0 && graphit->first.annealing == 0) 
			{
				curves.erase(graphit++);
			}
			
			if (graphit->first.material == "MCZ" && graphit->first.thickness == 200 && graphit->first.doping == "P" && graphit->first.fluence != 0 && graphit->first.annealing == 0) 
			{
				curves.erase(graphit++);
			}
			
			if (graphit->first.material == "MCZ" && graphit->first.thickness == 200 && graphit->first.doping == "Y" && graphit->first.fluence != 0 && graphit->first.annealing == 0) 
			{
				curves.erase(graphit++);
			}
			
			if (graphit->first.material == "FZ" && graphit->first.thickness == 200 && graphit->first.doping == "Y" && graphit->first.fluence == 1.5e15 && graphit->first.annealing == 0) 
			{
				curves.erase(graphit++);
			}
		}
	}
	//new map with bandcurves
	std::map<DetCurve, TGraphAsymmErrors*> bandmap;
	
	//now I have submaps
	for (int i = 0; i < 7; i++)
	{
		if (maps[i].size() > 1) // now I am sure that there are more than one curves that match the criteria
		{
			// std::cout << "Found a combination that has been measured multiple times! Looping through voltages!" << std::endl;
			
			std::string material;
			int thickness;
			std::string doping;
			int region;
			float fluence;
			std::string particle;
			int annealing;
			std::string measurement;
			TGraphAsymmErrors* bandgraph = new TGraphAsymmErrors(); //one for every submap
			
			double mean[100];
			double min[100];
			double max[100];
			int counter[100];
			
			for (int dummy = 0; dummy < 100; dummy++)
			{
				mean[dummy] = 0;
				min[dummy] = 30000;
				max[dummy] = 0;
				counter[dummy] = 0;
			}
				
			for (int voltage = 0; voltage < 1000; voltage+=10) //loop over voltage in 10 volt steps
			{
				for (std::map<DetCurve, TGraphErrors*>::iterator it = maps[i].begin(); it != maps[i].end(); it++) //loop through graphs in map
				{
					material = it->first.material;
					thickness = it->first.thickness;
					doping = it->first.doping;
					region = it->first.region;
					fluence = it->first.fluence;
					particle = it->first.particle;
					annealing = it->first.annealing;
					measurement = it->first.measurement;
					
					for (int graphsize = 0; graphsize < it->second->GetN(); graphsize++) //loop through graph
					{
						double x, y;
						it->second->GetPoint(graphsize,x,y);
							
						if (fabs(voltage-x) < 5) // there is a datapoint close to the actual voltage
						{	
							mean[voltage/10] += y;
							counter[voltage/10]++;
							if (y > max[voltage/10]) max[voltage/10] = y;
							if (y < min[voltage/10]) min[voltage/10] = y; //here
							// std::cout << "Found Datapoint # " << counter[voltage/10] << " within +- 5 V from Voltage " << voltage << " (Datapoint " << voltage/10 << " )" << " at x = " << x << " and y = " << y << std::endl;
						}
					}
				}
			}
			// std::cout << "Done filling mean, min and max arrays!" << std::endl;
			//here I should have the arrays filled
			//check if counter is different from 0 and build actual mean
			for (int dummy = 0; dummy < 100; dummy++)
			{
				if (counter[dummy] > 1)
				{
					mean[dummy] = mean[dummy]/counter[dummy];
					
					int np = bandgraph->GetN();
					bandgraph->SetPoint(np,dummy*10,mean[dummy]);
					bandgraph->SetPointError(np,0,0,mean[dummy]-min[dummy]+1,max[dummy]-mean[dummy]+1);
					// std::cout << "The Mean for Voltage " << dummy*10 << " is " << mean[dummy] << " and the minimum/maximum are " << min[dummy] << " / " << max[dummy] << std::endl;
				}
			}
			DetCurve bandcurve(0, "", material, thickness, doping, region, fluence, particle, annealing, measurement);
			bandcurve.set_properties_asymm(bandgraph);
			bandmap[bandcurve] = bandgraph;
		}
	}
	return bandmap;
}


void ResultPlotter::export_data(std::string settingfilename) {
    
    std::stringstream filename;
    filename << "Analysis_Results/Data_" << measurement << ".txt";
    
    std::ofstream outfile;
    outfile.open(filename.str().c_str(),std::ios::trunc);
	
	
    for (std::map<DetCurve, TGraphErrors*>::iterator graphit = this->curve_map.begin(); graphit != this->curve_map.end(); graphit++)
	 {
//        char region_dummy[3];
//        sprintf(region_dummy,"%i",graphit->first.region);
        std::stringstream ss;
        ss << graphit->first.name << " Region " << graphit->first.region << " Fluence " << graphit->first.fluence << " Particle " << graphit->first.particle << " Annealing[min] " << graphit->first.annealing << " @ 20 deg" ;
        std::stringstream voltagestream;
        std::stringstream valuestream;
        std::stringstream errorstream;
        for (int i=0; i<graphit->second->GetN(); i++) {
            double x,y;
            graphit->second->GetPoint(i,x,y);
            if (i==0) {
                voltagestream << "Voltage " << x << " " ;
                valuestream << measurement << " " << y << " " ;
                errorstream << "Error " << graphit->second->GetErrorY(i) << " " ;
            }
            else {
                voltagestream << x << " " ;
                valuestream << y << " " ;
                errorstream << graphit->second->GetErrorY(i) << " " ;
            }
        }
        std::string output = ss.str();
        std::string voltage = voltagestream.str();
        std::string value = valuestream.str();
        std::string error = errorstream.str();
        outfile << output << std::endl << voltage << std::endl << value << std::endl << error << std::endl << std::endl;
        
        ss.clear();
    }
    outfile.close();
}