//
//  DetCurve.cc
//
//
//  Created by Georg Auzinger on 14.11.12.
//
//

#include "DetCurve.h"


DetCurve::DetCurve() : testbeam(0), name(""), material(""), thickness(0), doping(""), region(0), fluence(0), particle(""), annealing(0), measurement("") {
}

DetCurve::DetCurve(int tb, std::string na, std::string mat, int thick, std::string dope, int reg, float flu, std::string part, int ann, std::string meas) : testbeam(tb), name(na), material(mat), thickness(thick), doping(dope), region(reg), fluence(flu), particle(part), annealing(ann), measurement(meas) {}

//Copy Constructor
DetCurve::DetCurve(const DetCurve& other) {
    this->testbeam = other.testbeam;
    this->name = other.name;
    this->material = other.material;
    this->thickness = other.thickness;
    this->doping = other.doping;
    this->region = other.region;
    this->fluence = other.fluence;
    this->particle = other.particle;
    this->annealing = other.annealing;
    this->measurement = other.measurement;
}

DetCurve::~DetCurve(){
}

bool DetCurve::operator<(const DetCurve& other) const
{
    if (this->testbeam != other.testbeam) return this->testbeam < other.testbeam;
    if (this->name != other.name) return this->name < other.name;
    if (this->material != other.material) return this->material < other.material;
    if (this->thickness != other.thickness) return this->thickness< other.thickness;
    if (this->doping != other.doping) return this->doping < other.doping;
    if (this->region != other.region) return this->region < other.region;
    if (this->fluence != other.fluence) return this->fluence < other.fluence;
    if (this->particle != other.particle) return this->particle < other.particle;
    if (this->annealing != other.annealing) return this->annealing < other.annealing;
    if (this->measurement != other.measurement) return this->measurement < other.measurement;

    return false;
}



DetCurve& DetCurve::operator=(const DetCurve& other) {
    if (this == &other)
        return *this;
    this->testbeam = other.testbeam;
    this->name = other.name;
    this->material = other.material;
    this->thickness = other.thickness;
    this->doping = other.doping;
    this->region = other.region;
    this->fluence = other.fluence;
    this->particle = other.particle;
    this->annealing = other.annealing;
    this->measurement = other.measurement;
    return *this;
}

//single material

void DetCurve::set_properties(TGraphErrors*& graph) {
   // if (this->doping == "N" ) graph->SetLineColor(LibreColor(1)), graph->SetMarkerColor(LibreColor(1));
  //  // else if (this->doping == "N" && this->region > 4) graph->SetLineColor(LibreColor(12)), graph->SetMarkerColor(LibreColor(12));
  //  else if (this->doping == "P") graph->SetLineColor(LibreColor(2)), graph->SetMarkerColor(LibreColor(2));
  //  // else if (this->doping == "P" && this->region > 4) graph->SetLineColor(LibreColor(11)), graph->SetMarkerColor(LibreColor(11));
  //  else if (this->doping == "Y") graph->SetLineColor(LibreColor(4)), graph->SetMarkerColor(LibreColor(4));
  //    // else if (this->doping == "Y" && this->region > 4) graph->SetLineColor(LibreColor(8)), graph->SetMarkerColor(LibreColor(8));
	
   if (this->doping == "N" && this->material == "FZ") graph->SetLineColor(LibreColor(1)), graph->SetMarkerColor(LibreColor(1)), graph->SetFillColor(LibreColor(1));
   else if (this->doping == "P" && this->material == "FZ") graph->SetLineColor(LibreColor(2)), graph->SetMarkerColor(LibreColor(2)), graph->SetFillColor(LibreColor(2));
   else if (this->doping == "Y" && this->material == "FZ") graph->SetLineColor(LibreColor(4)), graph->SetMarkerColor(LibreColor(4)), graph->SetFillColor(LibreColor(4));
   
   else if (this->doping == "N" && this->material == "MCZ") graph->SetLineColor(LibreColor(12)), graph->SetMarkerColor(LibreColor(12)), graph->SetFillColor(LibreColor(12));
   else if (this->doping == "P" && this->material == "MCZ") graph->SetLineColor(LibreColor(11)), graph->SetMarkerColor(LibreColor(11)), graph->SetFillColor(LibreColor(11));
   else if (this->doping == "Y" && this->material == "MCZ") graph->SetLineColor(LibreColor(8)), graph->SetMarkerColor(LibreColor(8)), graph->SetFillColor(LibreColor(8));
   
   else if (this->doping == "N" && this->material == "FTH") graph->SetLineColor(LibreColor(6)), graph->SetMarkerColor(LibreColor(6)), graph->SetFillColor(LibreColor(6));
   else if (this->doping == "P" && this->material == "FTH") graph->SetLineColor(LibreColor(5)), graph->SetMarkerColor(LibreColor(5)), graph->SetFillColor(LibreColor(5));
   else if (this->doping == "Y" && this->material == "FTH") graph->SetLineColor(LibreColor(7)), graph->SetMarkerColor(LibreColor(7)), graph->SetFillColor(LibreColor(7));
	 
   graph->SetLineWidth(2);
   graph->SetFillStyle(3002);
   
   if (this->material == "FZ") {
       if (this->thickness == 200) graph->SetMarkerStyle(22);
       else graph->SetMarkerStyle(26);
   }
   else if (this->material == "FTH") graph->SetMarkerStyle(23);
   else if (this->material == "MCZ") graph->SetMarkerStyle(20);
   if (this->thickness == 120)graph->SetMarkerSize(.8);
   else if (this->thickness == 200)graph->SetMarkerSize(1);
   else if (this->thickness == 320)graph->SetMarkerSize(1);
   
   if (this->annealing != 0) graph->SetLineStyle(2);
   if (this->particle == "n") graph->SetLineStyle(6);
   if (this->particle == "p") graph->SetLineStyle(9);
}

void DetCurve::set_properties_asymm(TGraphAsymmErrors*& graph) {
   // if (this->doping == "N" ) graph->SetLineColor(LibreColor(1)), graph->SetMarkerColor(LibreColor(1));
  //  // else if (this->doping == "N" && this->region > 4) graph->SetLineColor(LibreColor(12)), graph->SetMarkerColor(LibreColor(12));
  //  else if (this->doping == "P") graph->SetLineColor(LibreColor(2)), graph->SetMarkerColor(LibreColor(2));
  //  // else if (this->doping == "P" && this->region > 4) graph->SetLineColor(LibreColor(11)), graph->SetMarkerColor(LibreColor(11));
  //  else if (this->doping == "Y") graph->SetLineColor(LibreColor(4)), graph->SetMarkerColor(LibreColor(4));
  //    // else if (this->doping == "Y" && this->region > 4) graph->SetLineColor(LibreColor(8)), graph->SetMarkerColor(LibreColor(8));
	
   if (this->doping == "N" && this->material == "FZ") graph->SetLineColor(LibreColor(1)), graph->SetMarkerColor(LibreColor(1)), graph->SetFillColor(LibreColor(1));
   else if (this->doping == "P" && this->material == "FZ") graph->SetLineColor(LibreColor(2)), graph->SetMarkerColor(LibreColor(2)), graph->SetFillColor(LibreColor(2));
   else if (this->doping == "Y" && this->material == "FZ") graph->SetLineColor(LibreColor(4)), graph->SetMarkerColor(LibreColor(4)), graph->SetFillColor(LibreColor(4));
   
   else if (this->doping == "N" && this->material == "MCZ") graph->SetLineColor(LibreColor(12)), graph->SetMarkerColor(LibreColor(12)), graph->SetFillColor(LibreColor(12));
   else if (this->doping == "P" && this->material == "MCZ") graph->SetLineColor(LibreColor(11)), graph->SetMarkerColor(LibreColor(11)), graph->SetFillColor(LibreColor(11));
   else if (this->doping == "Y" && this->material == "MCZ") graph->SetLineColor(LibreColor(8)), graph->SetMarkerColor(LibreColor(8)), graph->SetFillColor(LibreColor(8));
   
   else if (this->doping == "N" && this->material == "FTH") graph->SetLineColor(LibreColor(6)), graph->SetMarkerColor(LibreColor(6)), graph->SetFillColor(LibreColor(6));
   else if (this->doping == "P" && this->material == "FTH") graph->SetLineColor(LibreColor(5)), graph->SetMarkerColor(LibreColor(5)), graph->SetFillColor(LibreColor(5));
   else if (this->doping == "Y" && this->material == "FTH") graph->SetLineColor(LibreColor(7)), graph->SetMarkerColor(LibreColor(7)), graph->SetFillColor(LibreColor(7));
	 
   graph->SetLineWidth(2);
   graph->SetFillStyle(3002);
   
   if (this->material == "FZ") {
       if (this->thickness == 200) graph->SetMarkerStyle(22);
       else graph->SetMarkerStyle(26);
   }
   else if (this->material == "FTH") graph->SetMarkerStyle(23);
   else if (this->material == "MCZ") graph->SetMarkerStyle(20);
   if (this->thickness == 120)graph->SetMarkerSize(.8);
   else if (this->thickness == 200)graph->SetMarkerSize(1);
   else if (this->thickness == 320)graph->SetMarkerSize(1);
   
   if (this->annealing != 0) graph->SetLineStyle(2);
   if (this->particle == "n") graph->SetLineStyle(6);
   if (this->particle == "p") graph->SetLineStyle(9);
}


// //thickness
// void DetCurve::set_properties(TGraphErrors*& graph) {
//     // if (this->thickness == 320) graph->SetLineColor(LibreColor(1)), graph->SetMarkerColor(LibreColor(1));
//     // else if (this->thickness == 200 && this->material=="FZ") graph->SetLineColor(LibreColor(2)), graph->SetMarkerColor(LibreColor(2));
//     // else if (this->thickness == 200 && this->material=="FTH") graph->SetLineColor(LibreColor(11)), graph->SetMarkerColor(LibreColor(11));
//     // else if (this->thickness == 200 && this->material=="MCZ") graph->SetLineColor(LibreColor(10)), graph->SetMarkerColor(LibreColor(10));
//     // else if (this->thickness == 120) graph->SetLineColor(LibreColor(4)), graph->SetMarkerColor(LibreColor(4));
// 	
//     if (this->thickness == 320) graph->SetLineColor(LibreColor(1)), graph->SetMarkerColor(LibreColor(1));
//     else if (this->thickness == 200 && this->material=="FZ") graph->SetLineColor(LibreColor(2)), graph->SetMarkerColor(LibreColor(2));
//     else if (this->thickness == 200 && this->material=="FTH") graph->SetLineColor(LibreColor(4)), graph->SetMarkerColor(LibreColor(4));
//     else if (this->thickness == 200 && this->material=="MCZ") graph->SetLineColor(LibreColor(8)), graph->SetMarkerColor(LibreColor(8));
//     else if (this->thickness == 120) graph->SetLineColor(LibreColor(5)), graph->SetMarkerColor(LibreColor(5));
// 	
//     graph->SetLineWidth(2);
// 	if (this->region <= 4 ) 
// 	{
// 		if (this->doping == "N") graph->SetMarkerStyle(20);
// 		else if (this->doping == "P") graph->SetMarkerStyle(21);
// 		else if (this->doping == "Y") graph->SetMarkerStyle(22);
// 	}
// 	if (this->region > 4 ) 
// 	{
// 		if (this->doping == "N") graph->SetMarkerStyle(24);
// 		else if (this->doping == "P") graph->SetMarkerStyle(25);
// 		else if (this->doping == "Y") graph->SetMarkerStyle(26);
// 	}
//     if (this->annealing != 0) graph->SetLineStyle(2);
//     if (this->particle == "n") graph->SetLineStyle(6);
//     if (this->particle == "p") graph->SetLineStyle(9);
//   
// }


Color_t DetCurve::LibreColor(const unsigned int& plotIndex) 
{
    std::string colorCode;
    
    if (plotIndex==0) colorCode = "#000000";
    else {
        int nColor=(plotIndex-1) % 12;
        switch (nColor) {
            case 0 :
                colorCode="#014586";
                break;
            case 1 :
                colorCode="#FF420E";
                break;
            case 2 :
                colorCode="#FFD320";
                break;
            case 3 :
                colorCode="#579D1C";
                break;
            case 4 :
                colorCode="#7E0021";
                break;
            case 5 :
                colorCode="#83CAFF";
                break;
            case 6 :
                colorCode="#314004";
                break;
            case 7 :
                colorCode="#AECF00";
                break;
            case 8 :
                colorCode="#4B1F6F";
                break;
            case 9 :
                colorCode="#FF950E";
                break;
            case 10 :
                colorCode="#C5000B";
                break;
            case 11 :
                colorCode="#0084D1";
                break;
            default :
                std::cerr << "ERROR: in Vizard::getNiceColor() n%12 is not an int between 0 and 11! This should not happen." << std::endl;
                colorCode="#000000";
                break;
        }
    }
    
    return TColor::GetColor(colorCode.c_str());
}
