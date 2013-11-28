//
//  trackfunctions.cc
//  
//
//  Created by Georg Auzinger on 22.08.12.
//
//
#include "trackfunctions.h"
#include <math.h>
//#include "include/trackfunctions.h"


int get_pitch_region(int Region) {
    if (Region==1) return 120;
    if (Region==2) return 240;
    if (Region==3) return 80;
    if (Region==4) return 70;
    
    if (Region==5) return 120;
    if (Region==6) return 240;
    if (Region==7) return 80;
    if (Region==8) return 70;
    
    if (Region==9) return 120;
    if (Region==10) return 240;
    if (Region==11) return 80;
    if (Region==12) return 70;
    
    return 0;
}

int track_good(double xCoordinate, double yCoordinate, int npitches) {
        if ((xCoordinate >0.94+npitches*get_pitch_region(12)*0.001) && (xCoordinate <3.24-npitches*get_pitch_region(12)*0.001)) return 1;
        
        else if ((xCoordinate >4.48+npitches*get_pitch_region(11)*0.001) && (xCoordinate <7.07-npitches*get_pitch_region(11)*0.001)) return 1;
        
        else if ((xCoordinate >8.34+npitches*get_pitch_region(10)*0.001) && (xCoordinate <16.16-npitches*get_pitch_region(10)*0.001)) return 1;
        
        else if ((xCoordinate >17.44+npitches*get_pitch_region(9)*0.001) && (xCoordinate <21.34-npitches*get_pitch_region(9)*0.001)) return 1;
        
        else if ((xCoordinate >22.6+npitches*get_pitch_region(8)*0.001) && (xCoordinate <24.87-npitches*get_pitch_region(8)*0.001)) return 1;
        
        else if ((xCoordinate >26.11+npitches*get_pitch_region(7)*0.001) && (xCoordinate <28.72-npitches*get_pitch_region(7)*0.001)) return 1;
        
        else if ((xCoordinate >29.97+npitches*get_pitch_region(6)*0.001) && (xCoordinate <37.82-npitches*get_pitch_region(6)*0.001)) return 1;
        
        else if ((xCoordinate >39.08+npitches*get_pitch_region(5)*0.001) && (xCoordinate <42.99-npitches*get_pitch_region(5)*0.001)) return 1;
        
        else if ((xCoordinate >44.22+npitches*get_pitch_region(4)*0.001) && (xCoordinate <46.52-npitches*get_pitch_region(4)*0.001)) return 1;
        
        else if ((xCoordinate >47.74+npitches*get_pitch_region(3)*0.001) && (xCoordinate <50.37-npitches*get_pitch_region(3)*0.001)) return 1;
        
        else if ((xCoordinate >51.59+npitches*get_pitch_region(2)*0.001) && (xCoordinate <59.48-npitches*get_pitch_region(2)*0.001)) return 1;
        
        else if ((xCoordinate >60.70+npitches*get_pitch_region(1)*0.001) && (xCoordinate <64.64-npitches*get_pitch_region(1)*0.001)) return 1;
        else return 0;
}

int get_region(double xCoordinate) {
    
    if ((xCoordinate >0.94) && (xCoordinate <3.24)) return 12;
    
    else if ((xCoordinate >4.48) && (xCoordinate <7.07)) return 11;
    
    else if ((xCoordinate >8.34) && (xCoordinate <16.16)) return 10;
    
    else if ((xCoordinate >17.44) && (xCoordinate <21.34)) return 9;
    
    else if ((xCoordinate >22.6) && (xCoordinate <24.87)) return 8;
    
    else if ((xCoordinate >26.11) && (xCoordinate <28.72)) return 7;
    
    else if ((xCoordinate >29.97) && (xCoordinate <37.82)) return 6;
    
    else if ((xCoordinate >39.08) && (xCoordinate <42.99)) return 5;
    
    else if ((xCoordinate >44.22) && (xCoordinate <46.52)) return 4;
    
    else if ((xCoordinate >47.74) && (xCoordinate <50.37)) return 3;
    
    else if ((xCoordinate >51.59) && (xCoordinate <59.48)) return 2;
    
    else if ((xCoordinate >60.70) && (xCoordinate <64.64)) return 1;
    
    else return 0;
    
}

int reverse_hitmaker(double xCoordinate) {
    double xPitch[12] = {0.12,0.24,0.08,0.07,0.12,0.24,0.08,0.07,0.12,0.24,0.08,0.07}; //mm
    double dist_edge = 0.900; //mm distance from physical Si sensor edge to "0" Position ("0" + 1 x Pitch = 1st Strip)
    double strip_no = 0;
    if ((xCoordinate >60.70) && (xCoordinate <64.64)) {
        strip_no = (xCoordinate - dist_edge - 59.7605) / xPitch[0]+352;
    }
    else if ((xCoordinate >51.59) && (xCoordinate <59.48)) {
        strip_no = (xCoordinate - dist_edge - 50.6545) / xPitch[1]+320;
    }
     else if ((xCoordinate >47.74) && (xCoordinate <50.37)) {
        strip_no = (xCoordinate - dist_edge - 46.8045) / xPitch[2]+288;
    }
    else if ((xCoordinate >44.22) && (xCoordinate <46.52)) {
        strip_no = (xCoordinate - dist_edge - 43.283) / xPitch[3]+256;
    }
    else if ((xCoordinate >39.08) && (xCoordinate <42.99)) {
        strip_no = (xCoordinate - dist_edge - 38.119) / xPitch[4]+224;
    }
    else if ((xCoordinate >29.97) && (xCoordinate <37.82)) {
        strip_no = (xCoordinate - dist_edge - 29.013) / xPitch[5]+192;
    }
    else if ((xCoordinate >26.11) && (xCoordinate <28.72)) {
        strip_no = (xCoordinate - dist_edge - 25.163) / xPitch[6]+160;
    }
//    else if ((xCoordinate >22.6) && (xCoordinate <24.87)) {
//        strip_no = (xCoordinate - dist_edge - 21.63075) / xPitch[7]+128;
//    }
    else if ((xCoordinate >22.6) && (xCoordinate <24.87)) { //test for region 8 misplacement
        strip_no = (xCoordinate - dist_edge - 21.63775) / xPitch[7]+128;
    }
    else if ((xCoordinate >17.44) && (xCoordinate <21.34)) {
        strip_no = (xCoordinate - dist_edge - 16.4775) / xPitch[8]+96;
    }
    else if ((xCoordinate >8.34) && (xCoordinate <16.16)) {
        strip_no = (xCoordinate - dist_edge - 7.36075) / xPitch[9]+64;
    }
    else if ((xCoordinate >4.48) && (xCoordinate <7.07)) {
        strip_no = (xCoordinate - dist_edge - 3.5215) / xPitch[10]+32;
    }
    else if ((xCoordinate >0.94) && (xCoordinate <3.24)) {
        strip_no = (xCoordinate - dist_edge - 0 / xPitch[11])+0;
    }
	else strip_no = -99;
	
    return floor(strip_no + 0.5) -1;
}

int get_antistrip(int strip) { // if strips start at 0
    if ((strip % 32) <= 13) return strip + 15;
    else if ((strip % 32) >=18) return strip - 15;
    else if ((strip % 32) > 13 && (strip % 32) <= 15) return strip + 13;
    else return strip - 13;
}

//int get_antistrip(int strip) { // if strips start at 1
//    if ((strip % 32) <= 13) return strip + 15;
//    else if ((strip % 32) >=18) return strip - 15;
//    else if ((strip % 32) > 13 && (strip % 32) <= 15) return strip + 13;
//    else return strip - 13;
//}

bool track_good_strips(int strip_no) {
    if (strip_no % 32 > 2 && strip_no % 32 < 29) return true; // if strip starts at 0
//    if (strip_no % 32 > 3 && strip_no % 32 < 30) return true; // if strip starts at 1
    else return false;
}

bool track_good_y (double yCoordinate, double y_low, double y_high) {
    if (yCoordinate > y_low && yCoordinate < y_high) return true;
    else return false;
}

double hitmaker(double cog_strip) {
    double xPosition=0;
    cog_strip+=1;
    double dist_edge = 0.900; //mm distance from physical Si sensor edge to "0" Position ("0" + 1 x Pitch = 1st Strip)
    
    double xPitch[12] = {0.12,0.24,0.08,0.07,0.12,0.24,0.08,0.07,0.12,0.24,0.08,0.07}; //mm
    if ( ( cog_strip >352 ) && ( cog_strip<=385 ) ) {
        xPosition = dist_edge + 59.7605 + ( cog_strip - 352 ) * xPitch[0] ;  //Region 1, 120um
        return xPosition;
        
    } else if ( ( cog_strip > 320 ) && ( cog_strip <= 352 ) ) {
        xPosition = dist_edge + 50.6545 + ( ( cog_strip - 320) * xPitch[1]) ;  //Region 2, 240um
        return xPosition;
        
    } else if ( ( cog_strip > 288 ) && ( cog_strip <= 320 ) ) {
        xPosition = dist_edge + 46.8045 + ( ( cog_strip - 288) * xPitch[2]) ;  //Region 3, 80um
        return xPosition;
        
    } else if ( ( cog_strip > 256 ) && ( cog_strip <= 288 ) ) {
        xPosition = dist_edge + 43.283 + ( ( cog_strip - 256) * xPitch[3]) ;  //Region 4, 70um
        return xPosition;
        
    } else if ( ( cog_strip > 224 ) && ( cog_strip <= 256 ) ) {
        xPosition = dist_edge + 38.119 + ( ( cog_strip - 224) * xPitch[4]) ;  //Region 5, 120um
        return xPosition;
        
    } else if ( ( cog_strip > 192 ) && ( cog_strip <= 224 ) ) {
        xPosition = dist_edge + 29.013 + ( ( cog_strip - 192) * xPitch[5]) ;  //Region 6, 240um
        return xPosition;
        
    } else if ( ( cog_strip > 160 ) && ( cog_strip <= 192 ) ) {
        xPosition = dist_edge + 25.163 + ( ( cog_strip - 160) * xPitch[6]) ;  //Region 7, 80um
        return xPosition;
        
    } else if ( ( cog_strip > 128 ) && ( cog_strip <= 160 ) ) {
        xPosition = dist_edge + 21.63075 + ( ( cog_strip - 128) * xPitch[7]) ;  //Region 8, 70um
        return xPosition;
        
    } else if ( ( cog_strip > 96 ) && ( cog_strip <= 128 ) ) {
        xPosition = dist_edge + 16.4775 + ( ( cog_strip - 96) * xPitch[8]) ;  //Region 9, 120um
        return xPosition;
        
    } else if ( ( cog_strip > 64 ) && ( cog_strip <= 96 ) ) {
        xPosition = dist_edge + 7.36075 + ( ( cog_strip - 64) * xPitch[9]) ;  //Region 10, 240um
        return xPosition;
        
    } else if ( ( cog_strip > 32 ) && ( cog_strip <= 64 ) ) {
        xPosition = dist_edge + 3.5215 + ( ( cog_strip - 32) * xPitch[10]) ;  //Region 11, 80um
        return xPosition;
        
    } else if ( cog_strip <= 32 )  {
        xPosition = dist_edge + 0 + ( ( cog_strip - 0) * xPitch[11]) ;  //Region 12, 70um
        return xPosition;
    } else return 0;
}

std::string find_matchfiledir(std::string datafilename)
{
	// for (int i =0; i < 2; i++) 
	// {
	// 	char* dummy;
	// 	dummy = strrchr(datafilename.c_str(),'/');
	// 	size_t last = dummy-datafilename.c_str();
	// 	datafilename = datafilename.substr(0,last);
	// }
	// std::string directoryname = datafilename +  "/Analysis/Matchfiles/";
	std::string directoryname = "/afs/cern.ch/work/g/gauzinge/public/Analysis/Matchfiles/";
	return directoryname;
}

std::string change_filetype (std::string filename, std::string filetype) {
	std::string newpath = "/afs/cern.ch/work/g/gauzinge/public/MssdData/";
	std::string newname = filename.substr(filename.length()-17, filename.length());
	size_t position = newname.find(".root");
	newname = newpath + newname.substr(0,position) + "_" + filetype + ".root";
	return newname;
}
