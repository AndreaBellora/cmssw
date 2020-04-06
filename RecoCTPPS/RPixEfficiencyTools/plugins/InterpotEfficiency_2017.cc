// -*- C++ -*-
//
// Package:    RecoCTPPS/RPixEfficiencyTools
// Class:      InterpotEfficiency_2017
//
/**\class InterpotEfficiency_2017 InterpotEfficiency_2017.cc
 RecoCTPPS/RPixEfficiencyTools/plugins/InterpotEfficiency_2017.cc

 Description: [one line class summary]

 Implementation:
                 [Notes on implementation]
*/
//
// Original Author:  Andrea Bellora
//         Created:  Wed, 07 Jul 2019 09:55:05 GMT
//
//

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <fstream>
#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/CTPPSDetId/interface/CTPPSPixelDetId.h"

#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelRecHit.h"

#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrackRecoInfo.h"
#include "DataFormats/ProtonReco/interface/ForwardProton.h"
#include "DataFormats/ProtonReco/interface/ForwardProtonFwd.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <TEfficiency.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <TObjArray.h>

class InterpotEfficiency_2017
    : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit InterpotEfficiency_2017(const edm::ParameterSet &);
  ~InterpotEfficiency_2017();
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event &, const edm::EventSetup &) override;
  virtual void endJob() override;
  bool Cut(CTPPSLocalTrackLite track);
  bool isPlaneUsedInTrack(CTPPSPixelLocalTrack track, uint32_t plane);

  bool debug_ = false;

  // z position of the pots (mm)
  std::map<CTPPSDetId, double> Z = {
      {CTPPSDetId(3, 0, 0, 3), -212550}, // strips, arm0, station0, rp3
      {CTPPSDetId(3, 1, 0, 3), 212550},  // strips, arm1, station0, rp3
      {CTPPSDetId(4, 0, 2, 3), -219550}, // pixels, arm0, station2, rp3
      {CTPPSDetId(4, 1, 2, 3), 219550}}; // pixels, arm1, station2, rp3

  // Data to get
  edm::EDGetTokenT<reco::ForwardProtonCollection> protonsToken_;
  edm::EDGetTokenT<reco::ForwardProtonCollection> multiRP_protonsToken_;

  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelRecHit>> pixelRecHitToken_;

  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelLocalTrack>>
      pixelHeavyTrackToken_; // CTPPSPixelLocalTrack-> not Lite

  // Parameter set
  std::string outputFileName_;
  int minNumberOfPlanesForEfficiency_;
  int minNumberOfPlanesForTrack_;
  int maxNumberOfPlanesForTrack_;
  int minTracksPerEvent;
  int maxTracksPerEvent;

  // Configs
  std::vector<uint32_t> listOfArms_ = {0, 1};
  std::vector<uint32_t> listOfStations_ = {2};
  std::vector<uint32_t> listOfPlanes_ = {0, 1, 2, 3, 4, 5};

  std::vector<CTPPSPixelDetId> detectorIdVector_;
  std::vector<CTPPSPixelDetId> romanPotIdVector_;

  int binGroupingX = 1;
  int binGroupingY = 1;

  int mapXbins_st2 = 200;
  float mapXmin_st2 = 0. * TMath::Cos(18.4 / 180. * TMath::Pi());
  float mapXmax_st2 = 30. * TMath::Cos(18.4 / 180. * TMath::Pi());
  int mapYbins_st2 = 240;
  float mapYmin_st2 = -16.;
  float mapYmax_st2 = 8.;

  int mapXbins_st0 = 200;
  float mapXmin_st0 = 0. * TMath::Cos(18.4 / 180. * TMath::Pi());
  float mapXmax_st0 = 30. * TMath::Cos(18.4 / 180. * TMath::Pi());
  int mapYbins_st0 = 240;
  float mapYmin_st0 = -16.;
  float mapYmax_st0 = 8.;

  int mapXbins = mapXbins_st0;
  float mapXmin = mapXmin_st0;
  float mapXmax = mapXmax_st0;
  int mapYbins = mapYbins_st0;
  float mapYmin = mapYmin_st0;
  float mapYmax = mapYmax_st0;

  double xiBins = 44;
  double xiMin = 0.0;
  double xiMax = 0.22;
  double angleBins = 100;
  double angleMin = -0.03;
  double angleMax = 0.03;
  int chiBins = 200;
  double chiMin = 0;
  double chiMax = 6;

  // Maximum difference between xi measurements of the same proton
  double xiMatch = 0.007;
  double yMatch = 1;
  // double xiMatch = 0.01;
  // double yMatch = 99;

  // Number of times that a Tag proton matched more than one Probe
  std::map<CTPPSPixelDetId, uint32_t> overmatches;
  std::map<CTPPSPixelDetId, uint32_t> tries;

  // Number of times that a multiRP track is found when there is a match
  std::map<CTPPSPixelDetId, uint32_t> multiRPfoundWhenMatchFound;
  std::map<CTPPSPixelDetId, uint32_t> multiRPfoundWhenNoMatchFound;
  std::map<CTPPSPixelDetId, uint32_t> triesWithMatch;
  std::map<CTPPSPixelDetId, uint32_t> triesWithoutMatch;

  // std::map<CTPPSPixelDetId, int> binAlignmentParameters = {
  //     {CTPPSPixelDetId(0, 0, 3), 0},
  //     {CTPPSPixelDetId(0, 2, 3), 0},
  //     {CTPPSPixelDetId(1, 0, 3), 0},
  //     {CTPPSPixelDetId(1, 2, 3), 0}};

  // output histograms
  std::map<CTPPSPixelDetId, TH2D *> h2AuxProtonHitDistribution_;
  std::map<CTPPSPixelDetId, TH2D *> h2InterPotEfficiencyMap_;
  std::map<CTPPSPixelDetId, TH1D *> h1AuxXi_;
  std::map<CTPPSPixelDetId, TH1D *> h1XiWhenPurityMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1InterPotEfficiencyVsXi_;
  std::map<CTPPSPixelDetId, TH1D *> h1DeltaXiMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1DeltaYMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1TxMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1TyMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1ProtonsInProbePotWhenNoMatchFound_;
  std::map<CTPPSPixelDetId, TH2D *> h2TxCorrelationMatch_;
  std::map<CTPPSPixelDetId, TH2D *> h2TyCorrelationMatch_;
  std::map<CTPPSPixelDetId, TH2D *> h2XCorrelationMatch_;
  std::map<CTPPSPixelDetId, TH2D *> h2YCorrelationMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1RecoInfoMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1PixelTrackMuxWithStrips_;
  std::map<CTPPSPixelDetId, TH2D *> h2ProtonDistributionWithStrips_;
  std::map<CTPPSPixelDetId, TH2D *> h2ProtonDistributionWithNoPurityMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1RecoInfo_;
  std::map<CTPPSPixelDetId, TH1D *> h1RecoInfoWithPurityMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1PixelTrackMux_;
  std::map<CTPPSPixelDetId, TH2D *> h2ProtonDistribution_;
  std::map<CTPPSPixelDetId, TH2D *> h2MatchingRate_;
  std::map<CTPPSPixelDetId, TH2D *> h2AuxMatchingRate_;
  std::map<CTPPSPixelDetId, TH1D *> h1MatchingRate_;
  std::map<CTPPSPixelDetId, TH1D *> h1AuxMatchingRate_;
  std::map<CTPPSPixelDetId, TH1D *> h1Xi_;
  std::map<CTPPSPixelDetId, TH1D *> h1XProjection_;
  std::map<CTPPSPixelDetId, TH1D *> h1Chi2WhenInterpotMatch_;
  std::map<CTPPSPixelDetId, TH1D *> h1Chi2WhenInterpotMatchANDmultiRP_;
  std::map<CTPPSPixelDetId, TH1D *> h1Chi2WhenInterpotMatchANDNOTmultiRP_;
  std::map<CTPPSPixelDetId, TH1D *> h1Chi2WhenNoInterpotMatchANDmultiRP_;

  std::map<CTPPSDetId, uint32_t> trackMux_;

  // file to insert the output hists in
  TFile *efficiencyFile_;

  std::vector<double> fiducialXLowVector_;
  std::vector<double> fiducialYLowVector_;
  std::vector<double> fiducialYHighVector_;
  std::map<std::pair<int, int>, double> fiducialXLow_;
  std::map<std::pair<int, int>, double> fiducialYLow_;
  std::map<std::pair<int, int>, double> fiducialYHigh_;
  int recoInfoCut_;
  int maxProtonsInPixelRPWithStrips_;
};

InterpotEfficiency_2017::InterpotEfficiency_2017(
    const edm::ParameterSet &iConfig) {
  usesResource("TFileService");
  protonsToken_ = consumes<reco::ForwardProtonCollection>(
      edm::InputTag("ctppsProtons", "singleRP"));
  multiRP_protonsToken_ = consumes<reco::ForwardProtonCollection>(
      edm::InputTag("ctppsProtons", "multiRP"));
  pixelRecHitToken_ = consumes<edm::DetSetVector<CTPPSPixelRecHit>>(
      edm::InputTag("ctppsPixelRecHits", ""));
  pixelHeavyTrackToken_ = consumes<edm::DetSetVector<CTPPSPixelLocalTrack>>(
      edm::InputTag("ctppsPixelLocalTracks", ""));

  outputFileName_ =
      iConfig.getUntrackedParameter<std::string>("outputFileName");
  minNumberOfPlanesForTrack_ =
      iConfig.getParameter<int>("minNumberOfPlanesForTrack");
  maxNumberOfPlanesForTrack_ =
      iConfig.getParameter<int>("maxNumberOfPlanesForTrack");
  minTracksPerEvent = iConfig.getParameter<int>("minTracksPerEvent"); // UNUSED!
  maxTracksPerEvent = iConfig.getParameter<int>("maxTracksPerEvent"); // UNUSED!
  binGroupingX = iConfig.getUntrackedParameter<int>("binGroupingX");  // UNUSED!
  binGroupingY = iConfig.getUntrackedParameter<int>("binGroupingY");  // UNUSED!
  fiducialXLowVector_ =
      iConfig.getUntrackedParameter<std::vector<double>>("fiducialXLow");
  fiducialYLowVector_ =
      iConfig.getUntrackedParameter<std::vector<double>>("fiducialYLow");
  fiducialYHighVector_ =
      iConfig.getUntrackedParameter<std::vector<double>>("fiducialYHigh");
  fiducialXLow_ = {
      {std::pair<int, int>(0, 0), fiducialXLowVector_.at(0)},
      {std::pair<int, int>(0, 2), fiducialXLowVector_.at(1)},
      {std::pair<int, int>(1, 0), fiducialXLowVector_.at(2)},
      {std::pair<int, int>(1, 2), fiducialXLowVector_.at(3)},
  };
  fiducialYLow_ = {
      {std::pair<int, int>(0, 0), fiducialYLowVector_.at(0)},
      {std::pair<int, int>(0, 2), fiducialYLowVector_.at(1)},
      {std::pair<int, int>(1, 0), fiducialYLowVector_.at(2)},
      {std::pair<int, int>(1, 2), fiducialYLowVector_.at(3)},
  };
  fiducialYHigh_ = {
      {std::pair<int, int>(0, 0), fiducialYHighVector_.at(0)},
      {std::pair<int, int>(0, 2), fiducialYHighVector_.at(1)},
      {std::pair<int, int>(1, 0), fiducialYHighVector_.at(2)},
      {std::pair<int, int>(1, 2), fiducialYHighVector_.at(3)},
  };
  recoInfoCut_ = iConfig.getUntrackedParameter<int>("recoInfo");
  maxProtonsInPixelRPWithStrips_ =
      iConfig.getUntrackedParameter<int>("maxProtonsInPixelRP");
  debug_ = iConfig.getUntrackedParameter<bool>("debug");
}

InterpotEfficiency_2017::~InterpotEfficiency_2017() {
  for (auto &rpId : romanPotIdVector_) {
    delete h2AuxProtonHitDistribution_[rpId];
    delete h2InterPotEfficiencyMap_[rpId];
    delete h1AuxXi_[rpId];
    delete h1XiWhenPurityMatch_[rpId];
    delete h1InterPotEfficiencyVsXi_[rpId];
    delete h1DeltaXiMatch_[rpId];
    delete h1DeltaYMatch_[rpId];
    delete h1TxMatch_[rpId];
    delete h1TyMatch_[rpId];
    delete h1ProtonsInProbePotWhenNoMatchFound_[rpId];
    delete h2TxCorrelationMatch_[rpId];
    delete h2TyCorrelationMatch_[rpId];
    delete h2XCorrelationMatch_[rpId];
    delete h2YCorrelationMatch_[rpId];
    delete h1PixelTrackMuxWithStrips_[rpId];
    delete h2ProtonDistributionWithStrips_[rpId];
    delete h2ProtonDistributionWithNoPurityMatch_[rpId];
    delete h1RecoInfo_[rpId];
    delete h1RecoInfoWithPurityMatch_[rpId];
    delete h1PixelTrackMux_[rpId];
    delete h2ProtonDistribution_[rpId];
    delete h2MatchingRate_[rpId];
    delete h2AuxMatchingRate_[rpId];
    delete h1MatchingRate_[rpId];
    delete h1AuxMatchingRate_[rpId];
    delete h1RecoInfoMatch_[rpId];
    delete h1Xi_[rpId];
    delete h1XProjection_[rpId];
    delete h1Chi2WhenInterpotMatch_[rpId];
    delete h1Chi2WhenInterpotMatchANDmultiRP_[rpId];
    delete h1Chi2WhenInterpotMatchANDNOTmultiRP_[rpId];
    delete h1Chi2WhenNoInterpotMatchANDmultiRP_[rpId];
  }
}

void InterpotEfficiency_2017::analyze(const edm::Event &iEvent,
                                      const edm::EventSetup &iSetup) {
  using namespace edm;

  Handle<edm::DetSetVector<CTPPSPixelRecHit>> pixelRecHits;
  iEvent.getByToken(pixelRecHitToken_, pixelRecHits);

  Handle<reco::ForwardProtonCollection> protons;
  iEvent.getByToken(protonsToken_, protons);

  Handle<reco::ForwardProtonCollection> multiRP_protons;
  iEvent.getByToken(multiRP_protonsToken_, multiRP_protons);

  Handle<edm::DetSetVector<CTPPSPixelLocalTrack>> pixelHeavyTracks;
  iEvent.getByToken(pixelHeavyTrackToken_, pixelHeavyTracks);

  std::map<CTPPSPixelDetId, int> mux;

  if (debug_)
    std::cout << "Event ID: " << iEvent.id() << std::endl;

  // Compute proton mux
  for (auto &proton : *protons) {
    if (!proton.validFit() ||
        proton.method() != reco::ForwardProton::ReconstructionMethod::singleRP)
      continue;
    CTPPSLocalTrackLite track = *(proton.contributingLocalTracks().at(0));
    CTPPSDetId detId = CTPPSDetId(track.getRPId());
    int arm = detId.arm();
    int station = detId.station();
    int rp = detId.rp();
    if (detId.station() != 0) {
      CTPPSPixelDetId pixelDetId(detId.rawId());
      if (h1PixelTrackMux_.find(pixelDetId) == h1PixelTrackMux_.end()) {
        romanPotIdVector_.push_back(pixelDetId);
        h1PixelTrackMux_[pixelDetId] = new TH1D(
            Form("h1PixelTrackMux_arm%i_st%i_rp%i", arm, station, rp),
            Form("h1PixelTrackMux_arm%i_st%i_rp%i;Protons", arm, station, rp),
            11, -0.5, 10.5);
      }
      mux[pixelDetId]++;
    }
  }

  for (auto &pixelDetId : romanPotIdVector_) {
    h1PixelTrackMux_[pixelDetId]->Fill(mux[pixelDetId]);
  }

  // Inter-Pot efficiency
  for (auto &proton_Tag : *protons) {
    if (!proton_Tag.validFit() ||
        proton_Tag.method() !=
            reco::ForwardProton::ReconstructionMethod::singleRP)
      continue;
    CTPPSLocalTrackLite track_Tag =
        *(proton_Tag.contributingLocalTracks().at(0));
    CTPPSDetId detId_Tag = CTPPSDetId(track_Tag.getRPId());
    int arm_Tag = detId_Tag.arm();
    int station_Tag = detId_Tag.station();
    int rp_Tag = detId_Tag.rp();
    double trackX0_Tag = track_Tag.getX();
    double trackY0_Tag = track_Tag.getY();
    double trackTx_Tag = track_Tag.getTx();
    double trackTy_Tag = track_Tag.getTy();
    double xi_Tag = proton_Tag.xi();

    if (detId_Tag.station() != 0) {
      // Here the tag proton can only be a pixel proton
      // Stuff to be done only on proton tracks
      CTPPSPixelDetId pixelDetId(detId_Tag.rawId());
      if (debug_)
        std::cout << "Analyzing tag track (PIXELS):\n"
                  << "Arm: " << arm_Tag << " Station: " << station_Tag
                  << " X: " << trackX0_Tag << " Y: " << trackY0_Tag
                  << " Tx: " << trackTx_Tag << " Ty: " << trackTy_Tag
                  << " Xi: " << xi_Tag << std::endl;

      if (h1RecoInfo_.find(pixelDetId) == h1RecoInfo_.end()) {
        mapXbins = mapXbins_st2;
        mapXmin = mapXmin_st2;
        mapXmax = mapXmax_st2;
        mapYbins = mapYbins_st2;
        mapYmin = mapYmin_st2;
        mapYmax = mapYmax_st2;

        h1RecoInfo_[pixelDetId] = new TH1D(
            Form("h1RecoInfo_arm%i_st%i_rp%i", arm_Tag, station_Tag, rp_Tag),
            Form("h1RecoInfo_arm%i_st%i_rp%i;recoInfo", arm_Tag, station_Tag,
                 rp_Tag),
            6, -0.5, 5.5);
        h1RecoInfoWithPurityMatch_[pixelDetId] =
            new TH1D(Form("h1RecoInfoWithPurityMatch_arm%i_st%i_rp%i", arm_Tag,
                          station_Tag, rp_Tag),
                     Form("h1RecoInfoWithPurityMatch_arm%i_st%i_rp%i;recoInfo",
                          arm_Tag, station_Tag, rp_Tag),
                     6, -0.5, 5.5);
        h2ProtonDistribution_[pixelDetId] =
            new TH2D(Form("h2ProtonDistribution_arm%i_st%i_rp%i", arm_Tag,
                          station_Tag, rp_Tag),
                     Form("h2ProtonDistribution_arm%i_st%i_rp%i;x (mm);y (mm)",
                          arm_Tag, station_Tag, rp_Tag),
                     mapXbins, mapXmin, mapXmax, mapYbins, mapYmin, mapYmax);
        h2ProtonDistributionWithNoPurityMatch_[pixelDetId] = new TH2D(
            Form("h2ProtonDistributionWithNoPurityMatch_arm%i_st%i_rp%i",
                 arm_Tag, station_Tag, rp_Tag),
            Form("h2ProtonDistributionWithNoPurityMatch_arm%i_st%i_rp%i;x "
                 "(mm);y (mm)",
                 arm_Tag, station_Tag, rp_Tag),
            mapXbins, mapXmin, mapXmax, mapYbins, mapYmin, mapYmax);
        h1Xi_[pixelDetId] = new TH1D(
            Form("h1Xi_arm%i_st%i_rp%i", arm_Tag, station_Tag, rp_Tag),
            Form("h1Xi_arm%i_st%i_rp%i;#xi", arm_Tag, station_Tag, rp_Tag),
            xiBins, xiMin, xiMax);
        h1XiWhenPurityMatch_[pixelDetId] =
            new TH1D(Form("h1XiWhenPurityMatch_arm%i_st%i_rp%i", arm_Tag,
                          station_Tag, rp_Tag),
                     Form("h1XiWhenPurityMatch_arm%i_st%i_rp%i;#xi", arm_Tag,
                          station_Tag, rp_Tag),
                     xiBins, xiMin, xiMax);
        h1XProjection_[pixelDetId] = new TH1D(
            Form("h1XProjection_arm%i_st%i_rp%i", arm_Tag, station_Tag, rp_Tag),
            Form("h1XProjection_arm%i_st%i_rp%i;x (mm)", arm_Tag, station_Tag,
                 rp_Tag),
            mapXbins, mapXmin, mapXmax);
        h2MatchingRate_[pixelDetId] =
            new TH2D(Form("h2MatchingRate_arm%i_st%i_rp%i", arm_Tag,
                          station_Tag, rp_Tag),
                     Form("h2MatchingRate_arm%i_st%i_rp%i;x (mm);y (mm)",
                          arm_Tag, station_Tag, rp_Tag),
                     mapXbins, mapXmin, mapXmax, mapYbins, mapYmin, mapYmax);
        h2AuxMatchingRate_[pixelDetId] =
            new TH2D(Form("h2AuxMatchingRate_arm%i_st%i_rp%i", arm_Tag,
                          station_Tag, rp_Tag),
                     Form("h2AuxMatchingRate_arm%i_st%i_rp%i;x (mm);y (mm)",
                          arm_Tag, station_Tag, rp_Tag),
                     mapXbins, mapXmin, mapXmax, mapYbins, mapYmin, mapYmax);
        h1MatchingRate_[pixelDetId] =
            new TH1D(Form("h1MatchingRate_arm%i_st%i_rp%i", arm_Tag,
                          station_Tag, rp_Tag),
                     Form("h1MatchingRate_arm%i_st%i_rp%i;#xi;Matching Rate",
                          arm_Tag, station_Tag, rp_Tag),
                     xiBins, xiMin, xiMax);
        h1AuxMatchingRate_[pixelDetId] =
            new TH1D(Form("h1AuxMatchingRate_arm%i_st%i_rp%i", arm_Tag,
                          station_Tag, rp_Tag),
                     Form("h1AuxMatchingRate_arm%i_st%i_rp%i;#xi;Matching Rate",
                          arm_Tag, station_Tag, rp_Tag),
                     xiBins, xiMin, xiMax);
      }
      if (mux[pixelDetId] > maxTracksPerEvent ||
          mux[pixelDetId] < minTracksPerEvent) {
        if (debug_)
          std::cout << "Mux cut not passed" << std::endl;
        continue;
      }

      if (Cut(track_Tag)) {
        if (debug_)
          std::cout << "Pixel track did not pass the cut" << std::endl;
        continue;
      }

      h1RecoInfo_[pixelDetId]->Fill((int)track_Tag.getPixelTrackRecoInfo());
      h2ProtonDistribution_[pixelDetId]->Fill(trackX0_Tag, trackY0_Tag);
      h1Xi_[pixelDetId]->Fill(proton_Tag.xi());
      h1XProjection_[pixelDetId]->Fill(trackX0_Tag);
      h2AuxMatchingRate_[pixelDetId]->Fill(trackX0_Tag, trackY0_Tag);
      h1AuxMatchingRate_[pixelDetId]->Fill(xi_Tag);

      int matches = 0;
      // Try to match with strips track to check purity
      for (auto &proton_Probe_purity :
           *protons) { // Probe -> Roman Pot Under Test

        if (!proton_Probe_purity.validFit() ||
            proton_Probe_purity.method() !=
                reco::ForwardProton::ReconstructionMethod::singleRP)
          continue;

        CTPPSLocalTrackLite track_Probe_purity =
            *(proton_Probe_purity.contributingLocalTracks().at(0));
        CTPPSDetId detId_Probe_purity =
            CTPPSDetId(track_Probe_purity.getRPId());

        double trackY0_Probe_purity = track_Probe_purity.getY();
        double xi_Probe_purity = proton_Probe_purity.xi();
        int arm_Probe_purity = detId_Probe_purity.arm();
        uint32_t station_Probe_purity = detId_Probe_purity.station();

        if (station_Probe_purity != 0 || arm_Probe_purity != arm_Tag)
          continue;

        if (debug_)
          std::cout << "Analyzing probe purity track (STRIPS):\n"
                    << "Arm: " << arm_Probe_purity
                    << " Station: " << station_Probe_purity
                    << " Xi: " << xi_Probe_purity
                    << " Y: " << trackY0_Probe_purity << std::endl;

        if (Cut(track_Probe_purity)) {
          if (debug_)
            std::cout << "Strips track did not pass the cut" << std::endl;
          continue;
        }

        if (TMath::Abs(xi_Tag - xi_Probe_purity) < xiMatch &&
            TMath::Abs(trackY0_Tag - trackY0_Probe_purity) < yMatch) {
          h2MatchingRate_[pixelDetId]->Fill(trackX0_Tag, trackY0_Tag);
          h1MatchingRate_[pixelDetId]->Fill(xi_Tag);
          h1XiWhenPurityMatch_[pixelDetId]->Fill(xi_Tag);
          h1RecoInfoWithPurityMatch_[pixelDetId]->Fill(
              (int)track_Tag.getPixelTrackRecoInfo());

          matches++;
          if (debug_)
            std::cout << "Purity match found!" << std::endl;
        } else {
          if (debug_) {
            std::cout << "***Failed match found for arm: ***" << arm_Tag
                      << " Event number: " << iEvent.id().event() << std::endl;
            std::cout << "Analyzing tag track (PIXELS):\n"
                      << "Arm: " << arm_Tag << " Station: " << station_Tag
                      << " X: " << trackX0_Tag << " Y: " << trackY0_Tag
                      << " Tx: " << trackTx_Tag << " Ty: " << trackTy_Tag
                      << " Xi: " << xi_Tag << std::endl;
            std::cout << "Analyzing probe purity track (STRIPS):\n"
                      << "Arm: " << arm_Probe_purity
                      << " Station: " << station_Probe_purity
                      << " Xi: " << xi_Probe_purity
                      << " Y: " << trackY0_Probe_purity << std::endl;
            std::cout << "**********************************" << std::endl;
          }
        }
      } // for proton_Probe_purity in protons
      if (matches == 0) {
        h2ProtonDistributionWithNoPurityMatch_[pixelDetId]->Fill(trackX0_Tag,
                                                                 trackY0_Tag);
        if (debug_)
          std::cout << "Failed match found for arm: " << arm_Tag
                    << " Event number: " << iEvent.id().event() << std::endl;
      }
      continue;
    } // Stuff to be done only on proton tracks
    // Here proton_Tag can only be a strips proton

    if (debug_)
      std::cout << "Analyzing tag track (STRIPS):\n"
                << "Arm: " << arm_Tag << " Station: " << station_Tag
                << " X: " << trackX0_Tag << " Y: " << trackY0_Tag
                << " Tx: " << trackTx_Tag << " Ty: " << trackTy_Tag
                << std::endl;

    // Apply the cuts
    if (Cut(track_Tag)) {
      if (debug_)
        std::cout << "Strips track did not pass the cut" << std::endl;
      continue;
    }

    uint32_t arm_Probe = detId_Tag.arm();
    uint32_t station_Probe = (detId_Tag.station() == 0) ? 2 : 0;
    uint32_t rp_Probe = detId_Tag.rp();

    // CTPPSPixelDetId that the probe proton must have
    CTPPSPixelDetId pixelDetId(arm_Probe, station_Probe, rp_Probe);
    CTPPSDetId detId_Probe(pixelDetId.rawId());

    double deltaZ = Z[detId_Probe] - Z[detId_Tag];
    // double expectedTrackX0_Probe = 0;
    // double expectedTrackY0_Probe = 0;
    double expectedTrackX0_Probe = trackX0_Tag; //+ trackTx_Tag * deltaZ;
    double expectedTrackY0_Probe = trackY0_Tag; //+ trackTy_Tag * deltaZ;
    int protonsInProbePot = 0;

    // Booking histograms
    if (h2InterPotEfficiencyMap_.find(pixelDetId) ==
        h2InterPotEfficiencyMap_.end()) {
      if (station_Probe == 2) {
        mapXbins = mapXbins_st2;
        mapXmin = mapXmin_st2;
        mapXmax = mapXmax_st2;
        mapYbins = mapYbins_st2;
        mapYmin = mapYmin_st2;
        mapYmax = mapYmax_st2;
      } else {
        mapXbins = mapXbins_st0;
        mapXmin = mapXmin_st0;
        mapXmax = mapXmax_st0;
        mapYbins = mapYbins_st0;
        mapYmin = mapYmin_st0;
        mapYmax = mapYmax_st0;
      }
      h2AuxProtonHitDistribution_[pixelDetId] = new TH2D(
          Form("h2ProtonHitExpectedDistribution_arm%i_st%i_rp%i", arm_Probe,
               station_Probe, rp_Probe),
          Form("h2ProtonHitExpectedDistribution_arm%i_st%i_rp%i;x (mm);y (mm)",
               arm_Probe, station_Probe, rp_Probe),
          mapXbins, mapXmin, mapXmax, mapYbins, mapYmin, mapYmax);
      h2InterPotEfficiencyMap_[pixelDetId] =
          new TH2D(Form("h2InterPotEfficiencyMap_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h2InterPotEfficiencyMap_arm%i_st%i_rp%i;x (mm);y (mm)",
                        arm_Probe, station_Probe, rp_Probe),
                   mapXbins, mapXmin, mapXmax, mapYbins, mapYmin, mapYmax);
      h1InterPotEfficiencyVsXi_[pixelDetId] = new TH1D(
          Form("h1InterPotEfficiencyVsXi_arm%i_st%i_rp%i", arm_Probe,
               station_Probe, rp_Probe),
          Form("h1InterPotEfficiencyVsXi_arm%i_st%i_rp%i;#xi;Efficiency",
               arm_Probe, station_Probe, rp_Probe),
          xiBins, xiMin, xiMax);
      h1AuxXi_[pixelDetId] = new TH1D(
          Form("h1AuxXi_arm%i_st%i_rp%i", arm_Probe, station_Probe, rp_Probe),
          Form("h1AuxXi_arm%i_st%i_rp%i;#xi;Efficiency", arm_Probe,
               station_Probe, rp_Probe),
          xiBins, xiMin, xiMax);
      h1DeltaXiMatch_[pixelDetId] =
          new TH1D(Form("h1DeltaXiMatch_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h1DeltaXiMatch_arm%i_st%i_rp%i;#Delta_{#xi}",
                        arm_Probe, station_Probe, rp_Probe),
                   100, -0.02, 0.02);
      h1DeltaYMatch_[pixelDetId] =
          new TH1D(Form("h1DeltaYMatch_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h1DeltaYMatch_arm%i_st%i_rp%i;#Delta_{y}", arm_Probe,
                        station_Probe, rp_Probe),
                   100, -5, 5);
      h1TxMatch_[pixelDetId] = new TH1D(
          Form("h1TxMatch_arm%i_st%i_rp%i", arm_Probe, station_Probe, rp_Probe),
          Form("h1TxMatch_arm%i_st%i_rp%i;Tx", arm_Probe, station_Probe,
               rp_Probe),
          100, -0.02, 0.02);
      h1TyMatch_[pixelDetId] = new TH1D(
          Form("h1TyMatch_arm%i_st%i_rp%i", arm_Probe, station_Probe, rp_Probe),
          Form("h1TyMatch_arm%i_st%i_rp%i;Ty", arm_Probe, station_Probe,
               rp_Probe),
          100, -0.02, 0.02);
      h1ProtonsInProbePotWhenNoMatchFound_[pixelDetId] =
          new TH1D(Form("h1ProtonsInProbePotWhenNoMatchFound_arm%i_st%i_rp%i",
                        arm_Probe, station_Probe, rp_Probe),
                   Form("h1ProtonsInProbePotWhenNoMatchFound_arm%i_st%i_rp%i",
                        arm_Probe, station_Probe, rp_Probe),
                   11, 0, 11);
      h2XCorrelationMatch_[pixelDetId] =
          new TH2D(Form("h2XCorrelationMatch_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h2XCorrelationMatch_arm%i_st%i_rp%i;x pixel (mm);x "
                        "strips (mm)",
                        arm_Probe, station_Probe, rp_Probe),
                   mapXbins, mapXmin, mapXmax, mapXbins, mapXmin, mapXmax);
      h2YCorrelationMatch_[pixelDetId] =
          new TH2D(Form("h2YCorrelationMatch_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h2YCorrelationMatch_arm%i_st%i_rp%i;y pixel (mm);y "
                        "strips (mm)",
                        arm_Probe, station_Probe, rp_Probe),
                   mapYbins, mapYmin, mapYmax, mapYbins, mapYmin, mapYmax);
      h2TxCorrelationMatch_[pixelDetId] =
          new TH2D(Form("h2TxCorrelationMatch_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h2TxCorrelationMatch_arm%i_st%i_rp%i;Tx pixel (mm);Tx "
                        "pixel (mm)",
                        arm_Probe, station_Probe, rp_Probe),
                   100, -0.01, 0.01, 100, -0.01, 0.01);
      h2TyCorrelationMatch_[pixelDetId] =
          new TH2D(Form("h2TyCorrelationMatch_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h2TyCorrelationMatch_arm%i_st%i_rp%i;Tx pixel (mm);Ty "
                        "pixel (mm)",
                        arm_Probe, station_Probe, rp_Probe),
                   100, -0.01, 0.01, 100, -0.01, 0.01);
      h1PixelTrackMuxWithStrips_[pixelDetId] =
          new TH1D(Form("h1PixelTrackMuxWithStrips_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h1PixelTrackMuxWithStrips_arm%i_st%i_rp%i;Protons",
                        arm_Probe, station_Probe, rp_Probe),
                   12, -0.5, 11.5);
      h2ProtonDistributionWithStrips_[pixelDetId] = new TH2D(
          Form("h2ProtonDistributionWithStrips_arm%i_st%i_rp%i", arm_Probe,
               station_Probe, rp_Probe),
          Form("h2ProtonDistributionWithStrips_arm%i_st%i_rp%i;x (mm);y (mm)",
               arm_Probe, station_Probe, rp_Probe),
          mapXbins, mapXmin, mapXmax, mapYbins, mapYmin, mapYmax);
      h1RecoInfoMatch_[pixelDetId] = new TH1D(
          Form("h1RecoInfoMatch_arm%i_st%i_rp%i", arm_Tag, station_Tag, rp_Tag),
          Form("h1RecoInfoMatch_arm%i_st%i_rp%i;recoInfo", arm_Tag, station_Tag,
               rp_Tag),
          6, -0.5, 5.5);
      h1Chi2WhenInterpotMatch_[pixelDetId] =
          new TH1D(Form("h1Chi2WhenInterpotMatch_arm%i_st%i_rp%i", arm_Probe,
                        station_Probe, rp_Probe),
                   Form("h1Chi2WhenInterpotMatch_arm%i_st%i_rp%i;#chi^{2}",
                        arm_Probe, station_Probe, rp_Probe),
                   chiBins, chiMin, chiMax);
      h1Chi2WhenInterpotMatchANDmultiRP_[pixelDetId] = new TH1D(
          Form("h1Chi2WhenInterpotMatchANDmultiRP_arm%i_st%i_rp%i", arm_Probe,
               station_Probe, rp_Probe),
          Form("h1Chi2WhenInterpotMatchANDmultiRP_arm%i_st%i_rp%i;#chi^{2}",
               arm_Probe, station_Probe, rp_Probe),
          chiBins, chiMin, chiMax);
      h1Chi2WhenInterpotMatchANDNOTmultiRP_[pixelDetId] = new TH1D(
          Form("h1Chi2WhenInterpotMatchANDNOTmultiRP_arm%i_st%i_rp%i",
               arm_Probe, station_Probe, rp_Probe),
          Form("h1Chi2WhenInterpotMatchANDNOTmultiRP_arm%i_st%i_rp%i;#chi^{2}",
               arm_Probe, station_Probe, rp_Probe),
          chiBins, chiMin, chiMax);
      h1Chi2WhenNoInterpotMatchANDmultiRP_[pixelDetId] = new TH1D(
          Form("h1Chi2WhenNoInterpotMatchANDmultiRP_arm%i_st%i_rp%i", arm_Probe,
               station_Probe, rp_Probe),
          Form("h1Chi2WhenNoInterpotMatchANDmultiRP_arm%i_st%i_rp%i;#chi^{2}",
               arm_Probe, station_Probe, rp_Probe),
          chiBins, chiMin, chiMax);
      overmatches[pixelDetId] = 0;
      tries[pixelDetId] = 0;
      multiRPfoundWhenMatchFound[pixelDetId] = 0;
      multiRPfoundWhenNoMatchFound[pixelDetId] = 0;
      triesWithMatch[pixelDetId] = 0;
      triesWithoutMatch[pixelDetId] = 0;
    }

    // Count protons in the probed RP
    for (auto &proton_Probe : *protons) { // Probe -> Roman Pot Under Test
      if (!proton_Probe.validFit() ||
          proton_Probe.method() !=
              reco::ForwardProton::ReconstructionMethod::singleRP)
        continue;
      CTPPSLocalTrackLite track_Probe =
          *(proton_Probe.contributingLocalTracks().at(0));

      if (detId_Probe != track_Probe.getRPId())
        continue;

      if (Cut(track_Probe)) {
        if (debug_)
          // std::cout << "Pixel track did not pass the cut" << std::endl;
          continue;
      }

      protonsInProbePot++;
    }
    h1PixelTrackMuxWithStrips_[pixelDetId]->Fill(protonsInProbePot);

    if (protonsInProbePot >
        maxProtonsInPixelRPWithStrips_) // Select events with no more than
                                        // maxProtonsInPixelRP protons in the
                                        // pixel RP
      continue;

    // recoInfo and Chi2 of the matched proton or, alternatively, the one
    // selected by multiRP
    double pixeltrack_Chi2 = -1;
    int pixeltrack_recoInfo = 5;

    // Logic to assert if a multiRP_proton has been found too
    uint32_t multiRPmatchFound = 0;
    for (auto &multiRP_proton : *multiRP_protons) {
      if (!multiRP_proton.validFit() ||
          multiRP_proton.method() !=
              reco::ForwardProton::ReconstructionMethod::multiRP)
        continue;

      if (debug_) {
        std::cout << "***Analyzing multiRP proton***" << std::endl;
        std::cout << "Xi = " << multiRP_proton.xi() << std::endl;
      }

      for (auto &track_ptr : multiRP_proton.contributingLocalTracks()) {
        CTPPSLocalTrackLite track = *track_ptr;
        CTPPSDetId detId = CTPPSDetId(track.getRPId());
        int arm = detId.arm();
        int station = detId.station();
        double trackX0 = track.getX();
        double trackY0 = track.getY();
        double trackTx = track.getTx();
        double trackTy = track.getTy();

        if (debug_) {
          std::cout << "Composed by track: " << std::endl;
          std::cout << "Arm: " << arm << " Station: " << station << std::endl
                    << " X: " << trackX0 << " Y: " << trackY0
                    << " Tx: " << trackTx << " Ty: " << trackTy
                    << " recoInfo: " << (int)track.getPixelTrackRecoInfo()
                    << std::endl;
        }

        if (arm == arm_Tag && station == 0 &&
            TMath::Abs(trackX0_Tag - trackX0) < 0.1 &&
            TMath::Abs(trackY0_Tag - trackY0) < 0.1) {
          multiRPmatchFound++;
          // Find pixel track and retrieve chi2
          for (auto &pixeltrack : multiRP_proton.contributingLocalTracks()) {
            if (CTPPSDetId(pixeltrack->getRPId()).station() == 2) {
              pixeltrack_Chi2 = pixeltrack->getChiSquaredOverNDF();
              // pixeltrack->getChiSquaredOverNDF() *
              // (2 * pixeltrack->getNumberOfPointsUsedForFit() - 4);
              pixeltrack_recoInfo = (int)pixeltrack->getPixelTrackRecoInfo();
            }
          }
        }
      }

      if (debug_)
        std::cout << "******************************" << std::endl;
    }

    int matches = 0;
    for (auto &proton_Probe : *protons) { // Probe -> Roman Pot Under Test
      if (!proton_Probe.validFit() ||
          proton_Probe.method() !=
              reco::ForwardProton::ReconstructionMethod::singleRP)
        continue;
      CTPPSLocalTrackLite track_Probe =
          *(proton_Probe.contributingLocalTracks().at(0));
      // CTPPSDetId detId_Probe = CTPPSDetId(track_Probe.getRPId());
      double trackX0_Probe = track_Probe.getX();
      double trackY0_Probe = track_Probe.getY();
      double trackTx_Probe = track_Probe.getTx();
      double trackTy_Probe = track_Probe.getTy();
      double xi_Probe = proton_Probe.xi();

      // Require the proton_Probe to be in the same arm, different station
      // This means that the CTPPSPixelDetId is the same as above
      // if (detId_Tag.station() == detId_Probe.station() ||
      //     detId_Tag.arm() != detId_Probe.arm())

      if (detId_Probe != track_Probe.getRPId())
        continue;

      if (debug_)
        std::cout << "Analyzing probe track (PIXELS):\n"
                  << "Arm: " << detId_Probe.arm()
                  << " Station: " << detId_Probe.station()
                  << " X: " << trackX0_Probe << " Y: " << trackY0_Probe
                  << " Chi2OverNDF: " << track_Probe.getChiSquaredOverNDF()
                  << "\nDeltaZ: " << deltaZ
                  << " Expected X: " << expectedTrackX0_Probe
                  << " Expected Y: " << expectedTrackY0_Probe
                  << " RecoInfo: " << (int)track_Probe.getPixelTrackRecoInfo()
                  << std::endl;

      // Apply the cuts
      if (Cut(track_Probe)) {
        if (debug_)
          std::cout << "Pixel track did not pass the cut" << std::endl;
        continue;
      }

      if (debug_)
        std::cout << "Looking for CTPPSPixelLocalTrack corresponding to "
                     "CTPPSLocalTrackLite"
                  << std::endl;

      auto rpPixelHeavyTracks = (*pixelHeavyTracks)[track_Probe.getRPId()];
      CTPPSPixelLocalTrack associatedHeavyTrack;
      bool associationFound = false;
      for (auto &pixelHeavyTrack : rpPixelHeavyTracks) {
        if (TMath::Abs(pixelHeavyTrack.getX0() - trackX0_Probe) < 0.1 &&
            TMath::Abs(pixelHeavyTrack.getY0() - trackY0_Probe) < 0.1 &&
            !associationFound) {
          associatedHeavyTrack = pixelHeavyTrack;
          associationFound = true;
          if (debug_)
            std::cout << "Found track to be associated" << std::endl;
        }
      }

      if (!associationFound) {
        std::cout << "WARNING: No track to be associated found. Skipping event"
                  << std::endl;
        std::cout << "Coordinate\tTrackLite\tHeavyTrack" << std::endl;
        std::cout << "X:\t\t" << trackX0_Probe << "\t\t"
                  << associatedHeavyTrack.getX0() << std::endl;
        std::cout << "Y:\t\t" << trackY0_Probe << "\t\t"
                  << associatedHeavyTrack.getY0() << std::endl;
        return;
      }

      h2ProtonDistributionWithStrips_[pixelDetId]->Fill(trackX0_Probe,
                                                        trackY0_Probe);
      h1DeltaXiMatch_[pixelDetId]->Fill(xi_Tag - xi_Probe);

      if (TMath::Abs(xi_Tag - xi_Probe) < xiMatch)
        h1DeltaYMatch_[pixelDetId]->Fill(trackY0_Tag - trackY0_Probe);

      if (TMath::Abs(xi_Tag - xi_Probe) < xiMatch &&
          TMath::Abs(trackY0_Tag - trackY0_Probe) < yMatch) {
        matches++;
        if (matches == 1) {
          pixeltrack_Chi2 = track_Probe.getChiSquaredOverNDF();
          // track_Probe.getChiSquaredOverNDF() *
          // (2 * track_Probe.getNumberOfPointsUsedForFit() - 4);
          pixeltrack_recoInfo = (int)track_Probe.getPixelTrackRecoInfo();

          if (debug_) {
            std::cout << "********MATCH FOUND********" << std::endl;
            std::cout << "Tag track:\n"
                      << "Arm: " << detId_Tag.arm()
                      << " Station: " << detId_Tag.station()
                      << " X: " << trackX0_Tag << " Y: " << trackY0_Tag
                      << " Tx: " << trackTx_Tag << " Ty: " << trackTy_Tag
                      << std::endl;
            std::cout << "Probe track:\n"
                      << "Arm: " << detId_Probe.arm()
                      << " Station: " << detId_Probe.station()
                      << " X: " << trackX0_Probe << " Y: " << trackY0_Probe
                      << "\nDeltaZ: " << deltaZ
                      << " Expected X: " << expectedTrackX0_Probe
                      << " Expected Y: " << expectedTrackY0_Probe
                      << " RecoInfo: "
                      << (int)track_Probe.getPixelTrackRecoInfo() << std::endl;
            std::cout << "**************************" << std::endl;
          }

          // THIS SHOULD BE COMMENTED OUT
          // Requiring plane 0 1 and 2 in the track to be efficient
          // if (isPlaneUsedInTrack(associatedHeavyTrack, 0) &&
          //     isPlaneUsedInTrack(associatedHeavyTrack, 1) &&
          //     isPlaneUsedInTrack(associatedHeavyTrack, 3)) {
          //   if (debug_)
          //     std::cout << "Planes 0,1,3 are used" << std::endl;
          //   h2InterPotEfficiencyMap_[pixelDetId]->Fill(expectedTrackX0_Probe,
          //                                              expectedTrackY0_Probe);
          //   h1InterPotEfficiencyVsXi_[pixelDetId]->Fill(
          //       xi_Tag); // xi_Tag and xi_Probe are expected to be the
          //                // same
          // } else if (debug_) {
          //   std::cout << "One of planes 1,2,3 is not used" << std::endl;
          //   if (isPlaneUsedInTrack(associatedHeavyTrack, 0) ||
          //       isPlaneUsedInTrack(associatedHeavyTrack, 1) ||
          //       isPlaneUsedInTrack(associatedHeavyTrack, 2) ||
          //       isPlaneUsedInTrack(associatedHeavyTrack, 3) ||
          //       isPlaneUsedInTrack(associatedHeavyTrack, 4) ||
          //       isPlaneUsedInTrack(associatedHeavyTrack, 5))
          //     std::cout << "NONE of the planes is used. WTF??" <<
          //     std::endl;
          // }
          // THIS SHOULD BE COMMENTED OUT

          h2InterPotEfficiencyMap_[pixelDetId]->Fill(expectedTrackX0_Probe,
                                                     expectedTrackY0_Probe);
          h1InterPotEfficiencyVsXi_[pixelDetId]->Fill(
              xi_Tag); // xi_Tag and xi_Probe are expected to be the same
          h1TxMatch_[pixelDetId]->Fill(trackTx_Tag);
          h1TyMatch_[pixelDetId]->Fill(trackTy_Tag);
          h2XCorrelationMatch_[pixelDetId]->Fill(trackX0_Probe, trackX0_Tag);
          h2YCorrelationMatch_[pixelDetId]->Fill(trackY0_Probe, trackY0_Tag);
          h2TxCorrelationMatch_[pixelDetId]->Fill(trackTx_Probe, trackTx_Tag);
          h2TyCorrelationMatch_[pixelDetId]->Fill(trackTy_Probe, trackTy_Tag);
          h1RecoInfoMatch_[pixelDetId]->Fill(
              (int)track_Probe.getPixelTrackRecoInfo());
        }
      }
    }

    h2AuxProtonHitDistribution_[pixelDetId]->Fill(expectedTrackX0_Probe,
                                                  expectedTrackY0_Probe);
    h1AuxXi_[pixelDetId]->Fill(xi_Tag);

    if (matches > 1) {
      overmatches[pixelDetId]++;
      if (debug_)
        std::cout << "***WARNING: Overmatching!***" << std::endl;
    }
    tries[pixelDetId]++;

    if (matches == 1 &&
        (pixeltrack_recoInfo == recoInfoCut_ || recoInfoCut_ == -1)) {
      triesWithMatch[pixelDetId]++;
      h1Chi2WhenInterpotMatch_[pixelDetId]->Fill(pixeltrack_Chi2);
    }
    if (matches == 0 &&
        (pixeltrack_recoInfo == recoInfoCut_ || recoInfoCut_ == -1)) {
      triesWithoutMatch[pixelDetId]++;
    }

    if (matches == 1 && multiRPmatchFound >= 1 &&
        (pixeltrack_recoInfo == recoInfoCut_ || recoInfoCut_ == -1)) {
      multiRPfoundWhenMatchFound[pixelDetId]++;
      h1Chi2WhenInterpotMatchANDmultiRP_[pixelDetId]->Fill(pixeltrack_Chi2);
      if (debug_)
        std::cout << "*****MultiRP track found while matching!!!*****"
                  << std::endl;
    } else if (matches == 1 &&
               (pixeltrack_recoInfo == recoInfoCut_ || recoInfoCut_ == -1)) {
      h1Chi2WhenInterpotMatchANDNOTmultiRP_[pixelDetId]->Fill(pixeltrack_Chi2);
      if (debug_)
        std::cout << "*****MultiRP track not found while matching!!!*****"
                  << std::endl;
    }
    if (matches == 0 && multiRPmatchFound >= 1 &&
        (pixeltrack_recoInfo == recoInfoCut_ || recoInfoCut_ == -1)) {
      multiRPfoundWhenNoMatchFound[pixelDetId]++;
      h1Chi2WhenNoInterpotMatchANDmultiRP_[pixelDetId]->Fill(pixeltrack_Chi2);
      if (debug_)
        std::cout << "*****MultiRP track found without a match!!!*****"
                  << std::endl;
    }

    if (matches == 0) {
      h1ProtonsInProbePotWhenNoMatchFound_[pixelDetId]->Fill(protonsInProbePot);
    }
  }
  if (debug_)
    std::cout << std::endl;
}

void InterpotEfficiency_2017::fillDescriptions(
    edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void InterpotEfficiency_2017::beginJob() {}

void InterpotEfficiency_2017::endJob() {
  TFile *outputFile_ = new TFile(outputFileName_.data(), "RECREATE");
  for (auto &rpId : romanPotIdVector_) {
    uint32_t arm = rpId.arm();
    uint32_t station = rpId.station();
    std::string rpDirName = Form("Arm%i_st%i_rp3", arm, station);
    outputFile_->mkdir(rpDirName.data());
    outputFile_->cd(rpDirName.data());

    if (h1InterPotEfficiencyVsXi_.find(rpId) !=
        h1InterPotEfficiencyVsXi_.end()) {

      h1InterPotEfficiencyVsXi_[rpId]->Divide(h1InterPotEfficiencyVsXi_[rpId],
                                              h1AuxXi_[rpId], 1., 1.);
      h1InterPotEfficiencyVsXi_[rpId]->SetMaximum(1.1);
      h1InterPotEfficiencyVsXi_[rpId]->SetMinimum(0);

      h1InterPotEfficiencyVsXi_[rpId]->Write();
      h1AuxXi_[rpId]->Write();
      h1DeltaXiMatch_[rpId]->Write();
      h1DeltaYMatch_[rpId]->Write();
      h1ProtonsInProbePotWhenNoMatchFound_[rpId]->Write();
      h1XiWhenPurityMatch_[rpId]->Write();

      h2InterPotEfficiencyMap_[rpId]->Divide(h2InterPotEfficiencyMap_[rpId],
                                             h2AuxProtonHitDistribution_[rpId],
                                             1., 1.);
      h2MatchingRate_[rpId]->Divide(h2MatchingRate_[rpId],
                                    h2AuxMatchingRate_[rpId], 1., 1.);
      h2MatchingRate_[rpId]->Write();
      h1MatchingRate_[rpId]->Divide(h1MatchingRate_[rpId],
                                    h1AuxMatchingRate_[rpId], 1., 1.);
      h1MatchingRate_[rpId]->Write();
      h2ProtonDistributionWithNoPurityMatch_[rpId]->Write();

      for (int xbin = 1; xbin < mapXbins_st2; xbin++) {
        for (int ybin = 1; ybin < mapYbins_st2; ybin++) {
          if (h2AuxProtonHitDistribution_[rpId]->GetBinContent(xbin, ybin) < 0)
            h2InterPotEfficiencyMap_[rpId]->SetBinContent(xbin, ybin, 0);
        }
      }

      h2InterPotEfficiencyMap_[rpId]->SetMinimum(0);
      h2InterPotEfficiencyMap_[rpId]->Write();
      h2AuxProtonHitDistribution_[rpId]->Write();

      h1TxMatch_[rpId]->Write();
      h1TyMatch_[rpId]->Write();
      h2XCorrelationMatch_[rpId]->Write();
      h2YCorrelationMatch_[rpId]->Write();
      h2TxCorrelationMatch_[rpId]->Write();
      h2TyCorrelationMatch_[rpId]->Write();
      h1PixelTrackMuxWithStrips_[rpId]->Write();
      h2ProtonDistributionWithStrips_[rpId]->Write();
      h1RecoInfoMatch_[rpId]->Write();

      h1RecoInfo_[rpId]->Write();
      h1RecoInfoWithPurityMatch_[rpId]->Write();
      h1PixelTrackMux_[rpId]->Write();
      h2ProtonDistribution_[rpId]->Write();
      h1Xi_[rpId]->Write();
      h1XProjection_[rpId]->Write();
      h1Chi2WhenInterpotMatch_[rpId]->Write();
      h1Chi2WhenInterpotMatchANDmultiRP_[rpId]->Write();
      h1Chi2WhenInterpotMatchANDNOTmultiRP_[rpId]->Write();
      h1Chi2WhenNoInterpotMatchANDmultiRP_[rpId]->Write();
    }
  }
  outputFile_->Close();
  delete outputFile_;

  for (auto pixelDetId : romanPotIdVector_) {
    std::cout << "*** Arm: " << pixelDetId.arm()
              << " Station: " << pixelDetId.station() << std::endl;
    std::cout << "The overmatch rate is: "
              << ((double)overmatches[pixelDetId] / tries[pixelDetId]) * 100
              << " %" << std::endl;
    std::cout << "In "
              << ((double)multiRPfoundWhenMatchFound[pixelDetId] /
                  triesWithMatch[pixelDetId]) *
                     100
              << " % of the events with a strips track where the match was "
                 "found, also a multiRP track was found "
              << std::endl;
    std::cout << "In "
              << ((double)multiRPfoundWhenNoMatchFound[pixelDetId] /
                  triesWithoutMatch[pixelDetId]) *
                     100
              << " % of the events with a strips track where the match was NOT "
                 "found, a multiRP track was found\n"
              << std::endl;
  }
}

bool InterpotEfficiency_2017::Cut(CTPPSLocalTrackLite track) {
  CTPPSDetId detId = CTPPSDetId(track.getRPId());
  uint32_t arm = detId.arm();
  uint32_t station = detId.station();
  uint32_t ndf = 2 * track.getNumberOfPointsUsedForFit() - 4;
  double x = track.getX();
  double y = track.getY();

  double maxTx = 0.02;
  double maxTy = 0.02;
  double maxChi2 = TMath::ChisquareQuantile(0.9999999, ndf);
  if (station == 2) {
    if (debug_) {
      if (track.getChiSquaredOverNDF() * ndf > maxChi2)
        std::cout << "Chi2 cut not passed" << std::endl;
      if (TMath::Abs(track.getTx()) > maxTx)
        std::cout << "maxTx cut not passed" << std::endl;
      if (TMath::Abs(track.getTy()) > maxTy)
        std::cout << "maxTy cut not passed" << std::endl;
      if (track.getNumberOfPointsUsedForFit() < minNumberOfPlanesForTrack_)
        std::cout << "Too few planes for track" << std::endl;
      if (track.getNumberOfPointsUsedForFit() > maxNumberOfPlanesForTrack_)
        std::cout << "Too many planes for track" << std::endl;
      if (y > fiducialYHigh_[std::pair<int, int>(arm, station)])
        std::cout << "fiducialYHigh cut not passed" << std::endl;
      if (y < fiducialYLow_[std::pair<int, int>(arm, station)])
        std::cout << "fiducialYLow cut not passed" << std::endl;
      if (x < fiducialXLow_[std::pair<int, int>(arm, station)])
        std::cout << "fiducialXLow cut not passed" << std::endl;
    }
    if (TMath::Abs(track.getTx()) > maxTx ||
        TMath::Abs(track.getTy()) > maxTy ||
        track.getChiSquaredOverNDF() * ndf > maxChi2 ||
        track.getNumberOfPointsUsedForFit() < minNumberOfPlanesForTrack_ ||
        track.getNumberOfPointsUsedForFit() > maxNumberOfPlanesForTrack_ ||
        y > fiducialYHigh_[std::pair<int, int>(arm, station)] ||
        y < fiducialYLow_[std::pair<int, int>(arm, station)] ||
        x < fiducialXLow_[std::pair<int, int>(arm, station)])
      return true;
    else {
      if (recoInfoCut_ != 5) {
        if (recoInfoCut_ != -1) {
          if ((int)track.getPixelTrackRecoInfo() != recoInfoCut_)
            return true;
          else
            return false;
        } else {
          if ((int)track.getPixelTrackRecoInfo() != 0 &&
              (int)track.getPixelTrackRecoInfo() != 2)
            return true;
          else
            return false;
        }
      } else
        return false;
    }
  } else {
    if (station == 0) {
      // if (TMath::Abs(track.getTx()) > maxTx ||
      //     TMath::Abs(track.getTy()) > maxTy ||
      //     track.getChiSquaredOverNDF() * ndf > maxChi2)
      //   return true;
      // else
      return false;
    } else
      throw "Station is neither 0 or 2!!!";
  }
}

bool InterpotEfficiency_2017::isPlaneUsedInTrack(CTPPSPixelLocalTrack track,
                                                 uint32_t plane) {
  for (auto &planeHits : track.getHits()) {
    if (CTPPSPixelDetId(planeHits.detId()).plane() == plane) {
      CTPPSPixelFittedRecHit hit =
          planeHits[0]; // there's only one hit per plane
      if (hit.getIsUsedForFit())
        return true;
      else
        return false;
    }
  }
  // if this happens, it's bad
  std::cout << "WARNING: Plane not found in track" << std::endl;
  return false;
}

// define this as a plug-in
DEFINE_FWK_MODULE(InterpotEfficiency_2017);