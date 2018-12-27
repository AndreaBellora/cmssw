#include <string>
using namespace std;


void LineParser(ifstream&, vector<string>&);						//Function to turn a multiple lines file into a vector of strings 
void Parser(ifstream& file_to_parse, vector<vector<string> >& );	//Function to turn a vector of lines coming from a text in a vector
							 										//of vectors containing the fields. spreadsheet should be empty

void EfficiencyVsLumiPlot(){
	vector<int> runs = {	// Add Runs here
				315512, 
				315713,
				315840, 
				316114, 
				316199, 
				316240, 
				316505, 
				316666, 
				316758, 
				316944, 
				317182, 
				317320, 
				317435, 
				317527, 
				317641, 
				317696, 
				// TS1
				319337, 
				319450, 
				319579, 
				319756, 
				319991, 
				320038, 
				320804, 
				320917, 
				321051, 
				321149, 
				321233, 
				321396, 
				321457, 
				321755, 
				321831, 
				321909, 
				321988, 
				322106,
				322252, 
				322356
	};
	vector<int> arms = {0,1};
	vector<int> stations = {0,2};
	
	TFile* runLumiFile = new TFile("data/RunLumiHist.root","READ");
	TH1D* h1RunLumiHist = (TH1D*)runLumiFile->Get("h1RunLumiDel");

	map<int,bool> missingRuns;
	map<int,double> runLumiMap; // map<run,integratedLuminosity>
	map< pair<int,int>, map<int,double> > avgEfficiencyPeak; // map< pair<arm,station>, map<run,avgEfficiencyValues> >
	map< pair<int,int>, map<int,double> > avgEfficiencyPeakFitted; // map< pair<arm,station>, map<run,avgEfficiencyFittedValues> >
	map< pair<int,int>, map<int,double> > xMaxAvgEfficiencyPeakFitted; // map< pair<arm,station>, map<run,xMaxAvgEfficiencyFitted> >
	map< pair<int,int>, map<int,double> > xMaxAvgEfficiencyPeakFix; // map< pair<arm,station>, map<run,xMaxAvgEfficiencyFix> >
	map< pair<int,int>, map<int,int> > pixelsUsedForAvgEfficiencyPeakFitted; // map< pair<arm,station>, map<run,pixelsUsedForAvgEfficiencyFitted> >
	map< pair<int,int>, map<int,double> > yParam_0; // map< pair<arm,station>, map<run,yParam_0> >
	map< pair<int,int>, map<int,double> > yParam_1; // map< pair<arm,station>, map<run,yParam_1> >
	map< pair<int,int>, map<int,double> > sigmaParam_0; // map< pair<arm,station>, map<run,yParam_0> >
	map< pair<int,int>, map<int,double> > sigmaParam_1; // map< pair<arm,station>, map<run,yParam_1> >


	int firstRun = 0;

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

		double alignmentForSt2 = 40;

		vector< vector<string> > avgEfficiencyFileContent;
		Parser(avgEfficiencyFile,avgEfficiencyFileContent);
		for(auto & line : avgEfficiencyFileContent){ //loop on every line of the file
			avgEfficiencyPeak[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(2));
			avgEfficiencyPeakFitted[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(3));
			xMaxAvgEfficiencyPeakFitted[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(4));
			pixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(5));
			xMaxAvgEfficiencyPeakFix[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(6));
			yParam_0[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(7));
			yParam_1[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(8));
			sigmaParam_0[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(10));
			sigmaParam_1[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] = stod(line.at(11));

			if(stoi(line.at(1)) == 2){
				xMaxAvgEfficiencyPeakFitted[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] -= alignmentForSt2;
			 	xMaxAvgEfficiencyPeakFix[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] -= alignmentForSt2;
			 	yParam_0[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] += alignmentForSt2*yParam_1[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run];
			 	sigmaParam_0[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run] += alignmentForSt2*sigmaParam_1[pair<int,int>(stoi(line.at(0)),stoi(line.at(1)))][run];
			}
		}
	}

	// Book and Fill graphs

	TMultiGraph *mgAvgEfficiencyPeak = new TMultiGraph("mgAvgEfficiencyPeak","Irradiation Peak Average Efficiency; Integrated Luminosity fb^{-1}; Average Efficiency");
	TMultiGraph *mgRelativeAvgEfficiencyPeak = new TMultiGraph("mgRelativeAvgEfficiencyPeak","Irradiation Peak Relative Average Efficiency; Integrated Luminosity (fb^{-1}); RelativeAverage Efficiency");
	map< pair<int,int>, TGraph* > gAvgEfficiencyPeak; 
	map< pair<int,int>, TGraph* > gRelativeAvgEfficiencyPeak; 

	TMultiGraph *mgAvgEfficiencyPeakFitted = new TMultiGraph("mgAvgEfficiencyPeakFitted","Irradiation Peak Average Efficiency (Fit Method); Integrated Luminosity (fb^{-1}); Average Efficiency");
	TMultiGraph *mgRelativeAvgEfficiencyPeakFitted = new TMultiGraph("mgRelativeAvgEfficiencyPeakFitted","Irradiation Peak Relative Average Efficiency (Fit Method); Integrated Luminosity fb^{-1}; RelativeAverage Efficiency");
	map< pair<int,int>, TGraph* > gAvgEfficiencyPeakFitted; 
	map< pair<int,int>, TGraph* > gRelativeAvgEfficiencyPeakFitted; 

	TMultiGraph *mgxMaxAvgEfficiencyPeakFitted = new TMultiGraph("mgxMaxAvgEfficiencyPeakFitted","X upper limit for avg efficiency; Integrated Luminosity (fb^{-1}); xMax");
	map< pair<int,int>, TGraph* > gxMaxAvgEfficiencyPeakFitted; 
	map< pair<int,int>, TGraph* > gxMaxAvgEfficiencyPeakFix; 
	
	TMultiGraph *mgPixelsUsedForAvgEfficiencyPeakFitted = new TMultiGraph("mgPixelsUsedForAvgEfficiencyPeakFitted","Pixels used for Avg Efficiency; Integrated Luminosity (fb^{-1}); Pixels Used");
	map< pair<int,int>, TGraph* > gPixelsUsedForAvgEfficiencyPeakFitted; 
	
	TMultiGraph *mgYFits = new TMultiGraph("mgYFits","y fit functions Arm 0 Station 2;x (mm);y position of the peak from fit (mm)");
	map< pair<int,int>, map<int,TF1*> > yFitFunctions; 
	map< pair<int,int>, map<int,TGraph*> > gYFits; 

	TMultiGraph *mgSigmaFits = new TMultiGraph("mgYFits","Sigma fit functions Arm 0 Station 2;x (mm);Area height (mm)");
	map< pair<int,int>, map<int,TF1*> > sigmaFitFunctions; 
	map< pair<int,int>, map<int,TGraph*> > gSigmaFits; 


	for(auto & arm : arms ){
		for(auto & station : stations){
			gAvgEfficiencyPeak[pair<int,int>(arm,station)] = new TGraph();
			gAvgEfficiencyPeak[pair<int,int>(arm,station)]->SetNameTitle(Form("gAvgEfficiencyPeak_arm%i_st%i",arm,station), Form("Arm %i Station %i",arm,station));
			gRelativeAvgEfficiencyPeak[pair<int,int>(arm,station)] = new TGraph();
			gRelativeAvgEfficiencyPeak[pair<int,int>(arm,station)]->SetNameTitle(Form("gAvgEfficiencyPeak_arm%i_st%i",arm,station), Form("Arm %i Station %i",arm,station));
			
			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)] = new TGraph();
			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetNameTitle(Form("gAvgEfficiencyPeakFitted_arm%i_st%i",arm,station), Form("Arm %i Station %i",arm,station));
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)] = new TGraph();
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetNameTitle(Form("gAvgEfficiencyPeakFitted_arm%i_st%i",arm,station), Form("Arm %i Station %i",arm,station));
			
			gxMaxAvgEfficiencyPeakFitted[pair<int,int>(arm,station)] = new TGraph();
			gxMaxAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetNameTitle(Form("gxMaxAvgEfficiencyPeakFitted_arm%i_st%i",arm,station), Form("Arm %i Station %i Fitted",arm,station));
			gxMaxAvgEfficiencyPeakFix[pair<int,int>(arm,station)] = new TGraph();
			gxMaxAvgEfficiencyPeakFix[pair<int,int>(arm,station)]->SetNameTitle(Form("gxMaxAvgEfficiencyPeakFix_arm%i_st%i",arm,station), Form("Arm %i Station %i Fix",arm,station));
			
			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)] = new TGraph();
			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetNameTitle(Form("gPixelsUsedForAvgEfficiencyPeakFitted_arm%i_st%i",arm,station), Form("Arm %i Station %i",arm,station));

			for(auto & run : runs){
				if(run == 315840 && arm == 0 && station == 0) continue; // Exclude the run where the pot wasn't inserted
				if(missingRuns[run]) continue;
				if(firstRun == 0) firstRun = run;
				gAvgEfficiencyPeak[pair<int,int>(arm,station)]->SetPoint(gAvgEfficiencyPeak[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],avgEfficiencyPeak[pair<int,int>(arm,station)][run]);
				gRelativeAvgEfficiencyPeak[pair<int,int>(arm,station)]->SetPoint(gRelativeAvgEfficiencyPeak[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],avgEfficiencyPeak[pair<int,int>(arm,station)][run]/avgEfficiencyPeak[pair<int,int>(arm,station)][firstRun]);
				
				gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],avgEfficiencyPeakFitted[pair<int,int>(arm,station)][run]);
				gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],avgEfficiencyPeakFitted[pair<int,int>(arm,station)][run]/avgEfficiencyPeakFitted[pair<int,int>(arm,station)][firstRun]);
			
				gxMaxAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gxMaxAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],xMaxAvgEfficiencyPeakFitted[pair<int,int>(arm,station)][run]);
				gxMaxAvgEfficiencyPeakFix[pair<int,int>(arm,station)]->SetPoint(gxMaxAvgEfficiencyPeakFix[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],xMaxAvgEfficiencyPeakFix[pair<int,int>(arm,station)][run]);
				gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetPoint(gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->GetN(),runLumiMap[run],pixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)][run]);
			
				yFitFunctions[pair<int,int>(arm,station)][run] = new TF1(Form("Run%i_arm%i_st%i",run,arm,station),"pol1",4,7.5);
				yFitFunctions[pair<int,int>(arm,station)][run]->SetParameters(yParam_0[pair<int,int>(arm,station)][run],yParam_1[pair<int,int>(arm,station)][run]); 
				gYFits[pair<int,int>(arm,station)][run] = new TGraph(yFitFunctions[pair<int,int>(arm,station)][run]);
				gYFits[pair<int,int>(arm,station)][run]->SetNameTitle(Form("Run%i_arm%i_st%i",run,arm,station), Form("Run %i Arm %i Station %i",run,arm,station));
			
				sigmaFitFunctions[pair<int,int>(arm,station)][run] = new TF1(Form("Run%i_arm%i_st%i",run,arm,station),"pol1",4,7.5);
				sigmaFitFunctions[pair<int,int>(arm,station)][run]->SetParameters(sigmaParam_0[pair<int,int>(arm,station)][run],sigmaParam_1[pair<int,int>(arm,station)][run]); 
				gSigmaFits[pair<int,int>(arm,station)][run] = new TGraph(sigmaFitFunctions[pair<int,int>(arm,station)][run]);
				gSigmaFits[pair<int,int>(arm,station)][run]->SetNameTitle(Form("Run%i_arm%i_st%i",run,arm,station), Form("Run %i Arm %i Station %i",run,arm,station));
			
				if(arm == 0 && station == 2){
					mgYFits->Add(gYFits[pair<int,int>(arm,station)][run],"L");
					mgSigmaFits->Add(gSigmaFits[pair<int,int>(arm,station)][run],"L");
				}
			}

			//Graph beauty farm
			gAvgEfficiencyPeak[pair<int,int>(arm,station)]->SetLineWidth(2);
			gAvgEfficiencyPeak[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);
			gRelativeAvgEfficiencyPeak[pair<int,int>(arm,station)]->SetLineWidth(2);
			gRelativeAvgEfficiencyPeak[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);


			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetLineWidth(2);
			gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetLineWidth(2);
			gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);

			gxMaxAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetLineWidth(2);
			gxMaxAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);
			gxMaxAvgEfficiencyPeakFix[pair<int,int>(arm,station)]->SetLineWidth(2);
			gxMaxAvgEfficiencyPeakFix[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);

			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetLineWidth(2);
			gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)]->SetMarkerStyle(kFullSquare);

			// Adding to multigraph
			mgAvgEfficiencyPeak->Add(gAvgEfficiencyPeak[pair<int,int>(arm,station)],"PL");
			mgRelativeAvgEfficiencyPeak->Add(gRelativeAvgEfficiencyPeak[pair<int,int>(arm,station)],"PL");

			mgAvgEfficiencyPeakFitted->Add(gAvgEfficiencyPeakFitted[pair<int,int>(arm,station)],"PL");
			mgRelativeAvgEfficiencyPeakFitted->Add(gRelativeAvgEfficiencyPeakFitted[pair<int,int>(arm,station)],"PL");

			mgxMaxAvgEfficiencyPeakFitted->Add(gxMaxAvgEfficiencyPeakFitted[pair<int,int>(arm,station)],"PL");
			mgxMaxAvgEfficiencyPeakFitted->Add(gxMaxAvgEfficiencyPeakFix[pair<int,int>(arm,station)],"PL");

			mgPixelsUsedForAvgEfficiencyPeakFitted->Add(gPixelsUsedForAvgEfficiencyPeakFitted[pair<int,int>(arm,station)],"PL");
		}
	}

	TCanvas *cAvgEfficiencyPeak = new TCanvas("cAvgEfficiencyPeak","AvgEfficiencyPeak");
	mgAvgEfficiencyPeak->Draw("A pmc plc");
	cAvgEfficiencyPeak->BuildLegend(0.6,0.1,0.9,0.3);
	TCanvas *cRelativeAvgEfficiencyPeak = new TCanvas("cRelativeAvgEfficiencyPeak","RelativeAvgEfficiencyPeak");
	mgRelativeAvgEfficiencyPeak->Draw("A pmc plc");
	cRelativeAvgEfficiencyPeak->BuildLegend(0.6,0.1,0.9,0.3);

	TCanvas *cAvgEfficiencyPeakFitted = new TCanvas("cAvgEfficiencyPeakFitted","AvgEfficiencyPeakFitted");
	mgAvgEfficiencyPeakFitted->Draw("A pmc plc");
	cAvgEfficiencyPeakFitted->BuildLegend(0.6,0.1,0.9,0.3);
	TCanvas *cRelativeAvgEfficiencyPeakFitted = new TCanvas("cRelativeAvgEfficiencyPeakFitted","RelativeAvgEfficiencyPeakFitted");
	mgRelativeAvgEfficiencyPeakFitted->Draw("A pmc plc");
	cRelativeAvgEfficiencyPeakFitted->BuildLegend(0.6,0.1,0.9,0.3);

	TCanvas *cxMaxAvgEfficiencyPeakFitted = new TCanvas("cxMaxAvgEfficiencyPeakFitted","xMaxAvgEfficiencyPeakFitted");
	mgxMaxAvgEfficiencyPeakFitted->Draw("A pmc plc");
	cxMaxAvgEfficiencyPeakFitted->BuildLegend(0.6,0.1,0.9,0.3);
	TCanvas *cPixelsUsedForAvgEfficiencyPeakFitted = new TCanvas("cPixelsUsedForAvgEfficiencyPeakFitted","PixelsUsedForAvgEfficiencyPeakFitted");
	mgPixelsUsedForAvgEfficiencyPeakFitted->Draw("A pmc plc");
	cPixelsUsedForAvgEfficiencyPeakFitted->BuildLegend(0.6,0.1,0.9,0.3);

	TCanvas *cYFits = new TCanvas("cYFits","YFits");
	mgYFits->Draw("A plc");
	cYFits->BuildLegend(0.6,0.1,0.9,0.3);
	TCanvas *cSigmaFits = new TCanvas("cSigmaFits","SigmaFits");
	mgSigmaFits->Draw("A plc");
	cSigmaFits->BuildLegend(0.6,0.1,0.9,0.3);
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