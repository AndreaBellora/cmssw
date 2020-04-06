import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('reRECO',eras.ctpps_2016)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('FWCore.ParameterSet.Types')


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        # '/store/data/Run2017B/DoubleEG/AOD/17Nov2017-v1/30000/B6A6D63A-A0D5-E711-AACE-E0071B7A7840.root'
        ),
    duplicateCheckMode = cms.untracked.string("checkAllFilesOpened"),
    # skipBadFiles = cms.untracked.bool(True),
)

from HLTrigger.HLTfilters.hltHighLevel_cfi import *
process.triggerSelection  = hltHighLevel.clone(TriggerResultsTag = "TriggerResults::HLT", HLTPaths = ['HLT_Ele32_WPTight_Gsf_L1DoubleEG_v*','HLT_Ele35_WPTight_Gsf_v*'])

from RecoCTPPS.Configuration.RecoCTPPS_EventContent_cff import RecoCTPPSAOD
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('ReReco_2017D.root'),
    outputCommands = cms.untracked.vstring("drop *")
)
process.output.outputCommands.extend(RecoCTPPSAOD.outputCommands)

# Path and EndPath definitions
process.output_step = cms.EndPath(process.output)

# Schedule definition
process.schedule = cms.Schedule(process.output_step)
