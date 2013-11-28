//
//  Noise_estimate.cc
//  
//
//  Created by Georg Auzinger on 11.12.12.
//
//

#include "Noise_estimate.h"
std::vector<TH1D*> fill_residuals(Config& myconfig) {
    
    double minrange = -10;
    double maxrange = 10;
    
    std::vector<TH1D*> residuals;
    for (unsigned int index = 1; index<=12 ; index++) {
        std::string title="Residuals Region ";
        char buffer[3];
        
        sprintf(buffer,"%i", index);
        title+=buffer;
        TH1D* resolution = new TH1D(title.c_str(),title.c_str(), 1000,minrange,maxrange);
        resolution->GetXaxis()->SetTitle("Residuals [mm]");
        residuals.push_back(resolution);
        
    }
    int counter = 0;
    int goodcounter = 0;
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        Cluster mycluster;
        Track mytrack;
        Alignment myalignment(filenames->datafilename, filenames->trackfilename);
        EventCollection mycollection;
        
        mytrack.readtree(filenames->trackfilename);
        mycluster.readtree(filenames->datafilename);
        myalignment.get_alignment(filenames->datafilename);
        //use tracks in collection <-> ask what is more clever
        for (unsigned int i=0; i<mycluster.get_n_entries(); i++) {
            mycluster.get_entry(i);
            mycollection.addHit(i, mycluster.EventNumber);
        }
        std::string matchfilename = myconfig.directory + "Matchfiles/";
        size_t last_int= filenames->datafilename.find("_MSSD");
        matchfilename += filenames->datafilename.substr(last_int-4,4);
        matchfilename += "_noiseEst.txt";
        std::ofstream binaryMatchFile(matchfilename.c_str());
        mycollection.Serialize(binaryMatchFile);
        
        std::ifstream inFile(matchfilename.c_str());
        mycollection.DeSerialize(inFile);
        
        for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
        {
            mytrack.get_entry(j);
            counter++; //counting total tracks
            mytrack.apply_alignment(myalignment.angle,myalignment.x_shift,myalignment.y_shift,myalignment.DUT_zPosition);
            if (track_good(mytrack.xPrime, mytrack.yPrime, 0) == 1 && track_good_y(mytrack.yPrime,myconfig.y_min,myconfig.y_max)) goodcounter++; //counting tracks in regions
            const std::vector<int>& eventTracks = mycollection.GetHitLines(mytrack.EventNumber);
            std::vector<int>::const_iterator it;
            
            for(it = eventTracks.begin() ; it!= eventTracks.end(); ++it)
            {
                const int& hitLine = *it;
                mycluster.get_entry(hitLine);
                
                //Residuals of All Clusters in specific region
                if (mycluster.region == get_region(mytrack.xPrime))  {
                    residuals[mycluster.region-1]->Fill(mycluster.xPosition-mytrack.xPrime);
                }
            }
        }
    }
    myconfig.warningstream << "[INFO] The total number of Reference Tracks from the Telescope is " << counter << " and " << goodcounter << " are within Regions!" << std::endl;
    return residuals;
}

void fit_residuals(Config& myconfig, std::vector<TH1D*> residuals) {
    
    TCanvas* noise_estimate_residuals = new TCanvas("Residuals", "Resolution of Regions");
    noise_estimate_residuals->Divide(4,3);
        
    for (unsigned int index = 0; index<12 ; index++) {
        double range = get_pitch_region(index+1)*0.001*3;
        double fmin, fmax;
        fmin = -10*range;
        fmax = +10*range;
        
        TF1* peak_fit = new TF1("peak", "gaus",fmin,fmax);
        TF1* background_fit = new TF1("background", "gaus",fmin,fmax);
        TF1* fit =  new TF1 ("distribution", "gaus(0)+gaus(3)", fmin,fmax);
        fit->SetParameters(200,0,0.001,10,0,1); //200 0 0.001 50 0 1
        fit->SetParLimits(1,-0.1,0.1);
        fit->SetParLimits(2,0,0.1);
        
        noise_estimate_residuals->cd(index+1);
        
        int status = residuals.at(index)->Fit(fit,"RNQ");
        if (status != -1) {
            double param[6];
            fit->GetParameters(param);
            peak_fit->SetParameters(&param[0]);
            background_fit->SetParameters(&param[3]);
            gPad->SetLogy();
            residuals.at(index)->Draw();
            peak_fit->SetLineColor(3);
            peak_fit->Draw("same");
            
            background_fit->SetLineColor(6);
            background_fit->Draw("same");
            
            // Integral Borders / Cuts
            double binwidth = residuals.at(index)->GetXaxis()->GetBinWidth(10);
            
            double area_peak = (peak_fit->Integral(-range,range))/binwidth; // A
            double area_back_incuts = (background_fit->Integral(-range, range))/binwidth; // B
            double area_total = residuals.at(index)->GetEntries();
            double area_outcuts =  area_total - area_peak - area_back_incuts; // C
//            std::cout << "Region " << index+1 << " A= " << area_peak << " B= " << area_back_incuts << " C= " << area_outcuts << " Purity " << area_peak/(area_peak+area_back_incuts) << std::endl;
            myconfig.myscale.A.at(index)=area_peak;
            myconfig.myscale.B.at(index)=area_back_incuts;
            myconfig.myscale.C.at(index)=area_outcuts;
            if (param[1]/param[2]*sqrt(area_peak)>2) {
                std::cout << "[WARNING] Region " << index + 1 << " Validity of Alignment (should be close to 0) " << param[1]/param[2]*sqrt(area_peak) << std::endl;
                myconfig.warningstream << "[WARNING] Region " << index + 1 << " Validity of Alignment (should be close to 0) " << param[1]/param[2]*sqrt(area_peak) << std::endl;
            }
            myconfig.warningstream << "[NOISE ESTIMATION] Total Area of Signal Peak " << area_peak << std::endl;
            
        }
    }
    std::string title1 = myconfig.foldername;
    title1 += "/EstimateNoise_Residuals";
    title1+=myconfig.detname;
    title1+=".png";
    noise_estimate_residuals->SaveAs(title1.c_str());
    myconfig.plotfile->WriteTObject(noise_estimate_residuals,"","Overwrite");
}


std::vector<TH1F*> fill_bad(Config& myconfig, std::string measurement, int unit) {
    std::vector<TH1F*> landau_vec;
    
    for (int i=0; i<12; i++) {
        std::string title;
        if (measurement == "signal") title = "Cluster Charge Distribution  for Region ";
        else title = "Cluster Signal to Noise for Region ";

        char buffer[5];
        sprintf(buffer,"%i", i+1);
        title += buffer;
        TH1F* landau;
        if (measurement == "signal" && unit >=0 && unit < 3) {
            landau = new TH1F(title.c_str(), title.c_str(),100,0,48700);
        } else landau = new TH1F(title.c_str(), title.c_str(),100,0,100);
        landau_vec.push_back(landau);
    }
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        Cluster mycluster;
        Track mytrack;
        Alignment myalignment(filenames->datafilename, filenames->trackfilename);
        EventCollection mycollection;
        
        mytrack.readtree(filenames->trackfilename);
        mycluster.readtree(filenames->datafilename);
        myalignment.get_alignment(filenames->datafilename);
        //use tracks in collection <-> ask what is more clever
        for (unsigned int i=0; i<mycluster.get_n_entries(); i++) {
            mycluster.get_entry(i);
            mycollection.addHit(i, mycluster.EventNumber);
        }
        std::string matchfilename = myconfig.directory + "Matchfiles/";
        size_t last_int= filenames->datafilename.find("_MSSD");
        matchfilename += filenames->datafilename.substr(last_int-4,4);
        matchfilename += "_noiseEst.txt";
        std::ofstream binaryMatchFile(matchfilename.c_str());
        mycollection.Serialize(binaryMatchFile);
        
        std::ifstream inFile(matchfilename.c_str());
        mycollection.DeSerialize(inFile);
        
        for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
        {
            mytrack.get_entry(j);
            mytrack.apply_alignment(myalignment.angle,myalignment.x_shift,myalignment.y_shift,myalignment.DUT_zPosition);
            
            const std::vector<int>& eventTracks = mycollection.GetHitLines(mytrack.EventNumber);
            std::vector<int>::const_iterator it;
            
            for(it = eventTracks.begin() ; it!= eventTracks.end(); ++it)
            {
                const int& hitLine = *it;

                mycluster.get_entry(hitLine);
                if ((fabs(mycluster.xPosition - mytrack.xPrime) >= get_pitch_region(get_region(mytrack.xPrime))*0.001*3) && mycluster.region == get_region(mytrack.xPrime)) //track on sensor but distance from cluster >= 3 Strips
                {
                    if (measurement == "signal") {
//                        landau_vec.at(mycluster.region-1)->Fill(mycluster.charge);
                        landau_vec.at(mycluster.region-1)->Fill(myconfig.adc_to_electrons(*filenames,mycluster.charge, myconfig.strip_to_chip(mycluster.first_strip),unit));
                    }
                    else landau_vec.at(mycluster.region-1)->Fill(mycluster.snr);

                }
            }
        }
    }
    return landau_vec;
}

void scale_distribution(Config& myconfig, std::vector<TH1F*> landau_vec, std::string measurement) {
    
    std::cout << "[NOISE ESTIMATION] " << measurement << std::endl;
    TCanvas* noise_estimate_FakeSignal;
    if (measurement == "signal") noise_estimate_FakeSignal = new TCanvas("FakeSignal","FakeCluster Total Charge ");
    else noise_estimate_FakeSignal = new TCanvas("FakeSNR","FakeCluster SNR");
    noise_estimate_FakeSignal->Divide(4,3);
    for (int i=0;i<12;i++) {
        TH1F* scaled_landau = new TH1F(*landau_vec.at(i));
        double nentries = scaled_landau->GetEntries();
        if (myconfig.myscale.A.at(i) != 0 && fabs(nentries-myconfig.myscale.C.at(i))/myconfig.myscale.C.at(i) > 0.1) {
            std::cout << "[WARNING] The number of Entries for Region " << i+1 << " is " << nentries << " while it should be " << myconfig.myscale.C.at(i) << " from the Fit of Residuals! Difference " << fabs(nentries-myconfig.myscale.C.at(i))/myconfig.myscale.C.at(i)*100 << "%!"<< std::endl;
        myconfig.warningstream << "[WARNING] The number of Entries for Region " << i+1 << " is " << nentries << " while it should be " << myconfig.myscale.C.at(i) << " from the Fit of Residuals! Difference " << fabs(nentries-myconfig.myscale.C.at(i))/myconfig.myscale.C.at(i)*100 << "%!"<< std::endl;
        }
        scaled_landau->Scale(myconfig.myscale.B.at(i)/nentries);
        scaled_landau->SetFillColor(46);
        landau_vec.at(i)->SetFillColor(38);
        noise_estimate_FakeSignal->cd(i+1);
        landau_vec.at(i)->Draw();
        if (myconfig.myscale.A.at(i) != 0) {
            scaled_landau->Draw("same");
            if (measurement == "signal") myconfig.myscale.noise_cluster_signal.at(i) = scaled_landau;
            else myconfig.myscale.noise_cluster_snr.at(i) = scaled_landau;
        }
    }
    
    std::string title = myconfig.foldername;
    title += "/Noise_Estimate/";
    if (measurement == "signal") title += "Signal_notMatched_";
    else title += "SNR_notMatched_";
    title+=myconfig.detname;
    title+=".png";
    noise_estimate_FakeSignal->SaveAs(title.c_str());
    myconfig.plotfile->WriteTObject(noise_estimate_FakeSignal,"","Overwrite");

}


void estimate_noise(Config& myconfig, int unit) {
    std::vector<TH1D*> residuals = fill_residuals(myconfig);
    fit_residuals(myconfig, residuals);
    std::vector<TH1F*> landau_bad = fill_bad(myconfig, "signal", unit);
    scale_distribution(myconfig, landau_bad, "signal");
    std::vector<TH1F*> snr_bad = fill_bad(myconfig, "snr", unit);
    scale_distribution(myconfig, snr_bad, "snr");
}

