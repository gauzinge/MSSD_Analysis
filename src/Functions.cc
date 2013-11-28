#include "Functions.h"
#include "trackfunctions.h"
//#include "include/Functions.h"

const int yeffcut = 5;

void get_y_cuts(Config& myconfig)
{
	int nbins = 100;
	TProfile* yprofile = new TProfile("YHitProfile","y Profile",nbins, -10, 45);
	yprofile->SetErrorOption("");
	
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
		matchfilename += "_y_Efficiency.txt";
        
        
		std::ofstream clusterMatchFile(matchfilename.c_str());
		mycollection.Serialize(clusterMatchFile);
        
		std::ifstream inFile(matchfilename.c_str());
		mycollection.DeSerialize(inFile);
        
		for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
		{
			mytrack.get_entry(j);
			mytrack.apply_alignment(myalignment.angle,myalignment.x_shift, myalignment.y_shift, myalignment.DUT_zPosition);
            
			const std::vector<int>& eventHits = mycollection.GetHitLines(mytrack.EventNumber);
			std::vector<int>::const_iterator it;
            
			for(it = eventHits.begin() ; it!= eventHits.end(); ++it)
			{
				const int& hitLine = *it;
				mycluster.get_entry(hitLine);
				int value = 0;
				if ((fabs(mycluster.xPosition - mytrack.xPrime) < get_pitch_region(get_region(mytrack.xPrime))*0.001*3)) 
				{	//cluster found
					value = 1;
				}
				yprofile->Fill(mytrack.yPrime,value);
			}
		}
	}
	
	TF1 *fit = new TF1("fit","pol0",-10,30);
	yprofile->Fit("fit", "RQON");
	double maximum = fit->GetParameter(0);
	double minimum = yprofile->GetMinimum();
	double maxvalue = (maximum - minimum)*0.90;
	
	//find min and max bin above threshold
	int minbin=0;
	int maxbin=nbins;
	while (minbin < nbins)
	{
		if (yprofile->GetBinContent(minbin) > maxvalue) break;
		else minbin++;
	}
	while (maxbin > 0)
	{
		if (yprofile->GetBinContent(maxbin) > maxvalue) break;
		else maxbin--;
	} 
	double y_min = yprofile->GetBinCenter(minbin);
	double y_max = yprofile->GetBinCenter(maxbin);
	
	myconfig.y_min = y_min;
	myconfig.y_max = y_max;
	
	TLine* line1 = new TLine(y_min,0,y_min,maxvalue);
	TLine* line2 = new TLine(y_max,0,y_max,maxvalue);
	TLine* line3 = new TLine(y_min,maxvalue,y_max,maxvalue);
	line1->SetLineColor(2);
	line1->SetLineWidth(2);
	line2->SetLineColor(2);
	line2->SetLineWidth(2);
	line3->SetLineColor(2);
	line3->SetLineWidth(2);
	
	//Draw Profile
	TCanvas* y_efficiency =  new TCanvas("y_efficiency","Y_Efficinecy");
	y_efficiency->cd();
	yprofile->Draw();
	line1->Draw("same");
	line2->Draw("same");
	line3->Draw("same");
	myconfig.name->Draw("same");
	std::string title1 = myconfig.foldername;
	title1 += "/Y_Efficiency_";
	title1+=myconfig.detname;
	title1+=".png";
	y_efficiency->SaveAs(title1.c_str());

	// myconfig.plotfile->WriteTObject(y_efficiency,"","Overwrite");
}

void get_signal_snr(Config& myconfig, int collection, int unit) {
    std::vector<TH1F*> signal_on;
    std::vector<TH1F*> signal_off;
    std::vector<TH1F*> snr;
    for (unsigned int region = 0; region < 12; region++) {
        std::string title = "On-track Signal Region ";
        std::string title2 = "Off-track Signal Region ";
        std::string name = "Signal_on_region";
        std::string name2 = "Signal_off_region";
        char buffer[3];
        sprintf(buffer,"%i", region+1);
        title += buffer;
        title2 += buffer;
        name += buffer;
        name2 += buffer;
        
        TH1F* signaldummy_on = new TH1F(name.c_str(), title.c_str(),150,-50,100);
        TH1F* signaldummy_off = new TH1F(name2.c_str(), title2.c_str(),150,-50,100);
		TH1F* snrdummy = new TH1F("SNRDummy", "SNRDummy",150,-50,100);

        signal_on.push_back(signaldummy_on);
        signal_off.push_back(signaldummy_off);
        snr.push_back(snrdummy);
    }
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        Track mytrack;
        mytrack.readtree(filenames->trackfilename);
        Data mydata;
        mydata.readtree(filenames->datafilename);
        Alignment myalignment(filenames->datafilename, filenames->trackfilename);
        myalignment.get_alignment(filenames->datafilename);
        EventCollection mycollection;
        
        for (unsigned int i=0; i<mydata.get_n_entries(); i++) {
            mydata.get_entry(i);
            mycollection.addHit(i, mydata.EventNumber);
        }
        std::string matchfilename = myconfig.directory + "Matchfiles/";
        size_t last_int= filenames->datafilename.find("_MSSD");
        matchfilename += filenames->datafilename.substr(last_int-4,4);
        matchfilename += "_Signal.txt";
        
        std::ofstream binaryMatchFile(matchfilename.c_str());
        mycollection.Serialize(binaryMatchFile);
        
        std::ifstream inFile(matchfilename.c_str());
        mycollection.DeSerialize(inFile);
        
        for (unsigned int ntrack=0; ntrack<mytrack.get_n_entries(); ntrack++)
        {
            mytrack.get_entry(ntrack);
            mytrack.apply_alignment(myalignment.angle, myalignment.x_shift, myalignment.y_shift, myalignment.DUT_zPosition);
            
            const std::vector<int>& eventTracks = mycollection.GetHitLines(mytrack.EventNumber);
            std::vector<int>::const_iterator it;
            
            for(it = eventTracks.begin() ; it!= eventTracks.end(); ++it)
            {
                const int& hitLine = *it;
                
                int region = get_region(mytrack.xPrime);
                unsigned int nearest_strip = (unsigned int)(reverse_hitmaker(mytrack.xPrime)); //to round to nearest integer instead of floor!!
                if (track_good_strips(nearest_strip) && (get_region(mytrack.xPrime) != 0) && track_good_y(mytrack.yPrime,myconfig.y_min,myconfig.y_max))
                {
                    unsigned int anti_strip =  get_antistrip(nearest_strip);
					
                    mydata.get_entry(hitLine);
                    
                    float charge = 0;
                    float charge_adc = 0;
                    float anticharge = 0;

                    for (unsigned int nstrip = nearest_strip - 1; nstrip <=nearest_strip+1; nstrip++)
                    {
                        if (unit >=0 && unit < 3) charge += myconfig.adc_to_electrons(*filenames,mydata.signal[nstrip], myconfig.strip_to_chip(nstrip),2)/1000;
                        else charge += mydata.signal[nstrip];
                        charge_adc += mydata.signal[nstrip];

                    }
                    for (unsigned int nstrip = anti_strip - 1; nstrip <=anti_strip+1; nstrip++)
                    {
                        if (unit >=0 && unit < 3) anticharge += myconfig.adc_to_electrons(*filenames,mydata.signal[nstrip], myconfig.strip_to_chip(nstrip),2)/1000;
                        else anticharge += mydata.signal[nstrip];
                    }
                    
                    
                    signal_on.at(region-1)->Fill(charge);
                    signal_off.at(region-1)->Fill(anticharge);
                    snr.at(region-1)->Fill(charge_adc/mydata.noise[anti_strip]);
                }
            }
        }
    }
		
    TCanvas* Signal_SNR = new TCanvas("Signal_SNR", "Track related Signal");
    std::vector<float> cut_vector;
    
    Signal_SNR->Divide(4,3);
    for (unsigned int region = 0; region < 12; region++) {
        gStyle->SetOptFit(1111);
        
        //SIGNAL
        Signal_SNR->cd(region+1);
        double mpv, mpv_error, landau_mpv, landau_mpv_error;
        
        TF1* gausfit = new TF1("noisefit","gaus",-30,30);
        
        gausfit->SetLineColor(4);
        signal_off.at(region)->Fit("noisefit","RQN");
        signal_off.at(region)->SetLineColor(1);
        signal_off.at(region)->Draw();
        gausfit->Draw("same");
        
        TF1* fit = langaus(signal_on.at(region), mpv, mpv_error, landau_mpv, landau_mpv_error, gausfit->GetParameter(2));
        
        signal_on.at(region)->Draw("same");
        fit->Draw("same");
        double snr_mpv, snr_mpv_error, snr_landau_mpv, snr_landau_mpv_error;
        TF1* snrfit = langaus(snr.at(region), snr_mpv, snr_mpv_error, snr_landau_mpv, snr_landau_mpv_error, 0);
        
        float signal = mpv - gausfit->GetParameter(1);
//        float strip_noise = gausfit->GetParameter(2) / sqrt(3);
        
        if (collection  == 2) {
            myconfig.mydetector.PeakDistance_signal.at(region) = signal;
            myconfig.mydetector.PeakDistance_signal_error.at(region) = mpv_error;
            myconfig.mydetector.PeakDistance_signal_chisq.at(region) = fit->GetChisquare();
            myconfig.mydetector.PeakDistance_signal_ndf.at(region) = fit->GetNDF();
            myconfig.mydetector.PeakDistance_snr.at(region) = snr_mpv;
            myconfig.mydetector.PeakDistance_snr_error.at(region) = snr_mpv_error;
        }
        delete snrfit;
        std::cout << "[ON_TRACK_CLUTER] Signal Region " << region+1 << " is " << signal << " ADC counts!" << std::endl;
//        myconfig.warningstream << "[ON_TRACK_CLUTER] Signal Region " << region+1 << " is " << signal << " ADC counts!" << std::endl;
    }
    std::string signaltitle = myconfig.foldername;
    signaltitle += "/On-track_Signal_";
    signaltitle += myconfig.detname;
    signaltitle += ".png";
    Signal_SNR->SaveAs(signaltitle.c_str());
    myconfig.plotfile->WriteTObject(Signal_SNR,"","Overwrite");
}



void get_noiseprofile (Config& myconfig, int unit) {
    
    std::vector<TGraph*> noise_vector;
    TLegend* noise_legend = new TLegend(0.590517,0.641949,0.900862,0.900424,"","NDC");
    
    for (int i=0; i<myconfig.nfiles; i++)
    {
        TGraph* noise= new TGraph();
        noise->SetTitle("Noise Profile");
        noise_vector.push_back(noise);
    }
    
    int counter = 0;
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        counter++;
        
        Data mydata;
        mydata.readtree(filenames->datafilename);
        mydata.get_entry(0);
        
        std::string title="Run ";
	    size_t last_int= filenames->datafilename.find("_MSSD");
		title += filenames->datafilename.substr(last_int-4,4);
        noise_legend->AddEntry(noise_vector.at(counter-1), title.c_str(),"p");

        for (unsigned int i=0; i<384; i++)
        {
            noise_vector.at(counter-1)->SetPoint(i+1, i+1, myconfig.adc_to_electrons(*filenames,mydata.noise[i], myconfig.strip_to_chip(i), unit));
        }
        noise_vector.at(counter-1)->SetMarkerColor(counter);
    }
    TCanvas* noiseprofile = new TCanvas("noiseprofile", "Noise");
    noiseprofile->cd();
    
    for (int i = 0; i<myconfig.nfiles; i++) 
    {
        
//        noise_legend->AddEntry(noise_vector.at(i));
        noise_vector.at(i)->GetXaxis()->SetRangeUser(0,384);
        noise_vector.at(i)->GetXaxis()->SetTitle("Strip Number");
        if (unit >= 0 && unit < 3)
        {
            noise_vector.at(i)->GetYaxis()->SetTitle("Noise [Electrons]");
            noise_vector.at(i)->GetYaxis()->SetRangeUser(300,3000);
        }
        else if (unit == 3)
        {
            noise_vector.at(i)->GetYaxis()->SetTitle("Noise [ADC]");
            noise_vector.at(i)->GetYaxis()->SetRangeUser(0,5);
        }
        
        noise_vector.at(i)->SetMarkerStyle(6);
        if (i ==0) noise_vector.at(i)->Draw("AP");
        else noise_vector.at(i)->Draw("Psame");
    }
    noise_legend->Draw();
    myconfig.name->Draw();
    
    std::string title = myconfig.foldername;
    title += "/NoiseProfile_";
    title+=myconfig.detname;
    title+=".png";
    noiseprofile->SaveAs(title.c_str());
    myconfig.plotfile->WriteTObject(noiseprofile,"","Overwrite");
}

void get_noise(Config& myconfig, int unit){
    TCanvas* average_noise = new TCanvas("average_noise","Noise per Region");
    TGraph* noise_graph = new TGraph();
    std::vector<TH1F*> noise_vec;
    
    
    for (int i=0; i<12; i++) {
        std::string title = "Noise Distribution for Region ";
        char buffer[5];
        sprintf(buffer,"%i", i+1);
        title += buffer;
        TH1F* noise;
        if ( unit >= 0 && unit < 3 ) noise = new TH1F(title.c_str(),title.c_str(), 100, 0, 1500);//Electrons
        else noise = new TH1F(title.c_str(), title.c_str(),100,0,10);
        noise_vec.push_back(noise);//ADCs
    }

    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        Data mydata;
        mydata.readtree(filenames->datafilename);
        mydata.get_entry(0);
        for (unsigned int i=0; i<384; i++)
        {
            noise_vec.at(11 - std::floor(i/32))->Fill(myconfig.adc_to_electrons(*filenames,mydata.noise[i], myconfig.strip_to_chip(i), unit));
        }
    }
    for (int i=0; i<12; i++) {
        noise_graph->SetPoint(i,i+1,noise_vec.at(i)->GetMean());
        
        myconfig.mydetector.noise.at(i) = noise_vec.at(i)->GetMean();
        
        // std::cout << "Mean " << noise_vec.at(i)->GetMean() << std::endl;

    }
    noise_graph->GetXaxis()->SetTitle("Region");
    if ( unit >= 0 && unit < 3 ) {
        noise_graph->GetYaxis()->SetTitle("Noise [Electrons]");
        noise_graph->GetYaxis()->SetRangeUser(0,1500);
    }
    else if ( unit == 3 ) {
        noise_graph->GetYaxis()->SetTitle("Noise [ADC]");
        noise_graph->GetYaxis()->SetRangeUser(0,3);
    }
    
    //DRAW
    average_noise->cd();
    noise_graph->SetTitle("Noise of Regions");
    noise_graph->SetMarkerColor(1);
    noise_graph->SetMarkerStyle(33);
    noise_graph->SetMarkerSize(2);
    noise_graph->Draw("AP");
    myconfig.name->Draw();
    
    std::string title = myconfig.foldername;
    title += "/Average_Noise_";
    title+=myconfig.detname;
    title+=".png";
    average_noise->SaveAs(title.c_str());
    myconfig.plotfile->WriteTObject(average_noise,"","Overwrite");
}

void check_data(Config& myconfig, int unit) {

    int counter = 0;
    int nevents=0;
    
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        Data mydata;
        mydata.readtree(filenames->datafilename);
        nevents += mydata.get_n_entries();
    }
    TCanvas* datafluctcanvas = new TCanvas("DataStability", "Signalfluctuation");
    TH2F* datafluct;
    if (unit >=0 && unit < 3) datafluct = new TH2F("Signal Distribution", "Fluctuation of Signal", nevents, 0, nevents,1000,-50000,50000);
    else  datafluct = new TH2F("Signal Distribution", "Fluctuation of Signal", nevents, 0, nevents,1000,-100,100);
    
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
        
        Data mydata;
        mydata.readtree(filenames->datafilename);
        
        for (unsigned int i=0; i<mydata.get_n_entries(); i++) {
            mydata.get_entry(i);
            for (unsigned int j=0; j<384; j++) {
                datafluct->Fill(counter,myconfig.adc_to_electrons(*filenames,mydata.signal[j], myconfig.strip_to_chip(j), unit));
            }
            counter++;
        }
    }
    
    datafluctcanvas->cd();
    datafluct->GetXaxis()->SetTitle("Event Number");
    if (unit >= 0 && unit < 3) datafluct->GetYaxis()->SetTitle("Signal [Electrons]");
    else if (unit == 3) datafluct->GetYaxis()->SetTitle("Signal [ADC]");
    datafluct->Draw("colz");
    myconfig.name->Draw();
    
    std::string title = myconfig.foldername;
    title += "/DataStability_";
    title+=myconfig.detname;
    title+=".png";
    datafluctcanvas->SaveAs(title.c_str());
//    myconfig.plotfile->WriteTObject(datafluctcanvas,"","Overwrite");
}

