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
  int minNumberOfPlanesPerEfficiency_;

  std::map<CTPPSPixelDetId,TH2D*> h2ModuleHitMap_;
  std::map<CTPPSPixelDetId,TH2D*> h2EfficiencyMap_;
  std::map<CTPPSPixelDetId,TH2D*> h2AuxEfficiencyMap_;
  std::map<CTPPSPixelDetId,TH2D*> h2EfficiencyNormalizationMap_;
  std::map<CTPPSPixelDetId,TH2D*> h2TrackHitDistribution_;
  std::map<CTPPSPixelDetId,TH2D*> h2TrackEfficiencyMap_;
  std::map<CTPPSPixelDetId,TGraph*> g1X0Correlation_;
  std::map<CTPPSPixelDetId,TGraph*> g1Y0Correlation_;
  std::map<CTPPSPixelDetId,TH2D*> h2X0Correlation_;
  std::map<CTPPSPixelDetId,TH2D*> h2Y0Correlation_;
  std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiency_;
  std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiencyError_;
  std::map<CTPPSPixelDetId,TH2D*> h2BetterInterPotEfficiency_;
  std::map<CTPPSPixelDetId,TH2D*> h2InterPotEfficiencyNormalization_;

  std::vector<CTPPSPixelDetId> detectorIdVector_;
  std::vector<CTPPSPixelDetId> romanPotIdVector_;

  std::vector<double> correlationCoefficients;
  std::vector<double> correlationConstants;
  std::vector<double> correlationTolerances;

  std::vector<uint32_t> listOfPlanes_ = {0,1,2,3,4,5};

  int interPotEffMinTracksStart = 0;
  int interPotEffMaxTracksStart = 99;

  int   mapXbins_st2 = 200 ;
  float mapXmin_st2  =  43.*TMath::Cos(18.4/180.*TMath::Pi());
  float mapXmax_st2  =  73.*TMath::Cos(18.4/180.*TMath::Pi());
  int   mapYbins_st2 = 240 ;
  float mapYmin_st2  = -12.;
  float mapYmax_st2  =  12.;
  float fitXmin_st2  = 6.;
  float fitXmax_st2  = 19.;

  int   mapXbins_st0 = 200 ;
  float mapXmin_st0  =  3.*TMath::Cos(18.4/180.*TMath::Pi());
  float mapXmax_st0  =  33.*TMath::Cos(18.4/180.*TMath::Pi());
  int   mapYbins_st0 = 240 ;
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


  int numberOfOvermatches = 0;
  int numberOfAttempts = 0;
};

EfficiencyTool_2018::EfficiencyTool_2018(const edm::ParameterSet& iConfig){
  usesResource("TFileService");
  pixelLocalTrackToken_ = consumes<edm::DetSetVector<CTPPSPixelLocalTrack> >(edm::InputTag("ctppsPixelLocalTracks",""));
  pixelRecHitToken_     = consumes<edm::DetSetVector<CTPPSPixelRecHit> >(edm::InputTag("ctppsPixelRecHits",""));
  outputFileName_ = iConfig.getUntrackedParameter<std::string>("outputFileName");
  minNumberOfPlanesPerEfficiency_ = iConfig.getParameter<int>("minNumberOfPlanesPerEfficiency");
  isCorrelationPlotEnabled = iConfig.getParameter<bool>("isCorrelationPlotEnabled");
  correlationCoefficients = iConfig.getParameter<std::vector<double> >("correlationCoefficients");
  correlationConstants = iConfig.getParameter<std::vector<double> >("correlationConstants");
  correlationTolerances = iConfig.getParameter<std::vector<double> >("correlationTolerances");
  interPotEffMinTracksStart = iConfig.getUntrackedParameter<int>("interPotEffMinTracksStart");
  interPotEffMaxTracksStart = iConfig.getUntrackedParameter<int>("interPotEffMaxTracksStart");
}

EfficiencyTool_2018::~EfficiencyTool_2018(){
  for(const auto & rpId :  romanPotIdVector_){
    delete h2TrackHitDistribution_[rpId];
    delete h2TrackEfficiencyMap_[rpId];
    delete g1X0Correlation_[rpId];
    delete g1Y0Correlation_[rpId];
    delete h2X0Correlation_[rpId];
    delete h2Y0Correlation_[rpId];
    delete h2InterPotEfficiency_[rpId];
    delete h2InterPotEfficiencyError_[rpId];
    delete h2BetterInterPotEfficiency_[rpId];
    delete h2InterPotEfficiencyNormalization_[rpId];
  }

  for(const auto & detId : detectorIdVector_){
    delete h2ModuleHitMap_[detId];
    delete h2EfficiencyNormalizationMap_[detId];

    delete h2EfficiencyMap_[detId];
    delete h2AuxEfficiencyMap_[detId];
  }
}

void EfficiencyTool_2018::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  using namespace edm;
  Handle<edm::DetSetVector<CTPPSPixelRecHit> > pixelRecHits;
  iEvent.getByToken(pixelRecHitToken_, pixelRecHits);

  Handle<edm::DetSetVector<CTPPSPixelLocalTrack> > pixelLocalTracks;
  iEvent.getByToken(pixelLocalTrackToken_, pixelLocalTracks);

  for(const auto & rpPixeltrack : *pixelLocalTracks){

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
    if(h2TrackHitDistribution_.find(rpId)==h2TrackHitDistribution_.end()){
      romanPotIdVector_.push_back(rpId);
      h2TrackHitDistribution_[rpId] = new TH2D(Form("h2TrackHitDistribution_arm%i_st%i_rp%i",arm,station,rp),
      Form("h2TrackHitDistribution_arm%i_st%i_rp%i;x (mm);y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
      h2TrackEfficiencyMap_[rpId] = new TH2D(Form("h2TrackEfficiencyMap_arm%i_st%i_rp%i",arm,station,rp),
      Form("h2TrackEfficiencyMap_arm%i_st%i_rp%i; x (mm); y (mm)",arm,station,rp),mapXbins,mapXmin,mapXmax,mapYbins,mapYmin,mapYmax);
    }
    
    for(const auto & pixeltrack : rpPixeltrack){
      float pixelX0 = pixeltrack.getX0();
      float pixelY0 = pixeltrack.getY0();

      h2TrackHitDistribution_[rpId]->Fill(pixelX0,pixelY0);

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
        }

        for(const auto & hit : planeHits){
          if(hit.getIsUsedForFit()){
            //Counting, for each plane, how many of the others were hit, to establish its efficiency is computable in the event
            for(auto pln : numberOfPointPerPlaneEff){
              if(pln.first == planeId.plane()) continue;
              numberOfPointPerPlaneEff[pln.first] = pln.second + 1;
            }
          }
          h2ModuleHitMap_[planeId]->Fill(hit.getGlobalCoordinates().x(),hit.getGlobalCoordinates().y());
        }
      }

      //Efficiency calculation
      for(const auto pln : listOfPlanes_){
        CTPPSPixelDetId planeId = rpId;
        planeId.setPlane(pln);
        edm::DetSet<CTPPSPixelFittedRecHit> hitOnPlane = fittedHits[planeId];
        float hitX0 = hitOnPlane[0].getGlobalCoordinates().x();
        float hitY0 = hitOnPlane[0].getGlobalCoordinates().y();
        if(numberOfPointPerPlaneEff[pln]>=minNumberOfPlanesPerEfficiency_){
          h2EfficiencyNormalizationMap_[planeId]->Fill(hitX0,hitY0);
          if(hitOnPlane[0].getIsRealHit()) h2AuxEfficiencyMap_[planeId]->Fill(hitX0,hitY0);
        }
      }

      //Adding inter-pot efficiency calculation. Pot Under Test (PUT) is the pot where the efficiency is being calculated. The correlation values of a pot are
      //taken for the python config file in ../python/ConfFile_cfg.py
      double X0 = pixeltrack.getX0();
      double Y0 = pixeltrack.getY0();
      if (rpPixeltrack.size() > (uint32_t)interPotEffMinTracksStart && rpPixeltrack.size() < (uint32_t)interPotEffMaxTracksStart){
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
            // Booking histograms and graphs if they aren't already
            if(h2InterPotEfficiency_.find(pixelRpId_PUT) == h2InterPotEfficiency_.end()){
              g1X0Correlation_[pixelRpId_PUT] = new TGraph();
              g1X0Correlation_[pixelRpId_PUT]->SetNameTitle(Form("g1X0Correlation_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
              Form("g1X0Correlation_arm%i_st%i_rp%i;arm%i_st%i_rp%i x (mm);arm%i_st%i_rp%i x (mm)",arm_PUT,station_PUT,rp_PUT,arm,station,rp,arm_PUT,station_PUT,rp_PUT));            
              g1Y0Correlation_[pixelRpId_PUT] = new TGraph();
              g1Y0Correlation_[pixelRpId_PUT]->SetNameTitle(Form("g1Y0Correlation_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
              Form("g1Y0Correlation_arm%i_st%i_rp%i;arm%i_st%i_rp%i y (mm);arm%i_st%i_rp%i y (mm)",arm_PUT,station_PUT,rp_PUT,arm,station,rp,arm_PUT,station_PUT,rp_PUT));
              h2X0Correlation_[pixelRpId_PUT] = new TH2D(Form("h2X0Correlation_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
              Form("h2X0Correlation_arm%i_st%i_rp%i;arm%i_st%i_rp%i x (mm);arm%i_st%i_rp%i x (mm)",arm_PUT,station_PUT,rp_PUT,arm,station,rp,arm_PUT,station_PUT,rp_PUT),
              mapXbins,mapXmin,mapXmax,mapXbins_PUT,mapXmin_PUT,mapXmax_PUT);
              h2Y0Correlation_[pixelRpId_PUT] = new TH2D(Form("h2Y0Correlation_arm%i_st%i_rp%i",arm_PUT,station_PUT,rp_PUT),
              Form("h2Y0Correlation_arm%i_st%i_rp%i;arm%i_st%i_rp%i y (mm);arm%i_st%i_rp%i y (mm)",arm_PUT,station_PUT,rp_PUT,arm,station,rp,arm_PUT,station_PUT,rp_PUT),
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
            }
            for(const auto & pixeltrack_PUT : rpPixeltrack_PUT){
              double X0_PUT = pixeltrack_PUT.getX0();
              double Y0_PUT = pixeltrack_PUT.getY0();
              if(isCorrelationPlotEnabled && rpPixeltrack.size() == 1 && rpPixeltrack_PUT.size() == 1){
                g1X0Correlation_[pixelRpId_PUT]->SetPoint(g1X0Correlation_[pixelRpId_PUT]->GetN(),X0,X0_PUT);
                g1Y0Correlation_[pixelRpId_PUT]->SetPoint(g1Y0Correlation_[pixelRpId_PUT]->GetN(),Y0,Y0_PUT);
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
            if(matches >= 1) h2InterPotEfficiency_[pixelRpId_PUT]->Fill(X0_expected,Y0_expected); 
            h2InterPotEfficiencyNormalization_[pixelRpId_PUT]->Fill(X0_expected,Y0_expected);
          }
        } 
      }
    }
  }

  #ifdef THIS_IS_AN_EVENT_EXAMPLE
  Handle<ExampleData> pIn;
  iEvent.getByLabel("example",pIn);
  #endif
     
  #ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  ESHandle<SetupData> pSetup;
  iSetup.get<SetupRecord>().get(pSetup);
#endif
}

void EfficiencyTool_2018::beginJob(){}

void EfficiencyTool_2018::endJob(){
  outputFile_ = new TFile(outputFileName_.data(), "RECREATE");
  std::cout<<"There have been found multiple matches for the same track " <<numberOfOvermatches << " times out of "<<numberOfAttempts << 
  " (" << (((double)numberOfOvermatches)/numberOfAttempts)*100 << "%)" << std::endl;
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
    // std::cout << "Creating directory for: " << romanPotFolderName << std::endl;
    outputFile_->mkdir(romanPotFolderName.data());
    outputFile_->cd(romanPotFolderName.data());
    h2TrackHitDistribution_[rpId]->Write();
    // Saving inter-pot efficiency histograms

    g1X0Correlation_[rpId]->Write();
    g1Y0Correlation_[rpId]->Write();
    h2X0Correlation_[rpId]->Write();
    h2Y0Correlation_[rpId]->Write();

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
    for(int xBin=1; xBin<=mapXbins; ++xBin){
      for(int yBin=1; yBin<=mapYbins; ++yBin){
        double e = h2InterPotEfficiency_[rpId]->GetBinContent(xBin,yBin);
        h2InterPotEfficiency_[rpId]->SetBinError(xBin,yBin,TMath::Sqrt(e*(1-e)/(h2InterPotEfficiencyNormalization_[rpId]->GetBinContent(xBin,yBin))));
        h2InterPotEfficiencyError_[rpId]->SetBinContent(xBin,yBin,TMath::Sqrt(e*(1-e)/(h2InterPotEfficiencyNormalization_[rpId]->GetBinContent(xBin,yBin))));
        if(h2InterPotEfficiencyNormalization_[rpId]->GetBinContent(xBin,yBin) > 15) {
          h2BetterInterPotEfficiency_[rpId]->SetBinContent(xBin,yBin,h2InterPotEfficiency_[rpId]->GetBinContent(xBin,yBin));
          h2BetterInterPotEfficiency_[rpId]->SetBinError(xBin,yBin,h2InterPotEfficiencyError_[rpId]->GetBinContent(xBin,yBin));
        }
      }
    }
    // h2InterPotEfficiency_[rpId]->Write();
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
    h2EfficiencyMap_[detId]->Write();
    h2EfficiencyNormalizationMap_[detId]->Write();
  }
  
  for(const auto & rpId :  romanPotIdVector_){
    uint32_t arm = rpId.arm();
    uint32_t rp = rpId.rp();
    uint32_t station = rpId.station();
    std::string romanPotFolderName = Form("Arm%i_st%i_rp%i",arm,station,rp);
    outputFile_->cd(romanPotFolderName.data());
    for(int xBin=1; xBin<=mapXbins; ++xBin){
      for(int yBin=1; yBin<=mapYbins; ++yBin){
        std::map<uint32_t, float> planeEfficiency;
        std::map<uint32_t, float> planeEfficiencyError;
        for(const auto & plane : listOfPlanes_){
          CTPPSPixelDetId planeId = CTPPSPixelDetId(rpId);
          planeId.setPlane(plane);
          planeEfficiency[plane] = h2EfficiencyMap_[planeId]->GetBinContent(xBin,yBin);
          planeEfficiencyError[plane] = h2EfficiencyMap_[planeId]->GetBinError(xBin,yBin);
        }
        float efficiency = probabilityCalculation(planeEfficiency);
        float efficiencyError = errorCalculation(planeEfficiency,planeEfficiencyError);
        h2TrackEfficiencyMap_[rpId]->SetBinContent(xBin,yBin,efficiency);
        h2TrackEfficiencyMap_[rpId]->SetBinError(xBin,yBin,efficiencyError);
      }
    }
    h2TrackEfficiencyMap_[rpId]->Write();
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
  if(numberToExtract>1){
    partialDerivative = probabilityNplanesBlind(modifiedInputPlaneList,numberToExtract,planeEfficiency) - probabilityNplanesBlind(modifiedInputPlaneList,numberToExtract-1,planeEfficiency);
  }
  else{
    partialDerivative = probabilityNplanesBlind(modifiedInputPlaneList, numberToExtract,planeEfficiency);
  }
  return partialDerivative;
}

float EfficiencyTool_2018::errorCalculation(const std::map<uint32_t, float> &planeEfficiency,const std::map<uint32_t, float> &planeEfficiencyError){
  int minNumberOfPlanes = 3;
  int maxNumberOfPlanes = listOfPlanes_.size();
  float rpEfficiencySquareError = 0.;
  for(const auto & plane : listOfPlanes_){
    float partialDerivative = 0.;
    for (uint32_t i=(uint32_t)minNumberOfPlanes+1; i<=(uint32_t)maxNumberOfPlanes; i++){
    partialDerivative += efficiencyPartialDerivativewrtPlane(plane,listOfPlanes_,i,planeEfficiency);
    }
    rpEfficiencySquareError += partialDerivative*partialDerivative* planeEfficiencyError.at(plane) * planeEfficiencyError.at(plane);
  }
  return TMath::Sqrt(rpEfficiencySquareError);
}


float EfficiencyTool_2018::probabilityCalculation(const std::map<uint32_t, float> &planeEfficiency){

  int minNumberOfPlanes = 3;
  int maxNumberOfPlanes = listOfPlanes_.size();
  float rpEfficiency = 1.;

  for(uint32_t i = (uint32_t)minNumberOfPlanes +1; i<=(uint32_t)maxNumberOfPlanes; i++){
    rpEfficiency-=probabilityNplanesBlind(listOfPlanes_, i, planeEfficiency);
  }
  return rpEfficiency;
}

//define this as a plug-in
DEFINE_FWK_MODULE(EfficiencyTool_2018);