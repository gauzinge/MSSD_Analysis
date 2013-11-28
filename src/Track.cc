//
//  Track.cc
//
//
//  Created by Georg Auzinger on 19.06.12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
#include "Track.h"
//#include "include/Track.h"


Track::Track(){
    EventNumber = 0;
    TrackFile = NULL;
    Tracks = NULL;
}

Track::~Track(){
    if (TrackFile != NULL) {
        TrackFile->Close();
        delete TrackFile;
    }
}

void Track::readtree(std::string TrackFile_Name) {
	if (TrackFile!=NULL){
		TrackFile->Close();
		delete TrackFile;
	}
	TrackFile = TFile::Open(TFile::AsyncOpen(TrackFile_Name.c_str()));
	if (TrackFile) {
		TrackFile->GetObject("TelescopeTrack", Tracks);
    
		//Define and Read Telescope Track Branches
		TBranch* EvtNo = Tracks->GetBranch("EventNo");
		TBranch* xSlope = Tracks->GetBranch("xSlope");
		TBranch* ySlope = Tracks->GetBranch("ySlope");
		TBranch* xIntercept = Tracks->GetBranch("xIntercept");
		TBranch* yIntercept = Tracks->GetBranch("yIntercept");
    
    
		//Assign Branches to variables Track (Class Members)
		EvtNo->SetAddress(&EventNumber);
		xSlope->SetAddress(&xSlope_local);
		ySlope->SetAddress(&ySlope_local);
		xIntercept->SetAddress(&xIntercept_local);
		yIntercept->SetAddress(&yIntercept_local);
	}
	else std::cout << "[ERROR] Could not open " << TrackFile << " !" << std::endl;    
	//    std::cout <<"[TRACK] Track Tree with " << Tracks->GetEntries() << " entries read successfully!" << std::endl; //not entirely true!
}


unsigned int Track::get_n_entries(){
    if (Tracks == NULL){
        std::cerr << "[ERROR] No Track Data available!" << std::endl;
        return 0;
    }
    return static_cast<unsigned int>(Tracks->GetEntries());
}
void Track::get_entry(int i_entry){
    Tracks->GetEntry(i_entry);
}

void Track::rotate_hit_coordinates (double x_shift, double y_shift, double angle) {
    //angle -45 is a counter clockwise rotation for the weird ref. planes coo. system
    //sine and cosine in cpp take radiants as argument, therefore:
    angle=angle * PI / 180;
    xPrime = (x * cos(angle) + y * sin(angle)) + x_shift;
    // yPrime = ( - x * sin(angle) + y * cos(angle)) + 15.04;
    yPrime = ( - x * sin(angle) + y * cos(angle)) + y_shift;
}

void Track::get_track_DUTPos(double z){
    x=0;
    y=0;
    x = xSlope_local * z + xIntercept_local;
    y = ySlope_local * z + yIntercept_local;
    
}

void Track::apply_alignment(double angle, double xshift, double yshift, double zPos) {
    Track::get_track_DUTPos(zPos);
    Track::rotate_hit_coordinates(xshift, yshift, angle);
}


