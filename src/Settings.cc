//
//  Settings.cc
//  
//
//  Created by Georg Auzinger on 16.11.12.
//
//

#include "Settings.h"

// enum unit {ELECTRONS_TICKS = 0, ELECTRONS_THICK, ELECTRONS_REF, ADC};
// enum collection {ONTRACKCLUSTERS = 0, TRACKHITS, PEAKDISTANCE};

void Settings::read_directory(std::string folder) {
    std::string foldername = folder;
    DIR           *d;
    struct dirent *dir;
    //    std::vector<std::string> filelist;
    int counter=0;
    d = opendir(foldername.c_str());
    if (d) {
//        std::cout << "The Foldername is: " << foldername << "/" << std::endl;
        while ((dir = readdir(d)) != NULL)
        {
            if ( strlen(dir->d_name) > 20 ) {
                std::string name = folder;  //used to be "Exported"
                name += "/";
                name += dir->d_name;
                std::string extension = ".root";
                if (name.find(extension) != std::string::npos) {
                    this->filelist.push_back(name);
                    counter++;
//                    std::cout<< "Found file: " << name << std::endl;
                    
                }
            }
        }
        closedir(d);
        this->nfiles=counter;
    } else std::cout << "[ERROR] The directory " << foldername << " does not exist!" << std::endl;
}

void Settings::read_settings_process(std::string filename) {
//    this->case_switch = 2;
    std::ifstream setting(filename.c_str());
    if (!setting) std::cerr << "[ERROR] The Settings File " << filename << " does not exist!" << std::endl;
    
    std::cout << "****************************************************************" << std::endl;
    std::cout << "[SETTINGS] The Settings from " << filename << " are: " << std::endl;
    std::cout << "****************************************************************" <<std::endl;
    
    while (!setting.eof())
        
    {   std::string type;
        std::string line;
        
        setting >> type;
        std::getline(setting, line);
        if (type.compare(0,1,"#") != 0) {
            
            std::istringstream ss(line);
            if (type == "PedeNoise" && !line.empty())
            {
                std::cout << "Pedestal and Noise Calculation: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->pedenoise = true;
                else this->pedenoise = false;
                std::cout << word << std::endl;
//                if (this->case_switch != 0) this->case_switch = 1;
            }
            else if (type == "ClusterAlign" && !line.empty())
            {
                std::cout << "Perform Clustering and Alignment: " ;
                std::string word;
				ss >> word;
                if (word == "true") this->clusteralign = true;
                else this->clusteralign = false;
                    std::cout << word << std::endl;
            }
            
            else if (type == "Trackhit" && !line.empty())
            {
                std::cout << "Build Trackhits: " ;
                std::string word;
				ss >> word;
                if (word == "true") this->trackhits = true;
                else this->trackhits = false;
                std::cout << word << std::endl;
            }
            else if (type == "onTrackCluster" && !line.empty())
            {
                std::cout << "Build onTrackClusters: " ;
                std::string word;
				ss >> word;
                if (word == "true") this->ontrackclusters = true;
                else this->ontrackclusters = false;
                std::cout << word << std::endl;
            }        }
        if (setting.eof()) break;
    }
        std::cout << "****************************************************************" << std::endl;
}

void Settings::read_settings_plot(std::string filename) {
    this->case_switch = 2;
    std::ifstream setting(filename.c_str());
    if (!setting) std::cerr << "[ERROR] The Settings File " << filename << " does not exist!" << std::endl;
    
    std::cout << "****************************************************************" << std::endl;
    std::cout << "[SETTINGS] The Settings from " << filename << " are: " << std::endl;
    std::cout << "****************************************************************" <<std::endl;
    
    while (!setting.eof())
        
    {   std::string type;
        std::string line;
        
        setting >> type;
        std::getline(setting, line);
        if (type.compare(0,1,"#") != 0) {
            
            std::istringstream ss(line);
            if (type == "ChargeUnit" && !line.empty())
            {
                std::cout << "Chargeunit: " ;
                std::string uni;
                ss >> uni;
                if (uni == "ELECTRONS_TICKS") this->unit = ELECTRONS_TICKS;
                else if (uni == "ELECTRONS_THICK") this->unit = ELECTRONS_THICK;
                else if (uni == "ELECTRONS_REF") this->unit = ELECTRONS_REF;
                else if (uni == "ADC") this->unit = ADC;
                else
				{
					std::cout << "Unknown charge unit, setting to ADC!" << std::endl; 
					this->unit = 3;
				}
                std::cout << uni << " ; The possible Values are: " << std::endl << "ELECTRONS_TICKS for electrons using tickmark calibration" << std::endl << "ELECTRONS_THICK for electrons using reference detector thickness" << std::endl << "ELECTRONS_REF for electrons using mean reference plane signal on a per-run basis" << std::endl << "ADC for ADC counts" << std::endl;
            }
            else if (type == "Collection" && !line.empty())
            {
                std::cout << "Collection: " ;
                std::string word;
                ss >> word;
                if (word == "ONTRACKCLUSTERS") this->collection = ONTRACKCLUSTERS;
                else if (word == "TRACKHITS") this->collection = TRACKHITS;
                else if (word == "PEAKDISTANCE") this->collection = PEAKDISTANCE;
                // else if std::cout << "Unknown Collection, setting to ONTRACKCLUSTERS!" << std::endl; this->collection = 0;
                std::cout << word << " ; The possible Values are: "<< std::endl << "ONTRACKCLUSTERS to use track induced clusters" << std::endl << "TRACKHITS to use tracks matched to clusters on the DUT" << std::endl << "PEAKDISTANCE to calculate the signal as the distance between maximum of Signal and Noise distribution" << std::endl;
            }
            
            else if (type == "InterstripPosition" && !line.empty())
            {
                std::cout << "Plot Interstrip position: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->interstrip = true;
                else this->interstrip = false;
                std::cout << word << std::endl;
            }
            else if (type == "Efficiencymap" && !line.empty())
            {
                std::cout << "Plot Efficiencymap: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->efficiencymap = true;
                else this->efficiencymap = false;
                std::cout << word << std::endl;
            }
            else if (type == "Chargemap" && !line.empty())
            {
                std::cout << "Plot Chargemap: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->chargemap = true;
                else this->chargemap = false;
                std::cout << word << std::endl;
            }
            else if (type == "Resolution" && !line.empty())
            {
                std::cout << "Plot Residuals / Resolution: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->resolution = true;
                else this->resolution = false;
                std::cout << word << std::endl;
            }
            else if (type == "Clusterwidth" && !line.empty())
            {
                std::cout << "Plot Clusterwidth: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->clusterwidth = true;
                else this->clusterwidth = false;
                std::cout << word << std::endl;
            }
            //Dataplots
            else if (type == "SignalProfile" && !line.empty())
            {
                std::cout << "Plot Profile of Signals: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->signalfluct = true;
                else this->signalfluct = false;
                std::cout << word << std::endl;
            }            
            else if (type == "Noise" && !line.empty())
            {
                std::cout << "Plot Noise of Regions: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->noisereg = true;
                else this->noisereg = false;
                std::cout << word << std::endl;
            }
            else if (type == "Noiseprofile" && !line.empty())
            {
                std::cout << "Plot Noiseprofile: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->noiseprofile = true;
                else this->noiseprofile = false;
                std::cout << word << std::endl;
            }
            else if (type == "Datafluctuations" && !line.empty())
            {
                std::cout << "Plot Fluctuations of Data: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->datafluct = true;
                else this->datafluct = false;
                std::cout << word << std::endl;
            }
            else if (type == "Efficiency" && !line.empty())
            {
                std::cout << "Plot Efficiency: " ;
                std::string word;
                ss >> word;
                if (word == "true") this->efficiency = true;
                else this->efficiency = false;
                std::cout << word << std::endl;
            }

        }
        if (setting.eof()) break;
    }
    
    std::cout << "****************************************************************" << std::endl;
}


void Settings::read_settings_compare(std::string filename) {
    this->case_switch = 2;
    std::ifstream setting(filename.c_str());
    if (!setting) std::cerr << "[ERROR] The Settings File " << filename << " does not exist!" << std::endl;
    
    std::cout << "****************************************************************" << std::endl;
    std::cout << "[SETTINGS] The Settings from " << filename << " are: " << std::endl;
    std::cout << "****************************************************************" <<std::endl;
    
    while (!setting.eof())
        
    {   std::string type;
        std::string line;
        
        setting >> type;
        std::getline(setting, line);
        if (type.compare(0,1,"#") != 0) {
            
            std::istringstream ss(line);
            if (type == "Name" && !line.empty())
            {
                std::cout << "Name: " ;
                std::string word;
                while (ss >> word) {
                    this->names.insert(word);
                    std::cout << word << " ";
                }
                std::cout << std::endl;
                if (this->case_switch != 0) this->case_switch = 1;
            }
            else if (type == "Measurement" && !line.empty())
            {
                std::cout << "Measurement: " ;
                std::string word;
                while (ss >> word) {
                    this->measurements.insert(word);
                    std::cout << word << " ";
                }
                std::cout << std::endl;
            }
            
            else if (type == "Material" && !line.empty())
            {
                std::cout << "Materials: " ;
                std::string word;
                while (ss >> word) {
                    this->materials.insert(word);
                    std::cout << word << " ";
                }
                std::cout << std::endl;
            }
            else if (type == "Doping" && !line.empty())
            {
                std::cout << "Doping: " ;
                std::string word;
                while (ss >> word) {
                    this->doping.insert(word);
                    std::cout << word << " ";
                }
                std::cout << std::endl;
            }
            else if (type == "Thickness" && !line.empty())
            {
                std::cout << "Thickness: " ;
                int thick;
                while (ss >> thick) {
                    this->thickness.insert(thick);
                    std::cout << thick << " ";
                }
                std::cout << std::endl;
            }
            else if (type == "Fluence" && !line.empty())
            {
                std::cout << "Fluence: " ;
                float flu;
                while (ss >> flu) {
                    this->fluence.insert(flu);
                    std::cout << flu << " ";
                }
                std::cout << std::endl;
            }
            else if (type == "Particles" && !line.empty())
            {
                std::cout << "Particles: " ;
                std::string word;
                while (ss >> word) {
                    this->particle.insert(word);
                    std::cout << word << " ";
                }
                std::cout << std::endl;
            }
            else if (type == "Region" && !line.empty())
            {
                std::cout << "Region: " ;
                int reg;
                while (ss >> reg) {
                    this->regions.insert(reg);
                    std::cout << reg << " ";
                }
                std::cout << std::endl;
            }
            else if (type == "XRange" && !line.empty())
            {
                std::cout << "X Range: ";
                int min, max;
                while (ss >> min >> max) this->xRange_low = min, this->xRange_high = max;
                std:: cout << xRange_low << "   " << xRange_high << std::endl;
            }
            else if (type == "Name/Region" && !line.empty())
            {
                std::cout << "Name: " ;
                std::string word;
                int regi;
                std::vector<int> regions_;
                ss >> word;
                this->names.insert(word);
                std::cout << word << " Regions ";
                while (ss >> regi) {
                    std::cout << regi << " ";
                    regions_.push_back(regi);
                }
                this->regions_for_names[word] = regions_;
                std::cout << std::endl;
                this->case_switch = 0;
            }
        }
        if (setting.eof()) break;
    }
    if (this->case_switch == 1 || this->case_switch == 2) {
        std::vector<int> regionvec;
        for (std::set<int>::iterator regit = this->regions.begin(); regit != this->regions.end(); regit++) {
            regionvec.push_back(*regit);
        }
        for (std::set<std::string>::iterator it = this->names.begin(); it != this->names.end(); it++) {
            regions_for_names[*it]=regionvec;
        }
    }
    std::cout << "****************************************************************" << std::endl;
    switch (this->case_switch) {
        case 0:
            std::cout << "Specific Detector Names / Regions have been provided, using these!" << std::endl;
            break;
        case 1:
            std::cout << "Specific Detector Names have been provide, using these with general Region Set!" << std::endl;
            break;
        case 2:
            std::cout << "No Names have been provided, using all Data for given Material" << std::endl << "/Thickness/Doping/Region/Fluence!" << std::endl;
            break;

    }
    std::cout << "****************************************************************" << std::endl << std::endl;
}

void Settings::read_settings_binary(std::string filename) {
//    this->case_switch = 2;
    std::ifstream setting(filename.c_str());
    if (!setting) std::cerr << "[ERROR] The Settings File " << filename << " does not exist!" << std::endl;
    
    std::cout << "****************************************************************" << std::endl;
    std::cout << "[SETTINGS] The Settings from " << filename << " are: " << std::endl;
    std::cout << "****************************************************************" <<std::endl;
    
    while (!setting.eof())
        
    {   std::string type;
        std::string line;
        
        setting >> type;
        std::getline(setting, line);
        if (type.compare(0,1,"#") != 0) {
            
            std::istringstream ss(line);
            if (type == "Algorithm" && !line.empty())
            {
                std::cout << "Algorithm: " ;
                std::string word;
                while (ss >> word) {
                    this->algorithm.insert(word);
                    std::cout << word << " ";
                }
                std::cout << std::endl << "The possible Algorithms are: " << std::endl << "barycentric" << std::endl << "binary12" << std::endl << "binary1" << std::endl << "leading" << std::endl;
            }
            
            else if (type == "Region" && !line.empty())
            {
                std::cout << "Region: " ;
                int reg;
                while (ss >> reg) {
                    this->regions.insert(reg);
                    std::cout << reg << " ";
                }
                std::cout << std::endl;
            }
		}
        if (setting.eof()) break;
    }
        std::cout << "****************************************************************" << std::endl;
}


void Settings::read_scales(std::string scalefilename) {
//    std::string detname;
//    float factor;
    std::ifstream scales(scalefilename.c_str());
    if (!scales) std::cerr << "[ERROR] The Settings File " << scalefilename << " does not exist!" << std::endl;
    
    std::cout << "****************************************************************" << std::endl;
    std::cout << "[SCALING] Scalefactors are being read from " << scalefilename <<  std::endl;
    std::cout << "****************************************************************" <<std::endl << std::endl;

    
//    std::cout << "****************************************************************" << std::endl;
//    std::cout << "[SCALING] The Scalefactors from " << scalefilename << " are: " << std::endl;
//    std::cout << "****************************************************************" <<std::endl;
    
    while (!scales.eof()) {
        std::string line;
        std::getline(scales, line);
        std::istringstream ss(line);
        
        if (line.compare(0,1,"#") != 0 ) {
            std::string name;
            float flue;
            int annea;
            float factor;
            std::string unique_identifier;
            ss >> name  >> flue >> annea >> factor;
            
            char fluence_buffer[8], annealing_buffer[8];
            sprintf(fluence_buffer,"%1.1e",flue);
            sprintf(annealing_buffer,"%06d",annea);
            unique_identifier = name + "_" + fluence_buffer + "_" + annealing_buffer;
            this->scaling[unique_identifier] = factor;
        }
        if (scales.eof()) break;
    }
//    std::cout << "****************************************************************" << std::endl << std::endl;
}
