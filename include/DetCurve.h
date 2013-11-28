//
//  DetCurve.h
//  
//
//  Created by Georg Auzinger on 14.11.12.
//
//

#ifndef ____DetCurve__
#define ____DetCurve__

//C++ includes
#include <string>
#include <vector>
#include <set>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

//ROOT
#include <TColor.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>

//custom


class DetCurve{
public:
    int testbeam;
    std::string name;
    std::string material;
    int thickness;
    std::string doping;
    int region;
    float fluence;
    std::string particle;
    int annealing;
    std::string measurement;
    
    DetCurve();
    DetCurve(int tb, std::string na, std::string mat, int thick, std::string dope, int reg, float flu, std::string part, int ann, std::string meas);
    DetCurve(const DetCurve&);
    ~DetCurve();
    bool operator<(const DetCurve& ) const;
    DetCurve& operator=(const DetCurve&);
    void set_properties(TGraphErrors*&);
	void set_properties_asymm(TGraphAsymmErrors*&);
	
private:
    Color_t LibreColor(const unsigned int&);
};

#endif /* defined(____utilities__) */
