//
//  chi2dist.cc
//  
//
//  Created by Georg Auzinger on 16.11.12.
//
//

#include "chi2dist.h"


double get_chi2(std::string collection, std::string measurement, Settings thesettings, double chi2multiple) {
    gStyle->SetOptStat(11111111);
	TH1D* chi2 = new TH1D("chi2","chi2",310,-10,300);
	
    // this->collection = collection;
  //   this->measurement = measurement;
    
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
						double result;
						if (collection == "onTrackCluster") 
						{
							result = mydetector.onTrackCluster_signal_chisq.at(*regionit-1)/mydetector.onTrackCluster_signal_ndf.at(*regionit-1);
						}
						else if (collection == "Trackhit") 
						{
							result = mydetector.Trackhit_signal_chisq.at(*regionit-1)/mydetector.Trackhit_signal_ndf.at(*regionit-1);
						}
						else if (collection == "PeakDistance") 
						{
							result = mydetector.PeakDistance_signal_chisq.at(*regionit-1)/mydetector.PeakDistance_signal_ndf.at(*regionit-1);
						}
						chi2->Fill(result);
						if (result > chi2->GetMean()*chi2multiple) std::cout << "[WARNING] Chi2/NDF = " << result << "  " << dummytestbeam << " " << mydetector.name << " Region " << *regionit << " Voltage " << mydetector.voltage << " Fluence " << mydetector.fluence << " Annealing " << mydetector.annealing << std::endl;
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
				double result;
				if (collection == "onTrackCluster") 
				{
					result = mydetector.onTrackCluster_signal_chisq.at(*regionit-1)/mydetector.onTrackCluster_signal_ndf.at(*regionit-1);
				}
				else if (collection == "Trackhit") 
				{
					result = mydetector.Trackhit_signal_chisq.at(*regionit-1)/mydetector.Trackhit_signal_ndf.at(*regionit-1);
				}
				else if (collection == "PeakDistance") 
				{
					result = mydetector.PeakDistance_signal_chisq.at(*regionit-1)/mydetector.PeakDistance_signal_ndf.at(*regionit-1);
				}
				chi2->Fill(result);
				if (result > chi2->GetMean()*chi2multiple) std::cout << "[WARNING] Chi2/NDF = " << result << "  " << dummytestbeam << " " << mydetector.name << " Region " << *regionit << " Voltage " << mydetector.voltage << " Fluence " << mydetector.fluence << " Annealing " << mydetector.annealing << std::endl;
			}
        }
    }
	chi2->GetMean();
	TCanvas* chi2canvas = new TCanvas("Chi2canvas","Chi2canvas");
	chi2canvas->cd();
	chi2->Draw();
	return 	chi2->GetMean()*chi2multiple;

}

