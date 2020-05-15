//include <string>
using namespace std;


void LineParser(ifstream&, vector<string>&);						//Function to turn a multiple lines file into a vector of strings 
void Parser(ifstream& file_to_parse, vector<vector<string> >& );	//Function to turn a vector of lines coming from a text in a vector
							 										//of vectors containing the fields. spreadsheet should be empty

void EfficiencyVsLumiPlot(){
	vector<int> runs = {	// Add Runs here
			// END of EraA
			297050, // DONE
			297219, // DONE
			// TS1
			299061, // DONE
			// // END of EraB
			299370, // DONE
			299597, // DONE
			300155, // DONE
			300280, // DONE
			300401, // DONE
			300558, // DONE
			300576, // DONE
			// // From 300802 45-220F 3+3
			// 301161,
			// 301323,
			// 301627,
			// 301998,
			// // END of EraC
			// 302225,
			// 302388,
			// 302485,
			// 302573,
			// 302663,
			// // END of EraD
			// // TS2
			// // 45-220F again in normal mode
			303832, // DONE 
			303885, // DONE
			304062, // DONE
			304144, // DONE
			304204, // DONE
			304333, // DONE 
			304447, // DONE
			304616, // DONE
			304671, // DONE
			304778, // DONE <- lowstat
			// // END of EraE
			305045, // DONE
			305081, // DONE
			// // From 305169 45-220F 3+3
			// 305188,
			// 305237,
			// 305310,
			// 305365,
			// 305377,
			// 305518,
			// 305636,
			// 305814,
			// 305842,
			// // From 305965 56-220 3+3
			// 306092,
			// 306125,
			// 306154,
			// 306256,
			// 306459,
			// END of EraF
	};
	vector<int> arms = {0,1};
	vector<int> stations = {2};
	
	TFile* runLumiFile = new TFile("data/RunLumiHist2017.root","READ");
	TFile *outputFile = new TFile("EfficiencyVsLumiPlot.root","RECREATE");
	TH1D* h1RunLumiHist = (TH1D*)runLumiFile->Get("h1RunLumiDel");

	map<int,bool> missingRuns;
	map<int,double> runLumiMap; // map<run,integratedLuminosity>
	map< pair<int,int>, map<int,double> > avgEfficiencyPeakFitted; // map< pair<arm,station>, map<run,avgEfficiencyFittedValues> >
	map< pair<int,int>, map<int,double> > controlAreaEfficiency; // map< pair<arm,station>, map<run,controlAreaEfficiency> >
	map< pair<int,int>, map<int,int> > pixelsUsedForAvgEfficiencyPeakFitted; // map< pair<arm,station>, map<run,pixelsUsedForAvgEfficiencyFitted> >
	map< pair<int,int>, map<int,double> > yAlignment; // map< pair<arm,station>, map<run,yAlignment> >

	int firstRun = 0;

	int palette[2] = {kRed,kBlue};
	gStyle->SetPalette(2,palette);

	for(auto & run : runs){
		cout << "Reading data from run " << run << endl;
		// Fill Run - Integrated Lumi (delivered) lookup table
		runLumiMap[run] = h1RunLumiHist->GetBinContent(h1RunLumiHist->GetXaxis()->FindBin(run));

		// Read all average efficiency measurements
		string avgEfficiencyFileName = Form("OutputFiles/avgEfficiency_Run%i.dat",run);
		ifstream avgEfficiencyFile(avgEfficiencyFileName);
		if(!avgEfficiencyFile){
			cout << "Average efficiency file for run " << run << " is missing. Skipping..." << endl;
			missingRuns[run] = true;
			continue;	
		}

		vector< vector<string> > avgEfficiencyFileContent;
		Parser(avgEfficiencyFile,avgEfficiencyFileContent);
		for(auto & line : avgEfficiencyFileContent){ //loop on every line of the file
			avgEfficiencyPeakFitted[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(2));
			pixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(3));
			yAlignment[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(4));
			controlAreaEfficiency[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(5));
		}
	}

	// Book and Fill graphs

	TMultiGraph *mgAvgEfficiencyPeakFitted = new TMultiGraph("mgAvgEfficiencyPeakFitted","Irradiation Peak Average Efficiency (Fit Method); Integrated Luminosity (fb^{-1}); Average Efficiency");
	TMultiGraph *mgRelativeAvgEfficiencyPeakFitted = new TMultiGraph("mgRelativeAvgEfficiencyPeakFitted","Irradiation Peak Relative Average Efficiency (Fit Method); Integrated Luminosity (fb^{-1}); RelativeAverage Efficiency");
	map< pair<int,int>, TGraph* > gAvgEfficiencyPeakFitted; 
	map< pair<int,int>, TGraph* > gRelativeAvgEfficiencyPeakFitted; 
	mgAvgEfficiencyPeakFitted->SetMinimum(0.3);
	mgRelativeAvgEfficiencyPeakFitted->SetMinimum(0.3);
	mgAvgEfficiencyPeakFitted->SetMaximum(1.1);
	mgRelativeAvgEfficiencyPeakFitted->SetMaximum(1.1);

	TMultiGraph *mgControlAreaEfficiency = new TMultiGraph("mgControlAreaEfficiency","ControlArea Area Average Efficiency (Fit Method); Integrated Luminosity (fb^{-1}); Average Efficiency");
	TMultiGraph *mgRelativeControlAreaEfficiency = new TMultiGraph("mgRelativeControlAreaEfficiency","ControlArea Area Relative Average Efficiency (Fit Method); Integrated Luminosity (fb^{-1}); RelativeAverage Efficiency");
	map< pair<int,int>, TGraph* > gControlAreaEfficiency; 
	map< pair<int,int>, TGraph* > gRelativeControlAreaEfficiency; 
	mgControlAreaEfficiency->SetMinimum(0.3);
	mgRelativeControlAreaEfficiency->SetMinimum(0.3);
	mgControlAreaEfficiency->SetMaximum(1.1);
	mgRelativeControlAreaEfficiency->SetMaximum(1.1);

	TMultiGraph *mgPixelsUsedForAvgEfficiencyPeakFitted = new TMultiGraph("mgPixelsUsedForAvgEfficiencyPeakFitted","Pixels used for Avg Efficiency; Integrated Luminosity (fb^{-1}); Pixels Used");
	map< pair<int,int>, TGraph* > gPixelsUsedForAvgEfficiencyPeakFitted; 

	TMultiGraph *mgYAlignmentSensorEdge = new TMultiGraph("mgYAlignmentSensorEdge","Y Alignment @ Sensor Edge; Integrated Luminosity (fb^{-1}); mm");
	map< pair<int,int>, TGraph* > gYAlignmentSensorEdge; 
	
	for(auto & arm : arms ){
		for(auto & station : stations){
			int sector = 0;
			int station_id = 0;
			if (arm == 0) sector = 45;
			if (arm == 1) sector = 56;
			if (station == 0) station_id = 210;
			if (station == 2) station_id = 220;

			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)] = new TGraph();
			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetNameTitle(Form("gAvgEfficiencyPeakFitted_arm%i_st%i",arm,station), Form("LHC Sector %i %i FAR",sector,station_id));
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)] = new TGraph();
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetNameTitle(Form("gAvgEfficiencyPeakFitted_arm%i_st%i",arm,station), Form("LHC Sector %i %i FAR",sector,station_id));
			
			gControlAreaEfficiency[pair<int,int>(arm,station)] = new TGraph();
			gControlAreaEfficiency[pair<int,int>(arm,station)]->SetNameTitle(Form("gControlAreaEfficiency_arm%i_st%i",arm,station), Form("LHC Sector %i %i FAR",sector,station_id));
			gRelativeControlAreaEfficiency[pair<int,int>(arm,station)] = new TGraph();
			gRelativeControlAreaEfficiency[pair<int,int>(arm,station)]->SetNameTitle(Form("gControlAreaEfficiency_arm%i_st%i",arm,station), Form("LHC Sector %i %i FAR",sector,station_id));
			
			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)] = new TGraph();
			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetNameTitle(Form("gPixelsUsedForAvgEfficiencyPeakFitted_arm%i_st%i",arm,station), Form("LHC Sector %i %i FAR",sector,station_id));
			gYAlignmentSensorEdge[pair<int,int>(arm,station)] = new TGraph();
			gYAlignmentSensorEdge[pair<int,int>(arm,station)]->SetNameTitle(Form("gYAlignmentSensorEdge%i_st%i",arm,station), Form("Y Alignment LHC Sector %i %i FAR",sector,station_id));

			for(auto & run : runs){
				if(run >= 301161 && run <= 302663 && arm == 0 && station == 2) continue; // Exclude some runs in 3+3
				if(missingRuns[run]) continue;
				if(firstRun == 0) firstRun = run;
				
				gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],avgEfficiencyPeakFitted[pair<int,int>(arm,station)][run]);
				gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],avgEfficiencyPeakFitted[pair<int,int>(arm,station)][run]/avgEfficiencyPeakFitted[pair<int,int>(arm,station)][firstRun]);
			
				gControlAreaEfficiency[pair<int,int>(arm,station)]->SetPoint(gControlAreaEfficiency[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],controlAreaEfficiency[pair<int,int>(arm,station)][run]);
				gRelativeControlAreaEfficiency[pair<int,int>(arm,station)]->SetPoint(gRelativeControlAreaEfficiency[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],controlAreaEfficiency[pair<int,int>(arm,station)][run]/controlAreaEfficiency[pair<int,int>(arm,station)][firstRun]);
			
				gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],pixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)][run]);
				gYAlignmentSensorEdge[pair<int,int>(arm,station)]->SetPoint(gYAlignmentSensorEdge[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],yAlignment[pair<int,int>(arm,station)][run]);
			}

			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),43.11,1);
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),43.11,1);
			
			gControlAreaEfficiency[pair<int,int>(arm,station)]->SetPoint(gControlAreaEfficiency[pair<int,int>(arm,station)]->GetN(),43.11,1);
			gRelativeControlAreaEfficiency[pair<int,int>(arm,station)]->SetPoint(gRelativeControlAreaEfficiency[pair<int,int>(arm,station)]->GetN(),43.11,1);
			
			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),43.11,1);
			gYAlignmentSensorEdge[pair<int,int>(arm,station)]->SetPoint(gYAlignmentSensorEdge[pair<int,int>(arm,station)]->GetN(),43.11,1);

			//Graph beauty farm

			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetLineWidth(2);
			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetLineWidth(2);
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);

			gControlAreaEfficiency[pair<int,int>(arm,station)]->SetLineWidth(2);
			gControlAreaEfficiency[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);
			gRelativeControlAreaEfficiency[pair<int,int>(arm,station)]->SetLineWidth(2);
			gRelativeControlAreaEfficiency[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);
			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetLineWidth(2);
			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);

			gYAlignmentSensorEdge[pair<int,int>(arm,station)]->SetLineWidth(2);
			gYAlignmentSensorEdge[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);

			// Saving Hists

			outputFile->cd();
			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->Write();

			// Adding to multigraph

			mgAvgEfficiencyPeakFitted->Add(gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)],"PL");
			mgRelativeAvgEfficiencyPeakFitted->Add(gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)],"PL");


			mgControlAreaEfficiency->Add(gControlAreaEfficiency[pair<int,int>(arm,station)],"PL");
			mgRelativeControlAreaEfficiency->Add(gRelativeControlAreaEfficiency[pair<int,int>(arm,station)],"PL");

			mgPixelsUsedForAvgEfficiencyPeakFitted->Add(gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)],"PL");

			mgYAlignmentSensorEdge->Add(gYAlignmentSensorEdge[pair<int,int>(arm,station)],"PL");
		}
	}

	TCanvas *cAvgEfficiencyPeakFitted = new TCanvas("cAvgEfficiencyPeakFitted","AvgEfficiencyPeakFitted");
	mgAvgEfficiencyPeakFitted->Draw("A pmc plc");
	cAvgEfficiencyPeakFitted->BuildLegend(0.1,0.1,0.35,0.3);
	TCanvas *cRelativeAvgEfficiencyPeakFitted = new TCanvas("cRelativeAvgEfficiencyPeakFitted","RelativeAvgEfficiencyPeakFitted");
	mgRelativeAvgEfficiencyPeakFitted->Draw("A pmc plc");
	cRelativeAvgEfficiencyPeakFitted->BuildLegend(0.1,0.1,0.35,0.3);

	TCanvas *cControlAreaEfficiency = new TCanvas("cControlAreaEfficiency","ControlAreaEfficiency");
	mgControlAreaEfficiency->Draw("A pmc plc");
	cControlAreaEfficiency->BuildLegend(0.1,0.1,0.35,0.3);
	TCanvas *cRelativeControlAreaEfficiency = new TCanvas("cRelativeControlAreaEfficiency","RelativeControlAreaEfficiency");
	mgRelativeControlAreaEfficiency->Draw("A pmc plc");
	cRelativeControlAreaEfficiency->BuildLegend(0.1,0.1,0.35,0.3);

	TCanvas *cPixelsUsedForAvgEfficiencyPeakFitted = new TCanvas("cPixelsUsedForAvgEfficiencyPeakFitted","PixelsUsedForAvgEfficiencyPeakFitted");
	mgPixelsUsedForAvgEfficiencyPeakFitted->Draw("A pmc plc");
	cPixelsUsedForAvgEfficiencyPeakFitted->BuildLegend(0.1,0.1,0.35,0.3);

	TCanvas *cYAlignmentSensorEdge = new TCanvas("cYAlignmentSensorEdge","cYAlignmentSensorEdge");
	mgYAlignmentSensorEdge->Draw("A pmc plc");
	cYAlignmentSensorEdge->BuildLegend(0.1,0.1,0.35,0.3);

	outputFile->Close();
}

void LineParser(ifstream& file_to_parse, vector<string>& lines){
  if (!file_to_parse){
    printf("ERROR in LineParser > file not found.\n");
    exit(1);
  }
  while (true){
    if (file_to_parse.eof())break;
    string line;
    getline(file_to_parse,line);
    if (line[0] != '#' && line.size() > 0){
      lines.push_back(line);
    }
  }
}

void Parser(ifstream& file_to_parse,vector<vector<string> >& spreadsheet){
  vector<string> lines;
  LineParser(file_to_parse,lines);
  for (vector<string>::iterator line=lines.begin();line!=lines.end();++line){
    string field_content;
    istringstream linestream(*line);
    if (!linestream.eof()){
      spreadsheet.push_back(vector<string>());
      while(1){
	getline(linestream,field_content,' ');
	spreadsheet.back().push_back(field_content);
	if (linestream.eof()){break;}
      }
    }
  }
}