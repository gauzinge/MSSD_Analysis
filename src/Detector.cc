//
//  Detector.cpp
//  
//
//  Created by Georg Auzinger on 13.11.12.
//
//

#include "Detector.h"

Detector::Detector() {
    this->name = "";
    this->material = "";
    this->doping = "";
    this->thickness = 0;
    this->fluence = 0;
    this->particle = "";
    this->annealing = 0;
    this->voltage = 0;
    
    for (int i =0; i<12; i++) {
        this->onTrackCluster_resolution.push_back(0);
        this->onTrackCluster_snr.push_back(0);
        this->onTrackCluster_snr_error.push_back(0);
        this->onTrackCluster_signal.push_back(0);
        this->onTrackCluster_signal_error.push_back(0);
        this->onTrackCluster_signal_chisq.push_back(0);
        this->onTrackCluster_signal_ndf.push_back(0);
		
        this->onTrackCluster_width.push_back(0);
        
        this->Trackhit_resolution.push_back(0);
        this->Trackhit_snr.push_back(0);
        this->Trackhit_snr_error.push_back(0);
        this->Trackhit_signal.push_back(0);
        this->Trackhit_signal_error.push_back(0);
		this->Trackhit_signal_chisq.push_back(0);
		this->Trackhit_signal_ndf.push_back(0);
        this->Trackhit_width.push_back(0);

        this->PeakDistance_snr.push_back(0);
        this->PeakDistance_snr_error.push_back(0);
        this->PeakDistance_signal.push_back(0);
        this->PeakDistance_signal_error.push_back(0);
        this->PeakDistance_signal_chisq.push_back(0);
        this->PeakDistance_signal_ndf.push_back(0);
		
        this->noise.push_back(0);
        this->efficiency.push_back(0);
		this->occupancy.push_back(0);
    }
    detectorfile=NULL;
    detectortree=NULL;
}

Detector::~Detector() {
    if (detectorfile!=NULL) {
        detectorfile->Close();
        delete detectorfile;
    }
}

void Detector::write_file(std::string filename) {
    
    char char_name[30];//=new char[30];
//    strcpy(char_name, name.c_str());
    sprintf(char_name,"%s",name.c_str());
    char char_material[5];//=new char[5];
//    strcpy(char_material, material.c_str());
    sprintf(char_material,"%s",material.c_str());
    char char_particles[15];//=new char[15];
//    strcpy(char_particles, particle.c_str());
    sprintf(char_particles,"%s",particle.c_str());
    char char_doping[2];//=new char[2];
//    strcpy(char_doping, doping.c_str());
    sprintf(char_doping,"%s",doping.c_str());

    float onTrackCluster_resolution_array[15], onTrackCluster_snr_array[15], onTrackCluster_snr_error_array[15], onTrackCluster_signal_array[15], onTrackCluster_signal_chisq_array[15], onTrackCluster_signal_ndf_array[15], onTrackCluster_signal_error_array[15], onTrackCluster_width_array[15];
    
    float Trackhit_resolution_array[15], Trackhit_snr_array[15], Trackhit_snr_error_array[15], Trackhit_signal_array[15], Trackhit_signal_chisq_array[15], Trackhit_signal_ndf_array[15], Trackhit_signal_error_array[15], Trackhit_width_array[15];
    
    float PeakDistance_snr_array[15], PeakDistance_snr_error_array[15], PeakDistance_signal_array[15], PeakDistance_signal_chisq_array[15], PeakDistance_signal_ndf_array[15], PeakDistance_signal_error_array[15];
    
    float noise_array[15], efficiency_array[15];
	float occupancy_array[15];
	
    for (int i=0; i<15; i++) {
        onTrackCluster_resolution_array[i]=0, onTrackCluster_snr_array[i]=0, onTrackCluster_snr_error_array[i]=0, onTrackCluster_signal_array[i]=0, onTrackCluster_signal_chisq_array[i]=0, onTrackCluster_signal_ndf_array[i]=0, onTrackCluster_signal_error_array[i]=0, onTrackCluster_width_array[i]=0;
        
        Trackhit_resolution_array[i]=0, Trackhit_snr_array[i]=0, Trackhit_snr_error_array[i]=0, Trackhit_signal_array[i]=0, Trackhit_signal_chisq_array[i]=0, Trackhit_signal_ndf_array[i]=0, Trackhit_signal_error_array[i]=0, Trackhit_width_array[i]=0;
        
         PeakDistance_snr_array[i]=0, PeakDistance_snr_error_array[i]=0, PeakDistance_signal_array[i]=0, PeakDistance_signal_ndf_array[i]=0, PeakDistance_signal_chisq_array[i]=0, PeakDistance_signal_error_array[i]=0;
        
        noise_array[i]=0,  efficiency_array[i]=0;
		occupancy_array[i]=0;
    }
	delete detectorfile;
    detectorfile = TFile::Open(filename.c_str(),"UPDATE");
    if (!detectorfile) std::cerr << "[ERROR] Cannot create Detector File!" << std::endl;
    else if (detectorfile->IsZombie()) std::cerr << "[ERROR] Error opening Detector File!" << std::endl;
    else {
        detectortree = new TTree("DetectorData", "Analysis Results for Detector");
        detectortree->Branch("Name", char_name, "Name[30]/C");
        detectortree->Branch("Material", char_material, "Material[5]/C");
        detectortree->Branch("Doping", char_doping,"Doping[2]/C");
        detectortree->Branch("Thickness", &thickness, "Thickness/I");
        detectortree->Branch("Fluence", &fluence, "Fluence/F");
        detectortree->Branch("Particles",char_particles,"Particles[15]/C");
        detectortree->Branch("Annealing",&annealing,"Annealing/I");
        detectortree->Branch("Voltage", &voltage,"Voltage/I");
        
        detectortree->Branch("Trackhit_Resolution",Trackhit_resolution_array,"Trackhit_Resolution[15]/F");
        detectortree->Branch("Trackhit_SNR",Trackhit_snr_array,"Trackhit_SNR[15]/F");
        detectortree->Branch("Trackhit_SNR_error",Trackhit_snr_error_array,"Trackhit_SNR_error[15]/F");
        detectortree->Branch("Trackhit_Signal",Trackhit_signal_array,"Trackhit_Signal[15]/F");
        detectortree->Branch("Trackhit_Signal_error",Trackhit_signal_error_array,"Trackhit_Signal_error[15]/F");
        detectortree->Branch("Trackhit_Signal_chisq",Trackhit_signal_chisq_array,"Trackhit_Signal_chisq[15]/F");
        detectortree->Branch("Trackhit_Signal_ndf",Trackhit_signal_ndf_array,"Trackhit_Signal_ndf[15]/F");
        detectortree->Branch("Trackhit_Width", Trackhit_width_array, "Trackhit_Width[15]/F");
        
        detectortree->Branch("onTrackCluster_Resolution",onTrackCluster_resolution_array,"onTrackCluster_Resolution[15]/F");
        detectortree->Branch("onTrackCluster_SNR",onTrackCluster_snr_array,"onTrackCluster_SNR[15]/F");
        detectortree->Branch("onTrackCluster_SNR_error",onTrackCluster_snr_error_array,"onTrackCluster_SNR_error[15]/F");
        detectortree->Branch("onTrackCluster_Signal",onTrackCluster_signal_array,"onTrackCluster_Signal[15]/F");
        detectortree->Branch("onTrackCluster_Signal_error",onTrackCluster_signal_error_array,"onTrackCluster_Signal_error[15]/F");
        detectortree->Branch("onTrackCluster_Signal_chisq",onTrackCluster_signal_chisq_array,"onTrackCluster_Signal_chisq[15]/F");
        detectortree->Branch("onTrackCluster_Signal_ndf",onTrackCluster_signal_ndf_array,"onTrackCluster_Signal_ndf[15]/F");
        detectortree->Branch("onTrackCluster_Width", onTrackCluster_width_array, "onTrackCluster_Width[15]/F");
        
        detectortree->Branch("PeakDistance_SNR",PeakDistance_snr_array,"PeakDistance_SNR[15]/F");
        detectortree->Branch("PeakDistance_SNR_error",PeakDistance_snr_error_array,"PeakDistance_SNR_error[15]/F");
        detectortree->Branch("PeakDistance_Signal",PeakDistance_signal_array,"PeakDistance_Signal[15]/F");
        detectortree->Branch("PeakDistance_Signal_error",PeakDistance_signal_error_array,"PeakDistance_Signal_error[15]/F");
        detectortree->Branch("PeakDistance_Signal_chisq",PeakDistance_signal_chisq_array,"PeakDistance_Signal_chisq[15]/F");
        detectortree->Branch("PeakDistance_Signal_ndf",PeakDistance_signal_ndf_array,"PeakDistance_Signal_ndf[15]/F");
		
        detectortree->Branch("Noise", noise_array, "Noise[15]/F");
        detectortree->Branch("Efficiency", efficiency_array, "Efficiency[15]/F");
        detectortree->Branch("Occupancy", occupancy_array, "Occupancy[15]/F");
        
        //Vectors to arrays
        
        for (int i=0; i<12; i++) {
            onTrackCluster_resolution_array[i] = this->onTrackCluster_resolution.at(i);
            onTrackCluster_snr_array[i] = this->onTrackCluster_snr.at(i);
            onTrackCluster_snr_error_array[i] = this->onTrackCluster_snr_error.at(i);
            onTrackCluster_signal_array[i] = this->onTrackCluster_signal.at(i);
            onTrackCluster_signal_error_array[i] = this->onTrackCluster_signal_error.at(i);
            onTrackCluster_signal_chisq_array[i] = this->onTrackCluster_signal_chisq.at(i);
            onTrackCluster_signal_ndf_array[i] = this->onTrackCluster_signal_ndf.at(i);
            onTrackCluster_width_array[i] = this->onTrackCluster_width.at(i);
            
            Trackhit_resolution_array[i] = this->Trackhit_resolution.at(i);
            Trackhit_snr_array[i] = this->Trackhit_snr.at(i);
            Trackhit_snr_error_array[i] = this->Trackhit_snr_error.at(i);
            Trackhit_signal_array[i] = this->Trackhit_signal.at(i);
            Trackhit_signal_error_array[i] = this->Trackhit_signal_error.at(i);
            Trackhit_signal_chisq_array[i] = this->Trackhit_signal_chisq.at(i);
            Trackhit_signal_ndf_array[i] = this->Trackhit_signal_ndf.at(i);
            Trackhit_width_array[i] = this->Trackhit_width.at(i);

            PeakDistance_snr_array[i] = this->PeakDistance_snr.at(i);
            PeakDistance_snr_error_array[i] = this->PeakDistance_snr_error.at(i);
            PeakDistance_signal_array[i] = this->PeakDistance_signal.at(i);
            PeakDistance_signal_error_array[i] = this->PeakDistance_signal_error.at(i);
            PeakDistance_signal_chisq_array[i] = this->PeakDistance_signal_chisq.at(i);
            PeakDistance_signal_ndf_array[i] = this->PeakDistance_signal_ndf.at(i);

            noise_array[i] = this->noise.at(i);
            efficiency_array[i] = this->efficiency.at(i);
			occupancy_array[i] = this->occupancy.at(i);
        }
        detectortree->Fill();
        std::cout << "Tree Filled" << std::endl;
    }
    detectorfile->Write("",TObject::kOverwrite);
    // detectorfile->Write();
    detectorfile->Close();
}

void Detector::read_file(std::string filename) {
    // if (detectorfile != NULL) {
//         detectorfile->Close();
//         delete detectorfile;
//     }
    
    char char_name[30];//=new char;
    char char_material[5];//=new char[5];
    char char_particles[15];//=new char[15];
    char char_doping[2];//=new char[2];
//    strcpy(char_doping, doping.c_str());
//    sprintf(char_doping,"%s", doping.c_str());

    float onTrackCluster_resolution_array[15], onTrackCluster_snr_array[15], onTrackCluster_snr_error_array[15], onTrackCluster_signal_array[15], onTrackCluster_signal_chisq_array[15], onTrackCluster_signal_ndf_array[15], onTrackCluster_signal_error_array[15], onTrackCluster_width_array[15];
    
    float Trackhit_resolution_array[15], Trackhit_snr_array[15], Trackhit_snr_error_array[15], Trackhit_signal_array[15], Trackhit_signal_chisq_array[15], Trackhit_signal_ndf_array[15], Trackhit_signal_error_array[15], Trackhit_width_array[15];
    
    float PeakDistance_snr_array[15], PeakDistance_snr_error_array[15], PeakDistance_signal_array[15], PeakDistance_signal_chisq_array[15], PeakDistance_signal_ndf_array[15], PeakDistance_signal_error_array[15];
    
    float noise_array[15], efficiency_array[15], occupancy_array[15];
    
    for (int i=0; i<15; i++) {
        onTrackCluster_resolution_array[i]=0, onTrackCluster_snr_array[i]=0, onTrackCluster_snr_error_array[i]=0, onTrackCluster_signal_array[i]=0, onTrackCluster_signal_chisq_array[i]=0, onTrackCluster_signal_ndf_array[i]=0, onTrackCluster_signal_error_array[i]=0, onTrackCluster_width_array[i]=0;
        
        Trackhit_resolution_array[i]=0, Trackhit_snr_array[i]=0, Trackhit_snr_error_array[i]=0, Trackhit_signal_array[i]=0, Trackhit_signal_chisq_array[i]=0, Trackhit_signal_ndf_array[i]=0, Trackhit_signal_error_array[i]=0, Trackhit_width_array[i]=0;
        
         PeakDistance_snr_array[i]=0, PeakDistance_snr_error_array[i]=0, PeakDistance_signal_array[i]=0, PeakDistance_signal_ndf_array[i]=0, PeakDistance_signal_chisq_array[i]=0, PeakDistance_signal_error_array[i]=0;
        
        noise_array[i]=0,  efficiency_array[i]=0,  occupancy_array[i]=0;
    }
    
    
    detectorfile = TFile::Open(filename.c_str());
	TTree* detectortree = (TTree*) detectorfile->Get("DetectorData");
    // detectorfile->GetObject("DetectorData", detectortree);
    
    TBranch* Name_br = detectortree->GetBranch("Name");
    TBranch* Material_br = detectortree->GetBranch("Material");
    TBranch* Doping_br = detectortree->GetBranch("Doping");
    TBranch* Thickness_br = detectortree->GetBranch("Thickness");
    TBranch* Fluence_br = detectortree->GetBranch("Fluence");
    TBranch* Particles_br = detectortree->GetBranch("Particles");
    TBranch* Annealing_br = detectortree->GetBranch("Annealing");
    TBranch* Voltage_br = detectortree->GetBranch("Voltage");
    
    TBranch* onTrackCluster_Resolution_br = detectortree->GetBranch("onTrackCluster_Resolution");
    TBranch* onTrackCluster_SNR_br = detectortree->GetBranch("onTrackCluster_SNR");
    TBranch* onTrackCluster_SNR_error_br = detectortree->GetBranch("onTrackCluster_SNR_error");
    TBranch* onTrackCluster_Signal_br = detectortree->GetBranch("onTrackCluster_Signal");
    TBranch* onTrackCluster_Signal_error_br = detectortree->GetBranch("onTrackCluster_Signal_error");
    TBranch* onTrackCluster_Signal_chisq_br = detectortree->GetBranch("onTrackCluster_Signal_chisq");
    TBranch* onTrackCluster_Signal_ndf_br = detectortree->GetBranch("onTrackCluster_Signal_ndf");
    TBranch* onTrackCluster_Width_br = detectortree->GetBranch("onTrackCluster_Width");

    TBranch* Trackhit_Resolution_br = detectortree->GetBranch("Trackhit_Resolution");
    TBranch* Trackhit_SNR_br = detectortree->GetBranch("Trackhit_SNR");
    TBranch* Trackhit_SNR_error_br = detectortree->GetBranch("Trackhit_SNR_error");
    TBranch* Trackhit_Signal_br = detectortree->GetBranch("Trackhit_Signal");
    TBranch* Trackhit_Signal_error_br = detectortree->GetBranch("Trackhit_Signal_error");
    TBranch* Trackhit_Signal_chisq_br = detectortree->GetBranch("Trackhit_Signal_chisq");
    TBranch* Trackhit_Signal_ndf_br = detectortree->GetBranch("Trackhit_Signal_ndf");
    TBranch* Trackhit_Width_br = detectortree->GetBranch("Trackhit_Width");

    TBranch* PeakDistance_SNR_br = detectortree->GetBranch("PeakDistance_SNR");
    TBranch* PeakDistance_SNR_error_br = detectortree->GetBranch("PeakDistance_SNR_error");
    TBranch* PeakDistance_Signal_br = detectortree->GetBranch("PeakDistance_Signal");
    TBranch* PeakDistance_Signal_error_br = detectortree->GetBranch("PeakDistance_Signal_error");
    TBranch* PeakDistance_Signal_chisq_br = detectortree->GetBranch("PeakDistance_Signal_chisq");
    TBranch* PeakDistance_Signal_ndf_br = detectortree->GetBranch("PeakDistance_Signal_ndf");

    TBranch* Noise_br = detectortree->GetBranch("Noise");
    TBranch* Efficiency_br = detectortree->GetBranch("Efficiency");
    TBranch* Occupancy_br = detectortree->GetBranch("Occupancy");
    
    Name_br->SetAddress(char_name);
    Material_br->SetAddress(char_material);
    Doping_br->SetAddress(char_doping);
    Thickness_br->SetAddress(&thickness);
    Fluence_br->SetAddress(&fluence);
    Particles_br->SetAddress(char_particles);
    Annealing_br->SetAddress(&annealing);
    Voltage_br->SetAddress(&voltage);
    
    onTrackCluster_Resolution_br->SetAddress(onTrackCluster_resolution_array);
    onTrackCluster_Signal_br->SetAddress(onTrackCluster_signal_array);
    onTrackCluster_Signal_error_br->SetAddress(onTrackCluster_signal_error_array);
    onTrackCluster_Signal_chisq_br->SetAddress(onTrackCluster_signal_chisq_array);
    onTrackCluster_Signal_ndf_br->SetAddress(onTrackCluster_signal_ndf_array);
    onTrackCluster_SNR_br->SetAddress(onTrackCluster_snr_array);
    onTrackCluster_SNR_error_br->SetAddress(onTrackCluster_snr_error_array);
    onTrackCluster_Width_br->SetAddress(onTrackCluster_width_array);
    
    Trackhit_Resolution_br->SetAddress(Trackhit_resolution_array);
    Trackhit_Signal_br->SetAddress(Trackhit_signal_array);
    Trackhit_Signal_error_br->SetAddress(Trackhit_signal_error_array);
    Trackhit_Signal_chisq_br->SetAddress(Trackhit_signal_chisq_array);
    Trackhit_Signal_ndf_br->SetAddress(Trackhit_signal_ndf_array);
    Trackhit_SNR_br->SetAddress(Trackhit_snr_array);
    Trackhit_SNR_error_br->SetAddress(Trackhit_snr_error_array);
    Trackhit_Width_br->SetAddress(Trackhit_width_array);

    PeakDistance_Signal_br->SetAddress(PeakDistance_signal_array);
    PeakDistance_Signal_error_br->SetAddress(PeakDistance_signal_error_array);
    PeakDistance_Signal_chisq_br->SetAddress(PeakDistance_signal_chisq_array);
    PeakDistance_Signal_ndf_br->SetAddress(PeakDistance_signal_ndf_array);
    PeakDistance_SNR_br->SetAddress(PeakDistance_snr_array);
    PeakDistance_SNR_error_br->SetAddress(PeakDistance_snr_error_array);

    Noise_br->SetAddress(noise_array);
    Efficiency_br->SetAddress(efficiency_array);
    Occupancy_br->SetAddress(occupancy_array);
    
    detectortree->GetEntry(0);

    this->name = char_name;
    this->material = char_material;
    this->doping = char_doping;
    this->particle = char_particles;
    
    for (int i =0; i<12; i++) {
        this->onTrackCluster_resolution.at(i) = onTrackCluster_resolution_array[i];
        this->onTrackCluster_signal.at(i) = onTrackCluster_signal_array[i];
        this->onTrackCluster_signal_error.at(i) = onTrackCluster_signal_error_array[i];
        this->onTrackCluster_signal_chisq.at(i) = onTrackCluster_signal_chisq_array[i];
        this->onTrackCluster_signal_ndf.at(i) = onTrackCluster_signal_ndf_array[i];
        this->onTrackCluster_snr.at(i) = onTrackCluster_snr_array[i];
        this->onTrackCluster_snr_error.at(i) = onTrackCluster_snr_error_array[i];
        this->onTrackCluster_width.at(i) = onTrackCluster_width_array[i];

        this->Trackhit_resolution.at(i) = Trackhit_resolution_array[i];
        this->Trackhit_signal.at(i) = Trackhit_signal_array[i];
        this->Trackhit_signal_error.at(i) = Trackhit_signal_error_array[i];
        this->Trackhit_signal_chisq.at(i) = Trackhit_signal_chisq_array[i];
        this->Trackhit_signal_ndf.at(i) = Trackhit_signal_ndf_array[i];
        this->Trackhit_snr.at(i) = Trackhit_snr_array[i];
        this->Trackhit_snr_error.at(i) = Trackhit_snr_error_array[i];
        this->Trackhit_width.at(i) = Trackhit_width_array[i];

        this->PeakDistance_signal.at(i) = PeakDistance_signal_array[i];
        this->PeakDistance_signal_error.at(i) = PeakDistance_signal_error_array[i];
        this->PeakDistance_signal_chisq.at(i) = PeakDistance_signal_chisq_array[i];
        this->PeakDistance_signal_ndf.at(i) = PeakDistance_signal_ndf_array[i];
        this->PeakDistance_snr.at(i) = PeakDistance_snr_array[i];
        this->PeakDistance_snr_error.at(i) = PeakDistance_snr_error_array[i];

        this->noise.at(i) = noise_array[i];
        this->efficiency.at(i) = efficiency_array[i];
		this->occupancy.at(i) = occupancy_array[i];
    }
}
