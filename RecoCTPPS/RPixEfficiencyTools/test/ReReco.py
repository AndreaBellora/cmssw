import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('reRECO',eras.Run2_2017)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.ParameterSet.Types')

import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ()
options.register('nameTag',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "String used to identify all files")
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
process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
    )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents),
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
            reportEvery = cms.untracked.int32(10000),
            limit = cms.untracked.int32(10000000)
        ),
        default = cms.untracked.PSet(
            limit = cms.untracked.int32(10000000)
        ),
        threshold = cms.untracked.string('WARNING')
        ),
    categories = cms.untracked.vstring(
        "FwkReport"
        ),
)

import FWCore.Utilities.FileUtils as FileUtils

if options.sourceFileName == '':
    sourceFileName = 'InputFiles/'+options.nameTag+'.dat'
else: 
    sourceFileName = options.sourceFileName

print("Source file list: "+sourceFileName)
fileList = FileUtils.loadListFromFile(sourceFileName) 
inputFiles = cms.untracked.vstring(*fileList)

import FWCore.PythonUtilities.LumiList as LumiList

if options.jsonFileName == '':
    jsonFileName = 'JSONFiles/'+options.nameTag+'.json'
else:
    jsonFileName = options.jsonFileName

print("JSON file: "+jsonFileName)

if options.outputFileName == '':
    outputFileName = options.nameTag+'.root'
else:
    outputFileName = options.outputFileName

print("Output file: "+outputFileName)

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string(outputFileName),
    outputCommands = cms.untracked.vstring("drop *",
        "keep *_*LocalTrack*_*_reRECO",
        "keep *_ctppsProtons_*_reRECO",
        "keep *_ctppsPixelRecHits_*_*",
        "keep *_*_*_reRECO", # to keep everything produced by this process
    )
)

process.source = cms.Source("PoolSource",
    fileNames = inputFiles,
    duplicateCheckMode = cms.untracked.string("checkAllFilesOpened"),
    skipBadFiles = cms.untracked.bool(True),
    skipEvents = cms.untracked.uint32(options.skipEvents)
)

process.source.lumisToProcess = LumiList.LumiList(filename = jsonFileName).getVLuminosityBlockRange()

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_dataRun2_2017_2018_Candidate_2019_06_21_14_08_20', '')
# process.GlobalTag = GlobalTag(process.GlobalTag, '101X_dataRun2_Prompt_v9', '')

# FOR THE ALIGNMENT RUN
# from Configuration.AlCa.GlobalTag import GlobalTag
# process.GlobalTag = GlobalTag(process.GlobalTag, '101X_dataRun2_Prompt_forCTPPS_ReReco', '')
# process.GlobalTag.toGet = cms.VPSet(
#   cms.PSet(record = cms.string("GeometryFileRcd"),
#            tag = cms.string("XMLFILE_CTPPS_Geometry_2018_101YV6"),
#            label = cms.untracked.string("CTPPS"),
#            connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
#           )
# )


from RecoCTPPS.Configuration.recoCTPPS_cff import *
process.load("RecoCTPPS.Configuration.recoCTPPS_cff")
process.load("EventFilter.CTPPSRawToDigi.ctppsRawToDigi_cff")

# Path and EndPath definitions

process.reco_step = cms.Path(
    # process.ctppsRawToDigi *
    process.recoCTPPS
)
# process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.output)

# # filter all path with the production filter sequence
# for path in process.paths:
#     #  getattr(process,path)._seq = process.ProductionFilterSequence * getattr(process,path)._seq
#     getattr(process,path)._seq = getattr(process,path)._seq

# processing sequence
# process.path = cms.Path(
#     process.reco_step  
# )

# # Schedule definition
process.schedule = cms.Schedule(
process.reco_step, process.output_step)
