#ifndef Data_h
#define Data_h
#include "trackfunctions.h"
#include "TFile.h"
#include "TTree.h"
#include <string.h>
#include <stdio.h>
#include <iostream>

class Data{
private:
    TFile* DUTFile;
	TFile* DUTPedeFile;
	TFile* DUTSignalFile;
	
    TTree* DUTData;
    TTree* DUTPedeNoise;
    TTree* DUTSignalTree;
    
public:   
	
    //All coordinates in DUT Frame of Reference x', y', z'=z
    int EventNumber;
    double zPosition;
    int adc[390];
    float pedestals[390];
    float noise[390];
    float signal[390];
    
    Data();
    
    ~Data();
    
    void readtree(std::string DUTFile_Name);
    
    void readtree_raw(std::string DUTFile_Name);
	
    unsigned int get_n_entries();
    
    void get_entry(int i_entry);
    
    void get_raw(int i_entry);
	
   };

#endif


