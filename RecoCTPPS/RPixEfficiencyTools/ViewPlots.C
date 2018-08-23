#include <algorithm>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>

void ViewPlots(Int_t RunNumber){
	gStyle->SetPalette(1);

	Int_t  screen_x=0, screen_y=0;
	UInt_t screen_w=1920, screen_h=1080;

	// cout<<screen_x<<" "<<screen_y<<" "<<screen_w<<" "<<screen_h<<endl;


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

	Int_t mapPlotsPadNumber = 0;
	Int_t correlationPlotsPadNumber = 0;
	Int_t planePlotsPadNumber = 0;

	for (const auto & arm : arms){
		for (const auto & station : stations){
			mapPlotsPadNumber++;
			cPixelHitmap->cd(mapPlotsPadNumber);
			((TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2TrackHitDistribution_arm%i_st%i_rp3",arm,station)))->DrawCopy("colz");
			cPixelTrackEfficiency->cd(mapPlotsPadNumber);
			((TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2TrackEfficiencyMap_arm%i_st%i_rp3",arm,station)))->DrawCopy("colz");
			cPixelInterPotEfficiency->cd(mapPlotsPadNumber);
			((TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2BetterInterPotEfficiency_arm%i_st%i_rp3",arm,station)))->DrawCopy("colz");
			for (const auto & plane : planes){
				planePlotsPadNumber++;
				cPlaneEfficiency->cd(planePlotsPadNumber);
				((TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->GetDirectory(Form("Arm%i_st%i_rp3_pl%i",arm,station,plane))->Get(Form("h2EfficiencyMap_arm%i_st%i_rp3_pl%i",arm,station,plane)))->DrawCopy("colz");
			}
		}
		correlationPlotsPadNumber++;
		cCorrelation->cd(correlationPlotsPadNumber);
		((TH2D*)inputFile->GetDirectory(Form("Arm%i_st0_rp3",arm))->Get(Form("h2X0Correlation_arm%i_st0_rp3",arm)))->DrawCopy("colz");
		correlationPlotsPadNumber++;
		cCorrelation->cd(correlationPlotsPadNumber);
		((TH2D*)inputFile->GetDirectory(Form("Arm%i_st0_rp3",arm))->Get(Form("h2Y0Correlation_arm%i_st0_rp3",arm)))->DrawCopy("colz");
	}
	inputFile->Close();
	delete inputFile;
}