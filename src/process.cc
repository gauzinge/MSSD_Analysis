//
//  analyze.cc
//  
//
//  Created by Georg Auzinger on 22.08.12.
//
//

#include <stdio.h>
#include <iostream>
#include "Alignment.h"
#include "Trackhit.h"
#include "Functions.h"
#include "Data.h"
#include "ConfigReader.h"
#include "onTrackCluster.h"
#include "pedenoise.h"
#include "Settings.h"
#include <TStyle.h>
#include <TROOT.h>

const float hitrejectcut = 3.5;
const std::string algorithm = "MEDIAN";

void syntax(char* progname) {
    std::cout << "Syntax: " << progname << " Configfile" << std::endl;
}

int main(int argc, char* argv[]){
    if (argc!=2) {
        syntax(argv[0]);
        return -1;
    }
    char* config=argv[1];
    
    Config myconfig;
    myconfig.readfile(config);
    myconfig.init_plotfile();
    std::vector<float> cuts;
    Settings settings;
    settings.read_settings_process(myconfig.directory + "preferences/process_settings.txt");
    
	
    // 1st loop for alignment, clustering, creation of trackhits
     for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
     {
		 bool cm = false;
         if (settings.pedenoise)
         {
			cm = true;
            get_pede_noise(filenames->datafilename, cm, hitrejectcut, algorithm, 2, true);
            calibrate_event(filenames->datafilename, cm, hitrejectcut, algorithm);
         }
         if (settings.clusteralign)
         {
             Cluster mycluster(3,2,0,0);//clusterwithcut disabled
             Alignment myalignment(filenames->datafilename, filenames->trackfilename);
             std::cout <<"[PROCESS] Processing Raw Data!" << std::endl;
             mycluster.make_tree(filenames->datafilename);
             myalignment.align_sensor(myconfig, -1, true);
         }
         
         if (settings.trackhits)
         {
             Trackhit mytrackhit;
             mytrackhit.make_tree(filenames->datafilename, filenames->trackfilename, cuts);
         }
     }
    
    if (settings.ontrackclusters)
    {
        // find threshold cut for track-associated clustering
        std::cout << "[PROCESS] Building on-track Clusters!" << std::endl;
        onTrackCluster myontrackcluster;
        cuts = get_cut(myconfig, 0.4,5);
        for (std::vector<float>::iterator it = cuts.begin(); it != cuts.end(); it++) {
            std::cout << *it << "   ";
        }
        std::cout << std::endl;
        // second loop for track-associated clustering
        for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
        {
            myontrackcluster.make_tree(filenames->datafilename, filenames->trackfilename, cuts);
        }
    }
    myconfig.close_plotfile();
}
