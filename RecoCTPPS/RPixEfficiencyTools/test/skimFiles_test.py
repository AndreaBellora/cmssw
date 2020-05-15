import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('reRECO',eras.ctpps_2016)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('FWCore.ParameterSet.Types')
from RecoCTPPS.Configuration.RecoCTPPS_EventContent_cff import RecoCTPPSAOD

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1),
)
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.MessageLogger.cerr.threshold = cms.untracked.string("INFO")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Run2017F/ZeroBias/AOD/09Aug2019_UL2017-v1/260001/24676EF1-7140-6C47-A9E0-DAFDD46EFCBD.root',
        '/store/data/Run2017F/SingleMuon/AOD/09Aug2019_UL2017-v1/130011/A1A1D4BB-716F-F344-9364-83EEE328453A.root'
    ),
    duplicateCheckMode = cms.untracked.string("checkAllFilesOpened"),
    skipBadFiles = cms.untracked.bool(True),
    skipEvents = cms.untracked.uint32(1000000),

)

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("test.root"),
    outputCommands = cms.untracked.vstring("drop *",
    )
)
process.output.outputCommands.extend(RecoCTPPSAOD.outputCommands)


process.output_step = cms.EndPath(  
    process.output
)