//
//  create_list.c
//  
//
//  Created by Georg Auzinger on 05.11.12.
//
//

#include <stdio.h>
//C++ includes
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

std::map<int, std::vector<int> > connections;

void read_connectionfile(std::string runfile) {
    std::ifstream runs(runfile.c_str());
    std::cout << "Opening Caen Connection File " << runfile << std::endl;
    while(!runs.eof()) {
        std::vector<int> caen_ch;
        int runnumber;
        int ch_number;
        runs >> runnumber;
        std::cout << runnumber << " ";
        for (int i = 0; i<4; i++) {
            runs >> ch_number;
            caen_ch.push_back(ch_number);
            std::cout << ch_number << " ";
        }
        connections[runnumber] = caen_ch;
        std::cout << std::endl;
        if (runs.eof()) break;
    }
}

void read_bias(std::string biasfile) {
    std::ifstream bias(biasfile.c_str());
    std::ostringstream dut_1, dut_2, dut_3, dut_4;
    std::cout << "Opening Bias Voltage File " << biasfile << std::endl;
    while (!bias.eof()) {
        //read
        int runnumber;
        std::map <int, int> ch_vs_voltage;
        int voltage[8];
        bias >> runnumber;
        bias >> voltage[0];
        ch_vs_voltage[20]=voltage[0];
        bias >> voltage[1];
        ch_vs_voltage[21]=voltage[1];
        bias >> voltage[2];
        ch_vs_voltage[22]=voltage[2];
        bias >> voltage[3];
        ch_vs_voltage[23]=voltage[3];
        bias >> voltage[4];
        ch_vs_voltage[24]=voltage[4];
        bias >> voltage[5];
        ch_vs_voltage[25]=voltage[5];
        bias >> voltage[6];
        ch_vs_voltage[26]=voltage[6];
        bias >> voltage[7];
        ch_vs_voltage[27]=voltage[7];

        //get connection
        if (ch_vs_voltage[connections[runnumber].at(0)] != -1) dut_1 << runnumber << " " << ch_vs_voltage[connections[runnumber].at(0)] << std::endl;
        if (ch_vs_voltage[connections[runnumber].at(1)] != -1) dut_2 << runnumber << " " << ch_vs_voltage[connections[runnumber].at(1)] << std::endl;
        if (ch_vs_voltage[connections[runnumber].at(2)] != -1) dut_3 << runnumber << " " << ch_vs_voltage[connections[runnumber].at(2)] << std::endl;
        if (ch_vs_voltage[connections[runnumber].at(3)] != -1) dut_4 << runnumber << " " << ch_vs_voltage[connections[runnumber].at(3)] << std::endl;
        
        if (bias.eof()) break;
    }
    std::ofstream slot_1, slot_2, slot_3, slot_4;
    slot_1.open("slot1.txt", std::ios::out | std::ios::app);
    slot_2.open("slot2.txt", std::ios::out | std::ios::app);
    slot_3.open("slot3.txt", std::ios::out | std::ios::app);
    slot_4.open("slot4.txt", std::ios::out | std::ios::app);

    slot_1 << dut_1.str();
    slot_2 << dut_2.str();
    slot_3 << dut_3.str();
    slot_4 << dut_4.str();

    slot_1.close();
    slot_2.close();
    slot_3.close();
    slot_4.close();
}

void make_dut_vs_voltage() {
    for (int i=1; i<=4; i++) {
        char buffer[3];
        std::string name = "slot";
        sprintf(buffer,"%i", i);
        name+=buffer;
        name+=".txt";
        //        std::cout << name << std::endl;
        std::ifstream slot(name.c_str());
        std::ostringstream dut1, dut2, dut3, dut4, dut5, dut6;
        dut1 << i-1 << std::endl;
        dut2 << i-1 << std::endl;
        dut3 << i-1 << std::endl;
        dut4 << i-1 << std::endl;
        dut5 << i-1 << std::endl;
        dut6 << i-1 << std::endl;

        while (!slot.eof()) {
            int runnumber;
            int voltage;
            slot >> runnumber;
            slot >> voltage;
            //read slot by slot and divide into dut sets according to runnumber
            if((runnumber <5527)) {
                dut1 << runnumber << " " << voltage << std::endl;
            }else if((runnumber >5527) && (runnumber <5586)) {
                dut2 << runnumber << " " << voltage << std::endl;
            }else if((runnumber >5586) && (runnumber <5660)) {
                dut3 << runnumber << " " << voltage << std::endl;
            }else if((runnumber >5660) && (runnumber <5721)) {
                dut4 << runnumber << " " << voltage << std::endl;
            }else if((runnumber >5721) && (runnumber <5818)) {
                dut5 << runnumber << " " << voltage << std::endl;
            }else if(runnumber > 5818){
                dut6 << runnumber << " " << voltage << std::endl;
            }
        }
        //write streams to file, according to slot# choose filenames for runsets
        std::ofstream dut_1, dut_2, dut_3, dut_4, dut_5, dut_6;
        if (i == 1) {//Slot 1
        dut_1.open("FTH200P_08_MSSD_1.txt", std::ios::out | std::ios::app);
        dut_2.open("MCZ200N_02_MSSD_2.txt", std::ios::out | std::ios::app);
        dut_3.open("FZ200N_04_MSSD_2.txt", std::ios::out | std::ios::app);
        dut_4.open("FZ320N_04_MSSD_1.txt", std::ios::out | std::ios::app);
        dut_5.open("FTH200P_08_MSSD_1.txt", std::ios::out | std::ios::app);
        dut_6.open("MCZ200N_04_MSSD_1_tilted.txt", std::ios::out | std::ios::app);

        } else if (i == 2) {//Slot 2
            dut_2.open("FTH200Y_03_MSSD_1.txt", std::ios::out | std::ios::app);
            dut_3.open("MCZ200N_04_MSSD_1.txt", std::ios::out | std::ios::app);
            dut_4.open("FZ200Y_06_MSSD_1.txt", std::ios::out | std::ios::app);
            dut_5.open("MCZ200N_02_MSSD_2.txt", std::ios::out | std::ios::app);
            dut_6.open("MCZ200P_01_MSSD_2_annealed.txt", std::ios::out | std::ios::app);
        }else if (i == 3){//Slot 3
            dut_2.open("FTH200N_24_MSSD_2.txt", std::ios::out | std::ios::app);
            dut_3.open("FZ320Y_05_MSSD_1.txt", std::ios::out | std::ios::app);
            dut_4.open("MCZ200Y_05_MSSD_1.txt", std::ios::out | std::ios::app);
            
            dut_6.open("FZ200Y_06_MSSD_1_annealed.txt", std::ios::out | std::ios::app);
        }else if (i ==4){//Slot 4
            dut_2.open("FZ200P_04_MSSD_1.txt", std::ios::out | std::ios::app);
            dut_3.open("FZ320N_05_MSSD_1.txt", std::ios::out | std::ios::app);
            dut_4.open("MCZ200P_01_MSSD_2.txt", std::ios::out | std::ios::app);
            dut_6.open("MCZ200Y_05_MSSD_1_annealed.txt", std::ios::out | std::ios::app);
        }
        dut_1 << dut1.str();
        dut_2 << dut2.str();
        dut_3 << dut3.str();
        dut_4 << dut4.str();
        dut_5 << dut5.str();
        dut_6 << dut6.str();

        
        dut_1.close();
        dut_2.close();
        dut_3.close();
        dut_4.close();
        dut_5.close();
        dut_6.close();
    }
}



void make_list_per_slot() {
    read_connectionfile("CaenConnections102012.txt");
    read_bias("Bias_voltages102012.txt");
}

void test() {
    make_dut_vs_voltage();
}