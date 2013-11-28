//
//  on_track_signal.cc
//  
//
//  Created by Georg Auzinger on 14.12.12.
//
//

#include "onTrackCluster.h"

const int onTrackCluster::maximumClusterNeighbours = 2;
const int maximumClusterNeighboursforCuts = 2;
//onTrackCluster::onTrackCluster(){}
//
//onTrackCluster::~onTrackCluster (){
//    if (file!=NULL){
//        file->Close();
//        delete file;
//    }
//}

void onTrackCluster::make_tree(std::string datafilename, std::string trackfilename, std::vector<float> cuts) {
    Cluster mycluster;
    double y_min;
	double y_max;
	int nbins = 100;
	TProfile* yprofile = new TProfile("YHitProfile","y Profile",nbins, -10, 45);
	yprofile->SetErrorOption("");
    file = NULL;
    tree = NULL;
    
    int counter=0;
    mydata.readtree(datafilename.c_str());
	mycluster.readtree(datafilename.c_str());
    mytrack.readtree(trackfilename.c_str());
    Alignment myalignment(datafilename, trackfilename);
    myalignment.get_alignment(datafilename.c_str());
    EventCollection mycollection;
    EventCollection myothercollection;
    

    //Data
    for (unsigned int i=0; i<mydata.get_n_entries(); i++) {
        mydata.get_entry(i);
        mycollection.addHit(i, mydata.EventNumber);
    }
    std::string matchfilename = find_matchfiledir(datafilename);
    size_t last_int= datafilename.find("_MSSD");
    matchfilename += datafilename.substr(last_int-4,4);
    matchfilename += "_onTrack.txt";
    
    std::ofstream binaryMatchFile(matchfilename.c_str());
    mycollection.Serialize(binaryMatchFile);
    
    std::ifstream inFile(matchfilename.c_str());
    mycollection.DeSerialize(inFile);

	//Cluster for y range
    for (unsigned int i=0; i<mycluster.get_n_entries(); i++) {
        mycluster.get_entry(i);
        myothercollection.addHit(i, mycluster.EventNumber);
    }
    std::string matchfilename1 = find_matchfiledir(datafilename);
    size_t last_int1= datafilename.find("_MSSD");
    matchfilename1 += datafilename.substr(last_int1-4,4);
    matchfilename1 += "_tmpCluster.txt";
    
    std::ofstream ontrackMatchFile(matchfilename.c_str());
    myothercollection.Serialize(ontrackMatchFile);
    
    std::ifstream inFile1(matchfilename.c_str());
    myothercollection.DeSerialize(inFile1);
	
	
    file = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"onTrackCluster").c_str(), "RECREATE"));
    if (!file)std::cerr << "[ERROR] Cannot create onTrackCluster File!" << std::endl;
    else if (file->IsZombie()) std::cerr << "[ERROR] Error opening onTrackCluster File!" << std::endl;
    
    else{
        if (tree!=NULL) {
            tree->Delete();
            std::cout << "[ON_TRACK_CLUSTER] Overwriting old onTrackCluster Tree!" << std::endl;
        }
        else if (!tree){
            tree = new TTree("onTrackClusters", "Clusters matched to Tracks");
            tree->Branch("EventNumber", &EventNumber,"EventNumber/I");
            tree->Branch("xPosition", &xPosition, "xPosition/D");
            tree->Branch("zPosition", &zPosition, "zPosition/D");
            tree->Branch("xTrack", &xTrack, "xTrack/D");
            tree->Branch("yTrack", &yTrack, "yTrack/D");
            tree->Branch("Region", &region, "Region/I");
            tree->Branch("FirstStrip", &first_strip, "FirstStrip/I");
            tree->Branch("Width", &width, "Width/I");
            tree->Branch("Charge", &charge, "Charge/F");
            tree->Branch("Charge_array", charge_array, "Charge_array[5]/F");
            tree->Branch("Charge_5strips", charge_5strip_array, "Charge_5strips[5]/F");
            tree->Branch("Noise", &noise, "Noise/F");
            tree->Branch("SNR", &snr, "SNR/F");
            tree->Branch("Cut", cut, "Cut_array[13]/F");

			// //y range
            for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
            {
                mytrack.get_entry(j);
                mytrack.apply_alignment(myalignment.angle,myalignment.x_shift,myalignment.y_shift,myalignment.DUT_zPosition);
                
                const std::vector<int>& eventHits = myothercollection.GetHitLines(mytrack.EventNumber);
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
			TF1 *fit = new TF1("fit","pol0",-10,30);
			yprofile->Fit("fit", "RQON");
			double maximum = fit->GetParameter(0);
			double minimum = yprofile->GetMinimum();
			double maxvalue = (maximum - minimum)*0.50;
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
			
			y_min = yprofile->GetBinCenter(minbin);
			y_max = yprofile->GetBinCenter(maxbin);
			
			delete fit;
			delete yprofile;
			
			//match trackhits
            for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
            {
                mytrack.get_entry(j);
                mytrack.apply_alignment(myalignment.angle,myalignment.x_shift,myalignment.y_shift,myalignment.DUT_zPosition);
                
                const std::vector<int>& eventHits = mycollection.GetHitLines(mytrack.EventNumber);
                std::vector<int>::const_iterator it;
                
                for(it = eventHits.begin() ; it!= eventHits.end(); ++it)
                {
                    const int& hitLine = *it;
					
                    mydata.get_entry(hitLine);
                    int nearest_strip = reverse_hitmaker(mytrack.xPrime);
                    if (track_good_strips(nearest_strip) && (track_good(mytrack.xPrime, mytrack.yPrime,0) == 1) && track_good_y(mytrack.yPrime,y_min,y_max))
                    {
						nearest_strip = static_cast<unsigned int> (nearest_strip);
                        mydata.get_entry(hitLine);
                        float signal = 0;
                        int reg = get_region(mytrack.xPrime);
                        unsigned int maxsignal_strip = 0;
                        float maxsignal = 0;
                        int widthcounter = 0;
                        unsigned int first_str = 384;
                        float center_strip = 0;
                        std::list<float> tmp_chargevector;
                        tmp_chargevector.clear();

                        // Let's cheat and start clustering where the track hit is :>
                        // From the central strip up...
                        for (int nstrip = nearest_strip; nstrip<=nearest_strip+maximumClusterNeighbours; nstrip++) {
                          if (mydata.signal[nstrip] < cuts.at(reg-1)) break;
                          signal += mydata.signal[nstrip];
                          tmp_chargevector.push_back(mydata.signal[nstrip]);
                          center_strip += mydata.signal[nstrip]*nstrip;
                          widthcounter++;
                          if (nstrip < static_cast<int>(first_str)) first_str = nstrip;
                          if (mydata.signal[nstrip] > maxsignal) maxsignal = mydata.signal[nstrip], maxsignal_strip = nstrip;
                        }
                        // ... and then down
                        for (int nstrip = nearest_strip-1; nstrip>=nearest_strip-maximumClusterNeighbours; nstrip--) {
                          if (mydata.signal[nstrip] < cuts.at(reg-1)) break;
                          signal += mydata.signal[nstrip];
                          tmp_chargevector.push_front(mydata.signal[nstrip]);
                          center_strip += mydata.signal[nstrip]*nstrip;
                          widthcounter++;
                          first_str = nstrip;
                          if (mydata.signal[nstrip] > maxsignal) maxsignal = mydata.signal[nstrip], maxsignal_strip = nstrip;
                        }
                        
                        unsigned int tmp_index=0;
                        for (int nstrip = nearest_strip-2; nstrip <=nearest_strip+2;nstrip++) {
                            charge_5strip_array[tmp_index] = mydata.signal[nstrip];
                            tmp_index++;
                        }
                        
                        charge_vector.clear();
                        for (std::list<float>::iterator it = tmp_chargevector.begin(); it!=tmp_chargevector.end(); ++it) {
                          this->charge_vector.push_back(*it);
                        }

                        this->first_strip = first_str;
                        this->charge = signal;
                        //this->charge_vector = tmp_chargevector;
                        this->width = widthcounter;
                        this->noise = mydata.noise[maxsignal_strip];
                        this->snr = this->charge / mydata.noise[maxsignal_strip];
                        this->xTrack = mytrack.xPrime;
                        this->yTrack = mytrack.yPrime;
                        this->zPosition =  mydata.zPosition;
                        this->EventNumber = mytrack.EventNumber;
                        this->region = reg;
                        this->xPosition = hitmaker(static_cast<double>(center_strip/this->charge));
                        for (int i=0; i<5; i++) {
                            charge_array[i] = 0;
                            this->charge_5strips.push_back(charge_5strip_array[i]);
                        }
                        int i=0;
                        for (std::vector<float>::iterator it = this->charge_vector.begin(); it != this->charge_vector.end(); it++) {
                            charge_array[i] = *it;
                            i++;
                        }
                        
                        int regionindex = 0;
                        for (int a=0; a<13; a++) {
                            cut[a] = 0;
                        }
                        
                        for (std::vector<float>::iterator it = cuts.begin(); it != cuts.end(); it++) {
                            cut[regionindex] = * it;
                            regionindex++;
                        }
                        
                        this->cut_vector = cuts;
                        counter++;
                        tree->Fill();
                    }
                }
            }
            file->Write("",TObject::kOverwrite);
            file->Close();
            std::cout <<"[ON_TRACK_CLUSTER] " << counter << " Tracks within Regions. Reconstructed Clusters appended to DUT Data File" << std::endl;

        }
    }
}

void onTrackCluster::readtree(std::string datafilename) {
    
	if(file != NULL){
		file->Close();
		delete file;
	}
	std::cout << datafilename << std::endl;
	file = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"onTrackCluster").c_str()));
	if (file) {
		file->GetObject("onTrackClusters", tree);
		TBranch* EventNumber_br = tree->GetBranch("EventNumber");
		TBranch* xPosition_br = tree->GetBranch("xPosition");
		TBranch* zPosition_br = tree->GetBranch("zPosition");
		TBranch* xTrack_br = tree->GetBranch("xTrack");
		TBranch* yTrack_br = tree->GetBranch("yTrack");
		TBranch* region_br = tree->GetBranch("Region");
		TBranch* first_strip_br = tree->GetBranch("FirstStrip");
		TBranch* width_br = tree->GetBranch("Width");
		TBranch* charge_br = tree->GetBranch("Charge");
		TBranch* charge_array_br = tree->GetBranch("Charge_array");
		TBranch* charge_5strip_array_br = tree->GetBranch("Charge_5strips");
		TBranch* noise_br = tree->GetBranch("Noise");
		TBranch* snr_br = tree->GetBranch("SNR");
		TBranch* cut_br = tree->GetBranch("Cut");

		EventNumber_br->SetAddress(&EventNumber);
		xPosition_br->SetAddress(&xPosition);
		zPosition_br->SetAddress(&zPosition);
		xTrack_br->SetAddress(&xTrack);
		yTrack_br->SetAddress(&yTrack);
		region_br->SetAddress(&region);
		first_strip_br->SetAddress(&first_strip);
		width_br->SetAddress(&width);
		charge_br->SetAddress(&charge);
		charge_array_br->SetAddress(charge_array);
		charge_5strip_array_br->SetAddress(charge_5strip_array);
		noise_br->SetAddress(&noise);
		snr_br->SetAddress(&snr);
		cut_br->SetAddress(cut);
	}
	//    std::cout << "[ON_TRACK_CLUSTER] OnTrackClusterTree with " << tree->GetEntries() << " Entries read successfully!" << std::endl;
}

//unsigned int onTrackCluster::get_n_entries(){
//    if (tree == NULL) {
//        std::cerr << "[ERROR] No onTrackCluster Data available!" << std::endl;
//        return 0;
//    }
//    return static_cast<unsigned int>(tree->GetEntries());
//}

void onTrackCluster::get_entry(int i_entry) {
    tree->GetEntry(i_entry);
    int i=0;
    this->charge_vector.clear();
    while (charge_array[i] != 0) {
        charge_vector.push_back(charge_array[i]);
        i++;
    }
    this->charge_5strips.clear();
    for (int h=0; h<5; h++) {
        charge_5strips.push_back(charge_5strip_array[h]);
    }
    int j=0;
    cut_vector.clear();
    while (cut[j] != 0) {
        cut_vector.push_back(cut[j]);
        j++;
    }
}

std::vector<float> get_cut(Config& myconfig, float threshold, float ycut) {
        
    std::vector<TH1F*> signal_on;
    std::vector<TH1F*> signal_off;
        
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
        
        TH1F* signaldummy_on = new TH1F(name.c_str(), title.c_str(),100,-30,70);
        TH1F* signaldummy_off = new TH1F(name2.c_str(), title2.c_str(),100,-30,70);


        signal_on.push_back(signaldummy_on);
        signal_off.push_back(signaldummy_off);
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
        matchfilename += "_onTrack.txt";
        
        std::ofstream binaryMatchFile(matchfilename.c_str());
        mycollection.Serialize(binaryMatchFile);
        
        std::ifstream inFile(matchfilename.c_str());
        mycollection.DeSerialize(inFile);
        
        for (unsigned int ntrack=0; ntrack<mytrack.get_n_entries(); ntrack++)
        {
            mytrack.get_entry(ntrack);
            mytrack.apply_alignment(myalignment.angle, myalignment.x_shift,myalignment.y_shift,myalignment.DUT_zPosition);
            
            const std::vector<int>& eventTracks = mycollection.GetHitLines(mytrack.EventNumber);
            std::vector<int>::const_iterator it;
            
            for(it = eventTracks.begin() ; it!= eventTracks.end(); ++it)
            {
                const int& hitLine = *it;
                
                int region = get_region(mytrack.xPrime);
                unsigned int nearest_strip = (unsigned int)(reverse_hitmaker(mytrack.xPrime)-1);
                if (track_good_strips(nearest_strip) && track_good(mytrack.xPrime, mytrack.yPrime,0) == 1)
                {
                    unsigned int anti_strip =  get_antistrip(nearest_strip);
                    mydata.get_entry(hitLine);
                    
                    float charge = 0;
                    float anticharge = 0;
//                    int maxsignal_strip = 0;
                    float maxsignal = 0;
//                    unsigned int anti_maxsignal_strip = 0;
                    float anti_maxsignal = 0;
                    
                    for (unsigned int nstrip = nearest_strip - maximumClusterNeighboursforCuts; nstrip <=nearest_strip+maximumClusterNeighboursforCuts; nstrip++)
                    {
                        charge += mydata.signal[nstrip];
                        if (mydata.signal[nstrip] > maxsignal){
                            maxsignal = mydata.signal[nstrip];
//                            maxsignal_strip = nstrip;
                        }
                    }
                    for (unsigned int nstrip = anti_strip - maximumClusterNeighboursforCuts; nstrip <=anti_strip+maximumClusterNeighboursforCuts; nstrip++)
                    {
                        anticharge += mydata.signal[nstrip];
                        if (mydata.signal[nstrip] > anti_maxsignal) anti_maxsignal = mydata.signal[nstrip]; //, anti_maxsignal_strip = nstrip;
                    }
                    
                
                    signal_on.at(region-1)->Fill(charge);
                    signal_off.at(region-1)->Fill(anticharge);
                }
            }
        }
    }
    
    TCanvas* OnTrackCuts = new TCanvas("OnTrackCuts", "Track related Signal");
    std::vector<float> cut_vector;
   
    OnTrackCuts->Divide(4,3);
    double adc_threshold;
    for (unsigned int region = 0; region < 12; region++) {
        gStyle->SetOptFit(1111);

        //SIGNAL
        OnTrackCuts->cd(region+1);
        double mpv, mpv_error, landau_mpv, landau_mpv_error;
        TF1* gausfit = new TF1("noisefit","gaus",-20,20);
        gausfit->SetLineColor(4);
        signal_off.at(region)->Fit("noisefit","RQN");
        signal_off.at(region)->SetLineColor(1);
        signal_off.at(region)->Draw();
        gausfit->Draw("same");
        TF1* fit = langaus(signal_on.at(region), mpv, mpv_error, landau_mpv, landau_mpv_error, gausfit->GetParameter(2));
        signal_on.at(region)->Draw("same");
        fit->Draw("same");
        
        adc_threshold = fabs(landau_mpv - gausfit->GetParameter(1)) * threshold + gausfit->GetParameter(1);
        cut_vector.push_back(adc_threshold);
        float ymax = signal_off.at(region)->GetMaximum();
        TLine* cutline = new TLine(adc_threshold,0,adc_threshold,ymax);
        cutline->SetLineColor(8);
        cutline->SetLineWidth(2);
        cutline->Draw("same");
        
        std::cout << "[ON_TRACK_CLUSTER] The Calculated Threshold (Signal * 0.4) for region " << region+1 << " is " << adc_threshold << " ADC counts!" << std::endl;
        myconfig.warningstream << "[ON_TRACK_CLUSTER] The Calculated Threshold (Signal * 0.4) for region " << region+1 << " is " << adc_threshold << " ADC counts!" << std::endl;
    }
    
    
    std::string signaltitle = "Results/";
    signaltitle += myconfig.detname;
    signaltitle += "/CalculateCuts.png";
    OnTrackCuts->SaveAs(signaltitle.c_str());
    // myconfig.plotfile->WriteTObject(OnTrackCuts,"","Overwrite");
    return cut_vector;
}
