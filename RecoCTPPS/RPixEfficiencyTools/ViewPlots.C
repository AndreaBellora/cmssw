#include <algorithm>
#include <fstream>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TString.h>

void SavePlotWithName(TH2D* h2, TString s);

void ViewPlots(Int_t RunNumber){
	gStyle->SetPalette(1);
	gStyle->SetOptStat(0);

	Int_t  screen_x=0, screen_y=0;
	UInt_t screen_w=1920, screen_h=1080;

	// cout<<screen_x<<" "<<screen_y<<" "<<screen_w<<" "<<screen_h<<endl;

	Int_t firstRunOfTheYear = 314247;
	Int_t lastRunPreTs1     = 317696;
	Int_t lastRunPreTs2     = 322633;
	Int_t lastRunOfTheYear  = 324897;

	// This part defines the area in which the track efficiency is going to be averaged. For every RP it will be computed on a rectangular region going from
	// (xbin, ybin-hbin) to (xbin+wbin, ybin+hbin).
	// vector<Int_t> areaForAvgEfficiency_armX_stX{xbin,ybin,hbin,wbin};
	std::vector<Int_t> areaForAvgEfficiency_arm0_st0{26,170,2,10};
	std::vector<Int_t> areaForAvgEfficiency_arm0_st2{22,156,2,10};
	std::vector<Int_t> areaForAvgEfficiency_arm1_st0{19,169,4,10};
	std::vector<Int_t> areaForAvgEfficiency_arm1_st2{24,155,4,10};

	bool Pre_TS1 = kFALSE;
	bool TS1_TS2 = kFALSE;
	bool Post_TS2 = kFALSE;
	Int_t shift = 0;

	if (RunNumber < firstRunOfTheYear){
		std::cout << "This run doesn't belong to 2018 data taking!" << std::endl;
		return;
	}
	if(RunNumber > firstRunOfTheYear && RunNumber <= lastRunPreTs1){
		std::cout << "This run belongs to Pre-TS1 data taking" << std::endl;
		Pre_TS1 = kTRUE;
	}
	if(RunNumber > lastRunPreTs1 && RunNumber <= lastRunPreTs2){
		std::cout << "This run belongs to the period of data taking beween TS1 and TS2" << std::endl;
		TS1_TS2 = kTRUE;
	}
	if(RunNumber > lastRunPreTs2 && RunNumber <= lastRunOfTheYear){
		std::cout << "This run belongs to Post-TS2 data taking" << std::endl;
		Post_TS2 = kTRUE;
	}
	if(RunNumber > lastRunOfTheYear){
		std::cout << "This run doesn't belong to 2018 data taking!" << std::endl;
		return;
	}

	if(TS1_TS2) shift = 5;
	if(Post_TS2) shift = 10;

	// areaForAvgEfficiency_arm0_st0[1] -= shift;
	// areaForAvgEfficiency_arm1_st0[1] -= shift;
	// areaForAvgEfficiency_arm0_st2[1] += shift;
	// areaForAvgEfficiency_arm1_st2[1] += shift;


	TCanvas *cPixelHitmap = new TCanvas("cPixelHitmap","PixelHitmap",screen_x,screen_y,screen_w,screen_h);
	TCanvas *cPixelTrackEfficiency = new TCanvas("cPixelTrackEfficiency","PixelTrackEfficiency",screen_x,screen_y,screen_w,screen_h);
	TCanvas *cPixelInterPotEfficiency = new TCanvas("cPixelInterPotEfficiency","PixelInterPotEfficiency",screen_x,screen_y,screen_w,screen_h);
	TCanvas *cPlaneEfficiency = new TCanvas("cPlaneEfficiency","PlaneEfficiency",screen_x,screen_y,screen_w,screen_h);
	TCanvas *cCorrelation = new TCanvas("cCorrelation","Correlation",screen_x,screen_y,screen_w,screen_h);

	TFile* inputFile = new TFile(Form("OutputFiles/Run%i.root",RunNumber), "UPDATE");
	cPixelHitmap->Divide(2,2);
	cPixelTrackEfficiency->Divide(2,2);
	cPixelInterPotEfficiency->Divide(2,2);
	cCorrelation->Divide(2,2);
	cPlaneEfficiency->Divide(6,4);

	std::vector<Int_t> planes{0,1,2,3,4,5};
	std::vector<Int_t> arms{0,1};
	std::vector<Int_t> stations{0,2};

	std::map <std::pair<Int_t,Int_t>,std::vector<Int_t> >areaForAvgEfficiency;
	areaForAvgEfficiency[std::pair<Int_t,Int_t>(0,0)] = areaForAvgEfficiency_arm0_st0;
	areaForAvgEfficiency[std::pair<Int_t,Int_t>(0,2)] = areaForAvgEfficiency_arm0_st2;
	areaForAvgEfficiency[std::pair<Int_t,Int_t>(1,0)] = areaForAvgEfficiency_arm1_st0;
	areaForAvgEfficiency[std::pair<Int_t,Int_t>(1,2)] = areaForAvgEfficiency_arm1_st2;

	ofstream avgEfficiencyOutputFile;
	avgEfficiencyOutputFile.open(Form("OutputFiles/avgEfficiency_Run%i.dat",RunNumber));
	Int_t mapPlotsPadNumber = 0;
	Int_t correlationPlotsPadNumber = 0;
	Int_t planePlotsPadNumber = 0;

	for (const auto & arm : arms){
		for (const auto & station : stations){
			mapPlotsPadNumber++;
			cPixelHitmap->cd(mapPlotsPadNumber);
			TH2D* h2PixelHitmap = (TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2TrackHitDistribution_arm%i_st%i_rp3",arm,station));
			h2PixelHitmap->SetNameTitle(Form("TrackHitDistribution_arm%i_st%i",arm,station),Form("TrackHitDistribution_arm%i_st%i",arm,station));
			h2PixelHitmap->DrawCopy("colz");

			cPixelInterPotEfficiency->cd(mapPlotsPadNumber);
			TH2D* h2InterPotEfficiency = (TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2BetterInterPotEfficiency_arm%i_st%i_rp3",arm,station));
			h2InterPotEfficiency->SetNameTitle(Form("InterPotEfficiency_arm%i_st%i",arm,station),Form("InterPotEfficiency_arm%i_st%i",arm,station));
			h2InterPotEfficiency->DrawCopy("colz");
			SavePlotWithName(h2InterPotEfficiency,Form("OutputFiles/PlotsRun%i/Run%i_InterPotEfficiency_arm%i_st%i.png",RunNumber,RunNumber,arm,station));

			cPixelTrackEfficiency->cd(mapPlotsPadNumber);
			TH2D* h2TrackEfficiencyMap = (TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2TrackEfficiencyMap_arm%i_st%i_rp3",arm,station));
			h2TrackEfficiencyMap->SetNameTitle(Form("TrackEfficiencyMap_arm%i_st%i",arm,station),Form("TrackEfficiencyMap_arm%i_st%i",arm,station));
			h2TrackEfficiencyMap->DrawCopy("colz");
			SavePlotWithName(h2TrackEfficiencyMap,Form("OutputFiles/PlotsRun%i/Run%i_TrackEfficiency_arm%i_st%i.png",RunNumber,RunNumber,arm,station));

			Double_t avgEfficiency = 0;
			Int_t counter=0;
			Int_t xbin = areaForAvgEfficiency[std::pair<Int_t,Int_t>(arm,station)].at(0);
			Int_t ybin = areaForAvgEfficiency[std::pair<Int_t,Int_t>(arm,station)].at(1);
			Int_t hbin = areaForAvgEfficiency[std::pair<Int_t,Int_t>(arm,station)].at(2);
			Int_t wbin = areaForAvgEfficiency[std::pair<Int_t,Int_t>(arm,station)].at(3);
			for(Int_t xPixel = xbin; xPixel <= xbin+wbin; xPixel++){
				for(Int_t yPixel = ybin-hbin; yPixel<=ybin+hbin; yPixel++){
					++counter;
					avgEfficiency+=h2TrackEfficiencyMap->GetBinContent(xPixel,yPixel);
				}
			}
			avgEfficiency = avgEfficiency/((Double_t)counter);
			std::cout << "The average efficiency on arm" << arm << "_station" << station << " is:" << " " << avgEfficiency << std::endl;
			avgEfficiencyOutputFile << arm << " " << station << " " << avgEfficiency << "\n";
			for (const auto & plane : planes){
				planePlotsPadNumber++;
				cPlaneEfficiency->cd(planePlotsPadNumber);
				TH2D* h2PlaneEfficiency = (TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->GetDirectory(Form("Arm%i_st%i_rp3_pl%i",arm,station,plane))->Get(Form("h2EfficiencyMap_arm%i_st%i_rp3_pl%i",arm,station,plane));
				h2PlaneEfficiency->SetNameTitle(Form("EfficiencyMap_arm%i_st%i_pl%i",arm,station,plane),Form("EfficiencyMap_arm%i_st%i_pl%i",arm,station,plane));
				h2PlaneEfficiency->DrawCopy("colz");
			}
		}
		correlationPlotsPadNumber++;
		cCorrelation->cd(correlationPlotsPadNumber);
		TH2D* h2X0Correlation = (TH2D*)inputFile->GetDirectory(Form("Arm%i_st0_rp3",arm))->Get(Form("h2X0Correlation_arm%i_st0_rp3",arm));
		h2X0Correlation->SetNameTitle(Form("XCorrelation_arm%i",arm),Form("XCorrelation_arm%i",arm));
		h2X0Correlation->DrawCopy("colz");
		correlationPlotsPadNumber++;
		cCorrelation->cd(correlationPlotsPadNumber);
		TH2D* h2Y0Correlation = (TH2D*)inputFile->GetDirectory(Form("Arm%i_st0_rp3",arm))->Get(Form("h2Y0Correlation_arm%i_st0_rp3",arm));
		h2Y0Correlation->SetNameTitle(Form("YCorrelation_arm%i",arm),Form("YCorrelation_arm%i",arm));
		h2Y0Correlation->DrawCopy("colz");

		gROOT->SetBatch(kTRUE);
		TCanvas* cCorrelationToSave = new TCanvas("cCorrelationToSave","Correlation Plots",screen_x,screen_y,screen_w,screen_h);
		cCorrelationToSave->Divide(2);
		cCorrelationToSave->cd(1);
		h2X0Correlation->DrawCopy("colz");
		cCorrelationToSave->cd(2);
		h2Y0Correlation->DrawCopy("colz");
		cCorrelationToSave->SaveAs(Form("OutputFiles/PlotsRun%i/Run%i_Correlation_arm%i.png",RunNumber,RunNumber,arm));
		delete cCorrelationToSave;
		gROOT->SetBatch(kFALSE);
	}
	avgEfficiencyOutputFile.close();
	std::cout << "Average efficiency data saved in " << Form("OutputFiles/avgEfficiency_Run%i.dat",RunNumber) << std::endl;
	inputFile->Close();
	delete inputFile;
}

void SavePlotWithName(TH2D* h2, TString s){
	Int_t  screen_x=0, screen_y=0;
	UInt_t screen_w=1920, screen_h=1080;
	gROOT->SetBatch(kTRUE);
	TCanvas* c = new TCanvas("","",screen_x,screen_y,screen_w,screen_h);
	h2->DrawCopy("colz");
	c->SaveAs(s);
	delete c;
	gROOT->SetBatch(kFALSE);
}