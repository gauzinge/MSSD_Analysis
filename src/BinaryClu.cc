//
//  BinaryClu.cc
//  
//
//  Created by Georg Auzinger on 01.11.12.
//
//

#include "BinaryClu.h"

const int yeffcut = 5;

void hitmaker (double* center, std::vector<double>& xPosition, int& reg) {
    for (int j = 0; j<4; j++) {
        int region =0;
        center[j]+=1;
        double dist_edge = 0.900; //mm distance from physical Si sensor edge to "0" Position ("0" + 1 x Pitch = 1st Strip)
        
        double xPitch[12] = {0.12,0.24,0.08,0.07,0.12,0.24,0.08,0.07,0.12,0.24,0.08,0.07}; //mm
        if ( ( center[j] >352 ) && ( center[j]<=385 ) ) {
            xPosition[j] = dist_edge + 59.7605 + ( center[j] - 352 ) * xPitch[0] ;
            region = 1; //Region 1, 120um
        } else if ( ( center[j] > 320 ) && ( center[j] <= 352 ) ) {
            xPosition[j] = dist_edge + 50.6545 + ( ( center[j] - 320) * xPitch[1]) ;
            region = 2; //Region 2, 240um
        } else if ( ( center[j] > 288 ) && ( center[j] <= 320 ) ) {
            xPosition[j] = dist_edge + 46.8045 + ( ( center[j] - 288) * xPitch[2]) ;
            region = 3;//Region 3, 80um
        } else if ( ( center[j] > 256 ) && ( center[j] <= 288 ) ) {
            xPosition[j] = dist_edge + 43.283 + ( ( center[j] - 256) * xPitch[3]) ;
            region = 4;//Region 4, 70um
        } else if ( ( center[j] > 224 ) && ( center[j] <= 256 ) ) {
            xPosition[j] = dist_edge + 38.119 + ( ( center[j] - 224) * xPitch[4]) ;
            region = 5;//Region 5, 120um
        } else if ( ( center[j] > 192 ) && ( center[j] <= 224 ) ) {
            xPosition[j] = dist_edge + 29.013 + ( ( center[j] - 192) * xPitch[5]) ;
            region = 6;//Region 6, 240um
        } else if ( ( center[j] > 160 ) && ( center[j] <= 192 ) ) {
            xPosition[j] = dist_edge + 25.163 + ( ( center[j] - 160) * xPitch[6]) ;
            region = 7;//Region 7, 80um
        } else if ( ( center[j] > 128 ) && ( center[j] <= 160 ) ) {
            xPosition[j] = dist_edge + 21.63075 + ( ( center[j] - 128) * xPitch[7]) ;
            region = 8;//Region 8, 70um
        } else if ( ( center[j] > 96 ) && ( center[j] <= 128 ) ) {
            xPosition[j] = dist_edge + 16.4775 + ( ( center[j] - 96) * xPitch[8]) ;
            region = 9;//Region 9, 120um
        } else if ( ( center[j] > 64 ) && ( center[j] <= 96 ) ) {
            xPosition[j] = dist_edge + 7.36075 + ( ( center[j] - 64) * xPitch[9]) ;
            region = 10;//Region 10, 240um
        } else if ( ( center[j] > 32 ) && ( center[j] <= 64 ) ) {
            xPosition[j] = dist_edge + 3.5215 + ( ( center[j] - 32) * xPitch[10]) ;
            region = 11;//Region 11, 80um
        } else if ( center[j] <= 32 )  {
            xPosition[j] = dist_edge + 0 + ( ( center[j] - 0) * xPitch[11]) ;
            region = 12;//Region 12, 70um
        }
        if (j == 0) reg = region;
    }
}


void binary_cluster(Config& myconfig, int max_treshold, std::vector<int> regions, int calibration) {
    int unit=0;
    if ( max_treshold<100 ) unit=1;
    TGraph* efficiency = new TGraph();
    efficiency->SetTitle("Efficiency");
    TGraph* purity = new TGraph();
    purity->SetTitle("Purity");
    
    std::vector<TGraph*> resolutiongraph_vector_bary;
    std::vector<TGraph*> resolutiongraph_vector_binary12;
    std::vector<TGraph*> resolutiongraph_vector_binary1;
    std::vector<TGraph*> resolutiongraph_vector_leading;
    std::vector<TProfile*> profile_vector;

    std::vector<TGraph*> widthgraph_vector;
    TLegend* legend = new TLegend( 0.590517,0.641949,0.900862,0.900424,"","NDC");
    
    for (int i=0; i<12; i++) {
        TGraph* resolutiongraph1 = new TGraph();
        TGraph* resolutiongraph2 = new TGraph();
        TGraph* resolutiongraph3 = new TGraph();
        TGraph* resolutiongraph4 = new TGraph();
        
        resolutiongraph_vector_bary.push_back(resolutiongraph1);
        resolutiongraph_vector_binary12.push_back(resolutiongraph2);
        resolutiongraph_vector_binary1.push_back(resolutiongraph3);
        resolutiongraph_vector_leading.push_back(resolutiongraph4);

        TGraph* widthgraph = new TGraph();
        widthgraph->SetTitle("Clusterwidth over Treshold");
        widthgraph_vector.push_back(widthgraph);
        char buffer[2];
        sprintf(buffer,"%i", i+1);
        std::string title = "Region ";
        title += buffer;
//        legend->AddEntry(resolutiongraph_vector.at(i), title.c_str(), "lp");
        
    }
    int tempindex=0;
    if (unit ==0 ) tempindex = (( max_treshold - 2000)/2000)+1;
    else if (unit == 1) tempindex = max_treshold;
    for(int treshold_index=0; treshold_index<tempindex; treshold_index++) { // make 21? 31?
        float treshold=0;
        if ( unit ==0 ) treshold=2000+treshold_index*2000;
        else if ( unit ==1 ) treshold=1+treshold_index;
        std::cout << "Treshold " << treshold << std::endl;
        
        
        std::vector<int> width_vector;
        std::vector<std::vector<double> > xPosition_vector;
        std::vector<int> region_vector;
        
        std::vector<TH1D*> resolution_histo_vector_bary;
        std::vector<TH1D*> resolution_histo_vector_binary12;
        std::vector<TH1D*> resolution_histo_vector_binary1;
        std::vector<TH1D*> resolution_histo_vector_leading;

        std::vector<TF1*> resolution_fits_vector_bary;
        std::vector<TF1*> resolution_fits_vector_binary12;
        std::vector<TF1*> resolution_fits_vector_binary1;
        std::vector<TF1*> resolution_fits_vector_leading;
        
        std::vector<TH1I*> width_histo_vector;
        
        for (unsigned int index = 1; index<=12 ; index++) {
            std::string title="Resolution Region ";
            std::string title1="Clusterwidth Region ";
            char buffer[3];
            sprintf(buffer,"%i", index);
            title+=buffer;
            title1+=buffer;
            
//            TH1D* resolution1 = new TH1D(title.c_str(),title.c_str(),1000,-.5,.5);
            TH1D* resolution1 = new TH1D("title1","Barycentric",1000,-.5,.5);
            TH1D* resolution2 = new TH1D("title2","Binary 1/2",1000,-.5,.5);
            TH1D* resolution3 = new TH1D("title3","Binary 1",1000,-.5,.5);
            TH1D* resolution4 = new TH1D("title4","Leading",1000,-.5,.5);

            TH1I* width_histo = new TH1I(title1.c_str(),title1.c_str(),10, 0, 10);
            width_histo_vector.push_back(width_histo);
            
            resolution_histo_vector_bary.push_back(resolution1);
            resolution_histo_vector_binary12.push_back(resolution2);
            resolution_histo_vector_binary1.push_back(resolution3);
            resolution_histo_vector_leading.push_back(resolution4);

            TF1* fit1 =  new TF1 ("fit1", "gaus", -0.5,0.5);
            TF1* fit2 =  new TF1 ("fit2", "gaus", -0.5,0.5);
            TF1* fit3 =  new TF1 ("fit3", "gaus", -0.5,0.5);
            TF1* fit4 =  new TF1 ("fit4", "gaus", -0.5,0.5);

            resolution_fits_vector_bary.push_back(fit1);
            resolution_fits_vector_binary12.push_back(fit2);
            resolution_fits_vector_binary1.push_back(fit3);
            resolution_fits_vector_leading.push_back(fit4);
        }
        
        int n_tracks = 0;
        int n_clusters = 0;
        int n_matched = 0;
        
        for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
        {
            int counter = 0;
            
            Data mydata;
            Track mytrack;
            Alignment myalignment(filenames->datafilename, filenames->trackfilename);
            EventCollection binarycollection;
            
            mydata.readtree(filenames->datafilename);
            mytrack.readtree(filenames->trackfilename);
            myalignment.get_alignment(filenames->datafilename);
            
            //            n_tracks += mytrack.get_n_entries();
            
            for (unsigned int i=0; i<mydata.get_n_entries(); i++) {
                mydata.get_entry(i);
                binarycollection.addHit(i, mydata.EventNumber);
            }
            
            std::string matchfilename = myconfig.directory + "Matchfiles/";
            matchfilename += filenames->datafilename.substr(8,6);
            matchfilename += "_binClu.txt";
            
            std::ofstream binaryMatchFile(matchfilename.c_str());
            binarycollection.Serialize(binaryMatchFile);
            
            std::ifstream inFile(matchfilename.c_str());
            binarycollection.DeSerialize(inFile);
            
            std::cout << "[BINARY CLUSTER] Clustering ... " << std::endl;
            
            for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
            {
                mytrack.get_entry(j);
                mytrack.apply_alignment(myalignment.angle,myalignment.x_shift, myalignment.y_shift, myalignment.DUT_zPosition);
                
                //check if in ROI
                 if (track_good(mytrack.xPrime, mytrack.yPrime, 1) && track_good_y(mytrack.yPrime,myconfig.y_min, myconfig.y_max)) n_tracks++;
                
                const std::vector<int>& eventTracks = binarycollection.GetHitLines(mytrack.EventNumber);
                std::vector<int>::const_iterator it;
                
                for(it = eventTracks.begin() ; it!= eventTracks.end(); ++it)
                {
                    const int& hitLine = *it;
                    mydata.get_entry(hitLine);
                    
                    //find binary clusters and store them into 2 vectors
                    width_vector.clear();
                    xPosition_vector.clear();
                    region_vector.clear();
                    
                    for (unsigned int reg=0; reg<12; reg++) {
                        
                        
                        int ir_start = 32*reg;
                        int ir_stop = 32*(reg+1);
                        
                        int i = ir_start;
                        while (i < ir_stop) {
                            
                            double center[4]={0,0,0,0};
                            double sum=0;
                            double weighted_sum=0;
                            std::vector<double> xPosition;
                            for (int size=0; size<4; size++) {
                                xPosition.push_back(0);
                            }
                            double charge=0;
                            int leading_strip=0;
                            double max_signal=0;
                            
                            if (myconfig.adc_to_electrons(*filenames,mydata.signal[i] ,myconfig.strip_to_chip(i), unit) > treshold) {
                                //cluster found
                                int width = 1;
                                sum = i;
                                weighted_sum = i * myconfig.adc_to_electrons(*filenames,mydata.signal[i] ,myconfig.strip_to_chip(i), unit);
                                charge = myconfig.adc_to_electrons(*filenames,mydata.signal[i] ,myconfig.strip_to_chip(i), unit);
                                max_signal = charge;
                                leading_strip = i;
                                while ((myconfig.adc_to_electrons(*filenames,mydata.signal[i+width], myconfig.strip_to_chip(i+width), unit) > treshold) && ((i+width) < ir_stop)){
                                    sum+=i+width;
                                    weighted_sum += (myconfig.adc_to_electrons(*filenames,mydata.signal[i+width], myconfig.strip_to_chip(i+width), unit)) * (i+width);
                                    charge += myconfig.adc_to_electrons(*filenames,mydata.signal[i+width], myconfig.strip_to_chip(i+width), unit);
                                    if (myconfig.adc_to_electrons(*filenames,mydata.signal[i+width], myconfig.strip_to_chip(i+width), unit) > max_signal) {
                                        leading_strip = i+width;
                                        max_signal = myconfig.adc_to_electrons(*filenames,mydata.signal[i+width], myconfig.strip_to_chip(i+width), unit);
                                    }
                                    width++;
                                }
                                //Barycentric -> weighted mean
                                center[0] = weighted_sum/charge;
                                //Binary with 1/2 strip Resolution
                                center[1] = sum/width;
                                //Binary with 1 strip Resolution
                                center[2] = static_cast<int>(sum/width);
                                //Leading Strip
                                center[3] = leading_strip;
                                //Clusterwidth
                                width_vector.push_back(width);
                                
                                // make physical hits
                                int region;
                                hitmaker(center, xPosition, region);
                                region_vector.push_back(region);
                                xPosition_vector.push_back(xPosition); //vector of vectors
                                i+=width;
                            } else {
                                i++;
                            }
                        }
                        
                    }
                    for (size_t ncluster=0; ncluster<xPosition_vector.size(); ncluster++) { //loop over all clusters for specific event
                        
                        n_clusters++;
                        if ((fabs(mytrack.xPrime - xPosition_vector.at(ncluster).at(0)) < get_pitch_region(region_vector.at(ncluster))*0.001*3) && (track_good(mytrack.xPrime, mytrack.yPrime, 1) == 1) && track_good_y(mytrack.yPrime, myconfig.y_min,myconfig.y_max)) { //use barycentric here because it should be most precise
                            width_histo_vector.at(region_vector.at(ncluster)-1)->Fill(width_vector.at(ncluster));  
                            //fill different histogramms with barycentric track - hit matching
                            resolution_histo_vector_bary.at(region_vector.at(ncluster)-1)->Fill(mytrack.xPrime-xPosition_vector.at(ncluster).at(0));
                            resolution_histo_vector_binary12.at(region_vector.at(ncluster)-1)->Fill(mytrack.xPrime-xPosition_vector.at(ncluster).at(1));
                            resolution_histo_vector_binary1.at(region_vector.at(ncluster)-1)->Fill(mytrack.xPrime-xPosition_vector.at(ncluster).at(2));
                            resolution_histo_vector_leading.at(region_vector.at(ncluster)-1)->Fill(mytrack.xPrime-xPosition_vector.at(ncluster).at(3));
                            
                            counter++;
                        }
                    }
                } //event tracks iterator
            }
            //            std::cout << "[BINARY CLUSTER] Matched " << counter << " Binary Clusters to Tracks!" << std::endl;
            n_matched += counter;
        }// done for all input runs
        
        std::cout << "Clustering done for all input runs" << std::endl;
        
        for (std::vector<int>::iterator regionit = regions.begin(); regionit != regions.end(); regionit++) {
            
            resolution_histo_vector_bary.at(*regionit-1)->Fit(resolution_fits_vector_bary.at(*regionit-1), "NQ");
            resolutiongraph_vector_bary.at(*regionit-1)->SetPoint(treshold_index, treshold*calibration, resolution_fits_vector_bary.at(*regionit-1)->GetParameter(2)*1000);
            
            resolution_histo_vector_binary12.at(*regionit-1)->Fit(resolution_fits_vector_binary12.at(*regionit-1), "NQ");
            resolutiongraph_vector_binary12.at(*regionit-1)->SetPoint(treshold_index, treshold*calibration, resolution_fits_vector_binary12.at(*regionit-1)->GetParameter(2)*1000);
            
            resolution_histo_vector_binary1.at(*regionit-1)->Fit(resolution_fits_vector_binary1.at(*regionit-1), "NQ");
            resolutiongraph_vector_binary1.at(*regionit-1)->SetPoint(treshold_index, treshold*calibration, resolution_fits_vector_binary1.at(*regionit-1)->GetParameter(2)*1000);
            
            resolution_histo_vector_leading.at(*regionit-1)->Fit(resolution_fits_vector_leading.at(*regionit-1), "NQ");
            resolutiongraph_vector_leading.at(*regionit-1)->SetPoint(treshold_index, treshold*calibration, resolution_fits_vector_leading.at(*regionit-1)->GetParameter(2)*1000);
            
            if (treshold_index == 0) {
            legend->AddEntry(resolutiongraph_vector_bary.at(*regionit-1),"Barycentric","lp");
            legend->AddEntry(resolutiongraph_vector_binary12.at(*regionit-1),"Binary 1/2 Strip","lp");
            legend->AddEntry(resolutiongraph_vector_binary1.at(*regionit-1),"Binary 1 Strip","lp");
            legend->AddEntry(resolutiongraph_vector_leading.at(*regionit-1),"Leading Strip","lp");
            }
//            std::cout << treshold_index << "    " << treshold << "    " << "Barycentric " << resolution_fits_vector_bary.at(*regionit-1)->GetParameter(2)*1000  << " Binary 1/2 " << resolution_fits_vector_binary12.at(*regionit-1)->GetParameter(2)*1000 << " Binary 1 " << resolution_fits_vector_binary1.at(*regionit-1)->GetParameter(2)*1000 << " Leading Strip " << resolution_fits_vector_leading.at(*regionit-1)->GetParameter(2)*1000 << std::endl;
            
            widthgraph_vector.at(*regionit-1)->SetPoint(treshold_index, treshold, width_histo_vector.at(*regionit-1)->GetMean());
            
        }
       

        
        efficiency->SetPoint(treshold_index, treshold, static_cast<double>(n_matched)/static_cast<double>(n_tracks)*100);
        purity->SetPoint(treshold_index, treshold, static_cast<double>(n_matched)/static_cast<double>(n_clusters)*100);

    } //end of treshold loop
    TCanvas* resolutioncanvas_binary = new TCanvas("BinaryResolution", "Resolutioincanvas_binary");
    TCanvas* widthcanvas = new TCanvas("BinaryClusterwidth","widthcanvas");
    TCanvas* efficiencycanvas = new TCanvas("BinaryEfficiency", "efficiencycanvas");
    
    //draw resolution for all algorithms
     for (std::vector<int>::iterator regionit = regions.begin(); regionit != regions.end(); regionit++) {
        resolutioncanvas_binary->cd();
         int i = 1;
         resolutiongraph_vector_bary.at(*regionit-1)->GetYaxis()->SetRangeUser(10, 40);
        resolutiongraph_vector_bary.at(*regionit-1)->SetMarkerColor(1);
        resolutiongraph_vector_bary.at(*regionit-1)->SetLineColor(1);
        resolutiongraph_vector_bary.at(*regionit-1)->SetLineWidth(2);

         resolutiongraph_vector_binary12.at(*regionit-1)->GetYaxis()->SetRangeUser(10, 40);
         resolutiongraph_vector_binary12.at(*regionit-1)->SetMarkerColor(2);
         resolutiongraph_vector_binary12.at(*regionit-1)->SetLineColor(2);
         resolutiongraph_vector_binary12.at(*regionit-1)->SetLineWidth(2);

         resolutiongraph_vector_binary1.at(*regionit-1)->GetYaxis()->SetRangeUser(10, 40);
         resolutiongraph_vector_binary1.at(*regionit-1)->SetMarkerColor(3);
         resolutiongraph_vector_binary1.at(*regionit-1)->SetLineColor(3);
         resolutiongraph_vector_binary1.at(*regionit-1)->SetLineWidth(2);

         resolutiongraph_vector_leading.at(*regionit-1)->GetYaxis()->SetRangeUser(10, 40);
         resolutiongraph_vector_leading.at(*regionit-1)->SetMarkerColor(4);
         resolutiongraph_vector_leading.at(*regionit-1)->SetLineColor(4);
         resolutiongraph_vector_leading.at(*regionit-1)->SetLineWidth(2);

         resolutiongraph_vector_bary.at(*regionit-1)->GetXaxis()->SetTitle("Clustering Threshold [Electrons]");
         resolutiongraph_vector_binary12.at(*regionit-1)->GetXaxis()->SetTitle("Clustering Threshold [Electrons]");
         resolutiongraph_vector_binary1.at(*regionit-1)->GetXaxis()->SetTitle("Clustering Threshold [Electrons]");
         resolutiongraph_vector_leading.at(*regionit-1)->GetXaxis()->SetTitle("Clustering Threshold [Electrons]");

         resolutiongraph_vector_bary.at(*regionit-1)->GetYaxis()->SetTitle("Resolution [#mum]");
         resolutiongraph_vector_binary12.at(*regionit-1)->GetYaxis()->SetTitle("Resolution [#mum]");
         resolutiongraph_vector_binary1.at(*regionit-1)->GetYaxis()->SetTitle("Resolution [#mum]");
         resolutiongraph_vector_leading.at(*regionit-1)->GetYaxis()->SetTitle("Resolution [#mum]");
         
         resolutiongraph_vector_bary.at(*regionit-1)->Draw("apl");
         resolutiongraph_vector_binary12.at(*regionit-1)->Draw("pl same");
         resolutiongraph_vector_binary1.at(*regionit-1)->Draw("pl same");
         resolutiongraph_vector_leading.at(*regionit-1)->Draw("pl same");
         resolutioncanvas_binary->Update();

        legend->Draw();
        myconfig.name->Draw();
        
        widthcanvas->cd();
        //        widthgraph_vector.at(*regionit-1)->GetXaxis()->SetRangeUser(1.5, max_treshold);
        widthgraph_vector.at(*regionit-1)->GetYaxis()->SetRangeUser(1, 1.4);
        widthgraph_vector.at(*regionit-1)->SetMarkerStyle(31);
        widthgraph_vector.at(*regionit-1)->SetMarkerColor(i+2);
        widthgraph_vector.at(*regionit-1)->SetLineColor(i+2);
        widthgraph_vector.at(*regionit-1)->SetLineWidth(2);
        if ( unit == 0 ) widthgraph_vector.at(*regionit-1)->GetXaxis()->SetTitle("Binary Cluster Treshold [Electrons]");
        else if ( unit ==1 ) widthgraph_vector.at(*regionit-1)->GetXaxis()->SetTitle("Binary Cluster Treshold [ADC]");
        widthgraph_vector.at(*regionit-1)->GetYaxis()->SetTitle("Average Cluster Width");
        
        if (i==1) widthgraph_vector.at(*regionit-1)->Draw("APL");
        else widthgraph_vector.at(*regionit-1)->Draw("PLsame");
        legend->Draw();
        myconfig.name->Draw();
         
        i++;
    }
    TLegend* efficiencylegend = new TLegend(0.645115,0.758475,0.899425,0.902542,"","NDC");
    efficiencylegend->AddEntry(efficiency, "Efficiency  n_matched/n_tracks", "lp");
    efficiencylegend->AddEntry(purity, "Purity  n_matched/n_clusters", "lp");
    
    efficiencycanvas->cd();
    efficiency->GetYaxis()->SetRangeUser(0,100);
    purity->GetYaxis()->SetRangeUser(0,100);
    efficiency->SetLineColor(8);
    purity->SetLineColor(9);
    if ( unit == 0 ) efficiency->GetXaxis()->SetTitle("Binary Cluster Treshold [Electrons]");
    else if ( unit == 1 ) efficiency->GetXaxis()->SetTitle("Binary Cluster Treshold [ADC]");
    efficiency->GetYaxis()->SetTitle("Efficiency / Purity [%]");
    
    efficiency->Draw("AL");
    purity->Draw("Lsame");
    efficiencylegend->Draw("same");
    myconfig.name->Draw();
   
    
    std::string title = "Results/";
    title += myconfig.detname;
    title += "/BinaryResolution_Treshold_";
    title+=myconfig.detname;
    title+=".root";
    resolutioncanvas_binary->SaveAs(title.c_str());
    
    std::string title1 = "Results/";
    title1 += myconfig.detname;
    title1 += "/BinaryCluwidth_Treshold_";
    title1+=myconfig.detname;
    title1+=".png";
    widthcanvas->SaveAs(title1.c_str());
    
    std::string title2 = "Results/";
    title2 += myconfig.detname;
    title2 += "/BinCluster_Efficiency";
    title2+=myconfig.detname;
    title2+=".png";
    efficiencycanvas->SaveAs(title2.c_str());
    
//    myconfig.plotfile->WriteTObject(resolutioncanvas_binary);
//    myconfig.plotfile->WriteTObject(efficiencycanvas);
//    myconfig.plotfile->WriteTObject(widthcanvas);

}

