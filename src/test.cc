//
//  test.cc
//  
//
//  Created by Georg Auzinger on 13.06.13.
//
//

#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"

#include "ConfigReader.h"
#include "Settings.h"
#include "Data.h"
#include "Cluster.h"
#include "Functions.h"
#include "Track.h"
#include "Alignment.h"
#include "pedenoise.h"
#include "trackfunctions.h"
#include "hitMatch.h"
#include "hitCollection.h"
#include "Trackhit.h"
#include "onTrackCluster.h"
#include "efficiency.h"


void test(std::string filename) {
    std::vector<float> cuts;
  	    Config myconfig;
 	    myconfig.readfile(filename);
	    // myconfig.init_resultfile();
    // 	    myconfig.init_plotfile();
    // 		
    // 	    Settings settings;
    // 	    settings.read_settings_plot(myconfig.directory + "preferences/plot_settings.txt");
    // 	    get_y_cuts(myconfig);
	    myconfig.init_resultfile();
	    myconfig.init_plotfile();
 	    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
		{
			// get_pede_noise(filenames->datafilename, true, 3.5, "MEDIAN", 1, true);
// 			std::cout << filenames->datafilename << std::endl;
// 			calibrate_event(filenames->datafilename,true,3.5,"MEDIAN");
// 			
			Cluster mycluster(3,2,0,0);
			Alignment myalignment(filenames->datafilename, filenames->trackfilename);
			std::cout <<"[PROCESS] Processing Raw Data!" << std::endl;
			mycluster.make_tree(filenames->datafilename);
			myalignment.align_sensor(myconfig, -1, true);
			
            Trackhit mytrackhit;
            mytrackhit.make_tree(filenames->datafilename, filenames->trackfilename, cuts);
		}
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
	    // calculate_efficiency_alternative(myconfig);
		
}

void syntax(char* progname) {
    std::cout << "Syntax: " << progname << " rootfile!" << std::endl;
}

void StandaloneApplication(int argc, char** argv) {
    if (argc<2) {
        syntax(argv[0]);
    }
    char* filename = argv[1];
    
    test(filename);
}

int main(int argc, char** argv) {
    gROOT->Reset();
    TApplication app("Root Application", &argc, argv);
    StandaloneApplication(app.Argc(), app.Argv());
    app.Run();
    return 0;
}