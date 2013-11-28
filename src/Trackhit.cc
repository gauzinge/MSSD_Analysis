//
//  trackhits.cc
//
//
//  Created by Georg Auzinger on 05.07.12.
//
#include "Trackhit.h"
//#include "include/Trackhit.h"

//Trackhit::Trackhit(){}

//Trackhit::~Trackhit (){
//    if (file!=NULL){
//        file->Close();
//        delete file;
//    }
//}

void Trackhit::make_tree(std::string datafilename, std::string trackfilename, std::vector<float> cuts) {
    
    file = NULL;
    tree = NULL;
	
    double y_min;
	double y_max;
	int nbins = 100;
	TProfile* yprofile = new TProfile("YHitProfile","y Profile",nbins, -10, 45);
	yprofile->SetErrorOption("");
	
    int counter=0;
    mycluster.readtree(datafilename.c_str());
    mytrack.readtree(trackfilename.c_str());
    Alignment myalignment(datafilename, trackfilename);
    myalignment.get_alignment(datafilename.c_str());
    EventCollection mycollection;
    
    for (unsigned int i=0; i<mycluster.get_n_entries(); i++) {
        mycluster.get_entry(i);
        mycollection.addHit(i, mycluster.EventNumber);
    }
    std::string matchfilename = find_matchfiledir(datafilename);
    size_t last_int= datafilename.find("_MSSD");
    matchfilename += datafilename.substr(last_int-4,4);
    matchfilename += "_Trackhit.txt";
    
    std::ofstream clusterMatchFile(matchfilename.c_str());
    mycollection.Serialize(clusterMatchFile);
    
    std::ifstream inFile(matchfilename.c_str());
    mycollection.DeSerialize(inFile);
    
    
    file = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"Trackhit").c_str(), "RECREATE"));
    if (!file)std::cerr << "[ERROR] Cannot create Trackhit File!" << std::endl;
    else if (file->IsZombie()) std::cerr << "[ERROR] Error opening Trackhit File!" << std::endl;
    
    else{
        if (tree!=NULL) {
            tree->Delete();
            std::cout << "[TRACKHIT] Overwriting old Trackhit Tree!" << std::endl;
        }
        
        else if (!tree){
//            std::cout << "[TRACKHIT] ClusterTree with " << mycluster.get_n_entries() << " Entries successfully read!" << std::endl;
            
            tree = new TTree("Trackhits", "Clusters matched to Tracks");
            tree->Branch("EventNumber", &EventNumber,"EventNumber/I");
            tree->Branch("xPosition", &xPosition, "xPosition/D");
            tree->Branch("zPosition", &zPosition, "zPosition/D");
            tree->Branch("xTrack", &xTrack, "xTrack/D");
            tree->Branch("yTrack", &yTrack, "yTrack/D");
            tree->Branch("Region", &region, "Region/I");
            tree->Branch("FirstStrip", &first_strip, "FirstStrip/I");
            tree->Branch("Width", &width, "Width/I");
            tree->Branch("Charge", &charge, "Charge/F");
            tree->Branch("Charge_array", charge_array, "Charge_array[15]/F");
            tree->Branch("Noise", &noise, "Noise/F");
            tree->Branch("SNR", &snr, "SNR/F");
            tree->Branch("Cut", cut, "Cut_array[13]/F");

			//y range
            for (unsigned int j=0; j<=mytrack.get_n_entries(); j++)
            {
                mytrack.get_entry(j);
                mytrack.apply_alignment(myalignment.angle,myalignment.x_shift,myalignment.y_shift,myalignment.DUT_zPosition);
                
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
			
			// TLine* line1 = new TLine(y_min,0,y_min,maxvalue);
// 			TLine* line2 = new TLine(y_max,0,y_max,maxvalue);
// 			TLine* line3 = new TLine(y_min,maxvalue,y_max,maxvalue);
// 			line1->SetLineColor(2);
// 			line1->SetLineWidth(2);
// 			line2->SetLineColor(2);
// 			line2->SetLineWidth(2);
// 			line3->SetLineColor(2);
// 			line3->SetLineWidth(2);
// 	
// 			//Draw Profile
// 			TCanvas* y_efficiency =  new TCanvas("y_Trackhits","Y_Trackhits");
// 			y_efficiency->cd();
// 			yprofile->Draw();
// 			line1->Draw("same");
// 			line2->Draw("same");
// 			line3->Draw("same");
// 			
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
                    mycluster.get_entry(hitLine);
                    
                    if ((fabs(mycluster.xPosition - mytrack.xPrime) < get_pitch_region(get_region(mytrack.xPrime))*0.001*3) && (track_good(mytrack.xPrime, mytrack.yPrime, 0) == 1) && track_good_y(mytrack.yPrime,y_min,y_max))
                    {
                        EventNumber = mycluster.EventNumber;
                        xPosition = mycluster.xPosition;
                        zPosition = mycluster.zPosition;
                        xTrack = mytrack.xPrime;
                        yTrack = mytrack.yPrime;
                        region = mycluster.region;
                        first_strip = mycluster.first_strip;
                        width = mycluster.width;
                        charge = mycluster.charge;
                        noise = mycluster.noise;
                        snr = mycluster.snr;
                        for (int a=0; a<15; a++) {
                            charge_array[a] = 0;
                        }
                        int i=0;
                        
                        for (std::vector<float>::iterator iter = mycluster.charge_vector.begin(); iter != mycluster.charge_vector.end(); iter++)
                        {
                            charge_array[i] = *iter;
                            i++;
                        }
                        for (int a=0; a<13; a++) {
                            cut[a] = 0;
                            if (a<12)this->cut_vector.push_back(0);
                        }
                        counter++;
                        tree->Fill();
                    }
                    
                }
            }
            file->Write();
            file->Close();
            
            std::cout << "[TRACKHIT] " << counter << " Clusters matched to Telescope Tracks" << std::endl;
//            std::cout <<"[TRACKHIT] Matched Hits Data appended to DUT Data File" << std::endl;
            
        }
    }
}

void Trackhit::readtree(std::string datafilename) {
	if(file != NULL){
		file->Close();
		delete file;
	}
	file = TFile::Open(TFile::AsyncOpen(change_filetype(datafilename,"Trackhit").c_str()));
	if (file) {
		file->GetObject("Trackhits", tree);
    
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
		noise_br->SetAddress(&noise);
		snr_br->SetAddress(&snr);
		cut_br->SetAddress(cut);
	}
	else std::cout <<"[ERROR] TFile " << file << " could not be opened!" << std::endl;
	//    std::cout << "[TRACKHIT] tree with " << tree->GetEntries() << " Entries read successfully!" << std::endl;
}

//unsigned int Trackhit::get_n_entries(){
//    if (tree == NULL) {
//        std::cerr << "[ERROR] No Trackhit Data available!" << std::endl;
//        return 0;
//    }
//    return static_cast<unsigned int>(tree->GetEntries());
//}

void Trackhit::get_entry(int i_entry) {
    tree->GetEntry(i_entry);
    int i=0;
    charge_vector.clear();
    while (charge_array[i] != 0) {
        charge_vector.push_back(charge_array[i]);
        i++;
    }
    int j=0;
    cut_vector.clear();
    while (j < 12) {
        cut_vector.push_back(0);
        j++;
    }
    for (int h=0; h<5; h++) {
        charge_5strips.push_back(0);
    }
}



