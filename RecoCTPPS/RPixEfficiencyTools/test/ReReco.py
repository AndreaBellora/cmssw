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

process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(10000000),
        )

process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('ERROR'))
)

# Track memory leaks
#process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",ignoreTotal = cms.untracked.int32(1) )

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
options.parseArguments()


import FWCore.Utilities.FileUtils as FileUtils

if options.sourceFileName == '':
    sourceFileName = 'InputFiles/Run'+str(options.runNumber)+'.dat'
else: 
    sourceFileName = options.sourceFileName

print("Source file list: "+sourceFileName)
fileList = FileUtils.loadListFromFile (sourceFileName) 
inputFiles = cms.untracked.vstring( *fileList)

import FWCore.PythonUtilities.LumiList as LumiList

if options.jsonFileName == '':
    jsonFileName = 'JSONFiles/Run'+str(options.runNumber)+'.json'
else:
    jsonFileName = options.jsonFileName

print("JSON file: "+jsonFileName)

if options.outputFileName == '':
    outputFileName = '/eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/ReRecoOutputTmp_CMSSW_10_6_0_pre3/Run'+str(options.runNumber)+'.root'
else:
    outputFileName = options.outputFileName

print("Output file: "+outputFileName)

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string(outputFileName),
    outputCommands = cms.untracked.vstring("drop *","keep CTPPS*_*_*_reRECO","keep *_RP*_*_*")
)

process.source = cms.Source("PoolSource",
    fileNames = inputFiles,
    duplicateCheckMode = cms.untracked.string("checkAllFilesOpened"),
    skipBadFiles = cms.untracked.bool(True),
)

process.source.lumisToProcess = LumiList.LumiList(filename = jsonFileName).getVLuminosityBlockRange()

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
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

# Path and EndPath definitions

process.reco_step = cms.Path(process.recoCTPPS)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.output)
# Schedule definition
process.schedule = cms.Schedule(
process.reco_step,process.endjob_step,process.output_step)

# filter all path with the production filter sequence
for path in process.paths:
    #  getattr(process,path)._seq = process.ProductionFilterSequence * getattr(process,path)._seq
    getattr(process,path)._seq = getattr(process,path)._seq

