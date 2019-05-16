import FWCore.ParameterSet.Config as cms

demo = cms.EDAnalyzer('EfficiencyTool_2018'
     ,tracks = cms.untracked.InputTag('ctfWithMaterialTracks')
)
