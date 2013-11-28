#include "Data.h"
//#include "include/Data.h"



Data::Data()
{
    DUTFile = NULL;
    DUTPedeFile = NULL;
    DUTData = NULL;
    DUTPedeNoise = NULL;
}

Data::~Data()
{
    // if (DUTFile != NULL){
 //        DUTFile->Close();
 //        delete DUTFile;
 //    }
 //    if (DUTPedeFile != NULL){
 //        DUTPedeFile->Close();
 //        delete DUTPedeFile;
 //    }
 //    if (DUTSignalFile != NULL){
 //        DUTSignalFile->Close();
 //        delete DUTSignalFile;
 //    }
}

void Data::readtree(std::string DUTFile_Name)
{
	// if(DUTFile != NULL){
// 		DUTFile->Close();
// 		delete DUTFile;
// 	}
// 	if(DUTPedeFile != NULL){
// 		DUTPedeFile->Close();
// 		delete DUTPedeFile;
// 	}
// 	if(DUTSignalFile != NULL){
// 		DUTSignalFile->Close();
// 		delete DUTSignalFile;
// 	}
	DUTFile = TFile::Open(TFile::AsyncOpen(DUTFile_Name.c_str()));
	DUTPedeFile = TFile::Open(TFile::AsyncOpen(change_filetype(DUTFile_Name,"PedeNoise").c_str()));
	DUTSignalFile = TFile::Open(TFile::AsyncOpen(change_filetype(DUTFile_Name,"Signal").c_str()));
	
	if (!DUTPedeFile) std::cerr << "[ERROR] No pedestal and noise Data available. Run pedenoise first!" << std::endl;
	if (!DUTSignalFile) std::cerr << "[ERROR] No calibrated Event Data available. Run calibrate_event first!" << std::endl;
	
	if (DUTFile->IsOpen() && DUTPedeFile->IsOpen() && DUTSignalFile->IsOpen()) {
		
		std::cout << "Reading RAW-, Calibrated- and Pede Noise Data from Files!" << std::endl;
		
		DUTData = (TTree*) DUTFile->Get("DUTData");
		DUTPedeNoise = (TTree*) DUTPedeFile->Get("DUTPedeNoise");    
    	DUTSignalTree = (TTree*) DUTSignalFile->Get("CalibratedEventData");

		if (DUTData && DUTPedeNoise && DUTSignalTree)
		{
			// std::cout << "Reading RAW-, Calibrated- and Pede Noise Trees!" << std::endl;
			
			// DUT data tree
			TBranch* EventNo = DUTData->GetBranch("EventNo");
			TBranch* adc_values_br = DUTData->GetBranch("ADCValues");
			TBranch* DUT_zPosition_br = DUTData->GetBranch("DUTzPosition");				
			EventNo->SetAddress(&EventNumber);
			adc_values_br->SetAddress(adc);
			DUT_zPosition_br->SetAddress(&zPosition);
		
			// Signal Tree
			TBranch* signal_br = DUTSignalTree->GetBranch("Signal");
			signal_br->SetAddress(signal);
		
			//PedeNoise Tree
			TBranch* pedestals_br = DUTPedeNoise->GetBranch("PedestalValues");
			TBranch* noise_br = DUTPedeNoise->GetBranch("NoiseValues");
			pedestals_br->SetAddress(pedestals);
			noise_br->SetAddress(noise);
			
		} else std::cerr << "Could not open all necessary Trees!" << std::endl;
	} else std::cout << "[ERROR] DUTData File " << DUTFile_Name << " or DUTPedeNoise File  " <<  DUTPedeFile << " or DUTSignal File " << DUTSignalFile << " could not be opened!" << std::endl;
}

void Data::readtree_raw(std::string DUTFile_Name)
{
	if(DUTFile != NULL){
		DUTFile->Close();
		delete DUTFile;
	}
	//Open DUTHit File and itialize Tree
	DUTFile = TFile::Open(TFile::AsyncOpen(DUTFile_Name.c_str()));
	if (DUTFile->IsOpen()) {
		DUTData = (TTree*) DUTFile->Get("DUTData");
		
		//Define and Read DUT HIT Branches
		TBranch* EventNo = DUTData->GetBranch("EventNo");
		TBranch* adc_values_br = DUTData->GetBranch("ADCValues");
		
		//Assign Branches to variables DUT (Class Members)
		EventNo->SetAddress(&EventNumber);
		adc_values_br->SetAddress(adc);
	} else std::cout << "[ERROR] TFile " << DUTFile_Name << " could not be opened!" << std::endl;
}

unsigned int Data::get_n_entries(){
    if (DUTData == NULL){
        std::cerr << "[ERROR] No DUT Data available!" << std::endl;
        return 0;
    }
    return static_cast<unsigned int>(DUTData->GetEntries());
}

void Data::get_entry(int i_entry)
{
    DUTData->GetEntry(i_entry);
	DUTSignalTree->GetEntry(i_entry);
    DUTPedeNoise->GetEntry(0);
}

void Data::get_raw(int i_entry)
{
    DUTData->GetEntry(i_entry);
}
