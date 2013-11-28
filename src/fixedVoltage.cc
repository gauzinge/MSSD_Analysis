//
//   fixedVoltage.cpp
//  
//
//  Created by Georg Auzinger on 14.01.13.
//
//

#include "fixedVoltage.h"


fixedVoltage::fixedVoltage(std::string collection, std::string measurement, Settings thesettings, double chi2cut) {
    
    xmin = 100;
    ymin = 5;
    xmax = ymax = 0;
    this->collection = collection;
    this->measurement = measurement;
    
    for (std::vector<std::string>::iterator files = thesettings.filelist.begin(); files != thesettings.filelist.end(); files++) {
        
        //read resultfiles
        std::string dummy = *files;
        size_t last_int= dummy.find("/");
		std::string dummy2 = dummy.substr(last_int+7,6);
        int dummytestbeam;
		std::istringstream (dummy2) >> dummytestbeam;
				
        Detector mydetector;
        mydetector.read_file(*files);
		
		//some a-posteriori cleanup which is unfortunately necessary
        if (mydetector.particle == "mixed") mydetector.particle = "p+n";
        if (mydetector.name == "FTH200N_24_MSSD_2") mydetector.annealing = 0;
		// if (mydetector.name == "FZ200Y_02_MSSD_2") {
// 			mydetector.fluence = 4e14;
// 			mydetector.particle = "n";
// 		}
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
                            plottitle = "Efficiency ";
                        }
                        char buffer[4];
                        sprintf(buffer,"%i", get_pitch_region(*regionit));
                        plottitle+=buffer;
                        plottitle+=" #mum";
                        
						if (chi2 < chi2cut)
						{
	                        if (this->curve_map.find(mycurve) == this->curve_map.end())//graph does not exist->insert
	                        {
	                            if (result != -999 && result_error != -999) { //sanity check
	                                TGraphErrors* tmp = new TGraphErrors();
	                                tmp->SetTitle(plottitle.c_str());
	                                mycurve.set_properties(tmp);
	                                int np = tmp->GetN();
	                                tmp->SetPoint(np,mydetector.voltage,result);
	                                tmp->SetPointError(np,0,result_error);
	                                this->curve_map[mycurve]=tmp;
	                            }
	                            // std::cout << "[NEW] Graph with Name " << mydetector.name << " Region " << *regionit << " Fluence " << mydetector.fluence << " Particle " << mydetector.particle << " and Annealing " << mydetector.annealing << " measured in testbeam " << dummytestbeam << " initialized!" << std::endl;
                            
	                            if (mydetector.voltage > xmax) xmax = mydetector.voltage;
	                            if (mydetector.voltage < xmin) xmin = mydetector.voltage;
	                            if (result > ymax) ymax = result;
	                            if (result < ymin) ymin = result;
	                        }
	                        else
	                        {
	                            std::map<DetCurve, TGraphErrors*>::iterator it = this->curve_map.find(mycurve);
	                            if (result != -999 && result_error != -999) {
	                                int np = it->second->GetN();
	                                it->second->SetPoint(np, mydetector.voltage, result);
	                                it->second->SetPointError(np,0,result_error);
	                            }
	                            if (mydetector.voltage > xmax) xmax = mydetector.voltage;
	                            if (mydetector.voltage < xmin) xmin = mydetector.voltage;
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
                    plottitle = "Efficiency ";
                }
                char buffer[3];
                sprintf(buffer,"%i", get_pitch_region(*regionit));
                plottitle+=buffer;
                plottitle+=" ";
				
                if (chi2 < chi2cut)
				{
	                if (this->curve_map.find(mycurve) == this->curve_map.end())//graph does not exist->insert
	                {
	                    if (result != -999 && result_error != -999) {
	                        TGraphErrors* tmp = new TGraphErrors();
	                        tmp->SetTitle(plottitle.c_str());
	                        mycurve.set_properties(tmp);
	                        int np = tmp->GetN();
	                        tmp->SetPoint(np,mydetector.voltage,result);
	                        tmp->SetPointError(np,0,result_error);
	                        this->curve_map[mycurve]=tmp;
	                    }
	                            // std::cout << "[NEW] Graph with Name " << mydetector.name << " Region " << *regionit << " Fluence " << mydetector.fluence << " Particle " << mydetector.particle << " and Annealing " << mydetector.annealing << " measured in testbeam " << dummytestbeam << " initialized!" << std::endl;
                    
	                    if (mydetector.voltage > xmax) xmax = mydetector.voltage;
	                    if (mydetector.voltage < xmin) xmin = mydetector.voltage;
	                    if (result > ymax) ymax = result;
	                    if (result < ymin) ymin = result;
	                }
	                else
	                {
	                    std::map<DetCurve, TGraphErrors*>::iterator it = this->curve_map.find(mycurve);
	                    if (result != -999 && result_error != -999) {
	                        int np = it->second->GetN();
	                        it->second->SetPoint(np, mydetector.voltage, result);
	                        it->second->SetPointError(np,0,result_error);
	                    }
	                    if (mydetector.voltage > xmax) xmax = mydetector.voltage;
	                    if (mydetector.voltage < xmin) xmin = mydetector.voltage;
	                    if (result > ymax) ymax = result;
	                    if (result < ymin) ymin = result;
	                }
				}
            }
            
        }
    }
}

fixedVoltage::~fixedVoltage(){//eventually iterate through graph set and delete TGraph*s
}
void fixedVoltage::make_historgram(std::string settingfilename, int voltage)
{
	std::string canvastitle = this->measurement + "_histo";
	TCanvas* plotcanvas = new TCanvas(canvastitle.c_str(), canvastitle.c_str());
	plotcanvas->cd();
	double minsig = 30000;
	double maxsig = 0;
	for (std::map<DetCurve, TGraphErrors*>::iterator graphit = this->curve_map.begin(); graphit != this->curve_map.end(); graphit++)
	{
		graphit->second->Sort();
		//check if voltage is in range of curve
		int n = graphit->second->GetN();
		double *x = graphit->second->GetX();
		int locmin = TMath::LocMin(n,x);
		int locmax = TMath::LocMax(n,x);
		double xmin = x[locmin];
		double xmax = x[locmax];
		
		if (voltage > xmin && voltage < xmax)
		{
			if (graphit->second->Eval(voltage) < minsig) minsig = graphit->second->Eval(voltage);
			if (graphit->second->Eval(voltage) > maxsig) maxsig = graphit->second->Eval(voltage);
		}
	}
	std::cout << minsig << " " << maxsig << std::endl;
	for (std::map<DetCurve, TGraphErrors*>::iterator graphit = this->curve_map.begin(); graphit != this->curve_map.end(); graphit++)
	{
		graphit->second->Sort();
		//check if voltage is in range of curve
		int n = graphit->second->GetN();
		double *x = graphit->second->GetX();
		int locmin = TMath::LocMin(n,x);
		int locmax = TMath::LocMax(n,x);
		double xmin = x[locmin];
		double xmax = x[locmax];
		
		if (voltage > xmin && voltage < xmax)
		{
			std::stringstream ss;
			ss << graphit->first.name << "_Region_" << graphit->first.region << "_Fluence_" << graphit->first.fluence << "_Annealing_" << graphit->first.annealing << "_" << graphit->first.testbeam;
			std::string title;
			if (graphit->first.region <= 4)	title = "w/p = 0.15";
			else if (graphit->first.region > 4 && graphit->first.region <=8) title = "w/p = 0.25";
			else if (graphit->first.region > 8 && graphit->first.region <=12) title = "w/p = 0.35";
			TH1D* tmp = new TH1D(ss.str().c_str(),title.c_str(), 300, 0.9*ymin, 1.1*ymax);
			tmp->Fill(graphit->second->Eval(voltage));
			set_properties_histo(graphit->first,tmp);
			tmp->GetXaxis()->SetRangeUser(minsig-1000, maxsig+1000);
            std::cout << "[NEW] Histogram with Name " << graphit->first.name << " Region " << graphit->first.region << " Fluence " << graphit->first.fluence << " Particle " << graphit->first.particle << " and Annealing " << graphit->first.annealing << " measured in testbeam " << graphit->first.testbeam << " initialized!" << std::endl;
			// std::cout << "Filling " << graphit->second->Eval(voltage) << " as value @ " << voltage << " Volts!" << std::endl;
			gStyle->SetOptStat(00000000);
			if (graphit == curve_map.begin()) tmp->Draw();
			else tmp->Draw("same");
		}
		else std::cout << "Voltage " << voltage << " out of range of the curve with " << graphit->first.name << std::endl;
	}
	std::string filename = "Results/Comparison/";
	settingfilename = settingfilename.substr(12, settingfilename.size()-16);
	filename += measurement + settingfilename + "_Histo.root";
	plotcanvas->SaveAs(filename.c_str());
}

void fixedVoltage::set_properties_histo(DetCurve curve, TH1D*& graph) {
  
   if (curve.doping == "N" && curve.material == "FZ") graph->SetLineColor(LibreColor(1)), graph->SetFillColor(LibreColor(1));
   else if (curve.doping == "P" && curve.material == "FZ") graph->SetLineColor(LibreColor(2)), graph->SetFillColor(LibreColor(2));
   else if (curve.doping == "Y" && curve.material == "FZ") graph->SetLineColor(LibreColor(4)), graph->SetFillColor(LibreColor(4));
   
   else if (curve.doping == "N" && curve.material == "MCZ") graph->SetLineColor(LibreColor(12)), graph->SetFillColor(LibreColor(12));
   else if (curve.doping == "P" && curve.material == "MCZ") graph->SetLineColor(LibreColor(11)), graph->SetFillColor(LibreColor(11));
   else if (curve.doping == "Y" && curve.material == "MCZ") graph->SetLineColor(LibreColor(8)), graph->SetFillColor(LibreColor(8));
   
   else if (curve.doping == "N" && curve.material == "FTH") graph->SetLineColor(LibreColor(6)), graph->SetFillColor(LibreColor(6));
   else if (curve.doping == "P" && curve.material == "FTH") graph->SetLineColor(LibreColor(5)), graph->SetFillColor(LibreColor(5));
   else if (curve.doping == "Y" && curve.material == "FTH") graph->SetLineColor(LibreColor(7)), graph->SetFillColor(LibreColor(7));
	 
   graph->SetLineWidth(2);
   if (curve.annealing != 0 && curve.thickness == 320) graph->SetFillStyle(0);
   else if (curve.annealing != 0 && curve.thickness == 200) graph->SetFillStyle(3003);
   
   else if (curve.thickness == 320) graph->SetFillStyle(3001);
   else if (curve.thickness == 200) graph->SetFillStyle(1001);
   
   if (curve.annealing != 0) graph->SetLineStyle(2);
   if (curve.particle == "n") graph->SetLineStyle(6);
   if (curve.particle == "p") graph->SetLineStyle(9);
}

Color_t LibreColor(const unsigned int& plotIndex) 
{
    std::string colorCode;
    
    if (plotIndex==0) colorCode = "#000000";
    else {
        int nColor=(plotIndex-1) % 12;
        switch (nColor) {
            case 0 :
                colorCode="#014586";
                break;
            case 1 :
                colorCode="#FF420E";
                break;
            case 2 :
                colorCode="#FFD320";
                break;
            case 3 :
                colorCode="#579D1C";
                break;
            case 4 :
                colorCode="#7E0021";
                break;
            case 5 :
                colorCode="#83CAFF";
                break;
            case 6 :
                colorCode="#314004";
                break;
            case 7 :
                colorCode="#AECF00";
                break;
            case 8 :
                colorCode="#4B1F6F";
                break;
            case 9 :
                colorCode="#FF950E";
                break;
            case 10 :
                colorCode="#C5000B";
                break;
            case 11 :
                colorCode="#0084D1";
                break;
            default :
                std::cerr << "ERROR: in Vizard::getNiceColor() n%12 is not an int between 0 and 11! This should not happen." << std::endl;
                colorCode="#000000";
                break;
        }
    }
    
    return TColor::GetColor(colorCode.c_str());
}
