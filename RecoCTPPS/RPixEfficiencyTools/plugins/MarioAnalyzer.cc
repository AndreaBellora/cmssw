// -*- C++ -*-
//
// Package:    RecoCTPPS/RPixEfficiencyTools
// Class:      MarioAnalyzer
//
/**\class MarioAnalyzer MarioAnalyzer.cc
 RecoCTPPS/RPixEfficiencyTools/plugins/MarioAnalyzer.cc

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
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelRecHit.h"
#include "DataFormats/CTPPSReco/interface/TotemRPLocalTrack.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <TEfficiency.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <TObjArray.h>

class MarioAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit MarioAnalyzer(const edm::ParameterSet &);
  ~MarioAnalyzer();
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event &, const edm::EventSetup &) override;
  virtual void endJob() override;

  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelRecHit>> pixelRecHitToken_;
  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelLocalTrack>>
      pixelLocalTrackToken_;
  edm::EDGetTokenT<edm::DetSetVector<TotemRPLocalTrack>> stripLocalTrackToken_;

  std::map<CTPPSPixelDetId, TH2D *> h2TrackHitDistribution_;

  std::vector<uint32_t> listOfArms_ = {0, 1};
  std::vector<uint32_t> listOfStations_ = {2};
  std::vector<uint32_t> listOfPlanes_ = {0, 1, 2, 3, 4, 5};

  std::vector<CTPPSPixelDetId> detectorIdVector_;
  std::vector<CTPPSPixelDetId> romanPotIdVector_;

  std::map<CTPPSPixelDetId, TH2D *> h2Hitmap_;
  std::string outputFileName_;
  TFile *outputFile_;
};

MarioAnalyzer::MarioAnalyzer(const edm::ParameterSet &iConfig) {
  usesResource("TFileService");
  pixelRecHitToken_ = consumes<edm::DetSetVector<CTPPSPixelRecHit>>(
      edm::InputTag("ctppsPixelRecHits", ""));
  pixelLocalTrackToken_ = consumes<edm::DetSetVector<CTPPSPixelLocalTrack>>(
      edm::InputTag("ctppsPixelLocalTracks", ""));
  stripLocalTrackToken_ = consumes<edm::DetSetVector<TotemRPLocalTrack>>(
      edm::InputTag("totemRPLocalTrackFitter", ""));
  outputFileName_ =
      iConfig.getUntrackedParameter<std::string>("outputFileName");
}

MarioAnalyzer::~MarioAnalyzer() {}

void MarioAnalyzer::analyze(const edm::Event &iEvent,
                            const edm::EventSetup &iSetup) {
  using namespace edm;
  Handle<edm::DetSetVector<CTPPSPixelRecHit>> pixelRecHits;
  iEvent.getByToken(pixelRecHitToken_, pixelRecHits);
  Handle<edm::DetSetVector<CTPPSPixelLocalTrack>> pixelLocalTracks;
  iEvent.getByToken(pixelLocalTrackToken_, pixelLocalTracks);
  Handle<edm::DetSetVector<TotemRPLocalTrack>> stripLocalTracks;
  iEvent.getByToken(stripLocalTrackToken_, stripLocalTracks);

  for (const auto &planeHits : *pixelRecHits) {
    CTPPSPixelDetId planeId = CTPPSPixelDetId(planeHits.detId());
    uint32_t arm = planeId.arm();
    uint32_t rp = planeId.rp();
    uint32_t station = planeId.station();
    uint32_t plane = planeId.plane();

    if (h2Hitmap_.find(planeId) == h2Hitmap_.end()) {
      detectorIdVector_.push_back(planeId);
      h2Hitmap_[planeId] = new TH2D(
          Form("h2Hitmap_arm%i_st%i_rp%i_pl%i", arm, station, rp, plane),
          Form("h2Hitmap_arm%i_st%i_rp%i_pl%i;x (mm);y (mm)", arm, station, rp,
               plane),
          180, -13.5, 13.5, 180, -9, 9);
    }
    for (const auto &hit : planeHits) {
      h2Hitmap_[planeId]->Fill(hit.getPoint().y(), hit.getPoint().x());
    }
  }

  for (const auto &rpPixelTracks : *pixelLocalTracks) {
    CTPPSDetId detId = CTPPSDetId(rpPixelTracks.detId());
    for (const auto &pixelTrack : rpPixelTracks) {
      if (!pixelTrack.isValid())
        continue;
      std::cout << detId << std::endl
                << "Pixel Track Z: " << pixelTrack.getZ0() << std::endl;
    }
  }
  for (const auto &rpStripTracks : *stripLocalTracks) {
    CTPPSDetId detId = CTPPSDetId(rpStripTracks.detId());
    for (const auto &stripTrack : rpStripTracks) {
      if (!stripTrack.isValid())
        continue;
      std::cout << detId << std::endl
                << "Strip Track Z: " << stripTrack.getZ0() << std::endl;
    }
  }
}

void MarioAnalyzer::fillDescriptions(
    edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void MarioAnalyzer::beginJob() {}

void MarioAnalyzer::endJob() {
  outputFile_ = new TFile(outputFileName_.data(), "RECREATE");
  for (const auto &planeId : detectorIdVector_) {
    uint32_t arm = planeId.arm();
    uint32_t rp = planeId.rp();
    uint32_t station = planeId.station();
    uint32_t plane = planeId.plane();

    std::string stationDirName = Form("Arm%i_st%i_rp%i", arm, station, rp);
    if (!outputFile_->Get(stationDirName.data()))
      outputFile_->mkdir(stationDirName.data());
    outputFile_->cd(stationDirName.data());
    h2Hitmap_[planeId]->Write();
  }
  outputFile_->Close();
}

// define this as a plug-in
DEFINE_FWK_MODULE(MarioAnalyzer);