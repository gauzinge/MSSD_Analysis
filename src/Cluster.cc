//
//  Cluster.cc
//
//
//  Created by Georg Auzinger on 21.06.12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Cluster.h"
//#include "include/Cluster.h"


Cluster::Cluster() : seed(4), neighbor(3), total(0), w_cut(3){
    HitFile=NULL;
    ClusterData=NULL;
}

Cluster::Cluster(int se, int neig, int to, int w) : seed(se), neighbor(neig), total(to), w_cut(w){
    HitFile=NULL;
    ClusterData=NULL;
}

Cluster::~Cluster(){
    if (HitFile!=NULL){
        HitFile->Close();
        delete HitFile;
    }
    
}


void Cluster::make_tree(std::string filename) {
    mydata.readtree(filename.c_str());
    HitFile = TFile::Open(TFile::AsyncOpen(change_filetype(filename,"Cluster").c_str(),"RECREATE"));
	// HitFile = mydata.DUTFile;
    if (!HitFile)std::cerr << "[ERROR] Cannot create Cluster File!" << std::endl;
    else if (HitFile->IsZombie()) std::cerr << "[ERROR] Error opening Cluster File!" << std::endl;
    
    else{
           // std::cout << "[CLUSTER] DataTree with " << mydata.get_n_entries() << " Entries successfully read!" << std::endl;
            ClusterData = new TTree ("ClusterData","Cluster Data");
            ClusterData->Branch("EventNumber", &EventNumber, "EventNumber/I");
            ClusterData->Branch("Clusterwidth", &width, "Clusterwidth/I");
            ClusterData->Branch("FirstStrip", &first_strip, "FirstStrip/I");
            ClusterData->Branch("xPosition", &xPosition, "xPosition/D");
            ClusterData->Branch("zPosition", &zPosition, "zPosition/D");
            ClusterData->Branch("Region", &region, "Region/I");
            ClusterData->Branch("Clu_charge", &charge, "Clu_charge/F");
            ClusterData->Branch("Clu_charge_array", charge_array, "Clu_charge_array[15]/F");
            ClusterData->Branch("Clu_noise", &noise, "Clu_noise/F");
            ClusterData->Branch("Clu_snr", &snr, "Clu_snr/F");
            ClusterData->Branch("Clu_significance", &significance, "Clu_significance/F");
            int counter=0;
            for (unsigned int j = 0; j<mydata.get_n_entries(); j++) {
                mydata.get_entry(j);
                Cluster::find_cluster(total, w_cut);
//                if (mydata.EventNumber%1000==0) std::cout<< "[CLUSTER] " << mydata.EventNumber << " Events Processed!" << std::endl;
                int n=cog_SeedStrip.size();
                //                    std::cout << "[CLUSTER] " << n << " Clusters in Event " << mydata.EventNumber << std::endl;
                counter+=n;
                for (int i=0; i<n; i++) {
                    if (found_cluster[i]==true) {
                        width=clu_width[i];
                        charge=clu_charge[i];
                        noise=clu_noise[i];
                        snr=clu_snr[i];
                        significance=clu_significance[i];
                        first_strip=first_st[i];
                        cog_strip=cog_SeedStrip[i];
                        zPosition=mydata.zPosition;
                        region=Cluster::hitmaker();
                        
                        charge_vector = found_vector.at(i);
                        for (int a=0; a<15; a++) charge_array[a] = 0;
                        int index = 0;
                        for (std::vector<float>::iterator it = charge_vector.begin(); it != charge_vector.end(); it++)
                        {
                            charge_array[index] = *it;
                            index++;
                        }
                        ClusterData->Fill();
                    }
                }
                
            }
            HitFile->Write();
            HitFile->Close();
			// mydata.DUTFile->Close();
            std::cout << "[CLUSTER] " << counter << " Clusters Found in DataSet!" << std::endl;
//            std::cout << "[CLUSTER] Cluster Data Tree appended to DUT Data File!" << std::endl;
    }
    
}

void Cluster::readtree(std::string filename) {
	if(HitFile != NULL){
		HitFile->Close();
		delete HitFile;
	}
	//Open DUTHit File and itialize Tree
	HitFile = TFile::Open(TFile::AsyncOpen(change_filetype(filename,"Cluster").c_str()));
	if (HitFile) {
		HitFile->GetObject("ClusterData", ClusterData);
    
		//Define and Read DUT HIT Branches
		TBranch* EventNo = ClusterData->GetBranch("EventNumber");
		TBranch* SeedStr = ClusterData->GetBranch("FirstStrip");
		TBranch* cluwidth = ClusterData->GetBranch("Clusterwidth");
		TBranch* xPos = ClusterData->GetBranch("xPosition");
		TBranch* zPos = ClusterData->GetBranch("zPosition");
		TBranch* reg = ClusterData->GetBranch("Region");
		TBranch* clucharge = ClusterData->GetBranch("Clu_charge");
		TBranch* clucharge_array = ClusterData->GetBranch("Clu_charge_array");
		TBranch* clunoise = ClusterData->GetBranch("Clu_noise");
		TBranch* clusnr = ClusterData->GetBranch("Clu_snr");
		TBranch* clusignificance = ClusterData->GetBranch("Clu_significance");
    
		EventNo->SetAddress(&EventNumber);
		SeedStr->SetAddress(&first_strip);
		xPos->SetAddress(&xPosition);
		zPos->SetAddress(&zPosition);
		reg->SetAddress(&region);
		cluwidth->SetAddress(&width);
		clucharge->SetAddress(&charge);
		clucharge_array->SetAddress(charge_array);
		clunoise->SetAddress(&noise);
		clusnr->SetAddress(&snr);
		clusignificance->SetAddress(&significance);
    
		//    std::cout <<"[CLUSTER] Cluster Data Tree with " << ClusterData->GetEntries() << " entries read successfully!" << std::endl;
	}
}

unsigned int Cluster::get_n_entries(){
    if (ClusterData == NULL){
        std::cerr << "[ERROR] No Cluster Data available!" << std::endl;
        return 0;
    }
    return static_cast<unsigned int>(ClusterData->GetEntries());
}

void Cluster::get_entry(int i_entry){
    ClusterData->GetEntry(i_entry);
    int i=0;
    charge_vector.clear();
    while (charge_array[i] != 0) {
        charge_vector.push_back(charge_array[i]);
        i++;
    }
}


void Cluster::find_cluster(int total_cut, int width_cut) {
    
    first_strip=0;
    cog_strip=0;
    width=0;
    charge=0;
    noise=0;
    snr=0;
    
    first_st.clear();
    found_cluster.clear();
    clu_charge.clear();
    clu_noise.clear();
    clu_snr.clear();
    clu_significance.clear();
    clu_width.clear();
    cog_SeedStrip.clear();
    found_vector.clear();
    
    for (unsigned int reg=0; reg<12; reg++) {
        
        
        int ir_start = 32*reg;
        int ir_stop = 32*(reg+1);
        
        int i = ir_start;
        
        while (i < ir_stop) {
            
            float square_sum_noise=0;
            float tmp_clu_charge = 0;
            float tmp_clu_snr = 0;
            float tmp_significance = 0;
            float tmp_clu_noise = 0;
            float tmp_clu_noise_sq = 0;
            double weighted_sum=0;
            bool found=false;
            
            if (mydata.signal[i]/mydata.noise[i] > seed) {
                int up = 0;
                int down = 0;
                
                while ((mydata.signal[i-down-1]/mydata.noise[i-down-1] > neighbor) && ((i-down-1) >= ir_start)) {
                    tmp_clu_charge += mydata.signal[i-down-1];
                    square_sum_noise += mydata.noise[i-down-1]*mydata.noise[i-down-1];
                    weighted_sum += static_cast<double>(mydata.signal[i-down-1] * (i-down-1));
                    down++;
                }
                while ((mydata.signal[i+up+1]/mydata.noise[i+up+1] > neighbor) && ((i+up+1) < ir_stop)) {
                    tmp_clu_charge += mydata.signal[i+up+1];
                    square_sum_noise += mydata.noise[i+up+1]*mydata.noise[i+up+1];
                    weighted_sum += static_cast<double>(mydata.signal[i+up+1] * (i+up+1));
                    up++;
                }
                
                square_sum_noise +=mydata.noise[i]*mydata.noise[i];
                tmp_clu_charge += mydata.signal[i];
                weighted_sum += static_cast<double>(mydata.signal[i]*i);
                tmp_clu_noise_sq = sqrt(square_sum_noise);
                tmp_clu_noise = mydata.noise[i];
                tmp_clu_snr = tmp_clu_charge/tmp_clu_noise;
                tmp_significance = tmp_clu_charge/tmp_clu_noise_sq;
                
                if ((tmp_significance > total_cut) && ((1+up+down)<15) /*&& ((1+up+down)<=width_cut)*/) // restrict to <14 because of array size
                {
                    found=true;
                    found_cluster.push_back(found);
                    cog_SeedStrip.push_back(static_cast<double>(weighted_sum / tmp_clu_charge));
                    clu_width.push_back(1+up+down);
                    clu_charge.push_back(tmp_clu_charge);
                    clu_noise.push_back(tmp_clu_noise);
                    clu_snr.push_back(tmp_clu_snr);
                    clu_significance.push_back(tmp_significance);
                    EventNumber = mydata.EventNumber;
                    first_st.push_back(i-down);
                    
                    
                    
                    charge_vector.clear();
                    for (int index=i-down; index<=i+up; index++)//ad 2 neighbors to any clusters charge vector in case i want to change eta function
                    {
                        charge_vector.push_back(mydata.signal[index]);
                    }
                    found_vector.push_back(charge_vector);
                    
                    //                        std::cout << "[CLUSTER] Found Cluster at Strip " << i << " with Clusterwidth " << 1+up+down << " , total Charge " << tmp_clu_charge << " and Cluster SNR " << tmp_clu_snr << " in Event " <<  EventNumber << std::endl;
                }
                i+=(up+1);
            } else {
                i++;
            }
        }
    }
}


int Cluster::hitmaker() {
    xPosition=0;
    cog_strip+=1;
    double dist_edge = 0.900; //mm distance from physical Si sensor edge to "0" Position ("0" + 1 x Pitch = 1st Strip)
    
    double xPitch[12] = {0.12,0.24,0.08,0.07,0.12,0.24,0.08,0.07,0.12,0.24,0.08,0.07}; //mm
    if ( ( cog_strip >352 ) && ( cog_strip<=385 ) ) {
        xPosition = dist_edge + 59.7605 + ( cog_strip - 352 ) * xPitch[0] ;  //Region 1, 120um
        return 1;
        
    } else if ( ( cog_strip > 320 ) && ( cog_strip <= 352 ) ) {
        xPosition = dist_edge + 50.6545 + ( ( cog_strip - 320) * xPitch[1]) ;  //Region 2, 240um
        return 2;
        
    } else if ( ( cog_strip > 288 ) && ( cog_strip <= 320 ) ) {
        xPosition = dist_edge + 46.8045 + ( ( cog_strip - 288) * xPitch[2]) ;  //Region 3, 80um
        return 3;
        
    } else if ( ( cog_strip > 256 ) && ( cog_strip <= 288 ) ) {
        xPosition = dist_edge + 43.283 + ( ( cog_strip - 256) * xPitch[3]) ;  //Region 4, 70um
        return 4;
        
    } else if ( ( cog_strip > 224 ) && ( cog_strip <= 256 ) ) {
        xPosition = dist_edge + 38.119 + ( ( cog_strip - 224) * xPitch[4]) ;  //Region 5, 120um
        return 5;
        
    } else if ( ( cog_strip > 192 ) && ( cog_strip <= 224 ) ) {
        xPosition = dist_edge + 29.013 + ( ( cog_strip - 192) * xPitch[5]) ;  //Region 6, 240um
        return 6;
        
    } else if ( ( cog_strip > 160 ) && ( cog_strip <= 192 ) ) {
        xPosition = dist_edge + 25.163 + ( ( cog_strip - 160) * xPitch[6]) ;  //Region 7, 80um
        return 7;
        
    } else if ( ( cog_strip > 128 ) && ( cog_strip <= 160 ) ) {
        xPosition = dist_edge + 21.63075 + ( ( cog_strip - 128) * xPitch[7]) ;  //Region 8, 70um
        return 8;
        
    } else if ( ( cog_strip > 96 ) && ( cog_strip <= 128 ) ) {
        xPosition = dist_edge + 16.4775 + ( ( cog_strip - 96) * xPitch[8]) ;  //Region 9, 120um
        return 9;
        
    } else if ( ( cog_strip > 64 ) && ( cog_strip <= 96 ) ) {
        xPosition = dist_edge + 7.36075 + ( ( cog_strip - 64) * xPitch[9]) ;  //Region 10, 240um
        return 10;
        
    } else if ( ( cog_strip > 32 ) && ( cog_strip <= 64 ) ) {
        xPosition = dist_edge + 3.5215 + ( ( cog_strip - 32) * xPitch[10]) ;  //Region 11, 80um
        return 11;
        
    } else if ( cog_strip <= 32 )  {
        xPosition = dist_edge + 0 + ( ( cog_strip - 0) * xPitch[11]) ;  //Region 12, 70um
        return 12;
    } else return 0;
    //        std::cout<<myhit.xPosition << " " << xPosition << std::endl;
    cog_strip-=1;
}

