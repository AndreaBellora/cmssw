import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

import FWCore.ParameterSet.VarParsing as VarParsing
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
    FailPath = cms.untracked.vstring('ProductNotFound','Type Mismatch')
    )

process.load("FWCore.MessageService.MessageLogger_cfi")

process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( 
        optionalPSet = cms.untracked.bool(True),
        INFO = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        noTimeStamps = cms.untracked.bool(False),
        FwkReport = cms.untracked.PSet(
            optionalPSet = cms.untracked.bool(True),
            reportEvery = cms.untracked.int32(1000),
            limit = cms.untracked.int32(10000000)
        ),
        default = cms.untracked.PSet(
            limit = cms.untracked.int32(10000000)
        ),
        threshold = cms.untracked.string('INFO')
        ),
    categories = cms.untracked.vstring(
        "FwkReport"
        ),
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(20) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        # 'file:/afs/cern.ch/work/a/abellora/public/Mario/ReRecoOutputs/Fill5839_150_fromRAW.root'
        'file:/eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2017/ReRecoOutputTmp_CMSSW_10_6_0_patch1/Run297050.root'
        )
)

process.demo = cms.EDAnalyzer('MarioAnalyzer',
    outputFileName=cms.untracked.string("/afs/cern.ch/work/a/abellora/public/Mario/Hitmaps.root"),
    # outputFileName=cms.untracked.string(options.outputFileName),
)

process.p = cms.Path(process.demo)