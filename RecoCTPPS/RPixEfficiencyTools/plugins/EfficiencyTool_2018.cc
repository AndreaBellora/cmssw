// -*- C++ -*-
//
// Package:    RecoCTPPS/RPixEfficiencyTools
// Class:      RPixEfficiencyTools
//
/**\class EfficiencyTool_2018 EfficiencyTool_2018.cc RecoCTPPS/RPixEfficiencyTools/plugins/EfficiencyTool_2018.cc

 Description: [one line class summary]

 Implementation:
		 [Notes on implementation]
*/
//
// Original Author:  Andrea Bellora
//         Created:  Wed, 22 Aug 2018 09:55:05 GMT
//
//


#include <memory>
#include <algorithm>
#include <fstream>
#include <exception>
#include <boost/algorithm/string.hpp>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelRecHit.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSPixelDetId.h"

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TGraphErrors.h>
#include <TF1.h>

class EfficiencyTool_2018 : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
	public:
	explicit EfficiencyTool_2018(const edm::ParameterSet&);
	~EfficiencyTool_2018();
	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

	private:
	virtual void beginJob() override;
	virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
	virtual void endJob() override;
	float probabilityNplanesBlind(const std::vector<uint32_t> &inputPlaneList, int numberToExtract, 
	const std::map<unsigned, float> &planeEfficiency);
	void getPlaneCombinations(const std::vector<uint32_t> &inputPlaneList, uint32_t numberToExtract, 
	std::vector<std::pair<std::vector<uint32_t>,std::vector<uint32_t> > > &planesExtractedAndNot);
	float probabilityCalculation(const std::map<uint32_t, float> &planeEfficiency);
	float errorCalculation(const std::map<uint32_t, float> &planeEfficiency,
	const std::map<uint32_t, float> &planeEfficiencyError);
	float efficiencyPartialDerivativewrtPlane(uint32_t plane, const std::vector<uint32_t> &inputPlaneList,
	int numberToExtract, const std::map<unsigned, float> &planeEfficiency);

	edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelLocalTrack> >  pixelLocalTrackToken_;
	edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelRecHit> >      pixelRecHitToken_;
	
	TFile *outputFile_;
	std::string outputFileName_;
	bool isCorrelationPlotEnabled;
	bool supplementaryPlots;
	int minNumberOfPlanesForEfficiency_;
	int minNumberOfPlanesForTrack_;
	int minTracksPerEvent;
	int maxTracksPerEvent;

	static const unsigned int totalNumberOfBunches_ = 3564;
	std::string bunchSelection_;
	std::string bunchListFileName_;
	bool validBunchArray_[totalNumberOfBunches_];

	TH1D *h1BunchCrossing_;
	std::map<CTPPSPixelDetId,TH2D*> h2ModuleHitMap_;
	std::map<CTPPSPixelDetId,TH2D*> h2EfficiencyMap_;
	std::map<CTPPSPixelDetId,TH2D*> h2AuxEfficiencyMap_;
	std::map<CTPPSPixelDetId,TH2D*> h2EfficiencyNormalizationMap_;
	std::map<CTPPSPixelDetId,TH2D*> h2TrackHitDistribution_;
	std::map<int,std::map<CTPPSPixelDetId,TH2D*> > h2TrackHitDistributionBinShift_;
	std::map<CTPPSPixelDetId,TH2D*> h23PointsTrackHitDistribution_;
	std::map<CTPPSPixelDetId,TH2D*> h2TrackEfficiencyMap_;
	std::map<CTPPSPixelDetId,TH2D*> h2TrackEfficiencyErrorMap_;
	std::map<CTPPSPixelDetId,TH1D*> h1NumberOfTracks_;
	std::map<CTPPSPixelDetId,TGraph*> g1X0Correlation_;
	std::map<CTPPSPixelDetId,TGraph*> g1Y0Correlation_;
	std::map<CTPPSPixelDetId,TH2D*> h2X0Correlation_;
	std::map<CTPPSPixelDetId,TH2D*> h2Y0Correlation_;
	std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiency_;
	std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiencyError_;
	std::map<CTPPSPixelDetId,TH2D*> h2BetterInterPotEfficiency_;
	std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiencyNormalization_;
	std::map<CTPPSPixelDetId,TH2D*> h2AvgPlanesUsed_;
	std::map<CTPPSPixelDetId,TH1D*> h1PlanesUsed_;
	std::map<CTPPSPixelDetId,TH1D*> h1ChiSquaredOverNDF_;

	std::map<CTPPSPixelDetId,TH2D*> h2ModuleHitMap_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2EfficiencyMap_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2AuxEfficiencyMap_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2EfficiencyNormalizationMap_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2TrackHitDistribution_rotated;
	std::map<int,std::map<CTPPSPixelDetId,TH2D*> > h2TrackHitDistributionBinShift_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2TrackEfficiencyMap_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2TrackEfficiencyErrorMap_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiency_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiencyError_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2BetterInterPotEfficiency_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiencyNormalization_rotated;
	std::map<CTPPSPixelDetId,TH2D*> h2AvgPlanesUsed_rotated;

	// Resolution histograms
	std::map<CTPPSPixelDetId,std::map<std::pair<int,int>,TH1D*> > h1X0Sigma; // map<DetId,map< pair< PlanesUsedForFit, PlanesWithColCls >, Sigma > >
	std::map<CTPPSPixelDetId,std::map<std::pair<int,int>,TH1D*> > h1Y0Sigma; // map<DetId,map< pair< PlanesUsedForFit, PlanesWithColCls >, Sigma > >
	std::map<CTPPSPixelDetId,std::map<std::pair<int,int>,TH1D*> > h1TxSigma; // map<DetId,map< pair< PlanesUsedForFit, PlanesWithColCls >, Sigma > >
	std::map<CTPPSPixelDetId,std::map<std::pair<int,int>,TH1D*> > h1TySigma; // map<DetId,map< pair< PlanesUsedForFit, PlanesWithColCls >, Sigma > >

	std::vector<CTPPSPixelDetId> detectorIdVector_;
	std::vector<CTPPSPixelDetId> romanPotIdVector_;

	std::vector<double> correlationCoefficients;
	std::vector<double> correlationConstants;
	std::vector<double> correlationTolerances;

	std::vector<uint32_t> listOfPlanes_ = {0,1,2,3,4,5};
	std::vector<int> binShifts_ = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140};

	int interPotEffMinTracksStart = 0;
	int interPotEffMaxTracksStart = 99;
	int binGroupingX = 1;
	int binGroupingY = 1;

	int   mapXbins_st2 = 200/binGroupingX ;
	float mapXmin_st2  =  43.*TMath::Cos(18.4/180.*TMath::Pi());
	float mapXmax_st2  =  73.*TMath::Cos(18.4/180.*TMath::Pi());
	int   mapYbins_st2 = 240/binGroupingY ;
	float mapYmin_st2  = -12.;
	float mapYmax_st2  =  12.;
	float fitXmin_st2  = 6.;
	float fitXmax_st2  = 19.;

	int   mapXbins_st0 = 200/binGroupingX ;
	float mapXmin_st0  =  3.*TMath::Cos(18.4/180.*TMath::Pi());
	float mapXmax_st0  =  33.*TMath::Cos(18.4/180.*TMath::Pi());
	int   mapYbins_st0 = 240/binGroupingY ;
	float mapYmin_st0  = -12.;
	float mapYmax_st0  =  12.;
	float fitXmin_st0  = 45.;
	float fitXmax_st0  = 58.;

	int   mapXbins = mapXbins_st0;
	float mapXmin = mapXmin_st0;
	float mapXmax = mapXmax_st0;
	int   mapYbins = mapYbins_st0;
	float mapYmin = mapYmin_st0;
	float mapYmax = mapYmax_st0;
	float fitXmin = fitXmin_st0;
	float fitXmax = fitXmax_st0;

	std::map<CTPPSPixelDetId,int > binAlignmentParameters = {
		{CTPPSPixelDetId(0,0,3), 50},
		{CTPPSPixelDetId(0,2,3), 140},
		{CTPPSPixelDetId(1,0,3), 50},
		{CTPPSPixelDetId(1,2,3), 80}
	};

	int numberOfOvermatches = 0;
	int numberOfAttempts = 0;
};

EfficiencyTool_2018::EfficiencyTool_2018(const edm::ParameterSet& iConfig){
	usesResource("TFileService");
	pixelLocalTrackToken_ = consumes<edm::DetSetVector<CTPPSPixelLocalTrack> >(edm::InputTag("ctppsPixelLocalTracks",""));
	pixelRecHitToken_     = consumes<edm::DetSetVector<CTPPSPixelRecHit> >(edm::InputTag("ctppsPixelRecHits",""));
	outputFileName_ = iConfig.getUntrackedParameter<std::string>("outputFileName");
	minNumberOfPlanesForEfficiency_ = iConfig.getParameter<int>("minNumberOfPlanesForEfficiency");
	minNumberOfPlanesForTrack_ = iConfig.getParameter<int>("minNumberOfPlanesForTrack");
	isCorrelationPlotEnabled = iConfig.getParameter<bool>("isCorrelationPlotEnabled");
	correlationCoefficients = iConfig.getParameter<std::vector<double> >("correlationCoefficients");
	correlationConstants = iConfig.getParameter<std::vector<double> >("correlationConstants");
	correlationTolerances = iConfig.getParameter<std::vector<double> >("correlationTolerances");
	interPotEffMinTracksStart = iConfig.getUntrackedParameter<int>("interPotEffMinTracksStart");
	interPotEffMaxTracksStart = iConfig.getUntrackedParameter<int>("interPotEffMaxTracksStart");
	minTracksPerEvent = iConfig.getParameter<int>("minTracksPerEvent");
	maxTracksPerEvent = iConfig.getParameter<int>("maxTracksPerEvent");
	supplementaryPlots = iConfig.getParameter<bool>("supplementaryPlots");
	bunchSelection_ = iConfig.getUntrackedParameter<std::string>("bunchSelection");
	bunchListFileName_ = iConfig.getUntrackedParameter<std::string>("bunchListFileName");
	binGroupingX = iConfig.getUntrackedParameter<int>("binGroupingX");
	binGroupingY = iConfig.getUntrackedParameter<int>("binGroupingY");
}

EfficiencyTool_2018::~EfficiencyTool_2018(){
	delete h1BunchCrossing_;
	for(const auto & rpId :  romanPotIdVector_){
		delete h2TrackHitDistribution_[rpId];
		if(supplementaryPlots){
			for(auto binShift : binShifts_)	delete h2TrackHitDistributionBinShift_[binShift][rpId];
		}
		delete h23PointsTrackHitDistribution_[rpId];
		delete h2TrackEfficiencyMap_[rpId];
		delete h2TrackEfficiencyErrorMap_[rpId];
		delete h1NumberOfTracks_[rpId];
		delete g1X0Correlation_[rpId];
		delete g1Y0Correlation_[rpId];
		delete h2X0Correlation_[rpId];
		delete h2Y0Correlation_[rpId];
		delete h2InterPotEfficiency_[rpId];
		delete h2InterPotEfficiencyError_[rpId];
		delete h2BetterInterPotEfficiency_[rpId];
		delete h2InterPotEfficiencyNormalization_[rpId];

		if(supplementaryPlots){
			for(int nPlanes = 3; nPlanes <= 6; nPlanes++){
				for(int numberOfCls = 0; numberOfCls <= nPlanes; numberOfCls++){
					// std::cout << "Deleting hist " << nPlanes << " " << numberOfCls << std::endl;
					delete h1X0Sigma[rpId][std::pair(nPlanes,numberOfCls)];
					delete h1Y0Sigma[rpId][std::pair(nPlanes,numberOfCls)];
					delete h1TxSigma[rpId][std::pair(nPlanes,numberOfCls)];
					delete h1TySigma[rpId][std::pair(nPlanes,numberOfCls)];
				}
			}	
		delete h2AvgPlanesUsed_[rpId];
		delete h1PlanesUsed_[rpId];
		delete h1ChiSquaredOverNDF_[rpId];
		}

		if(rpId.station() == 0){
			delete h2TrackEfficiencyMap_rotated[rpId];
			delete h2TrackEfficiencyErrorMap_rotated[rpId];
			delete h2InterPotEfficiency_rotated[rpId];
			delete h2InterPotEfficiencyError_rotated[rpId];
			delete h2BetterInterPotEfficiency_rotated[rpId];
			delete h2InterPotEfficiencyNormalization_rotated[rpId];
			if(supplementaryPlots){
				delete h2TrackHitDistribution_rotated[rpId];
				for(auto binShift : binShifts_)	delete h2TrackHitDistributionBinShift_rotated[binShift][rpId];
				delete h2AvgPlanesUsed_rotated[rpId];
			}
		}
	}

	for(const auto & detId : detectorIdVector_){
		delete h2ModuleHitMap_[detId];
		delete h2EfficiencyNormalizationMap_[detId];
		delete h2EfficiencyMap_[detId];
		delete h2AuxEfficiencyMap_[detId];
		
		if(detId.station() == 0){
			delete h2EfficiencyNormalizationMap_rotated[detId];
			delete h2EfficiencyMap_rotated[detId];
			delete h2AuxEfficiencyMap_rotated[detId];
			if(supplementaryPlots){
				delete h2ModuleHitMap_rotated[detId];
			}
		}
	}
}

void EfficiencyTool_2018::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
	using namespace edm;
	Handle<edm::DetSetVector<CTPPSPixelRecHit> > pixelRecHits;
	iEvent.getByToken(pixelRecHitToken_, pixelRecHits);

	Handle<edm::DetSetVector<CTPPSPixelLocalTrack> > pixelLocalTracks;
	iEvent.getByToken(pixelLocalTrackToken_, pixelLocalTracks);

	if(!validBunchArray_[iEvent.eventAuxiliary().bunchCrossing()]) return;
	h1BunchCrossing_->Fill(iEvent.eventAuxiliary().bunchCrossing());
	
	for(const auto & rpPixeltrack : *pixelLocalTracks){
		if ((uint32_t)minTracksPerEvent > rpPixeltrack.size() || rpPixeltrack.size() > (uint32_t)maxTracksPerEvent) continue;
		CTPPSPixelDetId rpId = CTPPSPixelDetId(rpPixeltrack.detId());
		uint32_t arm = rpId.arm();
		uint32_t rp = rpId.rp();
		uint32_t station = rpId.station();

		if (station == 2){
			mapXbins = mapXbins_st2;
			mapXmin = mapXmin_st2;
			mapXmax = mapXmax_st2;
			mapYbins = mapYbins_st2;
			mapYmin = mapYmin_st2;
			mapYmax = mapYmax_st2;
			fitXmin = fitXmin_st2;
			fitXmax = fitXmax_st2;
		}
		else{
			mapXbins = mapXbins_st0;
			mapXmin = mapXmin_st0;
			mapXmax = mapXmax_st0;
			mapYbins = mapYbins_st0;
			mapYmin = mapYmin_st0;
			mapYmax = mapYmax_st0;
			fitXmin = fitXmin_st0;
			fitXmax = fitXmax_st0;      
		}

		// Shift Xmin and Xmax to align bins with sensor edge

		double binSize = (mapXmax - mapXmin)/mapXbins;
		mapXmin += binAlignmentParameters[rpId]*binSize/150.;
		mapXmax += binAlignmentParameters[rpId]*binSize/150.;

		if(h2TrackHitDistribution_.find(rpId)==h2TrackHitDistribution_.end()){
			romanPotIdVector_.push_back(rpId);
			h2TrackHitDistribution_[rpId] = new TH2D(Form("h2TrackHitDistribution_arm%i_st%i_rp%i",arm,station,rp),
			Form("h2TrackHitDistribution_arm%i_st%i_rp%i;x (mm);y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
			if(supplementaryPlots){
				for(auto binShift : binShifts_){
					h2TrackHitDistributionBinShift_[binShift][rpId] = new TH2D(Form("h2TrackHitDistributionBinShift_%i_arm%i_st%i_rp%i",binShift,arm,station,rp),
					Form("h2TrackHitDistributionBinShift_%i_arm%i_st%i_rp%i;x (mm);y (mm)",binShift,arm,station,rp),mapXbins,mapXmin+binShift*binSize/150.,mapXmax+binShift*binSize/150.,mapYbins,mapYmin,mapYmax);
				}	
			}
			h23PointsTrackHitDistribution_[rpId] = new TH2D(Form("h23PointsTrackHitDistribution_arm%i_st%i_rp%i",arm,station,rp),
			Form("h23PointsTrackHitDistribution_arm%i_st%i_rp%i;x (mm);y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
			h2TrackEfficiencyMap_[rpId] = new TH2D(Form("h2TrackEfficiencyMap_arm%i_st%i_rp%i",arm,station,rp),
			Form("h2TrackEfficiencyMap_arm%i_st%i_rp%i; x (mm); y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
			h2TrackEfficiencyErrorMap_[rpId] = new TH2D(Form("h2TrackEfficiencyErrorMap_arm%i_st%i_rp%i",arm,station,rp),
			Form("h2TrackEfficiencyErrorMap_arm%i_st%i_rp%i; x (mm); y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
			h1NumberOfTracks_[rpId] = new TH1D(Form("h1NumberOfTracks_arm%i_st%i_rp%i",arm,station,rp),
      		Form("h1NumberOfTracks_arm%i_st%i_rp%i; Tracks;",arm,station,rp),16,-0.5,15.5);
		    if(supplementaryPlots){
		    	h2AvgPlanesUsed_[rpId] = new TH2D(Form("h2AvgPlanesUsed_arm%i_st%i_rp%i",arm,station,rp),
				Form("h2AvgPlanesUsed_arm%i_st%i_rp%i; x (mm); y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
				h1PlanesUsed_[rpId] = new TH1D(Form("h1PlanesUsed_arm%i_st%i_rp%i",arm,station,rp),
				Form("h1PlanesUsed_arm%i_st%i_rp%i; Planes",arm,station,rp),7,-0.5,6.5);
				h1ChiSquaredOverNDF_[rpId] = new TH1D(Form("h1ChiSquaredOverNDF_arm%i_st%i_rp%i",arm,station,rp),
				Form("h1ChiSquaredOverNDF_arm%i_st%i_rp%i; Planes",arm,station,rp),100,0,5);
				for(int nPlanes = 3; nPlanes <= 6; nPlanes++){
					for(int numberOfCls = 0; numberOfCls <= nPlanes; numberOfCls++){
						// std::cout << "Creating hist " << nPlanes << " " << numberOfCls << "Arm " << arm << "Station " << station <<std::endl; 
						h1X0Sigma[rpId][std::pair(nPlanes,numberOfCls)] = new TH1D(Form("h1X0Sigma_arm%i_st%i_rp%i_nPlanes%i_nCls%i",arm,station,rp,nPlanes,numberOfCls),
						Form("h1X0Sigma_arm%i_st%i_rp%i_nPlanes%i_nCls%i; #sigma_{x} (mm);",arm,station,rp,nPlanes,numberOfCls),100,0,0.1);
						h1Y0Sigma[rpId][std::pair(nPlanes,numberOfCls)] = new TH1D(Form("h1Y0Sigma_arm%i_st%i_rp%i_nPlanes%i_nCls%i",arm,station,rp,nPlanes,numberOfCls),
						Form("h1Y0Sigma_arm%i_st%i_rp%i_nPlanes%i_nCls%i; #sigma_{y} (mm);",arm,station,rp,nPlanes,numberOfCls),100,0,0.1); 
						h1TxSigma[rpId][std::pair(nPlanes,numberOfCls)] = new TH1D(Form("h1TxSigma_arm%i_st%i_rp%i_nPlanes%i_nCls%i",arm,station,rp,nPlanes,numberOfCls),
						Form("h1TxSigma_arm%i_st%i_rp%i_nPlanes%i_nCls%i; #sigma_{Tx};",arm,station,rp,nPlanes,numberOfCls),100,0,0.02);
						h1TySigma[rpId][std::pair(nPlanes,numberOfCls)] = new TH1D(Form("h1TySigma_arm%i_st%i_rp%i_nPlanes%i_nCls%i",arm,station,rp,nPlanes,numberOfCls),
						Form("h1TySigma_arm%i_st%i_rp%i_nPlanes%i_nCls%i; #sigma_{Ty};",arm,station,rp,nPlanes,numberOfCls),100,0,0.02); 
		    		}
				}
		    }
		    if(station == 0){
				h2TrackEfficiencyMap_rotated[rpId] = new TH2D(Form("h2TrackEfficiencyMap_rotated_arm%i_st%i_rp%i",arm,station,rp),
				Form("h2TrackEfficiencyMap_rotated_arm%i_st%i_rp%i; x (mm); y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
				h2TrackEfficiencyErrorMap_rotated[rpId] = new TH2D(Form("h2TrackEfficiencyErrorMap_rotated_arm%i_st%i_rp%i",arm,station,rp),
				Form("h2TrackEfficiencyErrorMap_rotated_arm%i_st%i_rp%i; x (mm); y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
				if(supplementaryPlots){
					h2TrackHitDistribution_rotated[rpId] = new TH2D(Form("h2TrackHitDistribution_rotated_arm%i_st%i_rp%i",arm,station,rp),
					Form("h2TrackHitDistribution_rotated_arm%i_st%i_rp%i;x (mm);y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);	
					for(auto binShift : binShifts_){
						h2TrackHitDistributionBinShift_rotated[binShift][rpId] = new TH2D(Form("h2TrackHitDistributionBinShift_rotated_%i_arm%i_st%i_rp%i",binShift,arm,station,rp),
						Form("h2TrackHitDistributionBinShift_rotated_%i_arm%i_st%i_rp%i;x (mm);y (mm)",binShift,arm,station,rp),mapXbins,mapXmin+binShift*binSize/150,mapXmax+binShift*binSize/150,mapYbins,mapYmin,mapYmax);							
					}
			    	h2AvgPlanesUsed_rotated[rpId] = new TH2D(Form("h2AvgPlanesUsed_rotated_arm%i_st%i_rp%i",arm,station,rp),
					Form("h2AvgPlanesUsed_rotated_arm%i_st%i_rp%i; x (mm); y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
				}
			}
		}
	    h1NumberOfTracks_[rpId]->Fill(rpPixeltrack.size());
		for(const auto & pixeltrack : rpPixeltrack){
			if ((pixeltrack.getNDF() +4 )/2 < minNumberOfPlanesForTrack_) continue;
			float pixelX0 = pixeltrack.getX0();
			float pixelY0 = pixeltrack.getY0();
			int numberOfRowCls2 = 0;
			int numberOfColCls2 = 0;
			// Rotating St0 tracks
			float pixelX0_rotated = 0;
			float pixelY0_rotated = 0;
			if(station == 0){
				pixelX0_rotated = pixeltrack.getX0()*TMath::Cos((-8./180.)*TMath::Pi()) - pixeltrack.getY0()*TMath::Sin((-8./180.)*TMath::Pi());
				pixelY0_rotated = pixeltrack.getX0()*TMath::Sin((-8./180.)*TMath::Pi()) + pixeltrack.getY0()*TMath::Cos((-8./180.)*TMath::Pi());
			}
		
			int numberOfFittedPoints = 0;
			edm::DetSetVector<CTPPSPixelFittedRecHit> fittedHits = pixeltrack.getHits();

			std::map<uint32_t, int> numberOfPointPerPlaneEff;
			for(const auto pln : listOfPlanes_){
				numberOfPointPerPlaneEff[pln]=0;
			}

			for(const auto & planeHits : fittedHits){
				CTPPSPixelDetId planeId = CTPPSPixelDetId(planeHits.detId());
				if(h2ModuleHitMap_.find(planeId)==h2ModuleHitMap_.end()){
					uint32_t plane = planeId.plane();
					detectorIdVector_.push_back(planeId);

					h2ModuleHitMap_[planeId] = new TH2D(Form("h2ModuleHitMap_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),
					Form("h2ModuleHitMap_arm%i_st%i_rp%i_pl%i; x (mm); y (mm)",arm,station,rp,plane),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
					h2EfficiencyMap_[planeId] = new TH2D(Form("h2EfficiencyMap_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),
					Form("h2EfficiencyMap_arm%i_st%i_rp%i_pl%i; x (mm); y (mm)",arm,station,rp,plane),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
					h2AuxEfficiencyMap_[planeId] = new TH2D(Form("h2AuxEfficiencyMap_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),
					Form("h2AuxEfficiencyMap_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
					h2EfficiencyNormalizationMap_[planeId] = new TH2D(Form("h2EfficiencyNormalizationMap_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),
					Form("h2EfficiencyNormalizationMap_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
					if(station == 0){
						h2EfficiencyMap_rotated[planeId] = new TH2D(Form("h2EfficiencyMap_rotated_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),
						Form("h2EfficiencyMap_rotated_arm%i_st%i_rp%i_pl%i; x (mm); y (mm)",arm,station,rp,plane),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
						h2AuxEfficiencyMap_rotated[planeId] = new TH2D(Form("h2AuxEfficiencyMap_rotated_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),
						Form("h2AuxEfficiencyMap_rotated_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
						h2EfficiencyNormalizationMap_rotated[planeId] = new TH2D(Form("h2EfficiencyNormalizationMap_rotated_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),
						Form("h2EfficiencyNormalizationMap_rotated_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);	

						if(supplementaryPlots){
							h2ModuleHitMap_rotated[planeId] = new TH2D(Form("h2ModuleHitMap_rotated_arm%i_st%i_rp%i_pl%i",arm,station,rp,plane),
							Form("h2ModuleHitMap_rotated_arm%i_st%i_rp%i_pl%i; x (mm); y (mm)",arm,station,rp,plane),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
						}	
					}			
				}

				for(const auto & hit : planeHits){
					if(hit.getIsUsedForFit()){
						++numberOfFittedPoints;
						//Counting, for each plane, how many of the others were hit, to establish if its efficiency is computable in the event
						for(auto pln : numberOfPointPerPlaneEff){
							if(pln.first == planeId.plane()) continue;
							numberOfPointPerPlaneEff[pln.first] = pln.second + 1;
						}
						double hitX0 = hit.getGlobalCoordinates().x() + hit.getXResidual();
						double hitY0 = hit.getGlobalCoordinates().y() + hit.getYResidual();
						double hitX0_rotated = 0;
						double hitY0_rotated = 0;
						if(station == 0){
							hitX0_rotated = hitX0*TMath::Cos((-8./180.)*TMath::Pi()) - hitY0*TMath::Sin((-8./180.)*TMath::Pi());
							hitY0_rotated = hitX0*TMath::Sin((-8./180.)*TMath::Pi()) + hitY0*TMath::Cos((-8./180.)*TMath::Pi());
						}	
						h2ModuleHitMap_[planeId]->Fill(hitX0,hitY0);

						if(supplementaryPlots && station == 0){
							h2ModuleHitMap_rotated[planeId]->Fill(hitX0_rotated,hitY0_rotated);
						} 
						if(hit.clusterSizeRow() == 2) ++numberOfRowCls2;
						if(hit.clusterSizeCol() == 2) ++numberOfColCls2;
					}
				}
			}

			h2TrackHitDistribution_[rpId]->Fill(pixelX0,pixelY0);
			if(supplementaryPlots){
				for(auto binShift : binShifts_){
					h2TrackHitDistributionBinShift_[binShift][rpId]->Fill(pixelX0,pixelY0);
				}
			}

			if(supplementaryPlots){
				h2AvgPlanesUsed_[rpId]->Fill(pixelX0,pixelY0,numberOfFittedPoints);
				h1PlanesUsed_[rpId]->Fill(numberOfFittedPoints);
				h1ChiSquaredOverNDF_[rpId]->Fill(pixeltrack.getChiSquaredOverNDF());
				if(station == 0){
					h2AvgPlanesUsed_rotated[rpId]->Fill(pixelX0_rotated,pixelY0_rotated,numberOfFittedPoints);
					h2TrackHitDistribution_rotated[rpId]->Fill(pixelX0_rotated,pixelY0_rotated);			
					for(auto binShift : binShifts_){
						h2TrackHitDistributionBinShift_rotated[binShift][rpId]->Fill(pixelX0_rotated,pixelY0_rotated);
					}		
				}
			}
			if(numberOfFittedPoints == 3) {
				// For Run 324841, to estimate 3 planes tracks in the damaged region
				// if(arm == 0 && station == 2){
				// 	if(pixelX0 > 43.90 && pixelX0 < 45.48 && pixelY0 > 3.39 && pixelY0 < 4.59) h23PointsTrackHitDistribution_[rpId]->Fill(pixelX0,pixelY0);
				// }
				// else if(arm == 0 && station == 0){
				// 	if(pixelX0 > 6.18 && pixelX0 < 7.46 && pixelY0 > 4.36 && pixelY0 < 5.19) h23PointsTrackHitDistribution_[rpId]->Fill(pixelX0,pixelY0);
				// } 
				// else if(arm == 1 && station ==2){
				// 	if(pixelX0 > 44.15 && pixelX0 < 45.29 && pixelY0 > 2.28 && pixelY0 < 4.89) h23PointsTrackHitDistribution_[rpId]->Fill(pixelX0,pixelY0);
				// }
				// else if(arm == 1 && station == 0){
				// 	if(pixelX0 > 5.31 && pixelX0 < 6.31 && pixelY0 > 3.7 && pixelY0 < 5.40) h23PointsTrackHitDistribution_[rpId]->Fill(pixelX0,pixelY0);					
				// }
				h23PointsTrackHitDistribution_[rpId]->Fill(pixelX0,pixelY0); 
			}

			if(supplementaryPlots && pixeltrack.getChiSquaredOverNDF() < 2.){
				h1X0Sigma[rpId][std::pair(numberOfFittedPoints,numberOfColCls2)]->Fill(pixeltrack.getX0Sigma());
				h1Y0Sigma[rpId][std::pair(numberOfFittedPoints,numberOfRowCls2)]->Fill(pixeltrack.getY0Sigma());
				h1TxSigma[rpId][std::pair(numberOfFittedPoints,numberOfColCls2)]->Fill(pixeltrack.getTxSigma());
				h1TySigma[rpId][std::pair(numberOfFittedPoints,numberOfRowCls2)]->Fill(pixeltrack.getTySigma());
			}
			//Efficiency calculation
			for(const auto pln : listOfPlanes_){
				CTPPSPixelDetId planeId = rpId;
				planeId.setPlane(pln);
				edm::DetSet<CTPPSPixelFittedRecHit> hitOnPlane = fittedHits[planeId];
				float hitX0 = hitOnPlane[0].getGlobalCoordinates().x() + hitOnPlane[0].getXResidual();;
				float hitY0 = hitOnPlane[0].getGlobalCoordinates().y() + hitOnPlane[0].getYResidual();;
				float hitX0_rotated = 0;
				float hitY0_rotated = 0;
				if(station == 0){
					hitX0_rotated = hitX0*TMath::Cos((-8./180.)*TMath::Pi()) - hitY0*TMath::Sin((-8./180.)*TMath::Pi());
					hitY0_rotated = hitX0*TMath::Sin((-8./180.)*TMath::Pi()) + hitY0*TMath::Cos((-8./180.)*TMath::Pi());
				}	
				if(numberOfPointPerPlaneEff[pln]>=minNumberOfPlanesForEfficiency_){
					h2EfficiencyNormalizationMap_[planeId]->Fill(hitX0,hitY0);

					if(station == 0){ 
						h2EfficiencyNormalizationMap_rotated[planeId]->Fill(hitX0_rotated,hitY0_rotated);
					}
					if(hitOnPlane[0].getIsRealHit()){
						h2AuxEfficiencyMap_[planeId]->Fill(hitX0,hitY0);
						if(station == 0){
							h2AuxEfficiencyMap_rotated[planeId]->Fill(hitX0_rotated,hitY0_rotated);
						}
					}
				}
			}

			//Adding inter-pot efficiency calculation. Pot Under Test (PUT) is the pot where the efficiency is being calculated. The correlation values of a pot are
			//taken for the python config file in ../python/ConfFile_cfg.py
			double X0 = pixeltrack.getX0();
			double Y0 = pixeltrack.getY0();
			if (rpPixeltrack.size() >= (uint32_t)interPotEffMinTracksStart && rpPixeltrack.size() <= (uint32_t)interPotEffMaxTracksStart){
				for(const auto & rpPixeltrack_PUT : *pixelLocalTracks){
					CTPPSPixelDetId pixelRpId_PUT = CTPPSPixelDetId(rpPixeltrack_PUT.detId());
					uint32_t arm_PUT = pixelRpId_PUT.arm();
					uint32_t station_PUT = pixelRpId_PUT.station();
					uint32_t rp_PUT = pixelRpId_PUT.rp();
					int mapXbins_PUT;
					float mapXmin_PUT;
					float mapXmax_PUT;
					int mapYbins_PUT;
					float mapYmin_PUT;
					float mapYmax_PUT;
					if (station_PUT == 2){
						mapXbins_PUT = mapXbins_st2;
						mapXmin_PUT = mapXmin_st2;
						mapXmax_PUT = mapXmax_st2;
						mapYbins_PUT = mapYbins_st2;
						mapYmin_PUT = mapYmin_st2;
						mapYmax_PUT = mapYmax_st2;
						fitXmin = fitXmin_st2;
						fitXmax = fitXmax_st2;
					}
					else{
						mapXbins_PUT = mapXbins_st0;
						mapXmin_PUT = mapXmin_st0;
						mapXmax_PUT = mapXmax_st0;
						mapYbins_PUT = mapYbins_st0;
						mapYmin_PUT = mapYmin_st0;
						mapYmax_PUT = mapYmax_st0;
						fitXmin = fitXmin_st0;
						fitXmax = fitXmax_st0;   
					}
					// Selecting the pot with the same arm, located in a different station
					if(pixelRpId_PUT.arm() == arm && pixelRpId_PUT.station() != station){
						int matches = 0;
						double correlationCoefficientX0 = 1;
						double correlationCoefficientY0 = 1;
						double correlationConstantX0 = 0;
						double correlationConstantY0 = 0;
						double correlationToleranceX0 = 0;
						double correlationToleranceY0 = 0;

						// SEC45_210F
						if(pixelRpId_PUT.arm() == 0 && pixelRpId_PUT.station() == 0){
							correlationCoefficientX0 = correlationCoefficients[0];
							correlationCoefficientY0 = correlationCoefficients[1];
							correlationConstantX0 = correlationConstants[0];
							correlationConstantY0 = correlationConstants[1];
							correlationToleranceX0 = correlationTolerances[0];
							correlationToleranceY0 = correlationTolerances[1];
						}
						// SEC45_220F
						if(pixelRpId_PUT.arm() == 0 && pixelRpId_PUT.station() == 2){
							correlationCoefficientX0 = correlationCoefficients[2];
							correlationCoefficientY0 = correlationCoefficients[3];
							correlationConstantX0 = correlationConstants[2];
							correlationConstantY0 = correlationConstants[3];
							correlationToleranceX0 = correlationTolerances[2];
							correlationToleranceY0 = correlationTolerances[3];
						}
						// SEC56_210F
						if(pixelRpId_PUT.arm() == 1 && pixelRpId_PUT.station() == 0){
							correlationCoefficientX0 = correlationCoefficients[4];
							correlationCoefficientY0 = correlationCoefficients[5];
							correlationConstantX0 = correlationConstants[4];
							correlationConstantY0 = correlationConstants[5];
							correlationToleranceX0 = correlationTolerances[4];
							correlationToleranceY0 = correlationTolerances[5];
						}
						// SEC56_220F
						if(pixelRpId_PUT.arm() == 1 && pixelRpId_PUT.station() == 2){
							correlationCoefficientX0 = correlationCoefficients[6];
							correlationCoefficientY0 = correlationCoefficients[7];
							correlationConstantX0 = correlationConstants[6];
							correlationConstantY0 = correlationConstants[7];
							correlationToleranceX0 = correlationTolerances[6];
							correlationToleranceY0 = correlationTolerances[7];
						}
						double X0_expected = X0 * correlationCoefficientX0 + correlationConstantX0;
						double Y0_expected = Y0 * correlationCoefficientY0 + correlationConstantY0;
						numberOfAttempts++;

						if(h2InterPotEfficiency_.find(pixelRpId_PUT) == h2InterPotEfficiency_.end()){
							g1X0Correlation_[pixelRpId_PUT] = new TGraph();
							g1X0Correlation_[pixelRpId_PUT]->SetNameTitle(Form("g1X0Correlation_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
							Form("g1X0Correlation_arm%i_st%i_rp%i;arm%i_st%i_rp%i x (mm);arm%i_st%i_rp%i x (mm)",arm_PUT,station_PUT,rp_PUT,arm,station,rp,arm_PUT,station_PUT,rp_PUT));            
							g1Y0Correlation_[pixelRpId_PUT] = new TGraph();
							g1Y0Correlation_[pixelRpId_PUT]->SetNameTitle(Form("g1Y0Correlation_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
							Form("g1Y0Correlation_arm%i_st%i_rp%i;arm%i_st%i_rp%i y (mm);arm%i_st%i_rp%i y (mm)",arm_PUT,station_PUT,rp_PUT,arm,station,rp,arm_PUT,station_PUT,rp_PUT));
							h2X0Correlation_[pixelRpId_PUT] = new TH2D(Form("h2X0Correlation_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
							Form("h2X0Correlation_arm%i_st%i_rp%i;arm%i_st%i x (mm);arm%i_st%i x (mm)",arm_PUT,station_PUT,rp_PUT,arm,station,arm_PUT,station_PUT),
							mapXbins,mapXmin,mapXmax,mapXbins_PUT,mapXmin_PUT,mapXmax_PUT);
							h2Y0Correlation_[pixelRpId_PUT] = new TH2D(Form("h2Y0Correlation_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
							Form("h2Y0Correlation_arm%i_st%i_rp%i;arm%i_st%i y (mm);arm%i_st%i y (mm)",arm_PUT,station_PUT,rp_PUT,arm,station,arm_PUT,station_PUT),
							mapYbins,mapYmin,mapYmax,mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);
							h2InterPotEfficiency_[pixelRpId_PUT] = new TH2D(Form("h2InterPotEfficiency_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
							Form("h2InterPotEfficiency_arm%i_st%i_rp%i;x (mm);y (mm)",arm_PUT,station_PUT,rp_PUT),mapXbins_PUT,mapXmin_PUT,mapXmax_PUT,
							mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);
							h2InterPotEfficiencyError_[pixelRpId_PUT] = new TH2D(Form("h2InterPotEfficiencyError_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
							Form("h2InterPotEfficiencyError_arm%i_st%i_rp%i;x (mm);y (mm)",arm_PUT,station_PUT,rp_PUT),mapXbins_PUT,mapXmin_PUT,mapXmax_PUT,
							mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);
							h2BetterInterPotEfficiency_[pixelRpId_PUT] = new TH2D(Form("h2BetterInterPotEfficiency_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
							Form("h2BetterInterPotEfficiency_arm%i_st%i_rp%i;x (mm);y (mm)",arm_PUT,station_PUT,rp_PUT),mapXbins_PUT,mapXmin_PUT,mapXmax_PUT,
							mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);
							h2InterPotEfficiencyNormalization_[pixelRpId_PUT] = new TH2D(Form("h2InterPotEfficiencyNormalization_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
							Form("h2InterPotEfficiencyNormalization_arm%i_st%i_rp%i;x (mm);y (mm)",arm_PUT,station_PUT,rp_PUT),
							mapXbins_PUT,mapXmin_PUT,mapXmax_PUT,mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);

							if(supplementaryPlots && station_PUT == 0){
								h2InterPotEfficiency_rotated[pixelRpId_PUT] = new TH2D(Form("h2InterPotEfficiency_rotated_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
								Form("h2InterPotEfficiency_rotated_arm%i_st%i_rp%i;x (mm);y (mm)",arm_PUT,station_PUT,rp_PUT),mapXbins_PUT,mapXmin_PUT,mapXmax_PUT,
								mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);
								h2InterPotEfficiencyError_rotated[pixelRpId_PUT] = new TH2D(Form("h2InterPotEfficiencyError_rotated_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
								Form("h2InterPotEfficiencyError_rotated_arm%i_st%i_rp%i;x (mm);y (mm)",arm_PUT,station_PUT,rp_PUT),mapXbins_PUT,mapXmin_PUT,mapXmax_PUT,
								mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);
								h2BetterInterPotEfficiency_rotated[pixelRpId_PUT] = new TH2D(Form("h2BetterInterPotEfficiency_rotated_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
								Form("h2BetterInterPotEfficiency_rotated_arm%i_st%i_rp%i;x (mm);y (mm)",arm_PUT,station_PUT,rp_PUT),mapXbins_PUT,mapXmin_PUT,mapXmax_PUT,
								mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);
								h2InterPotEfficiencyNormalization_rotated[pixelRpId_PUT] = new TH2D(Form("h2InterPotEfficiencyNormalization_rotated_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
								Form("h2InterPotEfficiencyNormalization_rotated_arm%i_st%i_rp%i;x (mm);y (mm)",arm_PUT,station_PUT,rp_PUT),
								mapXbins_PUT,mapXmin_PUT,mapXmax_PUT,mapYbins_PUT,mapYmin_PUT,mapYmax_PUT);
							}
						}
						for(const auto & pixeltrack_PUT : rpPixeltrack_PUT){
							double X0_PUT = pixeltrack_PUT.getX0();
							double Y0_PUT = pixeltrack_PUT.getY0();

							if(rpPixeltrack.size() == 1 && rpPixeltrack_PUT.size() == 1){
								if(isCorrelationPlotEnabled){
									g1X0Correlation_[pixelRpId_PUT]->SetPoint(g1X0Correlation_[pixelRpId_PUT]->GetN(),X0,X0_PUT);
									g1Y0Correlation_[pixelRpId_PUT]->SetPoint(g1Y0Correlation_[pixelRpId_PUT]->GetN(),Y0,Y0_PUT);                  
								}
								h2X0Correlation_[pixelRpId_PUT]->Fill(X0,X0_PUT);
								h2Y0Correlation_[pixelRpId_PUT]->Fill(Y0,Y0_PUT);
							}

							bool xCorrelated = false;
							bool yCorrelated = false;
							bool sameTrack = false;
							
							if(X0_PUT > X0_expected - correlationToleranceX0 && X0_PUT < X0_expected + correlationToleranceX0) xCorrelated = true;
							if(Y0_PUT > Y0_expected - correlationToleranceY0 && Y0_PUT < Y0_expected + correlationToleranceY0) yCorrelated = true;
							sameTrack = xCorrelated && yCorrelated;
							
							if(sameTrack){
								matches++;
							}
						}
						
						if(matches > 1) numberOfOvermatches++;

						double rotX0_expected = 0;
						double rotY0_expected = 0;
						
						if(station_PUT == 0){
							rotX0_expected = X0_expected*TMath::Cos((-8./180.)*TMath::Pi()) - Y0_expected*TMath::Sin((-8./180.)*TMath::Pi());
							rotY0_expected = X0_expected*TMath::Sin((-8./180.)*TMath::Pi()) + Y0_expected*TMath::Cos((-8./180.)*TMath::Pi());
						}	

						if(matches >= 1){
							h2InterPotEfficiency_[pixelRpId_PUT]->Fill(X0_expected,Y0_expected);
							if(supplementaryPlots && station_PUT == 0) h2InterPotEfficiency_rotated[pixelRpId_PUT]->Fill(rotX0_expected,rotY0_expected);
						} 
						h2InterPotEfficiencyNormalization_[pixelRpId_PUT]->Fill(X0_expected,Y0_expected);
						if(supplementaryPlots && station_PUT == 0) h2InterPotEfficiencyNormalization_rotated[pixelRpId_PUT]->Fill(rotX0_expected,rotY0_expected);
					}
				} 
			}
		}
	}
}

void EfficiencyTool_2018::beginJob(){

	// Applying bunch selection
	h1BunchCrossing_ = new TH1D("h1BunchCrossing","h1BunchCrossing",totalNumberOfBunches_,0.,totalNumberOfBunches_);
	std::ifstream bunchListFile(bunchListFileName_.data());
	if(bunchSelection_=="NoSelection"){
		std::fill_n(validBunchArray_, totalNumberOfBunches_, true);
		return;
	}
	if(!bunchListFile.good()){
		std::cout << "BunchList file not good. Skipping buch selection..." << std::endl;
		return;
	}

	bool filledBunchArray[totalNumberOfBunches_];
	std::fill_n(filledBunchArray, totalNumberOfBunches_, false);
	std::fill_n(validBunchArray_, totalNumberOfBunches_, false);

	bool startReading = false;
	while(bunchListFile.good()){
		std::string line;
		getline(bunchListFile,line);
		if(line=="" || line=="\r") continue;
		std::vector<std::string> elements;
		boost::split(elements, line, boost::is_any_of(","));
		if(elements.at(0) == "B1 bucket number"){
			startReading = true;
			continue;
		}
		if(line.find("HEAD ON COLLISIONS FOR B2")!= std::string::npos)  break;
		if(!startReading) continue;
		if(elements.at(3)!="-") filledBunchArray[(std::atoi(elements.at(3).data())-1)/10+1] = true;
	}
	for (unsigned int i = 0; i < totalNumberOfBunches_; ++i){
		if(bunchSelection_=="CentralBunchesInTrain"){
			if(i==0) validBunchArray_[i] = filledBunchArray[totalNumberOfBunches_-1] && filledBunchArray[i] && filledBunchArray[i+1];
			else if(i==totalNumberOfBunches_-1) validBunchArray_[i] = filledBunchArray[i-1] && filledBunchArray[i] && filledBunchArray[0];
			else validBunchArray_[i] = filledBunchArray[i-1] && filledBunchArray[i] && filledBunchArray[i+1];
		}
		else if(bunchSelection_=="FirstBunchInTrain"){
			if(i==0) validBunchArray_[i] = filledBunchArray[i] && !filledBunchArray[totalNumberOfBunches_-1];
			else validBunchArray_[i] = filledBunchArray[i] && !filledBunchArray[i-1];          
		}
		else if(bunchSelection_=="LastBunchInTrain"){
			if(i==totalNumberOfBunches_-1) validBunchArray_[i] = filledBunchArray[i] && !filledBunchArray[0];
			else validBunchArray_[i] = filledBunchArray[i] && !filledBunchArray[i+1];          
		}
		else if(bunchSelection_=="FilledBunches") validBunchArray_[i] = filledBunchArray[i];
	}
}

void EfficiencyTool_2018::endJob(){
	outputFile_ = new TFile(outputFileName_.data(), "RECREATE");
	std::cout<<"Multiple matches for the same track have been found " <<numberOfOvermatches << " times out of "<<numberOfAttempts << 
	" (" << (((double)numberOfOvermatches)/numberOfAttempts)*100 << "%) while computing the Interpot Efficiency" << std::endl;
	std::cout << "Saving output in: " << outputFile_->GetName() << std::endl;
	h1BunchCrossing_->Write();
	for(const auto & rpId :  romanPotIdVector_){
		uint32_t arm = rpId.arm();
		uint32_t rp = rpId.rp();
		uint32_t station = rpId.station();
		if (station == 2){
			mapXbins = mapXbins_st2;
			mapXmin = mapXmin_st2;
			mapXmax = mapXmax_st2;
			mapYbins = mapYbins_st2;
			mapYmin = mapYmin_st2;
			mapYmax = mapYmax_st2;
			fitXmin = fitXmin_st2;
			fitXmax = fitXmax_st2;
		}
		else{
			mapXbins = mapXbins_st0;
			mapXmin = mapXmin_st0;
			mapXmax = mapXmax_st0;
			mapYbins = mapYbins_st0;
			mapYmin = mapYmin_st0;
			mapYmax = mapYmax_st0;
			fitXmin = fitXmin_st0;
			fitXmax = fitXmax_st0;      
		}  

		std::string romanPotFolderName = Form("Arm%i_st%i_rp%i",arm,station,rp);
		std::string romanPotBinShiftFolderName = Form("Arm%i_st%i_rp%i/BinShift",arm,station,rp);
		// std::cout << "Creating directory for: " << romanPotFolderName << std::endl;

		outputFile_->mkdir(romanPotFolderName.data());
		outputFile_->cd(romanPotFolderName.data());
		h2TrackHitDistribution_[rpId]->Write();
		if(supplementaryPlots){
			outputFile_->mkdir(romanPotBinShiftFolderName.data());
			outputFile_->cd(romanPotBinShiftFolderName.data());
			for(auto binShift : binShifts_){
				h2TrackHitDistributionBinShift_[binShift][rpId]->Write();
				if(station == 0){
					h2TrackHitDistributionBinShift_rotated[binShift][rpId]->Write();
				}
			}
			outputFile_->cd(romanPotFolderName.data());
		}
		h23PointsTrackHitDistribution_[rpId]->Write();
		h2X0Correlation_[rpId]->Write();
		h2Y0Correlation_[rpId]->Write();
		if(isCorrelationPlotEnabled){
			g1X0Correlation_[rpId]->Write();
			g1Y0Correlation_[rpId]->Write();
		}

		// Interpot efficiency calibration
		TObjArray X0slices;
		TF1 *fGausX0 = new TF1("fGausX0","gaus",mapXmin,mapXmax);
		h2X0Correlation_[rpId]->FitSlicesY(fGausX0, 1, mapXbins, 1, "QNRG3", &X0slices);
		delete fGausX0;
		TObjArray Y0slices;
		TF1 *fGausY0 = new TF1("fGausX0","gaus",mapYmin,mapYmax);
		h2Y0Correlation_[rpId]->FitSlicesY(fGausY0, 1, mapYbins, 1, "QNRG3", &Y0slices);
		delete fGausX0;
		if(isCorrelationPlotEnabled){
			std::cout << "Fitting correlation with arm" << arm << "_st" << station << "_rp" << rp 
			<< " (Fitting TH2D slices with gaussian curves)" << std::endl;
			std::cout << "X0 Correlation Relation (Fitting Means) " << std::endl 
			<< "p1 = correlationCoefficient\tp0 = correlationConstant\tin python cfg";
			((TH1D*)X0slices[1])->Fit("pol1","N","",fitXmin,fitXmax);
			std::cout << std::endl << "X0 Correlation Tolerance (Fitting Sigmas)" << std::endl << "p0 = sigma\t\tin python cfg";
			((TH1D*)X0slices[2])->Fit("pol1","N","",fitXmin,fitXmax);
			std::cout << std::endl;

			std::cout << "Fitting correlation with arm" << arm << "_st" << station << "_rp" << rp 
			<< " (Fitting TH2D slices with gaussian curves)" << std::endl;
			std::cout << "Y0 Correlation Relation (Fitting Means)" << std::endl 
			<< "p1 = correlationCoefficient\tp0 = correlationConstant\tin python cfg";
			((TH1D*)Y0slices[1])->Fit("pol1","N","",-5,10);
			std::cout << std::endl << "Y0 Correlation Tolerance (Fitting Sigmas)" << std::endl << "p0 = sigma\t\tin python cfg";
			((TH1D*)Y0slices[2])->Fit("pol1","N","",-5,10);
			std::cout << std::endl;
		}

		h2InterPotEfficiency_[rpId]->Divide(h2InterPotEfficiencyNormalization_[rpId]);
		if(supplementaryPlots && station == 0){
			h2InterPotEfficiency_rotated[rpId]->Divide(h2InterPotEfficiencyNormalization_rotated[rpId]);
		}
		for(int xBin=1; xBin<=mapXbins; ++xBin){
			for(int yBin=1; yBin<=mapYbins; ++yBin){
				double e = h2InterPotEfficiency_[rpId]->GetBinContent(xBin,yBin);
				h2InterPotEfficiency_[rpId]->SetBinError(xBin,yBin,TMath::Sqrt(e*(1-e)/(h2InterPotEfficiencyNormalization_[rpId]->GetBinContent(xBin,yBin))));
				h2InterPotEfficiencyError_[rpId]->SetBinContent(xBin,yBin,TMath::Sqrt(e*(1-e)/(h2InterPotEfficiencyNormalization_[rpId]->GetBinContent(xBin,yBin))));
				if(h2InterPotEfficiencyNormalization_[rpId]->GetBinContent(xBin,yBin) > 15) {
					h2BetterInterPotEfficiency_[rpId]->SetBinContent(xBin,yBin,h2InterPotEfficiency_[rpId]->GetBinContent(xBin,yBin));
					h2BetterInterPotEfficiency_[rpId]->SetBinError(xBin,yBin,h2InterPotEfficiencyError_[rpId]->GetBinContent(xBin,yBin));
				}

				if(supplementaryPlots && station == 0){
					e = h2InterPotEfficiency_rotated[rpId]->GetBinContent(xBin,yBin);
					h2InterPotEfficiency_rotated[rpId]->SetBinError(xBin,yBin,TMath::Sqrt(e*(1-e)/(h2InterPotEfficiencyNormalization_rotated[rpId]->GetBinContent(xBin,yBin))));
					h2InterPotEfficiencyError_rotated[rpId]->SetBinContent(xBin,yBin,TMath::Sqrt(e*(1-e)/(h2InterPotEfficiencyNormalization_rotated[rpId]->GetBinContent(xBin,yBin))));
					if(h2InterPotEfficiencyNormalization_rotated[rpId]->GetBinContent(xBin,yBin) > 15) {
						h2BetterInterPotEfficiency_rotated[rpId]->SetBinContent(xBin,yBin,h2InterPotEfficiency_rotated[rpId]->GetBinContent(xBin,yBin));
						h2BetterInterPotEfficiency_rotated[rpId]->SetBinError(xBin,yBin,h2InterPotEfficiencyError_rotated[rpId]->GetBinContent(xBin,yBin));
					}
				}
			}
		}
		h2InterPotEfficiency_[rpId]->Write();
		h2BetterInterPotEfficiency_[rpId]->Write();
		h2InterPotEfficiencyError_[rpId]->Write();
		h2InterPotEfficiencyNormalization_[rpId]->Write();
	}

	for(const auto & detId : detectorIdVector_){
		uint32_t arm = detId.arm();
		uint32_t rp = detId.rp();
		uint32_t station = detId.station();
		uint32_t plane = detId.plane();
		std::string planeFolderName = Form("Arm%i_st%i_rp%i/Arm%i_st%i_rp%i_pl%i",arm,station,rp,arm,station,rp,plane);
		// std::cout << "Creating directory for: " << planeFolderName << std::endl;
		outputFile_->mkdir(planeFolderName.data());
		// std::cout << "Created directory for: " << planeFolderName << std::endl;
		outputFile_->cd(planeFolderName.data());

		h2ModuleHitMap_[detId]->Write();
		h2EfficiencyMap_[detId]->Divide(h2AuxEfficiencyMap_[detId], h2EfficiencyNormalizationMap_[detId],1.,1.,"B");
		h2EfficiencyMap_[detId]->SetMaximum(1.);
		h2EfficiencyMap_[detId]->Write();


		if(station == 0){
			h2EfficiencyMap_rotated[detId]->Divide(h2AuxEfficiencyMap_rotated[detId], h2EfficiencyNormalizationMap_rotated[detId],1.,1.,"B");
			h2EfficiencyMap_rotated[detId]->SetMaximum(1.);
			h2EfficiencyMap_rotated[detId]->Write();
			if(supplementaryPlots)h2ModuleHitMap_rotated[detId]->Write();
		}
	}

	for(const auto & rpId :  romanPotIdVector_){
		uint32_t arm = rpId.arm();
		uint32_t rp = rpId.rp();
		uint32_t station = rpId.station();
		if (station == 2){
			mapXbins = mapXbins_st2;
			mapXmin = mapXmin_st2;
			mapXmax = mapXmax_st2;
			mapYbins = mapYbins_st2;
			mapYmin = mapYmin_st2;
			mapYmax = mapYmax_st2;
			fitXmin = fitXmin_st2;
			fitXmax = fitXmax_st2;
		}
		else{
			mapXbins = mapXbins_st0;
			mapXmin = mapXmin_st0;
			mapXmax = mapXmax_st0;
			mapYbins = mapYbins_st0;
			mapYmin = mapYmin_st0;
			mapYmax = mapYmax_st0;
			fitXmin = fitXmin_st0;
			fitXmax = fitXmax_st0;      
		}

		std::string romanPotFolderName = Form("Arm%i_st%i_rp%i",arm,station,rp);
		outputFile_->cd(romanPotFolderName.data());
		for(int xBin=1; xBin<=mapXbins; ++xBin){
			for(int yBin=1; yBin<=mapYbins; ++yBin){
				std::map<uint32_t, float> planeEfficiency;
				std::map<uint32_t, float> planeEfficiencyError;
				std::map<uint32_t, float> planeEfficiency_rotated;
				std::map<uint32_t, float> planeEfficiencyError_rotated;
				
				for(const auto & plane : listOfPlanes_){
					CTPPSPixelDetId planeId = CTPPSPixelDetId(rpId);
					planeId.setPlane(plane);
					planeEfficiency[plane] = h2EfficiencyMap_[planeId]->GetBinContent(xBin,yBin);
					planeEfficiencyError[plane] = h2EfficiencyMap_[planeId]->GetBinError(xBin,yBin);
					if(station == 0){
						planeEfficiency_rotated[plane] = h2EfficiencyMap_rotated[planeId]->GetBinContent(xBin,yBin);
						planeEfficiencyError_rotated[plane] = h2EfficiencyMap_rotated[planeId]->GetBinError(xBin,yBin);
					}	

					// if(plane == 0){
					// 	planeEfficiency[plane] = 1.;	
					// 	if(station==0) planeEfficiency_rotated[plane] = 1.;
					// }
					// if(plane == 1){
					// 	planeEfficiency[plane] = 1.;	
					// 	if(station==0) planeEfficiency_rotated[plane] = 1.;
					// }
					// if(plane == 2){
					// 	planeEfficiency[plane] = 1.;	
					// 	if(station==0) planeEfficiency_rotated[plane] = 1.;
					// }
					// if(plane == 3){
					// 	planeEfficiency[plane] = 1.;	
					// 	if(station==0) planeEfficiency_rotated[plane] = 1.;
					// }
					// if(plane == 4){
					// 	planeEfficiency[plane] = 1.;	
					// 	if(station==0) planeEfficiency_rotated[plane] = 1.;
					// }
					// if(plane == 5){
					// 	planeEfficiency[plane] = 0.;	
					// 	if(station==0) planeEfficiency_rotated[plane] = 0.;
					// }
				}
				float efficiency = probabilityCalculation(planeEfficiency);
				float efficiencyError = errorCalculation(planeEfficiency,planeEfficiencyError);
				h2TrackEfficiencyMap_[rpId]->SetBinContent(xBin,yBin,efficiency);
				h2TrackEfficiencyMap_[rpId]->SetBinError(xBin,yBin,efficiencyError);
				h2TrackEfficiencyErrorMap_[rpId]->SetBinContent(xBin,yBin,efficiencyError);
				if(station == 0){
					float efficiency_rotated = probabilityCalculation(planeEfficiency_rotated);
					float efficiencyError_rotated = errorCalculation(planeEfficiency_rotated,planeEfficiencyError_rotated);
					h2TrackEfficiencyMap_rotated[rpId]->SetBinContent(xBin,yBin,efficiency_rotated);
					h2TrackEfficiencyMap_rotated[rpId]->SetBinError(xBin,yBin,efficiencyError_rotated);
					h2TrackEfficiencyErrorMap_rotated[rpId]->SetBinContent(xBin,yBin,efficiencyError_rotated);
				}
			}
		}
		h2TrackEfficiencyMap_[rpId]->Write();
		h2TrackEfficiencyErrorMap_[rpId]->Write();
		if(supplementaryPlots){
			outputFile_->mkdir((romanPotFolderName+"/ResolutionHistograms").data());
			// std::cout << (romanPotFolderName+"/ResolutionHistograms").data() << std::endl;
			outputFile_->cd((romanPotFolderName+"/ResolutionHistograms").data());
			for(int nPlanes = 3; nPlanes <= 6; nPlanes++){
				for(int numberOfCls = 0; numberOfCls <= nPlanes; numberOfCls++){
					h1X0Sigma[rpId][std::pair(nPlanes,numberOfCls)]->Write();
					h1Y0Sigma[rpId][std::pair(nPlanes,numberOfCls)]->Write();
					h1TxSigma[rpId][std::pair(nPlanes,numberOfCls)]->Write();
					h1TySigma[rpId][std::pair(nPlanes,numberOfCls)]->Write();
				}
			}

			outputFile_->cd(romanPotFolderName.data());

			h1NumberOfTracks_[rpId]->Write();
			h2AvgPlanesUsed_[rpId]->Divide(h2TrackHitDistribution_[rpId]);
			h2AvgPlanesUsed_[rpId]->Write();
			h1PlanesUsed_[rpId]->Write();	
			h1ChiSquaredOverNDF_[rpId]->Write();
		}
		if(station == 0){
			h2TrackEfficiencyMap_rotated[rpId]->Write();
			h2TrackEfficiencyErrorMap_rotated[rpId]->Write();
			if(supplementaryPlots){
				h2TrackHitDistribution_rotated[rpId]->Write();
				h2AvgPlanesUsed_rotated[rpId]->Divide(h2TrackHitDistribution_rotated[rpId]);
				h2AvgPlanesUsed_rotated[rpId]->Write();	
				h2InterPotEfficiency_rotated[rpId]->Write();
				h2BetterInterPotEfficiency_rotated[rpId]->Write();
				h2InterPotEfficiencyError_rotated[rpId]->Write();
				h2InterPotEfficiencyNormalization_rotated[rpId]->Write();
			}
		}
		if(supplementaryPlots && station != 0){
			TObjArray slices;
			TF1 *fGaus = new TF1("fGaus","gaus",mapYmin,mapYmax);
			h2TrackHitDistribution_[rpId]->FitSlicesY(fGaus, 1, mapXbins, 0, "QNG3", &slices);
			delete fGaus;
			((TH1D*)slices[0])->Write();
			((TH1D*)slices[1])->Write();
			((TH1D*)slices[2])->Write();
			((TH1D*)slices[3])->Write();
		}
		if(supplementaryPlots && station == 0){
			TObjArray slices;
			TF1 *fGaus = new TF1("fGaus","gaus",mapYmin,mapYmax);
			h2TrackHitDistribution_rotated[rpId]->FitSlicesY(fGaus, 1, mapXbins, 0, "QNG3", &slices);
			delete fGaus;
			((TH1D*)slices[0])->Write();
			((TH1D*)slices[1])->Write();
			((TH1D*)slices[2])->Write();
			((TH1D*)slices[3])->Write();			
		}
	}

	if(isCorrelationPlotEnabled) std::cout << "ATTENTION: Remember to insert the fitting parameters in the python configuration" << std::endl;
	outputFile_->Close();
}

void EfficiencyTool_2018::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}

//This function produces all the possible plane combinations extracting numberToExtract planes over numberOfPlanes planes
void EfficiencyTool_2018::getPlaneCombinations(const std::vector<uint32_t> &inputPlaneList, uint32_t numberToExtract, 
	std::vector<std::pair<std::vector<uint32_t>,std::vector<uint32_t> > > &planesExtractedAndNot){
	uint32_t numberOfPlanes = inputPlaneList.size();
	std::string bitmask(numberToExtract, 1); // numberToExtract leading 1's
	bitmask.resize(numberOfPlanes, 0); // numberOfPlanes-numberToExtract trailing 0's
	planesExtractedAndNot.clear();

	// store the combination and permute bitmask
	do {
	planesExtractedAndNot.push_back(std::pair<std::vector<uint32_t>,std::vector<uint32_t> >(std::vector<uint32_t>(),std::vector<uint32_t>()));
	for (uint32_t i = 0; i < numberOfPlanes; ++i) { // [0..numberOfPlanes-1] integers
		if (bitmask[i]) planesExtractedAndNot.back().second.push_back(inputPlaneList.at(i));
		else planesExtractedAndNot.back().first.push_back(inputPlaneList.at(i));
	}
	} while (std::prev_permutation(bitmask.begin(), bitmask.end()));

	return;
}

float EfficiencyTool_2018::probabilityNplanesBlind(const std::vector<uint32_t> &inputPlaneList, int numberToExtract, const std::map<unsigned, float> &planeEfficiency){
	std::vector<std::pair<std::vector<uint32_t>,std::vector<uint32_t> > > planesExtractedAndNot;
	getPlaneCombinations(inputPlaneList, numberToExtract, planesExtractedAndNot);

	float probability = 0.;

	for( const auto & combination : planesExtractedAndNot){
		float combinationProbability = 1.;
		for(const auto & efficientPlane : combination.first){
			combinationProbability*=planeEfficiency.at(efficientPlane);
		}
		for(const auto & notEfficientPlane : combination.second){
		combinationProbability*=(1.-planeEfficiency.at(notEfficientPlane));
		}
		probability+=combinationProbability;
	}
	return probability;
}

//Calculates the partial derivative of the rp efficiency with respect to the efficiency of a certain plane when extracting numberToExtract planes
float EfficiencyTool_2018::efficiencyPartialDerivativewrtPlane(uint32_t plane, const std::vector<uint32_t> &inputPlaneList, int numberToExtract, const std::map<unsigned, float> &planeEfficiency){
	std::vector<uint32_t> modifiedInputPlaneList = inputPlaneList;
	modifiedInputPlaneList.erase(std::find(modifiedInputPlaneList.begin(),modifiedInputPlaneList.end(),plane));
	float partialDerivative = 0.;
	if(numberToExtract > 0 && numberToExtract < 6 ){
		partialDerivative = - probabilityNplanesBlind(modifiedInputPlaneList,numberToExtract,planeEfficiency) + probabilityNplanesBlind(modifiedInputPlaneList,numberToExtract-1,planeEfficiency);
	}
	else {
		if(numberToExtract == 6){
		partialDerivative = probabilityNplanesBlind(modifiedInputPlaneList, numberToExtract-1,planeEfficiency);
		}
		else{
			partialDerivative = - probabilityNplanesBlind(modifiedInputPlaneList,numberToExtract,planeEfficiency);
		}
	}
	return partialDerivative;
}

float EfficiencyTool_2018::errorCalculation(const std::map<uint32_t, float> &planeEfficiency,const std::map<uint32_t, float> &planeEfficiencyError){
	int minNumberOfBlindPlanes = 3;
	int maxNumberOfBlindPlanes = listOfPlanes_.size();
	float rpEfficiencySquareError = 0.;
	for(const auto & plane : listOfPlanes_){
		float partialDerivative = 0.;
		for (uint32_t i=(uint32_t)minNumberOfBlindPlanes; i<=(uint32_t)maxNumberOfBlindPlanes; i++){
		partialDerivative += efficiencyPartialDerivativewrtPlane(plane,listOfPlanes_,i,planeEfficiency);
		}
		rpEfficiencySquareError += partialDerivative*partialDerivative* planeEfficiencyError.at(plane) * planeEfficiencyError.at(plane);
	}
	return TMath::Sqrt(rpEfficiencySquareError);
}


float EfficiencyTool_2018::probabilityCalculation(const std::map<uint32_t, float> &planeEfficiency){

	int minNumberOfBlindPlanes = 3;
	int maxNumberOfBlindPlanes = listOfPlanes_.size();
	float rpEfficiency = 1.;

	for(uint32_t i = (uint32_t)minNumberOfBlindPlanes; i<=(uint32_t)maxNumberOfBlindPlanes; i++){
		rpEfficiency-=probabilityNplanesBlind(listOfPlanes_, i, planeEfficiency);
	}
	return rpEfficiency;
}

//define this as a plug-in
DEFINE_FWK_MODULE(EfficiencyTool_2018);