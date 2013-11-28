//
//  FluencePlotter.cpp
//  
//
//  Created by Georg Auzinger on 14.01.13.
//
//

#include "FluencePlotter.h"
FluencePlotter::FluencePlotter(int Voltage, std::string collection, std::string measurement, Settings thesettings, double chi2cut) {
    
    this->xmin = 0;
    this->xmax = 20;
    this->ymin = 5;
    this->ymax = 0;
    this->collection = collection;
    this->measurement = measurement;
    this->Voltage = Voltage;
    
    for (std::vector<std::string>::iterator files = thesettings.filelist.begin(); files != thesettings.filelist.end(); files++) {
        
        //read resultfiles
        std::string dummy = *files;
        size_t last_int= dummy.find("/");
		std::string dummy2 = dummy.substr(last_int+1,6);
        int dummytestbeam;
		 // = atoi(dummy.substr(last_int+1,6).c_str());
		std::istringstream (dummy2) >> dummytestbeam;
        
        Detector mydetector;
        mydetector.read_file(*files);
        if (mydetector.particle == "mixed") mydetector.particle = "p+n";
        if (mydetector.fluence == 0) mydetector.fluence = 1;
        if (mydetector.name == "FZ200Y_10_MSSD_2") mydetector.particle = "n", mydetector.fluence = 4.5E14;
        if (mydetector.particle =="n") mydetector.fluence = 4.5E14;
        //Scale
        float scalefactor=1;
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
                        DetCurve mycurve(dummytestbeam, mydetector.name, mydetector.material, mydetector.thickness, mydetector.doping, *regionit, mydetector.fluence, mydetector.particle, mydetector.annealing, measurement);
                        
						double chi2;
						if (collection == "onTrackCluster") chi2 = mydetector.onTrackCluster_signal_chisq.at(*regionit-1)/mydetector.onTrackCluster_signal_ndf.at(*regionit-1);
						else if (collection == "Trackhit") chi2 = mydetector.Trackhit_signal_chisq.at(*regionit-1)/mydetector.Trackhit_signal_ndf.at(*regionit-1);
						else if (collection == "PeakDistance") chi2 = mydetector.PeakDistance_signal_chisq.at(*regionit-1)/mydetector.PeakDistance_signal_ndf.at(*regionit-1);
						
                        float result;
                        float result_error;
                        std::string plottitle;
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
                                result=mydetector.onTrackCluster_signal.at(*regionit-1) * scalefactor;
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
                                result=mydetector.Trackhit_signal.at(*regionit-1) * scalefactor;
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
                                result=mydetector.PeakDistance_signal.at(*regionit-1) * scalefactor;
                                result_error=mydetector.PeakDistance_signal_error.at(*regionit-1);
                                plottitle = "Signal ";
                            }
                        }
                        
                        if (measurement == "Noise") {
                            result=mydetector.noise.at(*regionit-1) * scalefactor;
                            result_error=0;
                            plottitle = "Average Noise ";
                        } else if (measurement == "Efficiency") {
                            result=mydetector.efficiency.at(*regionit-1);
                            result_error=0;
                            plottitle = "Efficiency ";
                        }
                        char buffer[5];
                        sprintf(buffer,"%i", get_pitch_region(*regionit));
                        plottitle+=buffer;
                        plottitle+="#mum ";

						if (chi2 < chi2cut)
						{
	                        if (this->curve_map.find(mycurve) == this->curve_map.end())//graph does not exist->insert
	                        {
	                            if (result != -999 && result_error != -999 && (mydetector.voltage > Voltage-25 && mydetector.voltage < Voltage+25)) { //HERE!!!!
	                                TGraphErrors* tmp = new TGraphErrors();
	                                tmp->SetTitle(plottitle.c_str());
	                                mycurve.set_properties(tmp);
	                                int np = tmp->GetN();
	                                tmp->SetPoint(np,mydetector.fluence/1E14,result);
	                                tmp->SetPointError(np,0,result_error);
	                                this->curve_map[mycurve]=tmp;
	                            }
	                            std::cout << "[NEW] Graph with Name " << mydetector.name << " Region " << *regionit << " Fluence " << mydetector.fluence << " and Particle " << mydetector.particle << " initialized!" << std::endl;
                                                       
	                            if (result > ymax) ymax = result;
	                            if (result < ymin) ymin = result;
	                        }
	                        else
	                        {
	                            std::map<DetCurve, TGraphErrors*>::iterator it = this->curve_map.find(mycurve);
	                            if (result != -999 && result_error != -999 && (mydetector.voltage > Voltage-25 && mydetector.voltage < Voltage+25)) {
	                                int np = it->second->GetN();
	                                it->second->SetPoint(np, mydetector.fluence/1E14, result);
	                                it->second->SetPointError(np,0,result_error);
	                            }
	                            if (result > ymax) ymax = result;
	                            if (result < ymin) ymin = result;
	                        }
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
				else if (collection == "Trackhit") chi2 = mydetector.Trackhit_signal_chisq.at(*regionit-1)/mydetector.Trackhit_signal_ndf.at(*regionit-1);
				else if (collection == "PeakDistance") chi2 = mydetector.PeakDistance_signal_chisq.at(*regionit-1)/mydetector.PeakDistance_signal_ndf.at(*regionit-1);
				
                float result;
                float result_error;
                std::string plottitle;
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
                        result=mydetector.onTrackCluster_signal.at(*regionit-1) * scalefactor;
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
                        result=mydetector.Trackhit_signal.at(*regionit-1) * scalefactor;
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
                        result=mydetector.PeakDistance_signal.at(*regionit-1) * scalefactor;
                        result_error=mydetector.PeakDistance_signal_error.at(*regionit-1);
                        plottitle = "Signal ";
                    }
                }
                
                if (measurement == "Noise") {
                    result=mydetector.noise.at(*regionit-1) * scalefactor;
                    result_error=0;
                    plottitle = "Average Noise ";
                } else if (measurement == "Efficiency") {
                    result=mydetector.efficiency.at(*regionit-1);
                    result_error=0;
                    plottitle = "Efficiency ";
                }
                char buffer[5];
                sprintf(buffer,"%i", get_pitch_region(*regionit));
                plottitle+=buffer;
                plottitle+="#mum ";

				if (chi2 < chi2cut)
				{
	                if (this->curve_map.find(mycurve) == this->curve_map.end())//graph does not exist->insert
	                {
	                    if (result != -999 && result_error != -999 && (mydetector.voltage > Voltage-25 && mydetector.voltage < Voltage+25)) {
	                        TGraphErrors* tmp = new TGraphErrors();
	                        tmp->SetTitle(plottitle.c_str());
	                        mycurve.set_properties(tmp);
	                        int np = tmp->GetN();
	                        tmp->SetPoint(np,mydetector.fluence/1E14,result);
	                        tmp->SetPointError(np,0,result_error);
	                        this->curve_map[mycurve]=tmp;
	                    }
	                    std::cout << "[NEW] Graph with Name " << mydetector.name << " Region " << *regionit << " Fluence " << mydetector.fluence << " and Particle " << mydetector.particle << " initialized!" << std::endl;
                    
                  
	                    if (result > ymax) ymax = result;
	                    if (result < ymin) ymin = result;
	                }
	                else
	                {
	                    std::map<DetCurve, TGraphErrors*>::iterator it = this->curve_map.find(mycurve);
	                    if (result != -999 && result_error != -999 && (mydetector.voltage > Voltage-25 && mydetector.voltage < Voltage+25)) {
	                        int np = it->second->GetN();
	                        it->second->SetPoint(np, mydetector.fluence/1E14, result);
	                        it->second->SetPointError(np,0,result_error);
	                    }
                   
	                    if (result > ymax) ymax = result;
	                    if (result < ymin) ymin = result;
	                }
				}
            }
            
        }
    }
}

FluencePlotter::~FluencePlotter(){//eventually iterate through graph set and delete TGraph*s
}

void FluencePlotter::draw_graphs(std::string settingfilename) {
    std::set<std::string> legend_name_set;
    TLegend* legend = new TLegend( 0.607759,0.0995763,0.897989,0.258475,"","NDC");
    legend->SetNColumns(2);
    std::string canvastitle = this->measurement + "_Fluence";
    TCanvas* plotcanvas = new TCanvas(canvastitle.c_str(), canvastitle.c_str());
    
    plotcanvas->cd();
//    plotcanvas->SetLogx();
    for (std::map<DetCurve, TGraphErrors*>::iterator graphit = this->curve_map.begin(); graphit != this->curve_map.end(); graphit++) {
        char region_dummy[3];
        char thickness_dummy[4];
        sprintf(region_dummy,"%i",graphit->first.region);
        sprintf(thickness_dummy,"%i",graphit->first.thickness);
        
        std::string title = graphit->first.material + thickness_dummy + graphit->first.doping + " Region " + region_dummy;
        if (legend_name_set.find(title) == legend_name_set.end()) {
            legend_name_set.insert(title);
            legend->AddEntry(graphit->second, title.c_str(),"p");
        }
                
        graphit->second->GetXaxis()->SetLimits(xmin, xmax);
        graphit->second->GetYaxis()->SetRangeUser(0.9*ymin, 1.1*ymax);
        graphit->second->GetYaxis()->SetTitleOffset(1.5);
        graphit->second->GetXaxis()->SetTitle("Fluence [10^{14} 1 MeV Neq cm^{-2}]");
        if (measurement == "SNR") {
            graphit->second->GetYaxis()->SetTitle("Signal-to-Noise");
        } else if (measurement == "Signal") {
            graphit->second->GetYaxis()->SetTitle("Signal [ADC]");
        } else if (measurement == "Signal_5Strips") {
            graphit->second->GetYaxis()->SetTitle("Signal_5Strips [ADC]");
        } else if (measurement == "Noise") {
            graphit->second->GetYaxis()->SetTitle("Noise [ADC]");
        } else if (measurement == "Width") {
            graphit->second->GetYaxis()->SetTitle("Cluster Width");
        } else if (measurement == "Efficiency") {
            graphit->second->GetYaxis()->SetTitle("Efficiency [%]");
        } else if (measurement == "Resolution") {
            graphit->second->GetYaxis()->SetTitle("Resolution [#mum]");
        }
        
        if (graphit->second->GetN()>0) {
//            graphit->second->GetXaxis()->SetRangeUser(0,100);
//            graphit->second->GetYaxis()->SetRangeUser(0,50);
            if (graphit == this->curve_map.begin()) {
                graphit->second->Draw("AP");
            }
            else graphit->second->Draw("P same");
        }
    }
    legend->Draw();
    std::string filename = "CompareResults/";
    settingfilename = settingfilename.substr(12, settingfilename.size()-16);
    filename += measurement + settingfilename + "_Fluence.root";
    //Think of a way of implementing the SettingfileName
    plotcanvas->SaveAs(filename.c_str());
}
