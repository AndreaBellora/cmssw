// -*- C++ -*-
//
// Package:    RecoCTPPS/RPixEfficiencyTools
// Class:      RPixEfficiencyTools
//
/**\class ResolutionTool_2017 ResolutionTool_2017.cc
 RecoCTPPS/RPixEfficiencyTools/plugins/ResolutionTool_2017.cc

 Description: [one line class summary]

 Implementation:
                 [Notes on implementation]
*/
//
// Original Author:  Andrea Bellora
//         Created:  Wed, 22 Aug 2017 09:55:05 GMT
//
//

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <fstream>
#include <memory>

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/Records/interface/VeryForwardRealGeometryRecord.h"
#include "Geometry/VeryForwardGeometryBuilder/interface/CTPPSGeometry.h"

#include "DataFormats/CTPPSDetId/interface/CTPPSPixelDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelRecHit.h"

#include "RecoCTPPS/PixelLocal/interface/RPixDetPatternFinder.h"
#include "RecoCTPPS/PixelLocal/interface/RPixPlaneCombinatoryTracking.h"

#include <TF1.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TTree.h>

using namespace std;

class ResolutionTool_2017
    : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit ResolutionTool_2017(const edm::ParameterSet &);
  ~ResolutionTool_2017();
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event &, const edm::EventSetup &) override;
  virtual void endJob() override;

  typedef std::vector<RPixDetPatternFinder::PointInPlane> PointInPlaneList;

  CTPPSPixelLocalTrack refitTrackWithoutPlane(CTPPSPixelLocalTrack track,
                                              uint32_t planeToExclude);
  CTPPSPixelLocalTrack fitTrack(PointInPlaneList pointList);
  bool calculatePointOnDetector(CTPPSPixelLocalTrack *track,
                                CTPPSPixelDetId planeId,
                                GlobalPoint &planeLineIntercept);
  template <class T> void find_or_insert(std::vector<T> &v, T t);
  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelLocalTrack>>
      pixelLocalTrackToken_;
  edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelRecHit>> pixelRecHitToken_;
  edm::ESWatcher<VeryForwardRealGeometryRecord> geometryWatcher_;
  CTPPSGeometry geometry_;

  TFile *outputFile_;
  std::string outputFileName_;
  int minTracksPerEvent;
  int maxTracksPerEvent;
  int minPointsForFit_;
  int maxPointsForFit_;
  int minNumberOfCls2_;
  int maxNumberOfCls2_;

  std::vector<CTPPSPixelDetId> romanPotIdVector_;
  std::vector<uint32_t> listOfPlanes_ = {0, 1, 2, 3, 4, 5};

  int binGroupingX = 1;
  int binGroupingY = 1;

  int mapXbins_st2 = 200 / binGroupingX;
  float mapXmin_st2 = 0. * TMath::Cos(18.4 / 180. * TMath::Pi());
  float mapXmax_st2 = 30. * TMath::Cos(18.4 / 180. * TMath::Pi());
  int mapYbins_st2 = 240 / binGroupingY;
  float mapYmin_st2 = -16.;
  float mapYmax_st2 = 8.;

  int mapXbins_st0 = 200 / binGroupingX;
  float mapXmin_st0 = 0. * TMath::Cos(18.4 / 180. * TMath::Pi());
  float mapXmax_st0 = 30. * TMath::Cos(18.4 / 180. * TMath::Pi());
  int mapYbins_st0 = 240 / binGroupingY;
  float mapYmin_st0 = -16.;
  float mapYmax_st0 = 8.;

  int mapXbins = mapXbins_st0;
  float mapXmin = mapXmin_st0;
  float mapXmax = mapXmax_st0;
  int mapYbins = mapYbins_st0;
  float mapYmin = mapYmin_st0;
  float mapYmax = mapYmax_st0;

  // Histograms

  // per RP
  std::map<CTPPSPixelDetId, TH1D *> h1PointsUsedForFit_;
  std::map<CTPPSPixelDetId, TH2D *> h2X0Correlation_;
  std::map<CTPPSPixelDetId, TH2D *> h2Y0Correlation_;

  // per plane
  std::map<CTPPSPixelDetId, TH1D *> h1XResiduals_;
  std::map<CTPPSPixelDetId, TH1D *> h1YResiduals_;
  std::map<CTPPSPixelDetId, TH1D *> h1XUnconstrainedResiduals_;
  std::map<CTPPSPixelDetId, TH1D *> h1YUnconstrainedResiduals_;
  std::map<CTPPSPixelDetId, TH1D *> h1XPulls_;
  std::map<CTPPSPixelDetId, TH1D *> h1YPulls_;
  std::map<CTPPSPixelDetId, TH1D *> h1XUnconstrainedPulls_;
  std::map<CTPPSPixelDetId, TH1D *> h1YUnconstrainedPulls_;
  std::map<CTPPSPixelDetId, TH1D *> h1XUnconstrainedResidualsCls2_;
  std::map<CTPPSPixelDetId, TH1D *> h1YUnconstrainedResidualsCls2_;
  std::map<CTPPSPixelDetId, TH1D *> h1XUnconstrainedPullsCls2_;
  std::map<CTPPSPixelDetId, TH1D *> h1YUnconstrainedPullsCls2_;

  // TTree
  TTree *tTracks_ = new TTree("tTracks", "RPix tracks");
  ;

  // TTree variables
  double x_, y_, chi2OverNDF_;
  int nPointsUsedForFit_; // points used to reconstruct the track
  int nPlanesWithColCls2_, nPlanesWithRowCls2_;
  std::vector<int> nHitsPerPlane_; // ordered per plane number
                                   // before refitting
  std::vector<double> xRefit_, yRefit_, chi2OverNDFRefit_;
  std::vector<double> residualX_, residualY_, residualUnconstrainedX_,
      residualUnconstrainedY_;
  std::vector<double> pullX_, pullY_, pullUnconstrainedX_, pullUnconstrainedY_;
  std::vector<int> clsSizeCol_, clsSizeRow_;
  std::vector<int> arm_, station_, plane_, recoInfo_;
};

ResolutionTool_2017::ResolutionTool_2017(const edm::ParameterSet &iConfig) {
  usesResource("TFileService");
  pixelLocalTrackToken_ = consumes<edm::DetSetVector<CTPPSPixelLocalTrack>>(
      edm::InputTag("ctppsPixelLocalTracks", ""));
  pixelRecHitToken_ = consumes<edm::DetSetVector<CTPPSPixelRecHit>>(
      edm::InputTag("ctppsPixelRecHits", ""));
  outputFileName_ =
      iConfig.getUntrackedParameter<std::string>("outputFileName");
  minTracksPerEvent = iConfig.getParameter<int>("minTracksPerEvent");
  maxTracksPerEvent = iConfig.getParameter<int>("maxTracksPerEvent");
  binGroupingX = iConfig.getUntrackedParameter<int>("binGroupingX");
  binGroupingY = iConfig.getUntrackedParameter<int>("binGroupingY");
  minPointsForFit_ = iConfig.getUntrackedParameter<int>("minPointsForFit");
  maxPointsForFit_ = iConfig.getUntrackedParameter<int>("maxPointsForFit");
  minNumberOfCls2_ = iConfig.getUntrackedParameter<int>("minNumberOfCls2");
  maxNumberOfCls2_ = iConfig.getUntrackedParameter<int>("maxNumberOfCls2");
}

ResolutionTool_2017::~ResolutionTool_2017() {
  delete tTracks_;

  for (const auto &rpId : romanPotIdVector_) {
    uint32_t arm = rpId.arm();
    uint32_t rp = rpId.rp();
    uint32_t station = rpId.station();

    delete h1PointsUsedForFit_[rpId];
    delete h2X0Correlation_[rpId];
    delete h2Y0Correlation_[rpId];

    for (const auto &plane : listOfPlanes_) {
      CTPPSPixelDetId planeId = CTPPSPixelDetId(arm, station, rp, plane);

      delete h1XResiduals_[planeId];
      delete h1YResiduals_[planeId];
      delete h1XUnconstrainedResiduals_[planeId];
      delete h1YUnconstrainedResiduals_[planeId];
      delete h1XPulls_[planeId];
      delete h1YPulls_[planeId];
      delete h1XUnconstrainedPulls_[planeId];
      delete h1YUnconstrainedPulls_[planeId];
      delete h1XUnconstrainedResidualsCls2_[planeId];
      delete h1YUnconstrainedResidualsCls2_[planeId];
      delete h1XUnconstrainedPullsCls2_[planeId];
      delete h1YUnconstrainedPullsCls2_[planeId];

    } // ...for each plane
  }   // ...for each rp
}

void ResolutionTool_2017::analyze(const edm::Event &iEvent,
                                  const edm::EventSetup &iSetup) {
  using namespace edm;

  Handle<edm::DetSetVector<CTPPSPixelLocalTrack>> pixelLocalTracks;
  iEvent.getByToken(pixelLocalTrackToken_, pixelLocalTracks);
  Handle<edm::DetSetVector<CTPPSPixelRecHit>> pixelRecHits;
  iEvent.getByToken(pixelRecHitToken_, pixelRecHits);

  // get geometry
  edm::ESHandle<CTPPSGeometry> geometryHandler;
  iSetup.get<VeryForwardRealGeometryRecord>().get(geometryHandler);
  geometry_ = *geometryHandler;
  geometryWatcher_.check(iSetup);

  for (const auto &rpPixelTrack : *pixelLocalTracks) {
    
    // For 2017
    if (CTPPSDetId(rpPixelTrack.detId()).station() == 0)
      continue;
    
    CTPPSPixelDetId rpId = CTPPSPixelDetId(rpPixelTrack.detId());
    uint32_t arm = rpId.arm();
    uint32_t rp = rpId.rp();
    uint32_t station = rpId.station();

    find_or_insert(romanPotIdVector_, rpId);

    if (station == 2) {
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

    if (h1PointsUsedForFit_.find(rpId) == h1PointsUsedForFit_.end()) {
      h1PointsUsedForFit_[rpId] =
          new TH1D(Form("h1PointsUsedForFit_%i_st%i_rp%i", arm, station, rp),
                   Form("h1PointsUsedForFit_%i_st%i_rp%i", arm, station, rp), 7,
                   -0.5, 6.5);
      h2X0Correlation_[rpId] =
          new TH2D(Form("h2X0Correlation_%i_st%i_rp%i", arm, station, rp),
                   Form("h2X0Correlation_%i_st%i_rp%i", arm, station, rp),
                   mapXbins, mapXmin, mapXmax, mapXbins, mapXmin, mapXmax);
      h2Y0Correlation_[rpId] =
          new TH2D(Form("h2Y0Correlation_%i_st%i_rp%i", arm, station, rp),
                   Form("h2Y0Correlation_%i_st%i_rp%i", arm, station, rp),
                   mapYbins, mapYmin, mapYmax, mapYbins, mapYmin, mapYmax);

    } // ...per rp histos booking

    for (const auto &pixeltrack : rpPixelTrack) {

      // Cuts on tracks
      if (pixeltrack.getNumberOfPointsUsedForFit() < minPointsForFit_ ||
          pixeltrack.getNumberOfPointsUsedForFit() > maxPointsForFit_)
        continue;

      // Fill per rp histos
      h1PointsUsedForFit_[rpId]->Fill(pixeltrack.getNumberOfPointsUsedForFit());

      int numberOfPointsWithXCls2 = 0;
      int numberOfPointsWithYCls2 = 0;

      for (const auto &plane : listOfPlanes_) {
        CTPPSPixelDetId planeId = CTPPSPixelDetId(arm, station, rp, plane);
        DetSetVector<CTPPSPixelRecHit>::const_iterator rh_it =
            (*pixelRecHits).find(planeId.rawId());

        if (rh_it != (*pixelRecHits).end()) {
          nHitsPerPlane_.push_back((*rh_it).size());
        } else {
          nHitsPerPlane_.push_back(0);
        }

        const auto originalFittedRecHits = pixeltrack.getHits();
        if (originalFittedRecHits[planeId][0].clusterSizeCol() == 2)
          ++numberOfPointsWithXCls2;
        if (originalFittedRecHits[planeId][0].clusterSizeRow() == 2)
          ++numberOfPointsWithYCls2;
      }
      nPlanesWithRowCls2_ = numberOfPointsWithYCls2;
      nPlanesWithColCls2_ = numberOfPointsWithXCls2;

      // Reset tree vectors
      nHitsPerPlane_.clear();
      xRefit_.clear();
      yRefit_.clear();
      chi2OverNDFRefit_.clear();
      residualX_.clear();
      residualY_.clear();
      residualUnconstrainedX_.clear();
      residualUnconstrainedY_.clear();
      pullX_.clear();
      pullY_.clear();
      pullUnconstrainedX_.clear();
      pullUnconstrainedY_.clear();
      clsSizeCol_.clear();
      clsSizeRow_.clear();
      recoInfo_.clear();
      arm_.clear();
      station_.clear();
      plane_.clear();

      // Fill per-track tree infos
      x_ = pixeltrack.getX0();
      y_ = pixeltrack.getY0();
      chi2OverNDF_ = pixeltrack.getChiSquaredOverNDF();

      for (const auto &plane : listOfPlanes_) {
        CTPPSPixelDetId planeId = CTPPSPixelDetId(arm, station, rp, plane);

        if (h1XUnconstrainedResiduals_.find(planeId) ==
            h1XUnconstrainedResiduals_.end()) {
          h1XResiduals_[planeId] =
              new TH1D(Form("h1XResiduals_arm%i_st%i_rp%i_pl%i", arm, station,
                            rp, plane),
                       Form("h1XResiduals_arm%i_st%i_rp%i_pl%i", arm, station,
                            rp, plane),
                       100, -0.2, 0.2);
          h1YResiduals_[planeId] =
              new TH1D(Form("h1YResiduals_arm%i_st%i_rp%i_pl%i", arm, station,
                            rp, plane),
                       Form("h1YResiduals_arm%i_st%i_rp%i_pl%i", arm, station,
                            rp, plane),
                       100, -0.2, 0.2);
          h1XUnconstrainedResiduals_[planeId] =
              new TH1D(Form("h1XUnconstrainedResiduals_arm%i_st%i_rp%i_pl%i",
                            arm, station, rp, plane),
                       Form("h1XUnconstrainedResiduals_arm%i_st%i_rp%i_pl%i",
                            arm, station, rp, plane),
                       100, -0.2, 0.2);
          h1YUnconstrainedResiduals_[planeId] =
              new TH1D(Form("h1YUnconstrainedResiduals_arm%i_st%i_rp%i_pl%i",
                            arm, station, rp, plane),
                       Form("h1YUnconstrainedResiduals_arm%i_st%i_rp%i_pl%i",
                            arm, station, rp, plane),
                       100, -0.2, 0.2);
          h1XPulls_[planeId] = new TH1D(
              Form("h1XPulls_arm%i_st%i_rp%i_pl%i", arm, station, rp, plane),
              Form("h1XPulls_arm%i_st%i_rp%i_pl%i", arm, station, rp, plane),
              100, -3, 3);
          h1YPulls_[planeId] = new TH1D(
              Form("h1YPulls_arm%i_st%i_rp%i_pl%i", arm, station, rp, plane),
              Form("h1YPulls_arm%i_st%i_rp%i_pl%i", arm, station, rp, plane),
              100, -3, 3);
          h1XUnconstrainedPulls_[planeId] =
              new TH1D(Form("h1XUnconstrainedPulls_arm%i_st%i_rp%i_pl%i", arm,
                            station, rp, plane),
                       Form("h1XUnconstrainedPulls_arm%i_st%i_rp%i_pl%i", arm,
                            station, rp, plane),
                       100, -3, 3);
          h1YUnconstrainedPulls_[planeId] =
              new TH1D(Form("h1YUnconstrainedPulls_arm%i_st%i_rp%i_pl%i", arm,
                            station, rp, plane),
                       Form("h1YUnconstrainedPulls_arm%i_st%i_rp%i_pl%i", arm,
                            station, rp, plane),
                       100, -3, 3);
          h1XUnconstrainedResidualsCls2_[planeId] = new TH1D(
              Form("h1XUnconstrainedResidualsCls2_arm%i_st%i_rp%i_pl%i", arm,
                   station, rp, plane),
              Form("h1XUnconstrainedResidualsCls2_arm%i_st%i_rp%i_pl%i", arm,
                   station, rp, plane),
              100, -0.2, 0.2);
          h1YUnconstrainedResidualsCls2_[planeId] = new TH1D(
              Form("h1YUnconstrainedResidualsCls2_arm%i_st%i_rp%i_pl%i", arm,
                   station, rp, plane),
              Form("h1YUnconstrainedResidualsCls2_arm%i_st%i_rp%i_pl%i", arm,
                   station, rp, plane),
              100, -0.2, 0.2);
          h1XUnconstrainedPullsCls2_[planeId] =
              new TH1D(Form("h1XUnconstrainedPullsCls2_arm%i_st%i_rp%i_pl%i",
                            arm, station, rp, plane),
                       Form("h1XUnconstrainedPullsCls2_arm%i_st%i_rp%i_pl%i",
                            arm, station, rp, plane),
                       100, -3, 3);
          h1YUnconstrainedPullsCls2_[planeId] =
              new TH1D(Form("h1YUnconstrainedPullsCls2_arm%i_st%i_rp%i_pl%i",
                            arm, station, rp, plane),
                       Form("h1YUnconstrainedPullsCls2_arm%i_st%i_rp%i_pl%i",
                            arm, station, rp, plane),
                       100, -3, 3);
        } // ...per plane histos booking

        const auto originalFittedRecHits = pixeltrack.getHits();

        // Cuts on hits
        if (!originalFittedRecHits[planeId][0].getIsUsedForFit())
          continue;

        CTPPSPixelLocalTrack trackWithoutPlane =
            refitTrackWithoutPlane(pixeltrack, plane);

        const auto newFittedRecHits = trackWithoutPlane.getHits();
        h2X0Correlation_[rpId]->Fill(pixeltrack.getX0(),
                                     trackWithoutPlane.getX0());
        h2Y0Correlation_[rpId]->Fill(pixeltrack.getY0(),
                                     trackWithoutPlane.getY0());

        // Fill per-plane tree infos
        xRefit_.push_back(trackWithoutPlane.getX0());
        yRefit_.push_back(trackWithoutPlane.getY0());
        chi2OverNDFRefit_.push_back(trackWithoutPlane.getChiSquaredOverNDF());
        residualX_.push_back(originalFittedRecHits[planeId][0].getXResidual());
        residualY_.push_back(originalFittedRecHits[planeId][0].getYResidual());
        residualUnconstrainedX_.push_back(
            newFittedRecHits[planeId][0].getXResidual());
        residualUnconstrainedY_.push_back(
            newFittedRecHits[planeId][0].getYResidual());
        pullX_.push_back(originalFittedRecHits[planeId][0].getXPull());
        pullY_.push_back(originalFittedRecHits[planeId][0].getYPull());
        pullUnconstrainedX_.push_back(newFittedRecHits[planeId][0].getXPull());
        pullUnconstrainedY_.push_back(newFittedRecHits[planeId][0].getYPull());
        clsSizeCol_.push_back(newFittedRecHits[planeId][0].clusterSizeCol());
        clsSizeRow_.push_back(newFittedRecHits[planeId][0].clusterSizeRow());
        recoInfo_.push_back((int)pixeltrack.getRecoInfo());
        arm_.push_back(arm);
        station_.push_back(station);
        plane_.push_back(plane);

        // Fill per plane histos
        if (numberOfPointsWithXCls2 >= minNumberOfCls2_ &&
            numberOfPointsWithXCls2 <= maxNumberOfCls2_) {
          h1XResiduals_[planeId]->Fill(
              originalFittedRecHits[planeId][0].getXResidual());
          h1XUnconstrainedResiduals_[planeId]->Fill(
              newFittedRecHits[planeId][0].getXResidual());
          h1XPulls_[planeId]->Fill(
              originalFittedRecHits[planeId][0].getXPull());
          h1XUnconstrainedPulls_[planeId]->Fill(
              newFittedRecHits[planeId][0].getXPull());
          if (originalFittedRecHits[planeId][0].clusterSizeCol() == 2) {
            h1XUnconstrainedResidualsCls2_[planeId]->Fill(
                newFittedRecHits[planeId][0].getXResidual());
            h1XUnconstrainedPullsCls2_[planeId]->Fill(
                newFittedRecHits[planeId][0].getXPull());
          }
        }
        if (numberOfPointsWithYCls2 >= minNumberOfCls2_ &&
            numberOfPointsWithYCls2 <= maxNumberOfCls2_) {
          h1YResiduals_[planeId]->Fill(
              originalFittedRecHits[planeId][0].getYResidual());
          h1YUnconstrainedResiduals_[planeId]->Fill(
              newFittedRecHits[planeId][0].getYResidual());
          h1YPulls_[planeId]->Fill(
              originalFittedRecHits[planeId][0].getYPull());
          h1YUnconstrainedPulls_[planeId]->Fill(
              newFittedRecHits[planeId][0].getYPull());
          if (originalFittedRecHits[planeId][0].clusterSizeRow() == 2) {
            h1YUnconstrainedResidualsCls2_[planeId]->Fill(
                newFittedRecHits[planeId][0].getXResidual());
            h1YUnconstrainedPullsCls2_[planeId]->Fill(
                newFittedRecHits[planeId][0].getXPull());
          }
        }
        edm::LogInfo("ResolutionTool_2017")
            << "\t\tNormal\t\tUnconstrained\n"
            << "xResidual\t" << originalFittedRecHits[planeId][0].getXResidual()
            << "\t" << newFittedRecHits[planeId][0].getXResidual() << std::endl
            << "yResidual\t" << originalFittedRecHits[planeId][0].getYResidual()
            << "\t" << newFittedRecHits[planeId][0].getYResidual() << std::endl
            << "xPull\t\t" << originalFittedRecHits[planeId][0].getXPull()
            << "\t" << newFittedRecHits[planeId][0].getXPull() << std::endl
            << "yPull\t\t" << originalFittedRecHits[planeId][0].getYPull()
            << "\t" << newFittedRecHits[planeId][0].getYPull() << std::endl;
        // if (pixeltrack.getNumberOfPointsUsedForFit() < 6)

        // std::cout << "Track fitted without plane " << plane
        //           << ":\nX: " << trackWithoutPlane.getX0()
        //           << "\tY: " << trackWithoutPlane.getY0() << endl;

      } // ...for each plane
      tTracks_->Fill();
    } // ...for each track in rp
  }   // ...for every rp in the event
  return;
}

void ResolutionTool_2017::beginJob() {
  tTracks_->Branch("x", &x_);
  tTracks_->Branch("y", &y_);
  tTracks_->Branch("xRefit", &xRefit_);
  tTracks_->Branch("yRefit", &yRefit_);
  tTracks_->Branch("chi2OverNDF", &chi2OverNDF_);
  tTracks_->Branch("chi2OverNDFRefit", &chi2OverNDFRefit_);
  tTracks_->Branch("nPlanesWithRowCls2_", &nPlanesWithRowCls2_);
  tTracks_->Branch("nPlanesWithColCls2_", &nPlanesWithColCls2_);
  tTracks_->Branch("residualX", &residualX_);
  tTracks_->Branch("residualY", &residualY_);
  tTracks_->Branch("residualUnconstrainedX", &residualUnconstrainedX_);
  tTracks_->Branch("residualUnconstrainedY", &residualUnconstrainedY_);
  tTracks_->Branch("pullX", &pullX_);
  tTracks_->Branch("pullY", &pullY_);
  tTracks_->Branch("pullUnconstrainedX", &pullUnconstrainedX_);
  tTracks_->Branch("pullUnconstrainedY", &pullUnconstrainedY_);
  tTracks_->Branch("clsSizeCol", &clsSizeCol_);
  tTracks_->Branch("clsSizeRow", &clsSizeRow_);
  tTracks_->Branch("nHitsPerPlane", &nHitsPerPlane_);
  tTracks_->Branch("nPointsUsedForFit", &nPointsUsedForFit_);
  tTracks_->Branch("recoInfo", &recoInfo_);
  tTracks_->Branch("arm", &arm_);
  tTracks_->Branch("station", &station_);
  tTracks_->Branch("plane", &plane_);
}

void ResolutionTool_2017::endJob() {
  outputFile_ = new TFile(outputFileName_.data(), "RECREATE");
  std::cout << "Saving output in: " << outputFile_->GetName() << std::endl;
  tTracks_->Write();

  for (const auto &rpId : romanPotIdVector_) {
    uint32_t arm = rpId.arm();
    uint32_t rp = rpId.rp();
    uint32_t station = rpId.station();
    std::string rpFolderName = Form("Arm%i_St%i_rp%i", arm, station, rp);
    outputFile_->mkdir(rpFolderName.data());
    outputFile_->cd(rpFolderName.data());
    // Write rp histos
    h1PointsUsedForFit_[rpId]->Write();
    h2X0Correlation_[rpId]->Write();
    h2Y0Correlation_[rpId]->Write();

    for (const auto &plane : listOfPlanes_) {
      CTPPSPixelDetId planeId = CTPPSPixelDetId(arm, station, rp, plane);
      std::string planeFolderName =
          Form("Arm%i_St%i_rp%i/Plane%i", arm, station, rp, plane);
      outputFile_->mkdir(planeFolderName.data());
      outputFile_->cd(planeFolderName.data());

      // Write plane histos
      h1XResiduals_[planeId]->Write();
      h1YResiduals_[planeId]->Write();
      h1XUnconstrainedResiduals_[planeId]->Write();
      h1YUnconstrainedResiduals_[planeId]->Write();
      h1XPulls_[planeId]->Write();
      h1YPulls_[planeId]->Write();
      h1XUnconstrainedPulls_[planeId]->Write();
      h1YUnconstrainedPulls_[planeId]->Write();
      h1XUnconstrainedResidualsCls2_[planeId]->Write();
      h1YUnconstrainedResidualsCls2_[planeId]->Write();
      h1XUnconstrainedPullsCls2_[planeId]->Write();
      h1YUnconstrainedPullsCls2_[planeId]->Write();
    }
  } // ...for each rp
}

void ResolutionTool_2017::fillDescriptions(
    edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

CTPPSPixelLocalTrack
ResolutionTool_2017::refitTrackWithoutPlane(CTPPSPixelLocalTrack track,
                                            uint32_t planeToExclude) {
  if (planeToExclude > 5)
    throw cms::Exception("ResolutionTool_2017")
        << "Exception in refitTrackWithoutPlane: trying to exclude a plane "
           "that doesn't exist";

  const auto fittedRecHits = track.getHits(); // DetSetVector
  PointInPlaneList pointsToBeFitted;

  for (const auto &planeFittedRecHits : fittedRecHits) {

    // check that only one hit per plane is found
    if (planeFittedRecHits.size() > 1)
      throw cms::Exception("ResolutionTool_2017")
          << "Exception in refitTrackWithoutPlane: more than one hit per plane "
             "in track";

    // get the plane and rp id and plane number
    CTPPSPixelDetId planeId = CTPPSPixelDetId(planeFittedRecHits.detId());
    CTPPSPixelDetId rpId(planeId.getRPId());
    uint32_t plane = planeId.plane();

    // rename the only fittedRecHit on the plane
    const auto fittedRecHit = planeFittedRecHits[0];

    // Exclude the selected plane
    if (plane == planeToExclude)
      continue;

    if (!fittedRecHit.getIsUsedForFit())
      continue;

    double globalPointX =
        fittedRecHit.getGlobalCoordinates().x() + fittedRecHit.getXResidual();
    double globalPointY =
        fittedRecHit.getGlobalCoordinates().y() + fittedRecHit.getYResidual();
    double globalPointZ = fittedRecHit.getGlobalCoordinates().z();

    math::Error<3>::type localError;
    localError[0][0] = fittedRecHit.getError().xx();
    localError[0][1] = fittedRecHit.getError().xy();
    localError[0][2] = 0.;
    localError[1][0] = fittedRecHit.getError().xy();
    localError[1][1] = fittedRecHit.getError().yy();
    localError[1][2] = 0.;
    localError[2][0] = 0.;
    localError[2][1] = 0.;
    localError[2][2] = 0.;
    DetGeomDesc::RotationMatrix theRotationMatrix =
        geometry_.getSensor(planeId)->rotation();
    AlgebraicMatrix33 theRotationTMatrix;
    theRotationMatrix.GetComponents(
        theRotationTMatrix(0, 0), theRotationTMatrix(0, 1),
        theRotationTMatrix(0, 2), theRotationTMatrix(1, 0),
        theRotationTMatrix(1, 1), theRotationTMatrix(1, 2),
        theRotationTMatrix(2, 0), theRotationTMatrix(2, 1),
        theRotationTMatrix(2, 2));

    CLHEP::Hep3Vector globalPoint = {globalPointX, globalPointY, globalPointZ};
    math::Error<3>::type globalError =
        ROOT::Math::SimilarityT(theRotationTMatrix, localError);

    RPixDetPatternFinder::PointInPlane pointInPlane = {globalPoint, globalError,
                                                       fittedRecHit, planeId};
    pointsToBeFitted.push_back(pointInPlane);
  }
  CTPPSPixelLocalTrack newTrack = fitTrack(pointsToBeFitted);

  // Add the excluded fittedRecHit back to the new track and recompute residuals
  // and pulls
  for (const auto &planeFittedRecHits : fittedRecHits) {
    CTPPSPixelDetId planeId = CTPPSPixelDetId(planeFittedRecHits.detId());
    CTPPSPixelDetId rpId(planeId.getRPId());
    uint32_t plane = planeId.plane();
    if (plane != planeToExclude)
      continue;

    const auto fittedRecHit = planeFittedRecHits[0];
    double globalPointX =
        fittedRecHit.getGlobalCoordinates().x() + fittedRecHit.getXResidual();
    double globalPointY =
        fittedRecHit.getGlobalCoordinates().y() + fittedRecHit.getYResidual();
    double globalPointZ = fittedRecHit.getGlobalCoordinates().z();

    math::Error<3>::type localError;
    localError[0][0] = fittedRecHit.getError().xx();
    localError[0][1] = fittedRecHit.getError().xy();
    localError[0][2] = 0.;
    localError[1][0] = fittedRecHit.getError().xy();
    localError[1][1] = fittedRecHit.getError().yy();
    localError[1][2] = 0.;
    localError[2][0] = 0.;
    localError[2][1] = 0.;
    localError[2][2] = 0.;
    DetGeomDesc::RotationMatrix theRotationMatrix =
        geometry_.getSensor(planeId)->rotation();
    AlgebraicMatrix33 theRotationTMatrix;
    theRotationMatrix.GetComponents(
        theRotationTMatrix(0, 0), theRotationTMatrix(0, 1),
        theRotationTMatrix(0, 2), theRotationTMatrix(1, 0),
        theRotationTMatrix(1, 1), theRotationTMatrix(1, 2),
        theRotationTMatrix(2, 0), theRotationTMatrix(2, 1),
        theRotationTMatrix(2, 2));

    CLHEP::Hep3Vector globalPoint = {globalPointX, globalPointY, globalPointZ};
    math::Error<3>::type globalError =
        ROOT::Math::SimilarityT(theRotationTMatrix, localError);

    RPixDetPatternFinder::PointInPlane hit = {globalPoint, globalError,
                                              fittedRecHit, planeId};

    GlobalPoint pointOnDet;
    bool foundPoint =
        calculatePointOnDetector(&newTrack, hit.detId, pointOnDet);
    if (!foundPoint) {
      CTPPSPixelLocalTrack badTrack;
      badTrack.setValid(false);
      return badTrack;
    }
    double xResidual = globalPoint.x() - pointOnDet.x();
    double yResidual = globalPoint.y() - pointOnDet.y();

    // Compute unconstrained residuals
    LocalPoint residualsUnconstrained(xResidual, yResidual);
    // Compute unconstrained pulls
    LocalPoint pullsUnconstrained(xResidual / std::sqrt(globalError(0, 0)),
                                  yResidual / std::sqrt(globalError(0, 0)));

    CTPPSPixelFittedRecHit fittedRecHitUnconstrained(
        hit.recHit, pointOnDet, residualsUnconstrained, pullsUnconstrained);
    fittedRecHitUnconstrained.setIsUsedForFit(false);
    newTrack.addHit(hit.detId, fittedRecHitUnconstrained);
  }
  return newTrack;
}

CTPPSPixelLocalTrack ResolutionTool_2017::fitTrack(PointInPlaneList pointList) {

  uint32_t const numberOfPlanes = 6;
  math::Vector<2 * numberOfPlanes>::type xyCoordinates;
  math::Error<2 * numberOfPlanes>::type varianceMatrix;
  math::Matrix<2 * numberOfPlanes, 4>::type zMatrix;
  double z0 = geometry_.getRPTranslation(pointList.at(0).detId.getRPId()).z();

  // The matrices and vector xyCoordinates, varianceMatrix and varianceMatrix
  // are built from the points
  for (uint32_t iHit = 0; iHit < numberOfPlanes; iHit++) {
    if (iHit < pointList.size()) {
      CLHEP::Hep3Vector globalPoint = pointList[iHit].globalPoint;
      xyCoordinates[2 * iHit] = globalPoint.x();
      xyCoordinates[2 * iHit + 1] = globalPoint.y();
      zMatrix(2 * iHit, 0) = 1.;
      zMatrix(2 * iHit, 2) = globalPoint.z() - z0;
      zMatrix(2 * iHit + 1, 1) = 1.;
      zMatrix(2 * iHit + 1, 3) = globalPoint.z() - z0;

      AlgebraicMatrix33 globalError = pointList[iHit].globalError;
      varianceMatrix(2 * iHit, 2 * iHit) = globalError(0, 0);
      varianceMatrix(2 * iHit, 2 * iHit + 1) = globalError(0, 1);
      varianceMatrix(2 * iHit + 1, 2 * iHit) = globalError(1, 0);
      varianceMatrix(2 * iHit + 1, 2 * iHit + 1) = globalError(1, 1);
    } else {
      varianceMatrix(2 * iHit, 2 * iHit) = 1.;
      varianceMatrix(2 * iHit + 1, 2 * iHit + 1) = 1.;
    }
  }

  // Get real point variance matrix
  if (!varianceMatrix.Invert()) {
    edm::LogError("ResolutionTool_2017")
        << "Error in ResolutionTool_2017::fitTrack -> "
        << "Point variance matrix is singular, skipping.";
    CTPPSPixelLocalTrack badTrack;
    badTrack.setValid(false);
    return badTrack;
  }

  math::Error<4>::type covarianceMatrix =
      ROOT::Math::SimilarityT(zMatrix, varianceMatrix);

  // To have the real parameter covariance matrix, covarianceMatrix needs to be
  // inverted
  if (!covarianceMatrix.Invert()) {
    edm::LogError("ResolutionTool_2017")
        << "Error in ResolutionTool_2017::fitTrack -> "
        << "Parameter covariance matrix is singular, skipping.";
    CTPPSPixelLocalTrack badTrack;
    badTrack.setValid(false);
    return badTrack;
  }

  // track parameters: (x0, y0, tx, ty); x = x0 + tx*(z-z0)
  math::Vector<4>::type zMatrixTransposeTimesVarianceMatrixTimesXyCoordinates =
      ROOT::Math::Transpose(zMatrix) * varianceMatrix * xyCoordinates;
  math::Vector<4>::type parameterVector =
      covarianceMatrix * zMatrixTransposeTimesVarianceMatrixTimesXyCoordinates;
  math::Vector<2 *numberOfPlanes>::type
      xyCoordinatesMinusZmatrixTimesParameters =
          xyCoordinates - (zMatrix * parameterVector);

  double chiSquare = ROOT::Math::Dot(
      xyCoordinatesMinusZmatrixTimesParameters,
      (varianceMatrix * xyCoordinatesMinusZmatrixTimesParameters));

  CTPPSPixelLocalTrack goodTrack(z0, parameterVector, covarianceMatrix,
                                 chiSquare);
  goodTrack.setValid(true);

  for (const auto &hit : pointList) {
    CLHEP::Hep3Vector globalPoint = hit.globalPoint;
    GlobalPoint pointOnDet;
    bool foundPoint =
        calculatePointOnDetector(&goodTrack, hit.detId, pointOnDet);
    if (!foundPoint) {
      CTPPSPixelLocalTrack badTrack;
      badTrack.setValid(false);
      return badTrack;
    }
    double xResidual = globalPoint.x() - pointOnDet.x();
    double yResidual = globalPoint.y() - pointOnDet.y();
    LocalPoint residuals(xResidual, yResidual);

    math::Error<3>::type globalError(hit.globalError);
    LocalPoint pulls(xResidual / std::sqrt(globalError(0, 0)),
                     yResidual / std::sqrt(globalError(0, 0)));

    CTPPSPixelFittedRecHit fittedRecHit(hit.recHit, pointOnDet, residuals,
                                        pulls);
    fittedRecHit.setIsUsedForFit(true);
    goodTrack.addHit(hit.detId, fittedRecHit);
  }

  return goodTrack;
}

bool ResolutionTool_2017::calculatePointOnDetector(
    CTPPSPixelLocalTrack *track, CTPPSPixelDetId planeId,
    GlobalPoint &planeLineIntercept) {
  double z0 = track->getZ0();
  CTPPSPixelLocalTrack::ParameterVector parameters =
      track->getParameterVector();

  math::Vector<3>::type pointOnLine(parameters[0], parameters[1], z0);
  GlobalVector tmpLineUnitVector = track->getDirectionVector();
  math::Vector<3>::type lineUnitVector(
      tmpLineUnitVector.x(), tmpLineUnitVector.y(), tmpLineUnitVector.z());

  CLHEP::Hep3Vector tmpPointLocal(0., 0., 0.);
  CLHEP::Hep3Vector tmpPointOnPlane =
      geometry_.localToGlobal(planeId, tmpPointLocal);

  math::Vector<3>::type pointOnPlane(tmpPointOnPlane.x(), tmpPointOnPlane.y(),
                                     tmpPointOnPlane.z());
  math::Vector<3>::type planeUnitVector(0., 0., 1.);

  DetGeomDesc::RotationMatrix theRotationMatrix =
      geometry_.getSensor(planeId)->rotation();
  AlgebraicMatrix33 tmpPlaneRotationMatrixMap;
  theRotationMatrix.GetComponents(
      tmpPlaneRotationMatrixMap(0, 0), tmpPlaneRotationMatrixMap(0, 1),
      tmpPlaneRotationMatrixMap(0, 2), tmpPlaneRotationMatrixMap(1, 0),
      tmpPlaneRotationMatrixMap(1, 1), tmpPlaneRotationMatrixMap(1, 2),
      tmpPlaneRotationMatrixMap(2, 0), tmpPlaneRotationMatrixMap(2, 1),
      tmpPlaneRotationMatrixMap(2, 2));

  planeUnitVector = tmpPlaneRotationMatrixMap * planeUnitVector;

  double denominator = ROOT::Math::Dot(lineUnitVector, planeUnitVector);
  if (denominator == 0) {
    edm::LogError("ResolutionTool_2017")
        << "Error in ResolutionTool_2017::calculatePointOnDetector -> "
        << "Fitted line and plane are parallel. Removing this track";
    return false;
  }

  double distanceFromLinePoint =
      ROOT::Math::Dot((pointOnPlane - pointOnLine), planeUnitVector) /
      denominator;

  math::Vector<3>::type tmpPlaneLineIntercept =
      distanceFromLinePoint * lineUnitVector + pointOnLine;
  planeLineIntercept =
      GlobalPoint(tmpPlaneLineIntercept[0], tmpPlaneLineIntercept[1],
                  tmpPlaneLineIntercept[2]);

  return true;
}

template <class T>
void ResolutionTool_2017::find_or_insert(std::vector<T> &v, T t) {
  if (std::find(v.begin(), v.end(), t) == v.end())
    v.push_back(t);
}

// define this as a plug-in
DEFINE_FWK_MODULE(ResolutionTool_2017);