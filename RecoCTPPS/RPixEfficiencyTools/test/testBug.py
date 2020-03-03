import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('reRECO',eras.Run2_25ns)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.ParameterSet.Types')

import FWCore.ParameterSet.VarParsing as VarParsing
process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
    )
options = VarParsing.VarParsing ()
options.register('runNumber',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Run Number")
options.register('outputFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "output ROOT file name")
options.register('sourceFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "source file list name")
options.register('jsonFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "JSON file list name")
options.register('maxEvents',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Maximum number of events to analyze")
options.register('skipEvents',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Events to be skipped")
options.parseArguments()

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1),
)

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
            reportEvery = cms.untracked.int32(1),
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

import FWCore.Utilities.FileUtils as FileUtils

if options.sourceFileName == '':
    sourceFileName = 'InputFiles/Run'+str(options.runNumber)+'.dat'
else: 
    sourceFileName = options.sourceFileName

if options.runNumber == '':
    runNumber = options.runNumber
else: 
    runNumber = 297219

# print("Source file list: "+sourceFileName)
# fileList = FileUtils.loadListFromFile (sourceFileName) 
# inputFiles = cms.untracked.vstring( *fileList)

import FWCore.PythonUtilities.LumiList as LumiList

if options.jsonFileName == '':
    jsonFileName = 'JSONFiles/Run'+str(options.runNumber)+'.json'
else:
    jsonFileName = options.jsonFileName

# print("JSON file: "+jsonFileName)

if options.outputFileName == '':
    outputFileName = 'outputReco.root'
else:
    outputFileName = options.outputFileName

print("Output file: "+outputFileName)

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string(outputFileName),
    outputCommands = cms.untracked.vstring("drop *","keep CTPPS*_*_*_reRECO","keep *_RP*_*_*")
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("/store/data/Run2017B/ZeroBias/AOD/23Jun2017-v1/90000/025ED269-B358-E711-B305-782BCB20BDE3.root"),
    duplicateCheckMode = cms.untracked.string("checkAllFilesOpened"),
    skipBadFiles = cms.untracked.bool(True),
    firstRun = cms.untracked.uint32(297219),
    skipEvents = cms.untracked.uint32(3750)
)

# process.source.lumisToProcess = LumiList.LumiList(runsAndLumis = {"297219": [85, 281]}).getVLuminosityBlockRange()

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')

from RecoCTPPS.Configuration.recoCTPPS_cff import *
process.load("RecoCTPPS.Configuration.recoCTPPS_cff")
process.load("EventFilter.CTPPSRawToDigi.ctppsRawToDigi_cff")

# Path and EndPath definitions

process.reco_step = cms.Path(
    # process.ctppsRawToDigi *
    process.recoCTPPS
)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.output)
# Schedule definition
process.schedule = cms.Schedule(
    process.reco_step,
    process.endjob_step,
    process.output_step
)

# filter all path with the production filter sequence
for path in process.paths:
    #  getattr(process,path)._seq = process.ProductionFilterSequence * getattr(process,path)._seq
    getattr(process,path)._seq = getattr(process,path)._seq

