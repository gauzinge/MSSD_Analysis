#include "resolution_scatterplot.h"

resolution_scatterplot::resolution_scatterplot(std::string collection, std::string measurement, Settings thesettings) {
	 
	this->multigraph = new TMultiGraph();
	this->collection = collection;
	this->measurement = measurement;
    
	for (std::vector<std::string>::iterator files = thesettings.filelist.begin(); files != thesettings.filelist.end(); files++) {
        
		//read resultfiles
		std::string dummy = *files;
		size_t last_int= dummy.find("/");
		std::string dummy2 = dummy.substr(last_int+1/*+7*/,6);
		int dummytestbeam;
		std::istringstream (dummy2) >> dummytestbeam;
				
		Detector mydetector;
		mydetector.read_file(*files);
		
		//some a-posteriori cleanup which is unfortunately necessary
		if (mydetector.particle == "mixed") mydetector.particle = "p+n";
		if (mydetector.name == "FTH200N_24_MSSD_2") mydetector.annealing = 0;
		if (mydetector.name == "FZ200Y_02_MSSD_2") {
			mydetector.fluence = 3e14;
			mydetector.particle = "p";
		}
		if (mydetector.name == "FZ200Y_10_MSSD_2") mydetector.particle = "n", mydetector.fluence = 4.5E14;
		if (mydetector.name == "FZ320N_04_MSSD_1") mydetector.particle = "none", mydetector.fluence = 0;
		
		
		char voltage_buffer[4],fluence_buffer[8], annealing_buffer[8];
		sprintf(voltage_buffer,"%03i",mydetector.voltage);
		sprintf(fluence_buffer,"%1.1e",mydetector.fluence);
		sprintf(annealing_buffer,"%06d",mydetector.annealing);

		std::string detname_long = mydetector.name + "_" + voltage_buffer + "V" + "_" + fluence_buffer + "_" + annealing_buffer;
		
		if (!thesettings.names.empty()) //names provided
		{
			for (std::set<std::string>::iterator nameit = thesettings.names.begin(); nameit != thesettings.names.end(); nameit++)
			{
				if (*nameit == mydetector.name)
				{
                  									
					float result015;
					float result025;
						
					std::string plottitle;
					if (this->collection == "onTrackCluster") {
						if (measurement == "Resolution") {
							result015 = mydetector.onTrackCluster_resolution.at(2);
							result025 = mydetector.onTrackCluster_resolution.at(6);
							plottitle = "Resolution_onTrackCluster";
						} 
					}
					if (this->collection == "Trackhit") {
						if (measurement == "Resolution") {
							result015 = mydetector.Trackhit_resolution.at(2);
							result025 = mydetector.Trackhit_resolution.at(6);
							plottitle = "Resolution_Trackhit";
						}
					}
                     
					DetCurve mycurve(dummytestbeam, mydetector.name, mydetector.material, mydetector.thickness, mydetector.doping, 0, mydetector.fluence, mydetector.particle, mydetector.annealing, measurement);   
	                 
						if (result015 != -999 && result025 != -999 && result015 > 0 && result025 > 0) 
						{ //sanity check
							TGraphErrors* tmp        = new TGraphErrors();
							tmp->SetTitle(plottitle.c_str());
							mycurve.set_properties(tmp);
							int np = tmp->GetN();
							tmp->SetPoint(np,result015,result025);
							tmp->SetPointError(np,0,0);
							this->multigraph->Add(tmp,"p");
						}
					
				}
			}
		}
		              
		else if (//no names provided: if detector material, doping, fluence, particle matches settings
			(thesettings.materials.find(mydetector.material) != thesettings.materials.end())
				&& (thesettings.doping.find(mydetector.doping) != thesettings.doping.end())
					&& (thesettings.thickness.find(mydetector.thickness) != thesettings.thickness.end())
						&& (thesettings.fluence.find(mydetector.fluence) != thesettings.fluence.end())
							&& (thesettings.particle.find(mydetector.particle) != thesettings.particle.end())
								)
		{				
				
			float result015;
			float result025;
					
			std::string plottitle;
			if (this->collection == "onTrackCluster") {
				if (measurement == "Resolution") {
					result015 = mydetector.onTrackCluster_resolution.at(2);
					result025 = mydetector.onTrackCluster_resolution.at(6);
					plottitle = "Resolution_onTrackCluster";
				} 
			}
			if (this->collection == "Trackhit") {
				if (measurement == "Resolution") {
					result015 = mydetector.Trackhit_resolution.at(2);
					result025 = mydetector.Trackhit_resolution.at(6);
					plottitle = "Resolution_Trackhit";
				}
			}
	
			DetCurve mycurve(dummytestbeam, mydetector.name, mydetector.material, mydetector.thickness, mydetector.doping, 0, mydetector.fluence, mydetector.particle, mydetector.annealing, measurement);
					
				if (result015 != -999 && result025 != -999 && result015 > 0 && result025 > 0) 
				{
					TGraphErrors* tmp = new TGraphErrors();
					tmp->SetTitle(plottitle.c_str());
					mycurve.set_properties(tmp);
					int np = tmp->GetN();
					tmp->SetPoint(np,result015,result025);
					tmp->SetPointError(np,0,0);
					this->multigraph->Add(tmp,"p");
				}
		}
            
	}
}

resolution_scatterplot::~resolution_scatterplot(){//eventually iterate through graph set and delete TGraph*s
}


void resolution_scatterplot::draw_graphs(std::string settingfilename)
{
	TCanvas* plotcanvas = new TCanvas("plotcanvas","Correlation of Resolution of w/p 0.15 and 0.25");
	gPad->SetLeftMargin(0.15);
	gPad->SetBottomMargin(0.15);
	gStyle->SetTitleOffset(1,"y");
	TF1* line = new TF1("line","x",0,100);
	this->multigraph->SetTitle(";#sigma_{w/p=0.15} [#mum];#sigma_{w/p=0.25} [#mum]");
	plotcanvas->cd();
	this->multigraph->Draw("A");
	line->Draw("same");
	this->multigraph->GetXaxis()->SetRangeUser(13,30);
	this->multigraph->GetYaxis()->SetRangeUser(13,30);
	plotcanvas->Modified();
}