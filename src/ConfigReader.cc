//
//  ConfigReader.cc
//
//
//  Created by Georg Auzinger on 06.07.12.//
#include "ConfigReader.h"
#include "annealing.cc"

int get_digits(int number){
    int digits=0;
    if (number == 0) digits=1;
    else{
        while (number != 0)
        {
            number /= 10;
            digits++;
        }
    }
    return digits;
}


Config::Config()
{
	this->directory = "/afs/cern.ch/work/g/gauzinge/public/Analysis/";
    this->foldername = "/afs/cern.ch/work/g/gauzinge/public/Analysis/Results/";
    this->resultfoldername = "/afs/cern.ch/work/g/gauzinge/public/Analysis/Resultfiles/";
    file_list.clear();
    for (unsigned int i =0; i<12; i++) {
        this->myscale.noise_cluster_signal.push_back(NULL);
        this->myscale.noise_cluster_snr.push_back(NULL);

        this->myscale.A.push_back(0);
        this->myscale.B.push_back(0);
        this->myscale.C.push_back(0);
    }
}

Config::~Config() {}



void Config::readfile(std::string configfile) {
    std::cout << std::endl;
    Config::read_configfiles();
    Config::read_runs(configfile);
    Config::read_calibration_factors();
    Config::check_calibration_refplanes();
    Config::check_ticks();
    Config::make_name();
}

void Config::read_configfiles() {
	std::string streamname = this->directory + "preferences/configfiles.txt";
    std::ifstream configfilelist;
	configfilelist.open(streamname.c_str());
    if (!configfilelist) std::cerr << "[ERROR] The Config-File List " << "preferences/configfiles.txt does not exist!" << std::endl;
    std::cout << "[CONFIG] Reading list of Conifg files and associating them to Testbeams!" << std::endl;
    std::string tb_date;
    while (!configfilelist.eof()) {
       
        std::string line;
        std::getline(configfilelist, line);
        std::istringstream ss(line);

        if (line.compare(0,1,"#") != 0 ) {
             if (line.find("TB") != std::string::npos) {
                 std::string dummystring;
                 ss >> dummystring >> tb_date;
//                 std::cout << dummystring << " " << tb_date << std::endl;
             } else {
                 std::string configfile_name;
                 ss >> configfile_name;
                 configfile_name += ".cfg";
//                 std::cout << tb_date << " " << configfile_name << std::endl;
                 this->config_vs_tb[configfile_name] = tb_date;
             }
        }
        if (configfilelist.eof()) break;
    }
}

void Config::make_filenames(int run_number, int DUT_number){
    char buffer[5];
    sprintf(buffer,"%i", run_number);
    char buffer1[2];
    sprintf(buffer1,"%i", DUT_number);
    // myfilepair.trackfilename = "/afs/cern.ch/work/g/gauzinge/public/TrackData/TelescopeTracks_run000";
    myfilepair.trackfilename = "root://eoscms.cern.ch//eos/cms/store/user/gauzinge/TrackData/TelescopeTracks_run000";
    myfilepair.trackfilename += buffer;
    myfilepair.trackfilename += ".root";
    // myfilepair.datafilename = "/afs/cern.ch/work/v/vbox/public/MssdData/00";
    myfilepair.datafilename = "root://eoscms.cern.ch//eos/cms/store/user/gauzinge/RawMssdData/00";
    myfilepair.datafilename += buffer;
    myfilepair.datafilename += "_MSSD_";
    myfilepair.datafilename += buffer1;
    myfilepair.datafilename += ".root";
    myfilepair.run = run_number;
    myfilepair.DUT = DUT_number;
    read_connection(DUT_number, run_number);
//    std::cout << myfilepair.trackfilename << "  " << myfilepair.datafilename << std::endl;
    
    myfilepair.goodticks = false;
    myfilepair.goodticks = read_tickmarks(run_number);
    read_calibration_refplanes(run_number, this->directory + "preferences/reference_detectors.txt");
    file_list.push_back(myfilepair);
}



void Config::read_runs (std::string configfile) {
    
    int text_length=0;
    size_t first_char=0;
    if (configfile.find("FTH") != std::string::npos) { 
        text_length = 17;
        first_char = configfile.find("FTH");
    }
    else if (configfile.find("MCZ", first_char) != std::string::npos) {
        text_length = 17;
        first_char = configfile.find("MCZ");
    }
    else if (configfile.find("FZ", first_char) != std::string::npos) {
        text_length = 16;
        first_char = configfile.find("FZ");
    }
    size_t last_char = configfile.find(".cfg");
    this->textname =  configfile.substr(first_char,text_length);
    this->detname = configfile.substr(first_char,last_char-first_char);
    this->myscale.detname = textname;
    
    std::string subfolder = this->config_vs_tb[detname+".cfg"];
    subfolder += "/";
    this->foldername += subfolder;
    this->resultfoldername += subfolder;
    mkdir(this->foldername.c_str(),S_IRWXU | S_IRWXG | S_IRWXO );
    mkdir(this->resultfoldername.c_str(),S_IRWXU | S_IRWXG | S_IRWXO );
    this->foldername += detname;
    mkdir(this->foldername.c_str(),S_IRWXU | S_IRWXG | S_IRWXO );

    std::cout << "[CONFIG] Writing Results to " << this->foldername << "/" << std::endl << std::endl;
    std::ifstream read(configfile.c_str());
    if (!read) std::cerr << "[ERROR] The Config File " << configfile << " does not exist!" << std::endl;
    
    std::cout << "[SETTINGS] The Run - Settings are:" <<std::endl;
    runs.clear();
    
    int filecounter = 0;
    int counter = 0;
    int time=0, temperature=0;

    while (!read.eof()) {
        std::string line;
        std::getline(read, line);
        counter++;
        std::istringstream ss(line);
        
        if (line.compare(0,1,"#") != 0) {
            
            if (counter == 1) ss >> Voltage;
            if (counter == 2) ss >> std::scientific >> Fluence;
            if (counter == 3) ss >> particle;
            else {
                int dummynumber;
                int DUTnumber;
                ss >> dummynumber;
                int digits = get_digits(dummynumber);
                if (digits == 1)
                {
                    DUTnumber=dummynumber;
                    slots.insert(DUTnumber);
                }
                if (( digits > 1) && (digits <4) && counter > 3)
                {
                    std::cerr << "[ERROR] Run Number must have 4 Digits!" << std::endl;
                }
                if (digits == 4)
                {
                    if(runs.find(dummynumber) == runs.end())
                    {
                        runs.insert(dummynumber);
                        make_filenames(dummynumber, DUTnumber);
                        run_vs_id[dummynumber]=DUTnumber;
                        this->DUT = DUTnumber;
                        filecounter++;
                    }
                    
                }
            }
            if (line.find("A") != std::string::npos) {
                std::istringstream ss(line);
                std::string dummystring;
                ss >> dummystring >> time >> temperature;
                std::cout << "[ANNEALING] " << time << " min @ " << temperature << " deg C " << std::endl;
                annealing = eq_at_RT(time, temperature);
                std::cout << "[ANNEALING] " << annealing << " min @ 21 deg C" << std::endl;
            }

        }

        if (read.eof()) break;
    }
    this->noticks = false;
    this->nfiles = filecounter;
    std::cout.precision(2);
    std::cout << std::endl << "[CONFIG] The Detector Name is " << this->textname << std::endl << "[CONFIG] The Voltage is " << Voltage << "V and the Fluence is " << std::scientific << Fluence << "/cm^2 1Mev Neq " << particle << "!" << std::endl << "[CONFIG] Annelaing " << time << " min @ " << temperature << " deg C equivalent to " << std::fixed << annealing << " min @ 21 deg C."<< std::endl << std::endl;
    
}

void Config::read_connection(int DUTID, int run_number){
	//also set the Telescope resolution for the specific testbeam here!
    std::string connectionfilename;
    if (run_number < 3955) {
        connectionfilename = this->directory + "preferences/sibt032011.txt";
        for (int i=0; i<4; i++) {
            this->myresolution.sigma_dut[0]=6.12;
        }
    }
    else if ((run_number > 3955) && (run_number <= 4890)) {
        connectionfilename = this->directory + "preferences/sibt092011.txt";
        this->myresolution.sigma_dut[0]=7.33;
        this->myresolution.sigma_dut[1]=6.47;
        this->myresolution.sigma_dut[2]=6.24;
        this->myresolution.sigma_dut[3]=6.71;
        
    }
    else if ((run_number > 4890) && (run_number <= 5393)) {
        connectionfilename = this->directory + "preferences/sibt052012.txt";
        this->myresolution.sigma_dut[0]=7.05;
        this->myresolution.sigma_dut[1]=6.24;
        this->myresolution.sigma_dut[2]=6.06;
        this->myresolution.sigma_dut[3]=6.56;
    }
    else if (run_number > 5393) {
        connectionfilename = this->directory + "preferences/sibt102012.txt";
        this->myresolution.sigma_dut[0]=6.78;
        this->myresolution.sigma_dut[1]=6.04;
        this->myresolution.sigma_dut[2]=5.92;
        this->myresolution.sigma_dut[3]=6.45;
    }
    
    std::ifstream geometry(connectionfilename.c_str());
    while(!geometry.eof()) {
        char dummychar;
        int dummyID;
        int dummyfed[2];
        int dummych[2];
        geometry >> dummyID;
        geometry >> dummyfed[0];
        geometry >> dummychar;
        geometry >> dummych[0];
        geometry >> dummyfed[1];
        geometry >> dummychar;
        geometry >> dummych[1];
        if (dummyID == DUTID){
            myfilepair.myconnection.ID = dummyID;
            for (int j=0; j<2; j++) {
                myfilepair.myconnection.fed[j]=dummyfed[j];
                myfilepair.myconnection.ch[j]=dummych[j];
            }
        }
        if (geometry.eof()) break;
    }
    std::cout << "[CONNECTION] Run# " << run_number << " DUT# " << myfilepair.myconnection.ID <<  " FED1 " << myfilepair.myconnection.fed[0] << "/" << myfilepair.myconnection.ch[0] << " FED2 " << myfilepair.myconnection.fed[1] << "/" << myfilepair.myconnection.ch[1] << " Connection: "<< connectionfilename << std::endl;
}


void Config::read_calibration_factors() {
	std::string streamname = this->directory + "preferences/scalefile.txt";
    std::ifstream calibrationstream;
	calibrationstream.open(streamname.c_str());
    if (!calibrationstream) std::cerr << "[ERROR] The Calibration Factor List " << "preferences/scalefile.txt does not exist!" << std::endl;
    std::cout << "[CALIBRATION THICKNESS] Reading list of Calibration Factors from preferences/scalefile.txt" << std::endl;
    std::stringstream tmpstream;
    tmpstream << this->textname << "_" << this->Voltage <<  "V";
    std::string tmp_name = tmpstream.str();

    while (!calibrationstream.eof()) {
        
        std::string line;
        std::getline(calibrationstream, line);
        std::istringstream ss(line);
        
        if (line.compare(0,1,"#") != 0 ) {
            std::string name;
            float flue;
            double annea;
            float factor;
            ss >> name  >> flue >> annea >> factor;
//            std::cout << tmp_name << " " << name << " " << this->Fluence << " " <<flue << " " << this->annealing << " " << annea << " " << factor << std::endl;
            if (tmp_name == name && this->Fluence == flue && this->annealing == annea) {
                this->calibration_factor = factor;
                std::cout << "*********************************************************************" << std::endl;
                std::cout << "[CALIBRATION THICKNESS] The calibration Factor for this Detector is: " << factor << std::endl;
                std::cout << "*********************************************************************" << std::endl << std::endl;
            }
//            std::string unique_identifier = name + "_" + fluence + "_" + annealing;
        }
        if (calibrationstream.eof()) break;
    }
}

void Config::read_calibration_refplanes(int run_number, std::string filename) {
    this->norefadc = false;
    std::ifstream calibrationstream(filename.c_str());
    if (!calibrationstream) std::cerr << "[ERROR] The Reference Plane ADC List " << filename << " does not exist!" << std::endl;
//    std::cout << "[REFPLANE CALIBRATION] Reading list of Reference Plane ADC's from " << filename << std::endl;
    while (!calibrationstream.eof()) {
        int run;
        float adc=-999;
        calibrationstream >> run >> adc;
        if (run == run_number) {
            myfilepair.refplane_adc = adc;
        }
        if(calibrationstream.eof()) break;
    }
//    if (myfilepair.refplane_adc != -999) std::cout << "[REFPLANE CALIBRATION] The Ref. Plane ADC Mean for Run " << run_number << " is: " << myfilepair.refplane_adc << std::endl;
//    else {
//        std::cout << "[WARNING] No reasonable Ref. Plane ADC found for Run " << run_number << std::endl;
//        this->norefadc = true;
//    }
}

void Config::check_calibration_refplanes() {
    std::cout << "[CALIBRATION REFPLANES]Checking Reference Plane MPVs for Availability and Consistency!" << std::endl;
    for (std::vector<Filepair>::iterator filenames = this->file_list.begin(); filenames != this->file_list.end(); filenames++)
    {
        if (filenames->refplane_adc == -999) {
            std::cout << "[WARNING] No reasonable Ref. Plane ADC found for Run " << filenames->run << std::endl;
            this->norefadc = true;
        }
        else std::cout << "[CALIBRATION REFPLANES] The Ref. Plane ADC Mean for Run " << filenames->run << " is: " << filenames->refplane_adc << std::endl;
    }
}

bool Config::read_tickmarks(int run_number) {
    std::vector<int> dummyvec;
    std::vector<int> zerovec;
    
    std::string tickfile = this->directory + "preferences/tickmarks.txt";
    std::ifstream read_marks(tickfile.c_str());
    if (!read_marks) std::cerr << "[ERROR] The Tickmark File " << tickfile << " does not exist!" << std::endl;
    
//    std::cout << "[TICKMARKS] Reading Tickmarks from file " << tickfile << std::endl;

    std::string dummy;
    int fed;
    int ch;
    int apv;
    int tick;
    int run;
    for (int i=0; i<4;i++) dummyvec.push_back(0);
    zerovec = dummyvec;
    
    while (!read_marks.eof()) {
        read_marks >> dummy;
        read_marks >> fed;
        read_marks >> dummy;
        read_marks >> ch;
        read_marks >> dummy;
        read_marks >> apv;
        read_marks >>dummy;
        read_marks >> tick;
        read_marks >> dummy;
        read_marks >> run;
        if (run == run_number) {
            if (fed == myfilepair.myconnection.fed[0] && ch == myfilepair.myconnection.ch[0] && apv == 0 && tick > 50) dummyvec.at(0)=tick;
            else if (fed == myfilepair.myconnection.fed[0] && ch == myfilepair.myconnection.ch[0] && apv == 1 && tick > 50) dummyvec.at(1)=tick;
            else if (fed == myfilepair.myconnection.fed[1] && ch == myfilepair.myconnection.ch[1] && apv == 0 && tick > 50) dummyvec.at(2)=tick;
            else if (fed == myfilepair.myconnection.fed[1] && ch == myfilepair.myconnection.ch[1] && apv == 1 && tick > 50) dummyvec.at(3)=tick;
        }
        if (read_marks.eof()) break;
    }
    int zerocounter=0;
    for (std::vector<int>::iterator it = dummyvec.begin(); it != dummyvec.end(); it++) {
        if(*it == 0) zerocounter++;
    }
    if (zerocounter == 0)//vector not empty, reasonable ticks found
    {
        myfilepair.tickmarks = dummyvec;
        return true;
    }
    else
    {
        myfilepair.tickmarks = zerovec;
        return false;
    }
}

void Config::check_ticks(){
    std::cout <<"[TICKMARKS] Checking Tickmarks for Availability and Consistency!" << std::endl;
    std::set<int> good_runs;
    std::set<int> bad_runs;
    std::map<int, int> runset;
    std::map<int, std::vector<int> > ticks;
    
    for (std::vector<Filepair>::iterator filenames = this->file_list.begin(); filenames != this->file_list.end(); filenames++)
    {
        if (filenames->tickmarks.at(0) == 0 && filenames->tickmarks.at(3) == 0)
        {
            bad_runs.insert(filenames->run);
            runset[filenames->run]=-1;
            
        }
        else {
            good_runs.insert(filenames->run);
            runset[filenames->run]=filenames->run;
        }
        ticks[filenames->run]=filenames->tickmarks;
    }
    if (good_runs.size() != 0) {
        for (std::set<int>::iterator badrun = bad_runs.begin(); badrun != bad_runs.end(); badrun++) {//etc.
            std::map<int, int>::iterator up, down, dummy;
            up = runset.find(*badrun);
            down = runset.find(*badrun);
            while (up->second == -1 && std::distance( up, runset.end() ) > 1 ) { //up points to bad run and is not the last element
                up++;//this happens at least once
                if (good_runs.find(up->first) != good_runs.end()) {//the next element in runset is a good run -> check config
                    if (run_vs_id[up->first] == run_vs_id[*badrun]) {//same config->victory
                        runset[*badrun] = up->first;
                        break;
                    }//else increment
                }//not found in good run -> increment
            }//if it is the last element ->pech, try your luck somewhere else
            while (down->second == -1 && down != runset.begin()) { // down points to bad run and is not the first element
                down--;
                if (good_runs.find(down->first) != good_runs.end()) {//the element before is a good one
                    if (run_vs_id[down->first] == run_vs_id[*badrun]) { //same config -> victory
                        runset[*badrun] = down->first;
                        break;
                    }//else decrement
                }//not in good run, decrement
            }//if it is the first element, hope that up was more successfull
            if (std::distance( up, runset.end() ) == 1 && down == runset.begin()) {//down is first and up is last
                std::cout << "[WARNING] No Tickmarks for Runset in Slot " << run_vs_id[*badrun] << " found, switching to ADC!" << std::endl;
                this->noticks = true;
            }
        }
        std::map<int, int>::iterator it=runset.begin();
        for (std::vector<Filepair>::iterator filenames = this->file_list.begin(); filenames != this->file_list.end(); filenames++)
        {
            filenames->tickmarks = ticks[it->second];
            if (good_runs.find(it->first) != good_runs.end()) {
                std::cout << "[TICKMARKS] Run# " << filenames->run << " : " << filenames->tickmarks.at(0) << "/" << filenames->tickmarks.at(1) << "/" << filenames->tickmarks.at(2) << "/" << filenames->tickmarks.at(3) << std::endl;
            }
            else std::cout << "[WARNING] Run# " << filenames->run << " : No reasonable Ticks found, using " << it->second << std::endl;
            it++;
            
        }
    } else {
        for (std::vector<Filepair>::iterator filenames = this->file_list.begin(); filenames != this->file_list.end(); filenames++)
        {
            filenames->goodticks = false;
        }
        std::cout << "[WARNING] No Tickmarks for any run found, switching to ADC!" << std::endl;
        this->noticks = true;
    }
    
}

void Config::make_name() {
    // DETNAME STRING FOR PLOTS
    std::string s;
    std::stringstream namestream;
    namestream.precision(1);
    if ((Fluence == 0) && (particle == "none")) namestream << textname << " @ " << Voltage << "V  / unirradiated"  << std::endl;
    else if (annealing == 0) namestream << textname << " @ " << Voltage << "V  / " << std::scientific << Fluence << " 1MeV N_{eq} " << std::fixed << particle << std::endl;
    else namestream << textname << " @ " << Voltage << "V  / " << std::scientific << Fluence << " 1MeV N_{eq} " << std::fixed << particle << " and " << annealing/60 << " h@RT" << std::endl;
    s=namestream.str();
    this->name=new TLatex(.1,.017,s.c_str());
    //        name->SetTextAlign(33);
    this->name->SetNDC(kTRUE); // <- use NDC coordinate
    this->name->SetTextFont(62);
    this->name->SetTextSize(0.035);
}




int Config::strip_to_chip (int strip_no) {
    if (strip_no < 128) return 0;
    else if ((strip_no>=128) && (strip_no < 256)) return 1;
    else if ((strip_no>=256) && (strip_no < 384)) return 2;
    else return 3;
}

float Config::adc_to_electrons(Filepair afilepair ,float adc, int chip, int unit) {
	int ref_thickness = 300;
    float nominal_e_signal = ref_thickness * 78;
    int chargeunit;
    if ((unit == 0 && this->noticks)||(unit == 2 && this->norefadc))
    {
        chargeunit = 3;
    }
    else chargeunit = unit;
    
    if (chargeunit == 0)
    {
        float electron_value=-99;
        electron_value = adc * static_cast<float>( 175000 / afilepair.tickmarks.at(chip) );
        return electron_value;
    }
    else if (chargeunit == 1)
    {
        float electron_value=-99;
        electron_value = adc * this->calibration_factor;
        return electron_value;
    }
    else if (chargeunit == 2)
    {
        float electron_value=-99;
        electron_value =  adc * nominal_e_signal/afilepair.refplane_adc;
        return electron_value;
    }
    else return adc;
}


void Config::init_plotfile(){
//Plotfile
std::string plotfilename = this->foldername + "/" + detname + ".root";
plotfile = TFile::Open(plotfilename.c_str());
//    if (plotfile != NULL) {
//        std::cout << std::endl << "[WARNING] The Plotfile " << plotfilename << " already exists! Plots will be updated!" << std::endl;
//    }
//    else {
plotfile = new TFile(plotfilename.c_str(),"UPDATE");
std::cout << std::endl << "[NEW FILE] Initializing new Plotfile!" << std::endl;
//    }
}

void Config::init_resultfile(){
    //Resultfile Set Properties
    std::string resultname = this->resultfoldername + detname + "_Results.root";
    resultfile = TFile::Open(resultname.c_str());
    if (resultfile != NULL) {
 		delete resultfile;
        this->mydetector.read_file(resultname.c_str());
        std::cout << std::endl << "[WARNING] The Resultfile " << resultname << " already exists! Results will be updated!" << std::endl;
    }
    else {
        std::cout << std::endl << "[NEW FILE] Initializing new Resultfile!" << std::endl;
    this->mydetector.name = this->textname;
    if (this->textname.find("FZ") != std::string::npos) this->mydetector.material = "FZ";
    else if (this->textname.find("FTH") != std::string::npos) this->mydetector.material = "FTH";
    else if (this->textname.find("MCZ") != std::string::npos) this->mydetector.material = "MCZ";
    else this->mydetector.material = "-999";
    
    if (this->textname.find("N") != std::string::npos) this->mydetector.doping = "N";
    else if (this->textname.find("P") != std::string::npos) this->mydetector.doping = "P";
    else if (this->textname.find("Y") != std::string::npos) this->mydetector.doping = "Y";
    else this->mydetector.doping = "-999";
    
    
    if (this->textname.find("120") != std::string::npos) this->mydetector.thickness = 120;
    else if (this->textname.find("200") != std::string::npos) this->mydetector.thickness = 200;
    else if (this->textname.find("320") != std::string::npos) this->mydetector.thickness = 320;
    else this->mydetector.thickness = -999;
    
    this->mydetector.fluence = this->Fluence;
    this->mydetector.particle = this->particle;
    this->mydetector.annealing = static_cast<int>(annealing);
    this->mydetector.voltage = this->Voltage;
    }
}

void Config::close_resultfile() {
    std::cout << "[CONFIG] Closing Resultfile ... " << std::endl;
    std::string resultname = this->resultfoldername  + detname + "_Results.root";
    std::cout<< "[RESULTFILE] The name of the Resultfile is " << resultname << std::endl;
    
    this->mydetector.write_file(resultname);
    //Write Warningstream to file
    std::string warningfilename = this->foldername + "/Warningfile.txt";
    std::ofstream warningfile(warningfilename.c_str());
    warningfile.write(this->warningstream.str().c_str(), this->warningstream.str().length());
    warningfile.close();
}

void Config::close_plotfile() {
    std::cout << "[CONFIG] Closing Plotfile ... " << std::endl;
    plotfile->Write();
    plotfile->Close();
}

void Config::write_scales_raw() {
    std::string filename = "Scalefile.txt";
    std::ofstream scalestream;
    scalestream.open(filename.c_str(), std::ios_base::app);
    scalestream << this->detname << "   " << this->Fluence << "   " << this->annealing << std::endl;
    scalestream.close();
}
