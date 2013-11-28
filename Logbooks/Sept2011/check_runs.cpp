//
//  check_runs.cpp
//  
//
//  Created by Georg Auzinger on 26.10.12.
//
//

#include <stdio.h>
#include <set>
//C++ includes
#include <string>
#include <vector>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<std::string> filelist;

std::vector<std::string> badfilelist;
std::vector<int> badvoltagelist;
int delete_counter=0;

std::set<int> runset;
std::set<int>::iterator setit;

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

void read_directory(std::string folder) {
    std::string foldername = folder;
    DIR           *d;
    struct dirent *dir;
    int counter=0;
    d = opendir(foldername.c_str());
    if (d) {
        std::cout << "The Foldername is: " << foldername << "/" << std::endl;
        while ((dir = readdir(d)) != NULL)
        {
            if ( strlen(dir->d_name) > 20 ) {
                std::cout<< "Found file: " << dir->d_name << std::endl;
                std::string name = folder;  //used to be "Exported"
                name += "/";
                name += dir->d_name;
                filelist.push_back(name.c_str());
                counter++;
            }
        }
        closedir(d);
    } else std::cout << "[ERROR] The directory " << foldername << " does not exist!" << std::endl;
}

void read_runfile(std::string runfile) {
    std::ifstream runs(runfile.c_str());
    std::cout << "Opening Runlist " << runfile << std::endl << "The runs Present are ";
    while(!runs.eof()) {
        int runnumber;
        runs >> runnumber;
        std::cout << runnumber << " ";
        runset.insert(runnumber);
        if (runs.eof()) break;
    }
    std::cout << std::endl;
}

void check_configfiles(std::string configfilename) {
    int counter = 0;
    int counter_bad=0;
    int Voltage=0;
    float Fluence=0;
    std::string particle="";
    
    std::ostringstream result;
    std::ifstream read(configfilename.c_str());
    
    
    if (!read) std::cerr << "[ERROR] The Config File " << configfilename << " does not exist!" << std::endl;
    if (!read.eof()) read >> Voltage;
    if (!read.eof()) read >> std::scientific >> Fluence;
    if (!read.eof()) read >> particle;
    
    result << Voltage << std::endl << Fluence << std::endl << particle << std::endl;
    //    std::cout << Voltage << std::endl << Fluence << std::endl << particle << std::endl;
    
    while (!read.eof())
    {
        int dummynumber;
        read >> dummynumber;
        int digits = get_digits(dummynumber);
        if (digits == 1)
        {
            int DUTnumber=dummynumber;
            result << DUTnumber << std::endl;
            //            std::cout << DUTnumber << std::endl;
            
        }
        else if (( digits > 1) && (digits <4))
        {
            std::cerr << "[ERROR] Run Number must have 4 Digits!" << std::endl;
        }
        else if (digits == 4)
        {
            counter++;
            if (runset.find(dummynumber) != runset.end()) //found
            {
                //                std::cout << "Runnumber " << dummynumber << " present; keeping!" <<std::endl;
                result << dummynumber << std::endl;
            }
            else if (runset.find(dummynumber) == runset.end()) //not found
            {
                counter_bad++;
                std::cout << "[ALERT] Runnumber " << dummynumber << " not present; deleting!" << std::endl;
            }
        }
        if (read.eof()) break;
    }
    if (counter > counter_bad) {
        std::ofstream result_stream;
        std::string resultfile = configfilename;
        resultfile += ".cfg";
        if( remove( resultfile.c_str() ) != 0 )
            perror( "Error deleting file" );
        else
            puts( "File successfully deleted" );
        result_stream.open(resultfile.c_str(), std::ios::out | std::ios::app);
        result_stream << result.str();
        result_stream.close();
        std::cout << counter-counter_bad << " out of " << counter << " Runnumbers present; Creating File!" << std::endl;
        
    }
    else {std::cout << counter_bad << " out of " << counter << " Runnumbers not present; not creating file" << std::endl;
        delete_counter++;
        badfilelist.push_back(configfilename);
        badvoltagelist.push_back(Voltage);
    }
}

void check_runs(){
    read_runfile("runlist.txt");
    read_directory("raw");
    for (std::vector<std::string>::iterator filenames = filelist.begin(); filenames != filelist.end(); filenames++) {
        std::cout << "Checking Config File " << *filenames << std::endl;
        check_configfiles(*filenames);
        
    }
    std::cout << delete_counter << " Files not created! Details: " << std::endl;
    for (size_t i=0; i<badfilelist.size(); i++) {
        std::cout<< badfilelist.at(i) << "  " << badvoltagelist.at(i) << std::endl;
    }
}
