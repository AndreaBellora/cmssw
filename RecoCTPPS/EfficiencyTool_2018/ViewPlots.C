#include <algorithm>
#include <TH2D.h>
#include <TFile.h>

void ViewPlots(Int_t RunNumber){
	Int_t  screen_x, screen_y;
	UInt_t screen_w, screen_h;
	gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),screen_x,screen_y,screen_w,screen_h);
	gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),screen_x,screen_y,screen_w,screen_h);//Black magic requires to do this twice
	
	
	// cout<<screen_x<<" "<<screen_y<<" "<<screen_w<<" "<<screen_h<<endl;

	TFile* inputFile = new TFile(Form("OutputFiles/Run%i.root",RunNumber));

	TCanvas *cPixelHitmap = new TCanvas("cPixelHitmap","PixelHitmap",screen_x,screen_y,screen_w,screen_h);
	TCanvas *cPixelTrackEfficiency = new TCanvas("cPixelTrackEfficiency","PixelTrackEfficiency",screen_x,screen_y,screen_w,screen_h);
	TCanvas *cPixelInterPotEfficiency = new TCanvas("cPixelInterPotEfficiency","PixelInterPotEfficiency",screen_x,screen_y,screen_w,screen_h);
	TCanvas *cPlaneEfficiency = new TCanvas("cPlaneEfficiency","PlaneEfficiency",screen_x,screen_y,screen_w,screen_h);
	TCanvas *cCorrelation = new TCanvas("cCorrelation","Correlation",screen_x,screen_y,screen_w,screen_h);

	cPixelHitmap->Divide(2,2);
	cPixelTrackEfficiency->Divide(2,2);
	cPixelInterPotEfficiency->Divide(2,2);
	cCorrelation->Divide(2,2);
	cPlaneEfficiency->Divide(6,4);

	vector<Int_t> planes = {0,1,2,3,4,5};
	vector<Int_t> arms = {0,1};
	vector<Int_t> stations = {0,2};

	Int_t mapPlotsPadNumber = 0;
	Int_t correlationPlotsPadNumber = 0;
	Int_t planePlotsPadNumber = 0;

	for (const auto & arm : arms){
		for (const auto & station : stations){
			mapPlotsPadNumber++;
			cPixelHitmap->cd(mapPlotsPadNumber);
			((TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2TrackHitDistribution_arm%i_st%i_rp3",arm,station)))->Draw("colz");
			cPixelTrackEfficiency->cd(mapPlotsPadNumber);
			((TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2TrackEfficiencyMap_arm%i_st%i_rp3",arm,station)))->Draw("colz");
			cPixelInterPotEfficiency->cd(mapPlotsPadNumber);
			((TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->Get(Form("h2BetterInterPotEfficiency_arm%i_st%i_rp3",arm,station)))->Draw("colz");
			for (const auto & plane : planes){
				planePlotsPadNumber++;
				cPlaneEfficiency->cd(planePlotsPadNumber);
				((TH2D*)inputFile->GetDirectory(Form("Arm%i_st%i_rp3",arm,station))->GetDirectory(Form("Arm%i_st%i_rp3_pl%i",arm,station,plane))->Get(Form("h2EfficiencyMap_arm%i_st%i_rp3_pl%i",arm,station,plane)))->Draw("colz");
			}
		}
		correlationPlotsPadNumber++;
		cCorrelation->cd(correlationPlotsPadNumber);
		((TH2D*)inputFile->GetDirectory(Form("Arm%i_st0_rp3",arm))->Get(Form("h2X0Correlation_arm%i_st0_rp3",arm)))->Draw("colz");
		correlationPlotsPadNumber++;
		cCorrelation->cd(correlationPlotsPadNumber);
		((TH2D*)inputFile->GetDirectory(Form("Arm%i_st0_rp3",arm))->Get(Form("h2Y0Correlation_arm%i_st0_rp3",arm)))->Draw("colz");
	}
}