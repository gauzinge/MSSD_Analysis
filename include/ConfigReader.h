//
//  ConfigReader.cc
//
//
//  Created by Georg Auzinger on 06.07.12.//

#ifndef ConfigReader_h
#define ConfigReader_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <sys/stat.h>
#include <TText.h>
#include <TLatex.h>
#include <TFile.h>
#include <TH1.h>
#include <math.h>
#include <sstream>
#include "Detector.h"

struct Noise_Scale {
    std::string detname;
    std::vector<double> A;
    std::vector<double> B;
    std::vector<double> C;
    std::vector<TH1F*> noise_cluster_signal;
    std::vector<TH1F*> noise_cluster_snr;

};

struct Telescope_Resolution {
    double sigma_dut[4];
};

struct Connection{
    int fed[2];
    int ch[2];
    int ID;
};

struct Filepair {
    std::string datafilename;
    std::string trackfilename;
    int run;
    int DUT;
    Connection myconnection;
    std::vector<int> tickmarks;
    float refplane_adc;
    bool goodticks;
};

class Config {
private:
    Filepair myfilepair;
    std::set<int> runs;
    std::set<int> slots;
    std::string textname;
    std::map<int, int> run_vs_id;
    std::map<std::string, std::string> config_vs_tb;
    bool noticks, norefadc;
    
public:
    int DUT;
    int Voltage;
    float Fluence;
    double annealing;
    std::string particle;
    std::string detname;
	std::string directory;
    std::string foldername;
    std::string resultfoldername;
    TLatex* name;
    int nfiles;
    std::vector<Filepair> file_list;
    Noise_Scale myscale;
    Telescope_Resolution myresolution;
    float calibration_factor;

    Detector mydetector;
    TFile* resultfile;
    TFile* plotfile;
    std::stringstream warningstream;
    double y_min;
	double y_max;
    Config();
    
    ~Config();
    
    void readfile(std::string configfile);
    
    void read_configfiles();
    
    void make_filenames(int run_number, int DUT_number);
    
    void read_runs (std::string configfile);
    
    void make_name();
    
    void read_connection(int, int);
    
    void read_calibration_factors();
    
    void read_calibration_refplanes(int, std::string);
    
    void check_calibration_refplanes();
    
    bool read_tickmarks(int);
    
    void check_ticks();
    
    int strip_to_chip (int strip_no);
    
    float adc_to_electrons(Filepair afilepair,float adc, int chip, int unit);
        
    void init_resultfile();
    
    void init_plotfile();
    
    void close_resultfile();
    
    void close_plotfile();
    
    void write_scales_raw();
//    void write_results(std::string resultstring);
};


#endif
