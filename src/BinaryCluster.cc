//
//  BinaryCluster.cc
//  
//
//  Created by Georg Auzinger on 17.01.13.
//
//

#include "BinaryCluster.h"

const int maximumClusterNeighbours = 2;

Curve::Curve(int reg, std::string algo)
{
	this->region = reg;
	this->algorithm = algo;
}

Curve::~Curve(){}

void Curve::set_properties(TGraph*& graph)
{
	graph->SetLineWidth(2);
	if (this->region < 4)  graph->SetLineStyle(0);
	if (this->region >= 4 && this->region < 9 ) graph->SetLineStyle(1);
	else graph->SetLineStyle(2);
	
	if (this->region == 3 || this->region == 7 || this->region == 11)
	{
		if (this->algorithm == "barycentric") graph->SetLineColor(LibreColor(3));
		if (this->algorithm == "binary12")    graph->SetLineColor(LibreColor(1));
		if (this->algorithm == "binary1")     graph->SetLineColor(LibreColor(2));
		if (this->algorithm == "leading")     graph->SetLineColor(LibreColor(4));
		if (this->algorithm == "width") graph->SetLineColor(LibreColor(1));
	}
	else if (this->region == 1 || this->region == 5 || this->region == 9)
	{
		if (this->algorithm == "barycentric") graph->SetLineColor(LibreColor(5));
		if (this->algorithm == "binary12")    graph->SetLineColor(LibreColor(12));
		if (this->algorithm == "binary1")     graph->SetLineColor(LibreColor(11));
		if (this->algorithm == "leading")     graph->SetLineColor(LibreColor(8));
		if (this->algorithm == "width") graph->SetLineColor(LibreColor(12));
	}
	if (this->algorithm == "global_efficiency") graph->SetLineColor(LibreColor(10));
	if (this->algorithm == "noise_occupancy") graph->SetLineColor(LibreColor(9));

	if (this->algorithm == "global_efficiency_width") graph->SetLineColor(LibreColor(10));
	if (this->algorithm == "noise_occupancy_width") graph->SetLineColor(LibreColor(9));
	
}   
    
    
bool Curve::operator<(const Curve& other) const
{
    if (this->region != other.region) return this->region < other.region;
    if (this->algorithm != other.algorithm) return this->algorithm < other.algorithm;
    return false;
}

float get_calibfactor(Config& myconfig)
{
 	int ref_thickness = 300;
    float nominal_e_signal = ref_thickness * 78;
	int counter = 0;
	float mean = 0;
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
    {
		mean += filenames->refplane_adc;
		counter++;
	}
	mean /=counter;
	std::cout << "[BINARY] The mean Calibration factor is " << nominal_e_signal/mean << std::endl;
	return nominal_e_signal / mean;
}

// Detector read_resolution_noise(std::string configfilename)
// {
// 	configfilename.replace(0,5,"Resultfiles");
// 	configfilename = configfilename.substr(0,configfilename.length()-4);
// 	configfilename += "_Results.root";
// 	std::cout << "Reading results from " << configfilename << std::endl;
// 	
// 	Detector mydetector;
// 	mydetector.read_file(configfilename.c_str());
// 	return mydetector;
// }

resolution_noise read_resolution_noise(Config& myconfig)
{
	std::string filename = myconfig.resultfoldername + myconfig.detname + "_Results.root";
	Detector mydetector;
	mydetector.read_file(filename.c_str());
	resolution_noise myresonoise;
	myresonoise.resolution = mydetector.onTrackCluster_resolution;
	myresonoise.noise = mydetector.noise;
	return myresonoise;
}


Result find_cluster_raw(Config& myconfig, float threshold) {
    
    std::vector<TH1D*> residuals_bary;
    std::vector<TH1D*> residuals_binary12;
    std::vector<TH1D*> residuals_binary1;
    std::vector<TH1D*> residuals_leading;
    
    std::vector<TF1*> bary_fit;
    std::vector<TF1*> binary12_fit;
    std::vector<TF1*> binary1_fit;
    std::vector<TF1*> leading_fit;
    
    std::vector<TH1I*> width_vector;
    
    for (unsigned int index = 0; index<12 ; index++) {
        std::string title="Binary Resolution Region ";
        std::string title1="Binary Clusterwidth Region ";
        char buffer[3];
        sprintf(buffer,"%i", index+1);
        title+=buffer;
        title1+=buffer;
        
		std::string barystr = "bary_";
		barystr += buffer;
		std::string binary12str = "binary12_";
		binary12str += buffer;
		std::string binary1str = "binary1_";
		binary1str += buffer;
		std::string leadingstr = "leading_";
		leadingstr += buffer;
		
		int pitch = get_pitch_region(index+1)/1000;
        TH1D* bary = new TH1D(barystr.c_str(), title.c_str(), 1000, -3*pitch,3*pitch);
        TH1D* binary12 = new TH1D(binary12str.c_str(), title.c_str(), 1000, -3*pitch,3*pitch);
        TH1D* binary1 = new TH1D(binary1str.c_str(), title.c_str(), 1000, -3*pitch,3*pitch);
        TH1D* leading = new TH1D(leadingstr.c_str(), title.c_str(), 1000, -3*pitch,3*pitch);
        
        residuals_bary.push_back(bary);
        residuals_binary12.push_back(binary12);
        residuals_binary1.push_back(binary1);
        residuals_leading.push_back(leading);
        
		std::string widthstr = "Width_";
		widthstr += buffer;
        TH1I* width = new TH1I(widthstr.c_str(), title1.c_str(),10,0,10);
        
        width_vector.push_back(width);
        
        TF1* fit1 =  new TF1 (barystr.c_str(), "gaus", -0.5,0.5);
        TF1* fit2 =  new TF1 (binary12str.c_str(), "gaus", -0.5,0.5);
        TF1* fit3 =  new TF1 (binary1str.c_str(), "gaus", -0.5,0.5);
        TF1* fit4 =  new TF1 (leadingstr.c_str(), "gaus", -0.5,0.5);
        
        bary_fit.push_back(fit1);
        binary12_fit.push_back(fit2);
        binary1_fit.push_back(fit3);
        leading_fit.push_back(fit4);
  		
    }
    
    std::vector<int> n_global_tracks (12,0);
	std::vector<int> n_found_clusters (12,0);
	std::vector<int> noiseoccupancy (12,0);
	std::vector<int> countvector (12,0);
	
    for (std::vector<Filepair>::iterator filenames = myconfig.file_list.begin(); filenames != myconfig.file_list.end(); filenames++)
	{
		Track mytrack;
		mytrack.readtree(filenames->trackfilename);
		Data mydata;
		mydata.readtree(filenames->datafilename);
		Alignment myalignment(filenames->datafilename, filenames->trackfilename);
		myalignment.get_alignment(filenames->datafilename);
		EventCollection mycollection;
        
		for (unsigned int i=0; i<mydata.get_n_entries(); i++) {
			mydata.get_entry(i);
			mycollection.addHit(i, mydata.EventNumber);
		}
		std::string matchfilename = myconfig.directory + "Matchfiles/";
		size_t last_int= filenames->datafilename.find("_MSSD");
		matchfilename += filenames->datafilename.substr(last_int-4,4);
		matchfilename += "_Binary.txt";
        
		std::ofstream binaryMatchFile(matchfilename.c_str());
		mycollection.Serialize(binaryMatchFile);
        
		std::ifstream inFile(matchfilename.c_str());
		mycollection.DeSerialize(inFile);
        
		for (unsigned int ntrack=0; ntrack<mytrack.get_n_entries(); ntrack++)
		{
			mytrack.get_entry(ntrack);
			mytrack.apply_alignment(myalignment.angle, myalignment.x_shift,myalignment.y_shift, myalignment.DUT_zPosition);
			//*****************************************************************************************
			//if no onTrackcluster exists for the track but the track is good i increment the global track number
			int nearest_strip = reverse_hitmaker(mytrack.xPrime);
			int region = get_region(mytrack.xPrime);
							   
			if (track_good_strips(nearest_strip) && (track_good(mytrack.xPrime, mytrack.yPrime,0) == 1) && track_good_y(mytrack.yPrime,myconfig.y_min,myconfig.y_max))
			{
				n_global_tracks[region-1]++;
			
				const std::vector<int>& eventTracks = mycollection.GetHitLines(mytrack.EventNumber);
				std::vector<int>::const_iterator it;
				for(it = eventTracks.begin() ; it!= eventTracks.end(); ++it)
				{
					const int& hitLine = *it;
					mydata.get_entry(hitLine);
					//here I have the matching data!

					double center[4]={0,0,0,0};
					float charge = 0;
					int width = 0;
					float weightedSum = 0;
					int sum=0;
					float max_signal = 0;
					int leadingStrip=0;
                    
					//first the efficiency
					bool found = false;
					for (int i = nearest_strip - maximumClusterNeighbours; i <= nearest_strip + maximumClusterNeighbours; i++)
					{
						if (mydata.signal[i] > threshold) 
						{
							found = true;
							break;
						}
					}
					if (found) 
					{
						n_found_clusters[region-1]++;
						int seed_position = -99;
						for (int j = 0; j < 5; j++)
						{
							int seed_leg = ((j%2)*2-1)*((j+1)/2);
							int seed_candidate = nearest_strip + seed_leg;
							if (mydata.signal[seed_candidate] > threshold) //seed found
							{
								seed_position = seed_candidate;
								charge += mydata.signal[seed_candidate];
								max_signal = mydata.signal[seed_candidate];
								leadingStrip = seed_candidate; 
								sum += seed_candidate;
								weightedSum += mydata.signal[seed_candidate]*seed_candidate;
								width++;
								break;
							}
						}
						if (seed_position == -99) std::cout << "Error, no seed found although a hit was detected!" << std::endl;
						else //really only if i found a seed
						{
							//now search for neighbors
							//from seed upwards
							int region_start = (seed_position/32)*32+2;
							int region_end = (seed_position/32+1)*32-3;
							for (int i = seed_position+1; i < region_end; i++)
							{
								if (mydata.signal[i] > threshold)
								{
									charge += mydata.signal[i];
									sum += i;
									weightedSum += mydata.signal[i]*i;
									if (mydata.signal[i] > max_signal)
									{
										max_signal = mydata.signal[i];
										leadingStrip = i;
									}
									width++;
								}
								else break;
							}
							//and then downwards
							for (int i = seed_position-1; i > region_start; i--)
							{
								if (mydata.signal[i] > threshold)
								{
									charge += mydata.signal[i];
									sum += i;
									weightedSum += mydata.signal[i]*i;
									if (mydata.signal[i] > max_signal)
									{
										max_signal = mydata.signal[i];
										leadingStrip = i;
									}
									width++;
								}
								else break;
							}
							//as I have found something, do the computations
							//Barycentric - > weighted mean
							center[0] = (weightedSum/charge);
							//Binary with 1/2 strip resolution
							center[1] = (sum/static_cast<float>(width));
							//Binary with 1 strip resolution
							center[2] = sum/width;
							//Leading strip
							center[3] = leadingStrip;
                        
							double xPosition[4]={0,0,0,0};
							for (int i =0; i<4; i++) {
								xPosition[i] = hitmaker(center[i]);
							} //now I have physical hits for 4 algorithms
							width_vector.at(region-1)->Fill(width);
							residuals_bary.at(region-1)->Fill(mytrack.xPrime-xPosition[0]);
							residuals_binary12.at(region-1)->Fill(mytrack.xPrime-xPosition[1]);
							residuals_binary1.at(region-1)->Fill(mytrack.xPrime-xPosition[2]);
							residuals_leading.at(region-1)->Fill(mytrack.xPrime-xPosition[3]);
						}
					}
					
					//not done yet: I still have to compute the noise occupancy
					int first_strip = (12-(region)) * 32;
					for (int strip = first_strip + 2; strip < first_strip + 30; strip++)
					{
						if (fabs(nearest_strip - strip) > maximumClusterNeighbours) //strips not close to track
						{
							if (mydata.signal[strip] > threshold) //found fake track
							{
								noiseoccupancy.at(region-1)++;
							}
							countvector.at(region-1)++; 
						} 						
						else strip++;
					}
				}
			}
		}
	}// Done with the input Runs! lets get to Fitting!
        
    std::cout << "I have finished searching for Clusters! Now lets Fit!" << std::endl;
    
    Result myresult;
	//     TCanvas* mycanvas = new TCanvas("","");
	// mycanvas->Divide(4,3);
	
    for (int i = 0; i < 12; i++) {
	    std::cout << "Found " << n_found_clusters[i] << " in Region  " << i+1 << " global Tracks: " << n_global_tracks[i] << " fake Tracks " << noiseoccupancy[i] << std::endl;
		
        residuals_bary.at(i)->Fit(bary_fit.at(i),"NQ");
        residuals_binary12.at(i)->Fit(binary12_fit.at(i),"NQ");
        residuals_binary1.at(i)->Fit(binary1_fit.at(i),"NQ");
        residuals_leading.at(i)->Fit(leading_fit.at(i),"NQ");
        
        myresult.resolution_bary.push_back(residuals_bary.at(i)->GetRMS()*1000);
        myresult.resolution_binary12.push_back(residuals_binary12.at(i)->GetRMS()*1000);
        myresult.resolution_binary1.push_back(residuals_binary1.at(i)->GetRMS()*1000);
        myresult.resolution_leading.push_back(residuals_leading.at(i)->GetRMS()*1000);
        myresult.clusterwidth.push_back(width_vector.at(i)->GetMean());
		
		// mycanvas->cd(i+1);
// 		residuals_binary12.at(i)->Draw();
// 		std::stringstream ss;
// 		ss << "residuals_" << threshold << ".root";
// 		mycanvas->SaveAs(ss.str().c_str());
		
		double noise_occ = (noiseoccupancy.at(i)/static_cast<double>(countvector.at(i)));
		double prob_hit = 1 - noise_occ;
		double prob_fake = 1- pow(prob_hit,5);
	    double efficiency = ((static_cast<double>(n_found_clusters[i])/static_cast<double>(n_global_tracks[i])) - prob_fake)/(1 - prob_fake); 
	   
	   myresult.global_efficiency.push_back(efficiency*100);
	   myresult.noise_occupancy.push_back(noise_occ*100);
		
       // std::cout << "Region " << i+1 << " Bary " << bary_fit.at(i)->GetParameter(2)*1000 << " with RMS " << residuals_bary.at(i)->GetRMS()*1000 << std::endl;
		
	    residuals_bary.at(i)->Delete();
	    residuals_binary12.at(i)->Delete();
	    residuals_binary1.at(i)->Delete();
	    residuals_leading.at(i)->Delete();
		
	    bary_fit.at(i)->Delete();
 	    binary12_fit.at(i)->Delete();
 	    binary1_fit.at(i)->Delete();
 	    leading_fit.at(i)->Delete();
    }
    std::cout << "Clustersearch for threshold " << threshold << " finished!" << std::endl;
    return myresult;
}



void draw_result(Config& myconfig, Settings mysettings, int max_threshold) {
    std::string name = myconfig.foldername + "/Binary";
    mkdir(name.c_str(),S_IRWXU | S_IRWXG | S_IRWXO );
	
	TMultiGraph* multigraph = new TMultiGraph();
	// TMultiGraph* efficiency_multigraph = new TMultiGraph();
	TMultiGraph* width_multigraph = new TMultiGraph();
	
	std::map<Curve,TGraph*> curvemap;
    
	//bools for algorithms... it's just easier to handle
	bool bary = false, binary12 =  false, binary1 = false, leading = false;
	if (mysettings.algorithm.find("barycentric") != mysettings.algorithm.end()) bary = true;
	if (mysettings.algorithm.find("binary12") != mysettings.algorithm.end()) binary12 = true;
	if (mysettings.algorithm.find("binary1") != mysettings.algorithm.end()) binary1 = true;
	if (mysettings.algorithm.find("leading") != mysettings.algorithm.end()) leading = true;
	
	std::cout << bary << " " << binary12 << " " << binary1 << " " << leading << std::endl;

    //initialize a map of curve vs TGraph
	for (std::set<int>::iterator region = mysettings.regions.begin(); region != mysettings.regions.end(); region++) {
		std::stringstream ss;
		ss << *region;
		std::string name = "Region_" + ss.str() + "_";
		// if (bary)
// 		{
		for (std::set<std::string>::iterator it = mysettings.algorithm.begin(); it != mysettings.algorithm.end(); it++) 
		{
			std::cout << "Initializing Region " << *region << " Algorithm " << *it << std::endl; 
			name += *it;
			Curve mycurve(*region, *it);
			if (curvemap.find(mycurve) == curvemap.end())
			{
				TGraph* resolution = new TGraph();
				resolution->SetTitle(name.c_str());
				mycurve.set_properties(resolution);
				curvemap[mycurve] = resolution;
				name.replace(name.find(*it),it->size(),"");
			}
		}

		Curve widthcurve(*region,"width");
		if (curvemap.find(widthcurve) == curvemap.end())
		{
			TGraph* widthgraph = new TGraph();
			widthcurve.set_properties(widthgraph);
			curvemap[widthcurve] = widthgraph;
		}
		Curve global_eff_curve(*region,"global_efficiency");
		Curve noise_occupancy_curve(*region,"noise_occupancy");
		if (curvemap.find(global_eff_curve) == curvemap.end())
		{
			TGraph* global_eff_graph = new TGraph();
			global_eff_curve.set_properties(global_eff_graph);
			curvemap[global_eff_curve] = global_eff_graph;
		}
		if (curvemap.find(noise_occupancy_curve) == curvemap.end())
		{
			TGraph* noise_occupancy_graph = new TGraph();
			noise_occupancy_curve.set_properties(noise_occupancy_graph);
			curvemap[noise_occupancy_curve] = noise_occupancy_graph;
		}
		
		//for clusterwidth plot
		Curve global_eff_width(*region,"global_efficiency_width");
		Curve noise_occupancy_width(*region,"noise_occupancy_width");
		if (curvemap.find(global_eff_width) == curvemap.end())
		{
			TGraph* global_eff_graph_width = new TGraph();
			global_eff_width.set_properties(global_eff_graph_width);
			curvemap[global_eff_width] = global_eff_graph_width;
		}
		if (curvemap.find(noise_occupancy_width) == curvemap.end())
		{
			TGraph* noise_occupancy_graph_width = new TGraph();
			noise_occupancy_width.set_properties(noise_occupancy_graph_width);
			curvemap[noise_occupancy_width] = noise_occupancy_graph_width;
		}
		
	}
    
	//now set the curve properties and add them to the multigraph by looping over the map 
	std::cout << "[BINARY] I've initialized the follwoing graphs: " << curvemap.size() << std::endl;
	for (std::map<Curve, TGraph*>::iterator mapit = curvemap.begin(); mapit != curvemap.end(); mapit++)
	{
		 std::cout << "Region: " << mapit->first.region << " Algorithm: " << mapit->first.algorithm << std::endl;
		 if (mapit->first.algorithm == "barycentric" || mapit->first.algorithm == "binary12" || mapit->first.algorithm == "binary1" || mapit->first.algorithm == "leading" /*|| mapit->first.algorithm == "global_efficiency" || mapit->first.algorithm == "local efficiency"*/) 
			 {
				 //only resolution graphs go in here
				 multigraph->Add(mapit->second,"l");
			 }
	
		 if (mapit->first.algorithm == "width") width_multigraph->Add(mapit->second,"l");
	 }
	
	 float calibration = get_calibfactor(myconfig);
	
    for (int i = 0; i < max_threshold; i++) { //threshold loop
        float threshold = 1 + i;
        
        std::cout << "Processing threshold " << threshold * calibration << "..." << std::endl;
        
        Result myresult;
        myresult = find_cluster_raw(myconfig, threshold);
		
		for (std::set<int>::iterator regit = mysettings.regions.begin(); regit != mysettings.regions.end(); regit++)
		{
			//correct by telescope resolution!
			double sq_subracted_value;
			int region = *regit;
			if (bary)
			{
				Curve mycurve(region,"barycentric");
				sq_subracted_value = sqrt(pow(myresult.resolution_bary.at(region-1),2) - pow(myconfig.myresolution.sigma_dut[myconfig.DUT],2));
				curvemap[mycurve]->SetPoint(curvemap[mycurve]->GetN(), threshold * calibration, sq_subracted_value);
			} 
			if (binary12)
			{
				Curve mycurve(region,"binary12");
				sq_subracted_value = sqrt(pow(myresult.resolution_binary12.at(region-1),2) - pow(myconfig.myresolution.sigma_dut[myconfig.DUT],2));
				curvemap[mycurve]->SetPoint(curvemap[mycurve]->GetN(), threshold * calibration, sq_subracted_value);
			}
			if (binary1) 
			{
				Curve mycurve(region,"binary1");
				sq_subracted_value = sqrt(pow(myresult.resolution_binary1.at(region-1),2) - pow(myconfig.myresolution.sigma_dut[myconfig.DUT],2));
				curvemap[mycurve]->SetPoint(curvemap[mycurve]->GetN(), threshold * calibration, sq_subracted_value);
			}
			if (leading) 
			{
				Curve mycurve(region,"leading");
				sq_subracted_value = sqrt(pow(myresult.resolution_leading.at(region-1),2) - pow(myconfig.myresolution.sigma_dut[myconfig.DUT],2));
				curvemap[mycurve]->SetPoint(curvemap[mycurve]->GetN(), threshold * calibration, sq_subracted_value);
			}

			Curve widthcurve(region,"width");
			curvemap[widthcurve]->SetPoint(curvemap[widthcurve]->GetN(), threshold * calibration, myresult.clusterwidth.at(region-1));
		
			Curve global_eff_curve(region,"global_efficiency");
			Curve noise_occupancy_curve(region,"noise_occupancy");
			
			Curve global_eff_width(region,"global_efficiency_width");
			Curve noise_occupancy_width(region,"noise_occupancy_width");
			
			//the maximum value on the resolution scale is 50, therefore the efficiency can be divided by 2 to work in the same canvas
			curvemap[global_eff_curve]->SetPoint(curvemap[global_eff_curve]->GetN(), threshold * calibration, myresult.global_efficiency.at(region-1)/2);
			curvemap[noise_occupancy_curve]->SetPoint(curvemap[noise_occupancy_curve]->GetN(), threshold * calibration, myresult.noise_occupancy.at(region-1)/2);	
			
			//for the efficiency in the clusterwidth plot, scale by 0.01
			curvemap[global_eff_width]->SetPoint(curvemap[global_eff_width]->GetN(), threshold * calibration, myresult.global_efficiency.at(region-1)*0.005+1);
			curvemap[noise_occupancy_width]->SetPoint(curvemap[noise_occupancy_width]->GetN(), threshold * calibration, myresult.noise_occupancy.at(region-1)*0.005+1);	
			std::cout << "EFFICIENCY " <<myresult.global_efficiency.at(region-1) << "		" << 	myresult.noise_occupancy.at(region-1) << std::endl;	
		}
    }
	
    std::cout << "I am done looping through thresholds! Now it is time to plot!" << std::endl;
    std::string res_name =  myconfig.detname + "_resolution";
    std::string width_name =  myconfig.detname + "_width";
	
	TCanvas* res_canvas = new TCanvas(res_name.c_str(),res_name.c_str());
	double xmin = 0;
	double xmax = max_threshold * calibration;
	double ymin = 0;
	double ymax = 55;
	multigraph->Draw("a");
	
	gPad->SetLeftMargin(0.12);
	gPad->SetBottomMargin(0.12);
	gPad->SetRightMargin(0.12);
	gPad->SetTopMargin(0.12);
	
	resolution_noise detresolutionnoise = read_resolution_noise(myconfig);
	double noise = 0;
	
	for (std::set<int>::iterator regit = mysettings.regions.begin(); regit != mysettings.regions.end(); regit++)
	{
		int region = *regit;
		Curve global_eff_curve(region,"global_efficiency");
		Curve noise_occupancy_curve(region,"noise_occupancy");
		
		double analog_res = detresolutionnoise.resolution.at(region-1);
		noise += detresolutionnoise.noise.at(region-1);
		
		//line indicating the analogue resolution as measured
		TLine* aline = new TLine(xmin,analog_res,xmax,analog_res);
		if (region < 4)  aline->SetLineStyle(0);
		if (region >= 4 && region < 9 ) aline->SetLineStyle(1);
		else aline->SetLineStyle(2);
		aline->SetLineWidth(2);
		
		aline->Draw("same");
	
		curvemap[global_eff_curve]->Draw("l same");
		curvemap[noise_occupancy_curve]->Draw("l same");	
	}
	
	//For second axis showing the threshold in multiple of the noise
	noise /= mysettings.regions.size();
	double xmax_2 = xmax / noise;
		
	multigraph->GetYaxis()->SetRangeUser(ymin,	ymax);
	multigraph->GetXaxis()->SetLimits(xmin,xmax);
	multigraph->GetXaxis()->SetTitle("Threshold [electrons]");
	multigraph->GetYaxis()->SetTitle("Resolution [#mum]");
	multigraph->GetYaxis()->SetTitleOffset(1);
	
	TGaxis *effaxis= new TGaxis(xmax,ymin,xmax,ymax,0,110,510,"+L");
	effaxis->SetTitle("Efficiency / Noise Occupancy [%]");
	effaxis->SetTitleOffset(1.2);
	effaxis->SetTitleColor(LibreColor(10));
	effaxis->SetLabelColor(LibreColor(10));
	effaxis->SetLabelOffset(0.01);
	effaxis->SetLabelFont(42);
	effaxis->SetLabelSize(0.05);
	effaxis->Draw("same");
	
	TGaxis *sigmaaxis= new TGaxis(xmin,ymax,xmax,ymax,0,xmax_2,10,"-LI");
	sigmaaxis->SetTitle("Threshold [#sigma]");
	sigmaaxis->SetTitleOffset(1);
	sigmaaxis->SetTitleColor(LibreColor(0));
	sigmaaxis->SetLabelColor(LibreColor(0));
	sigmaaxis->SetLabelOffset(0.005);
	sigmaaxis->SetLabelFont(42);
	sigmaaxis->SetLabelSize(0.05);
	sigmaaxis->SetTitleSize(0.05);
	// sigmaaxis->SetMaxDigits(1);
	
	sigmaaxis->Draw("same");
	
	res_canvas->Modified();
	
	/////////////////////////////////////////////////////////////////////////////////////
	
    TCanvas* width_canvas =  new TCanvas(width_name.c_str(), width_name.c_str());
	width_canvas->cd();
	
	width_multigraph->Draw("a");
	width_multigraph->GetXaxis()->SetTitle("Threshold [electrons]");
	width_multigraph->GetYaxis()->SetTitle("Clusterwidth");
	width_multigraph->GetYaxis()->SetRangeUser(1,1.55);
	width_multigraph->GetXaxis()->SetLimits(xmin,xmax);
	width_multigraph->GetYaxis()->SetTitleOffset(1);
	
	for (std::set<int>::iterator regit = mysettings.regions.begin(); regit != mysettings.regions.end(); regit++)
	{
		int region = *regit;
		Curve global_eff_width(region,"global_efficiency_width");
		Curve noise_occupancy_width(region,"noise_occupancy_width");
	
		curvemap[global_eff_width]->Draw("l same");
		curvemap[noise_occupancy_width]->Draw("l same");	
	
	}
		
	TGaxis *effaxis_w= new TGaxis(xmax,1,xmax,1.55,0,110,510,"+L");
	effaxis_w->SetTitle("Efficiency / Noise Occupancy [%]");
	effaxis_w->SetTitleOffset(1.2);
	effaxis_w->SetTitleColor(LibreColor(10));
	effaxis_w->SetLabelColor(LibreColor(10));
	effaxis_w->SetLabelOffset(0.01);
	effaxis_w->SetLabelFont(42);
	effaxis_w->SetLabelSize(0.05);
	effaxis_w->Draw("same");
	
	TGaxis *sigmaaxis_w= new TGaxis(xmin,1.55,xmax,1.55,0,xmax_2,10,"-LI");
	sigmaaxis_w->SetTitle("Threshold [#sigma]");
	sigmaaxis_w->SetTitleOffset(1);
	sigmaaxis_w->SetTitleColor(LibreColor(0));
	sigmaaxis_w->SetLabelColor(LibreColor(0));
	sigmaaxis_w->SetLabelOffset(0.005);
	sigmaaxis_w->SetLabelFont(42);
	sigmaaxis_w->SetLabelSize(0.05);
	sigmaaxis_w->SetTitleSize(0.05);
	
	sigmaaxis_w->Draw("same");
	
	width_canvas->Modified();
	
    std::string title = "Results/Binary/BinaryResolution_";
    title+=myconfig.detname;
    title+=".root";
    res_canvas->SaveAs(title.c_str());

    std::string title1 = "Results/Binary/BinaryCluwidth_";
    title1+=myconfig.detname;
    title1+=".root";
    width_canvas->SaveAs(title1.c_str());
    
    // myconfig.plotfile->WriteTObject(res_canvas);
//     myconfig.plotfile->WriteTObject(width_canvas);
}

Color_t LibreColor(const unsigned int& plotIndex) {
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

