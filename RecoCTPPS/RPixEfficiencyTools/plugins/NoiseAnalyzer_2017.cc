// -*- C++ -*-
//
// Package:    RecoCTPPS/RPixEfficiencyTools
// Class:      NoiseAnalyzer
//
/**\class NoiseAnalyzer_2017 NoiseAnalyzer_2017.cc
 RecoCTPPS/RPixEfficiencyTools/plugins/NoiseAnalyzer_2017.cc

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
#include "DataFormats/CTPPSReco/interface/CTPPSPixelCluster.h"
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

using namespace std;

class NoiseAnalyzer_2017
    : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit NoiseAnalyzer_2017(const edm::ParameterSet &);
  ~NoiseAnalyzer_2017();
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event &, const edm::EventSetup &) override;
  virtual void endJob() override;
  bool Cut(CTPPSLocalTrackLite track);
  bool CutWithNoNumberOfPoints(CTPPSLocalTrackLite track);

  bool debug_ = false;

  // Data to get
  edm::EDGetTokenT<reco::ForwardProtonCollection> protonsToken_;
  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelLocalTrack>>
      pixelLocalTrackToken_;
  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelRecHit>> pixelRecHitToken_;
  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelCluster>> pixelClusterToken_;

  // Parameter set
  string outputFileName_;
  double maxChi2Prob_;
  int minNumberOfPlanesForTrack_;
  int maxNumberOfPlanesForTrack_ = 6;
  int minTracksPerEvent;
  int maxTracksPerEvent;

  // Configs
  vector<uint32_t> listOfArms_ = {0, 1};
  vector<uint32_t> listOfStations_ = {2};
  vector<uint32_t> listOfPlanes_ = {0, 1, 2, 3, 4, 5};

  vector<CTPPSPixelDetId> planeIdVector_;
  vector<CTPPSPixelDetId> romanPotIdVector_;

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

  double chargeBins = 200;
  double chargeMin = 0;
  double chargeMax = 50000;

  int mapRowBins = 160;
  float mapRowMin = 0;
  float mapRowMax = 160;
  int mapColBins = 156;
  float mapColMin = 0;
  float mapColMax = 156;

  // Matching parameters

  // Cuts for 2017_UL
  double xiMatchMean45 = 0;
  double xiMatchMean56 = 0;
  double yMatchMean45 = 0;
  double yMatchMean56 = 0;
  double xMatchMean45 = 0;
  double xMatchMean56 = 0;
  double xiMatchWindow45 = 0.010;
  double xiMatchWindow56 = 0.015;
  double yMatchWindow45 = 99;
  double yMatchWindow56 = 99;
  double xMatchWindow45 = 99;
  double xMatchWindow56 = 99;
  bool excludeMultipleMatches = true;

  // Cuts for 2017 re-MINIAOD
  // double xiMatchMean45 = +6.0695e-5;
  // double xiMatchMean56 = +8.012857e-5;
  // double yMatchMean45 = 0;
  // double yMatchMean56 = -0.022612;
  // double xMatchMean45 = 0;
  // double xMatchMean56 = 0;
  // double xiMatchWindow45 = 5. * 0.00121;
  // double xiMatchWindow56 = 5. * 0.0020627;
  // double yMatchWindow45 = 99;
  // double yMatchWindow56 = 5. * 0.14777;
  // double xMatchWindow45 = 99;
  // double xMatchWindow56 = 99;
  // bool excludeMultipleMatches = true;

  // output histograms
  map<CTPPSPixelDetId, TH1D *> h1ProtonMux_;
  map<CTPPSPixelDetId, TH1D *> h1AllClustersCharge_;
  map<CTPPSPixelDetId, TH1D *> h1AllClustersChargeWhenNoProton_;
  map<CTPPSPixelDetId, TH2D *> h2NoiseHits_;
  map<CTPPSPixelDetId, TH1D *> h1FittedClustersCharge_;
  map<CTPPSPixelDetId, TH1D *> h1MatchingTrackClustersCharge_;
  map<CTPPSPixelDetId, TH1D *> h1MatchingTrackClustersChargeOneTrack_;
  map<CTPPSPixelDetId, TH1D *> h1NotMatchingTrackClustersCharge_;
  map<CTPPSPixelDetId, TH1D *> h1NumberOfFakeHitsInMatchingTrack_;
  map<CTPPSPixelDetId, TH1D *> h1NumberOfFakeHitsInNotMatchingTrack_;

  vector<double> fiducialXLowVector_;
  vector<double> fiducialXHighVector_;
  vector<double> fiducialYLowVector_;
  vector<double> fiducialYHighVector_;
  map<pair<int, int>, double> fiducialXLow_;
  map<pair<int, int>, double> fiducialXHigh_;
  map<pair<int, int>, double> fiducialYLow_;
  map<pair<int, int>, double> fiducialYHigh_;
  int recoInfoCut_;
  int maxProtonsInPixelRP_;

  // Noise parameters
  int noiseThreshold = 9000;

  std::map<CTPPSPixelDetId, uint32_t> numberOfFakeTracks;
  std::map<CTPPSPixelDetId, uint32_t> matchingFakeTracks;
  std::map<CTPPSPixelDetId, uint32_t> nonMatchingFakeTracks;
  std::map<CTPPSPixelDetId, uint32_t> matchingRealTracks;
  std::map<CTPPSPixelDetId, uint32_t> nonMatchingRealTracks;
  std::map<CTPPSPixelDetId, uint32_t> numberOfTracks;
  std::map<CTPPSPixelDetId, uint32_t> matchingTracks;
  std::map<CTPPSPixelDetId, uint32_t> nonMatchingTracks;
};

NoiseAnalyzer_2017::NoiseAnalyzer_2017(const edm::ParameterSet &iConfig) {
  usesResource("TFileService");
  protonsToken_ = consumes<reco::ForwardProtonCollection>(
      edm::InputTag("ctppsProtons", "singleRP"));
  pixelLocalTrackToken_ = consumes<edm::DetSetVector<CTPPSPixelLocalTrack>>(
      edm::InputTag("ctppsPixelLocalTracks", ""));
  pixelRecHitToken_ = consumes<edm::DetSetVector<CTPPSPixelRecHit>>(
      edm::InputTag("ctppsPixelRecHits", ""));
  pixelClusterToken_ = consumes<edm::DetSetVector<CTPPSPixelCluster>>(
      edm::InputTag("ctppsPixelClusters", ""));

  outputFileName_ = iConfig.getUntrackedParameter<string>("outputFileName");
  minNumberOfPlanesForTrack_ =
      iConfig.getParameter<int>("minNumberOfPlanesForTrack");
  maxChi2Prob_ = iConfig.getUntrackedParameter<double>("maxChi2Prob");
  minTracksPerEvent = iConfig.getParameter<int>("minTracksPerEvent"); // UNUSED!
  maxTracksPerEvent = iConfig.getParameter<int>("maxTracksPerEvent"); // UNUSED!
  binGroupingX = iConfig.getUntrackedParameter<int>("binGroupingX");  // UNUSED!
  binGroupingY = iConfig.getUntrackedParameter<int>("binGroupingY");  // UNUSED!
  fiducialXLowVector_ =
      iConfig.getUntrackedParameter<vector<double>>("fiducialXLow");
  fiducialXHighVector_ =
      iConfig.getUntrackedParameter<vector<double>>("fiducialXHigh");
  fiducialYLowVector_ =
      iConfig.getUntrackedParameter<vector<double>>("fiducialYLow");
  fiducialYHighVector_ =
      iConfig.getUntrackedParameter<vector<double>>("fiducialYHigh");
  fiducialXLow_ = {
      {pair<int, int>(0, 0), fiducialXLowVector_.at(0)},
      {pair<int, int>(0, 2), fiducialXLowVector_.at(1)},
      {pair<int, int>(1, 0), fiducialXLowVector_.at(2)},
      {pair<int, int>(1, 2), fiducialXLowVector_.at(3)},
  };
  fiducialXHigh_ = {
      {pair<int, int>(0, 0), fiducialXHighVector_.at(0)},
      {pair<int, int>(0, 2), fiducialXHighVector_.at(1)},
      {pair<int, int>(1, 0), fiducialXHighVector_.at(2)},
      {pair<int, int>(1, 2), fiducialXHighVector_.at(3)},
  };
  fiducialYLow_ = {
      {pair<int, int>(0, 0), fiducialYLowVector_.at(0)},
      {pair<int, int>(0, 2), fiducialYLowVector_.at(1)},
      {pair<int, int>(1, 0), fiducialYLowVector_.at(2)},
      {pair<int, int>(1, 2), fiducialYLowVector_.at(3)},
  };
  fiducialYHigh_ = {
      {pair<int, int>(0, 0), fiducialYHighVector_.at(0)},
      {pair<int, int>(0, 2), fiducialYHighVector_.at(1)},
      {pair<int, int>(1, 0), fiducialYHighVector_.at(2)},
      {pair<int, int>(1, 2), fiducialYHighVector_.at(3)},
  };
  recoInfoCut_ = iConfig.getUntrackedParameter<int>("recoInfo");
  maxProtonsInPixelRP_ =
      iConfig.getUntrackedParameter<int>("maxProtonsInPixelRP");
  debug_ = iConfig.getUntrackedParameter<bool>("debug");
}

NoiseAnalyzer_2017::~NoiseAnalyzer_2017() {
  for (auto &rpId : romanPotIdVector_) {
    delete h1ProtonMux_[rpId];
    delete h1NumberOfFakeHitsInMatchingTrack_[rpId];
    delete h1NumberOfFakeHitsInNotMatchingTrack_[rpId];
  }

  for (auto &planeId : planeIdVector_) {
    delete h1AllClustersCharge_[planeId];
    delete h1AllClustersChargeWhenNoProton_[planeId];
    delete h2NoiseHits_[planeId];
    delete h1FittedClustersCharge_[planeId];
    delete h1MatchingTrackClustersCharge_[planeId];
    delete h1MatchingTrackClustersChargeOneTrack_[planeId];
    delete h1NotMatchingTrackClustersCharge_[planeId];
  }
}

void NoiseAnalyzer_2017::fillDescriptions(
    edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void NoiseAnalyzer_2017::beginJob() {}

void NoiseAnalyzer_2017::analyze(const edm::Event &iEvent,
                                 const edm::EventSetup &iSetup) {
  using namespace edm;

  Handle<reco::ForwardProtonCollection> protons;
  iEvent.getByToken(protonsToken_, protons);
  Handle<edm::DetSetVector<CTPPSPixelLocalTrack>> pixelTracks;
  iEvent.getByToken(pixelLocalTrackToken_, pixelTracks);
  Handle<edm::DetSetVector<CTPPSPixelRecHit>> pixelRecHits;
  iEvent.getByToken(pixelRecHitToken_, pixelRecHits);
  Handle<edm::DetSetVector<CTPPSPixelCluster>> pixelClusters;
  iEvent.getByToken(pixelClusterToken_, pixelClusters);

  // Compute proton mux
  std::map<CTPPSPixelDetId, int> mux;
  std::map<CTPPSPixelDetId, int> recHits_mux;

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
      if (h1ProtonMux_.find(pixelDetId) == h1ProtonMux_.end()) {
        romanPotIdVector_.push_back(pixelDetId);
        h1ProtonMux_[pixelDetId] = new TH1D(
            Form("h1ProtonMux_arm%i_st%i_rp%i", arm, station, rp),
            Form("h1ProtonMux_arm%i_st%i_rp%i;Protons", arm, station, rp), 11,
            -0.5, 10.5);
        h1NumberOfFakeHitsInMatchingTrack_[pixelDetId] = new TH1D(
            Form("h1NumberOfFakeHitsInMatchingTrack_arm%i_st%i_rp%i", arm,
                 station, rp),
            Form("h1NumberOfFakeHitsInMatchingTrack_arm%i_st%i_rp%i;Protons",
                 arm, station, rp),
            7, 0, 7);
        h1NumberOfFakeHitsInNotMatchingTrack_[pixelDetId] = new TH1D(
            Form("h1NumberOfFakeHitsInNotMatchingTrack_arm%i_st%i_rp%i", arm,
                 station, rp),
            Form("h1NumberOfFakeHitsInNotMatchingTrack_arm%i_st%i_rp%i;Protons",
                 arm, station, rp),
            7, 0, 7);
        numberOfFakeTracks[pixelDetId] = 0;
        matchingFakeTracks[pixelDetId] = 0;
        nonMatchingFakeTracks[pixelDetId] = 0;
        matchingRealTracks[pixelDetId] = 0;
        nonMatchingRealTracks[pixelDetId] = 0;
        numberOfTracks[pixelDetId] = 0;
        matchingTracks[pixelDetId] = 0;
        nonMatchingTracks[pixelDetId] = 0;
      }
      mux[pixelDetId]++;
    }
  }

  // Fill mux histogram
  for (auto &pixelDetId : romanPotIdVector_) {
    h1ProtonMux_[pixelDetId]->Fill(mux[pixelDetId]);
  }

  // Fill rechit mux
  for (auto &recHitDs : *pixelRecHits) {
    // Get the id of the plane
    CTPPSPixelDetId recHitId = CTPPSPixelDetId(recHitDs.id);

    // Make it the RP id
    CTPPSPixelDetId pixelDetId = recHitId;
    pixelDetId.setPlane(0);

    recHits_mux[pixelDetId] += recHitDs.data.size();
  }

  // Analyze clusters
  for (auto &clusterDs : *pixelClusters) {
    CTPPSPixelDetId clusterId = CTPPSPixelDetId(clusterDs.id);
    int arm = clusterId.arm();
    int station = clusterId.station();
    int rp = clusterId.rp();
    int plane = clusterId.plane();
    CTPPSPixelDetId rpId = CTPPSPixelDetId(arm, station, rp);

    if (h1AllClustersCharge_.find(clusterId) == h1AllClustersCharge_.end()) {
      planeIdVector_.push_back(clusterId);
      TString planeTag = Form("arm%i_st%i_rp%i_pl%i", arm, station, rp, plane);
      h1AllClustersCharge_[clusterId] =
          new TH1D("h1AllClustersCharge_" + planeTag,
                   "h1AllClustersCharge_" + planeTag + ";Electrons", chargeBins,
                   chargeMin, chargeMax);
      h1AllClustersChargeWhenNoProton_[clusterId] =
          new TH1D("h1AllClustersChargeWhenNoProton_" + planeTag,
                   "h1AllClustersChargeWhenNoProton_" + planeTag + ";Electrons",
                   chargeBins, chargeMin, chargeMax);
      h2NoiseHits_[clusterId] = new TH2D(
          "h2NoiseHits_" + planeTag, "h2NoiseHits_" + planeTag + ";Col;Row",
          mapColMax, mapColMin, mapColMax, mapRowBins, mapRowMin, mapRowMax);
    }

    for (auto &cluster : clusterDs.data) {
      h1AllClustersCharge_[clusterId]->Fill(cluster.charge());
      if (mux[rpId] == 0 && recHits_mux[rpId] < 40)
        h1AllClustersChargeWhenNoProton_[clusterId]->Fill(cluster.charge());
      if (cluster.charge() < 9000 && mux[rpId] == 0 && recHits_mux[rpId] < 40)
        h2NoiseHits_[clusterId]->Fill(cluster.minPixelCol(),
                                      cluster.minPixelRow());
    }
  }

  // Analyze the event
  for (auto &proton : *protons) {
    bool skipProton = false;
    if (!proton.validFit())
      continue;

    CTPPSLocalTrackLite trackLite = *(proton.contributingLocalTracks().at(0));
    CTPPSDetId detId = CTPPSDetId(trackLite.getRPId());
    int arm = detId.arm();
    int station = detId.station();
    int rp = detId.rp();
    double trackX0 = trackLite.getX();
    double trackY0 = trackLite.getY();
    double xi = proton.xi();
    int numberOfPlanesUsed = trackLite.getNumberOfPointsUsedForFit();
    int numberOfNoiseHits = 0;

    // Only pixel singleRP protons pass
    if (station != 2)
      continue;

    // Apply the usual general-purpose cut
    if (Cut(trackLite))
      continue;

    // Don't analyze protons when too many or too few protons are in the same RP
    if (mux[detId] > maxTracksPerEvent || mux[detId] < minTracksPerEvent) {
      if (debug_)
        cout << "Mux cut not passed" << endl;
      continue;
    }

    // Just a few printouts for debugging
    if (debug_) {
      cout << endl;
      cout << endl;
      cout << "Analyzing track in arm " << arm << " station " << station
           << endl;
      cout << "X = " << trackX0 << "\tY = " << trackY0 << "\tXi = " << xi
           << endl;
    }

    // Matching part
    bool interPotMatch = false;
    for (auto &proton_Probe : *protons) {
      if (!proton.validFit())
        continue;
      CTPPSLocalTrackLite trackLite_Probe =
          *(proton_Probe.contributingLocalTracks().at(0));
      CTPPSDetId detId_Probe = CTPPSDetId(trackLite_Probe.getRPId());

      // // Apply the usual general-purpose cut
      // if (CutWithNoNumberOfPoints(trackLite_Probe))
      //   continue;

      // Ask the singleRP proton to be in the same arm, different station
      if ((int)detId_Probe.arm() != arm ||
          (int)detId_Probe.station() == station)
        continue;

      bool xiMatchPass = false;
      bool yMatchPass = false;
      bool xMatchPass = false;

      double trackX0_Probe = trackLite_Probe.getX();
      double trackY0_Probe = trackLite_Probe.getY();
      double xi_Probe = proton_Probe.xi();

      if (arm == 0 &&
          TMath::Abs(xi_Probe - xi - xiMatchMean45) < xiMatchWindow45)
        xiMatchPass = true;
      if (arm == 0 &&
          TMath::Abs(trackY0 - trackY0 - yMatchMean45) < yMatchWindow45)
        yMatchPass = true;
      if (arm == 0 &&
          TMath::Abs(trackX0 - trackX0 - xMatchMean45) < xMatchWindow45)
        xMatchPass = true;
      if (arm == 1 &&
          TMath::Abs(xi_Probe - xi - xiMatchMean56) < xiMatchWindow56)
        xiMatchPass = true;
      if (arm == 1 &&
          TMath::Abs(trackY0_Probe - trackY0 - yMatchMean56) < yMatchWindow56)
        yMatchPass = true;
      if (arm == 1 &&
          TMath::Abs(trackX0_Probe - trackX0 - xMatchMean56) < xMatchWindow56)
        xMatchPass = true;

      if (xiMatchPass && xMatchPass && yMatchPass) {
        interPotMatch = true;
        if (debug_ && interPotMatch)
          cout << "Found interPot match!" << endl;
      } else if (debug_)
        cout << "InterPot match not found" << endl;
    }

    // Find fat track correspondent to trackLite
    auto rpPixelTracks = (*pixelTracks)[trackLite.getRPId()];
    CTPPSPixelLocalTrack track;
    bool associatedTrackFound = false;
    for (auto &pixelTrack : rpPixelTracks) {
      if (TMath::Abs(pixelTrack.getX0() - trackX0) < 0.1 &&
          TMath::Abs(pixelTrack.getY0() - trackY0) < 0.1 &&
          !associatedTrackFound) {
        track = pixelTrack;
        associatedTrackFound = true;
        if (debug_)
          cout << "Found track to be associated" << endl;
      }
    } // for (auto &pixelTrack : rpPixelTracks)
    if (!associatedTrackFound) {
      cout << "WARNING: no track associated to trackLite found!" << endl;
      continue;
    }

    // Get the hits in the track
    edm::DetSetVector<CTPPSPixelFittedRecHit> fittedHits = track.getHits();
    for (auto &plane : listOfPlanes_) {
      CTPPSPixelDetId planeId(detId);
      planeId.setPlane(plane);

      if (h1FittedClustersCharge_.find(planeId) ==
          h1FittedClustersCharge_.end()) {
        TString planeTag =
            Form("arm%i_st%i_rp%i_pl%i", arm, station, rp, plane);
        h1FittedClustersCharge_[planeId] =
            new TH1D("h1FittedClustersCharge_" + planeTag,
                     "h1FittedClustersCharge_" + planeTag + ";Electrons",
                     chargeBins, chargeMin, chargeMax);
        h1MatchingTrackClustersCharge_[planeId] =
            new TH1D("h1MatchingTrackClustersCharge_" + planeTag,
                     "h1MatchingTrackClustersCharge_" + planeTag + ";Electrons",
                     chargeBins, chargeMin, chargeMax);
        h1MatchingTrackClustersChargeOneTrack_[planeId] = new TH1D(
            "h1MatchingTrackClustersChargeOneTrack_" + planeTag,
            "h1MatchingTrackClustersChargeOneTrack_" + planeTag + ";Electrons",
            chargeBins, chargeMin, chargeMax);
        h1NotMatchingTrackClustersCharge_[planeId] = new TH1D(
            "h1NotMatchingTrackClustersCharge_" + planeTag,
            "h1NotMatchingTrackClustersCharge_" + planeTag + ";Electrons",
            chargeBins, chargeMin, chargeMax);
      }

      auto planeFittedHit =
          fittedHits[planeId.rawId()][0]; // There is always only one fitted hit

      // Require the hit to be real, not on edge and not between rocs
      if (!planeFittedHit.getIsRealHit() || planeFittedHit.isOnEdge() ||
          planeFittedHit.spanTwoRocs())
        continue;

      double hitX0 = planeFittedHit.getGlobalCoordinates().x() +
                     planeFittedHit.getXResidual();
      double hitY0 = planeFittedHit.getGlobalCoordinates().y() +
                     planeFittedHit.getYResidual();

      int minPixelRow = planeFittedHit.minPixelRow();
      int minPixelCol = planeFittedHit.minPixelCol();
      int clsRow = planeFittedHit.clusterSizeRow();
      int clsCol = planeFittedHit.clusterSizeCol();

      // Few printouts for debugging
      if (debug_) {
        cout << endl;
        cout << "Analyzing hit on plane: " << plane
             << "\tID: " << planeId.rawId() << endl;
        cout << "X = " << hitX0 << "\tY = " << hitY0
             << "\tminRow = " << minPixelRow << "\tminCol = " << minPixelCol
             << "\tclsRow = " << clsRow << "\tclsCol = " << clsCol << endl;
      }

      // Find the cluster associated with the hit
      CTPPSPixelCluster cluster;
      bool associatedClusterFound = false;
      for (auto &planeCluster : (*pixelClusters)[planeId.rawId()]) {

        if ((int)planeCluster.minPixelCol() == minPixelCol &&
            (int)planeCluster.minPixelRow() == minPixelRow) {
          if (debug_) {
            cout << "Found cluster!" << endl;
            cout << "Charge = " << planeCluster.charge() << endl;
          }
          associatedClusterFound = true;
          cluster = planeCluster;
        }
      }
      if (!associatedClusterFound) {
        if (debug_)
          cout << "WARNING: no cluster associated to real fittedRecHit found!"
               << endl;
        continue;
      }

      // Count hits below noise threshold
      if (cluster.charge() < noiseThreshold)
        numberOfNoiseHits++;

      h1FittedClustersCharge_[planeId]->Fill(cluster.charge());

      if (interPotMatch) {
        h1MatchingTrackClustersCharge_[planeId]->Fill(cluster.charge());
        if (mux[detId] == 1)
          h1MatchingTrackClustersChargeOneTrack_[planeId]->Fill(cluster.charge());
      } else
        h1NotMatchingTrackClustersCharge_[planeId]->Fill(cluster.charge());
    } // for (auto &plane : listOfPlanes_)

    if (interPotMatch)
      h1NumberOfFakeHitsInMatchingTrack_[detId]->Fill(numberOfNoiseHits);
    else
      h1NumberOfFakeHitsInNotMatchingTrack_[detId]->Fill(numberOfNoiseHits);

    // Fake track definition!
    if (numberOfNoiseHits > numberOfPlanesUsed / 2) {
      numberOfFakeTracks[detId]++;
      if (interPotMatch)
        matchingFakeTracks[detId]++;
      else
        nonMatchingFakeTracks[detId]++;
    } else {
      if (interPotMatch)
        matchingRealTracks[detId]++;
      else
        nonMatchingRealTracks[detId]++;
    }

    if (interPotMatch)
      matchingTracks[detId]++;
    else
      nonMatchingTracks[detId]++;

    numberOfTracks[detId]++;
  } // for (auto &proton : *protons)
}

void NoiseAnalyzer_2017::endJob() {
  TFile *outputFile_ = new TFile(outputFileName_.data(), "RECREATE");
  for (auto &rpId : romanPotIdVector_) {
    uint32_t arm = rpId.arm();
    uint32_t station = rpId.station();
    string rpDirName = Form("Arm%i_st%i_rp3", arm, station);
    outputFile_->mkdir(rpDirName.data());
    outputFile_->cd(rpDirName.data());

    if (h1ProtonMux_.find(rpId) != h1ProtonMux_.end()) {
      h1ProtonMux_[rpId]->Write();
      h1NumberOfFakeHitsInMatchingTrack_[rpId]->Write();
      h1NumberOfFakeHitsInNotMatchingTrack_[rpId]->Write();
    }

    // Print fake rates
    cout << "Arm " << arm << " Station " << station << endl;
    cout << "Matching rate: "
         << ((double)matchingTracks[rpId] / numberOfTracks[rpId]) * 100 << " % "
         << endl;
    cout << "Fake rate: "
         << ((double)numberOfFakeTracks[rpId] / numberOfTracks[rpId]) * 100
         << " %" << endl;
    cout << "Between fake tracks: " << endl
         << "Matching: "
         << ((double)matchingFakeTracks[rpId] / numberOfFakeTracks[rpId]) * 100
         << " %" << endl
         << "Non matching: "
         << ((double)nonMatchingFakeTracks[rpId] / numberOfFakeTracks[rpId]) *
                100
         << " %" << endl;
    cout << "Between real tracks: " << endl
         << "Matching: "
         << ((double)matchingRealTracks[rpId] /
             (numberOfTracks[rpId] - numberOfFakeTracks[rpId])) *
                100
         << " %" << endl
         << "Non matching: "
         << ((double)nonMatchingRealTracks[rpId] /
             (numberOfTracks[rpId] - numberOfFakeTracks[rpId])) *
                100
         << " %" << endl;

    cout << endl;
  }

  for (auto &planeId : planeIdVector_) {
    uint32_t arm = planeId.arm();
    uint32_t station = planeId.station();
    uint32_t plane = planeId.plane();

    string planeDirName = Form("Arm%i_st%i_rp3/Arm%i_st%i_rp3_pl%i", arm,
                               station, arm, station, plane);
    outputFile_->mkdir(planeDirName.data());
    outputFile_->cd(planeDirName.data());

    if (h1AllClustersCharge_.find(planeId) != h1AllClustersCharge_.end()) {
      h1AllClustersCharge_[planeId]->Write();
      h1AllClustersChargeWhenNoProton_[planeId]->Write();
      h2NoiseHits_[planeId]->Write();
      h1FittedClustersCharge_[planeId]->Write();
      h1MatchingTrackClustersCharge_[planeId]->Write();
      h1MatchingTrackClustersChargeOneTrack_[planeId]->Write();
      h1NotMatchingTrackClustersCharge_[planeId]->Write();
    }
  }
  outputFile_->Close();
  delete outputFile_;
}

bool NoiseAnalyzer_2017::Cut(CTPPSLocalTrackLite track) {
  CTPPSDetId detId = CTPPSDetId(track.getRPId());
  uint32_t arm = detId.arm();
  uint32_t station = detId.station();
  uint32_t ndf = 2 * track.getNumberOfPointsUsedForFit() - 4;
  double x = track.getX();
  double y = track.getY();

  double maxTx = 0.03;
  double maxTy = 0.04;
  double maxChi2 = TMath::ChisquareQuantile(maxChi2Prob_, ndf);
  if (station == 2) {
    if (debug_) {
      if (track.getChiSquaredOverNDF() * ndf > maxChi2)
        cout << "Chi2 cut not passed" << endl;
      if (TMath::Abs(track.getTx()) > maxTx)
        cout << "maxTx cut not passed" << endl;
      if (TMath::Abs(track.getTy()) > maxTy)
        cout << "maxTy cut not passed" << endl;
      if (track.getNumberOfPointsUsedForFit() < minNumberOfPlanesForTrack_)
        cout << "Too few planes for track" << endl;
      if (track.getNumberOfPointsUsedForFit() > maxNumberOfPlanesForTrack_)
        cout << "Too many planes for track" << endl;
      if (y > fiducialYHigh_[pair<int, int>(arm, station)])
        cout << "fiducialYHigh cut not passed" << endl;
      if (y < fiducialYLow_[pair<int, int>(arm, station)])
        cout << "fiducialYLow cut not passed" << endl;
      if (x < fiducialXLow_[pair<int, int>(arm, station)])
        cout << "fiducialXLow cut not passed" << endl;
    }
    if (TMath::Abs(track.getTx()) > maxTx ||
        TMath::Abs(track.getTy()) > maxTy ||
        track.getChiSquaredOverNDF() * ndf > maxChi2 ||
        track.getNumberOfPointsUsedForFit() < minNumberOfPlanesForTrack_ ||
        track.getNumberOfPointsUsedForFit() > maxNumberOfPlanesForTrack_ ||
        y > fiducialYHigh_[pair<int, int>(arm, station)] ||
        y < fiducialYLow_[pair<int, int>(arm, station)] ||
        x < fiducialXLow_[pair<int, int>(arm, station)] ||
        x > fiducialXHigh_[pair<int, int>(arm, station)])
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
      if (TMath::Abs(track.getTx()) > maxTx ||
          TMath::Abs(track.getTy()) > maxTy)
        return true;
      else
        return false;
    } else
      throw "Station is neither 0 or 2!!!";
  }
}

bool NoiseAnalyzer_2017::CutWithNoNumberOfPoints(CTPPSLocalTrackLite track) {
  CTPPSDetId detId = CTPPSDetId(track.getRPId());
  uint32_t arm = detId.arm();
  uint32_t station = detId.station();
  uint32_t ndf = 2 * track.getNumberOfPointsUsedForFit() - 4;
  double x = track.getX();
  double y = track.getY();

  double maxTx = 0.03;
  double maxTy = 0.04;
  double maxChi2 = TMath::ChisquareQuantile(maxChi2Prob_, ndf);
  if (station == 2) {
    if (debug_) {
      if (track.getChiSquaredOverNDF() * ndf > maxChi2)
        cout << "Chi2 cut not passed" << endl;
      if (TMath::Abs(track.getTx()) > maxTx)
        cout << "maxTx cut not passed" << endl;
      if (TMath::Abs(track.getTy()) > maxTy)
        cout << "maxTy cut not passed" << endl;
      if (y > fiducialYHigh_[pair<int, int>(arm, station)])
        cout << "fiducialYHigh cut not passed" << endl;
      if (y < fiducialYLow_[pair<int, int>(arm, station)])
        cout << "fiducialYLow cut not passed" << endl;
      if (x < fiducialXLow_[pair<int, int>(arm, station)])
        cout << "fiducialXLow cut not passed" << endl;
    }
    if (TMath::Abs(track.getTx()) > maxTx ||
        TMath::Abs(track.getTy()) > maxTy ||
        track.getChiSquaredOverNDF() * ndf > maxChi2 ||
        y > fiducialYHigh_[pair<int, int>(arm, station)] ||
        y < fiducialYLow_[pair<int, int>(arm, station)] ||
        x < fiducialXLow_[pair<int, int>(arm, station)])
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
      if (TMath::Abs(track.getTx()) > maxTx ||
          TMath::Abs(track.getTy()) > maxTy)
        return true;
      else
        return false;
    } else
      throw "Station is neither 0 or 2!!!";
  }
}

// define this as a plug-in
DEFINE_FWK_MODULE(NoiseAnalyzer_2017);