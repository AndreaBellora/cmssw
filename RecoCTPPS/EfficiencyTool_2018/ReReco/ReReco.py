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
        input = cms.untracked.int32(3000000),
        )

process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('ERROR'))
)
process.source = cms.Source("EmptyIOVSource",
    timetype = cms.string('runnumber'),
    firstValue = cms.uint64(1),
    lastValue = cms.uint64(1),
    interval = cms.uint64(1)
)

# Track memory leaks
#process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",ignoreTotal = cms.untracked.int32(1) )

import FWCore.ParameterSet.VarParsing as VarParsing
process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
    )
options = VarParsing.VarParsing ()

options.register('RunNumber',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Run Number")
options.parseArguments()

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('OutputFiles/Run'+str(options.RunNumber)+'.root'),
    outputCommands = cms.untracked.vstring("drop *","keep CTPPS*_*_*_reRECO","keep *_RP*_*_*")
)

import FWCore.Utilities.FileUtils as FileUtils
sourceFileList = 'InputFiles/Run'+str(options.RunNumber)+'.dat'
fileList = FileUtils.loadListFromFile (sourceFileList) 
inputFiles = cms.untracked.vstring( *fileList)

process.source = cms.Source("PoolSource",
    fileNames = inputFiles,
    duplicateCheckMode = cms.untracked.string("checkEachFile"),
)


import FWCore.PythonUtilities.LumiList as LumiList
jsonFile = 'JSONFiles/Run'+str(options.RunNumber)+'.json'
process.source.lumisToProcess = LumiList.LumiList(filename = jsonFile).getVLuminosityBlockRange()

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '101X_dataRun2_Prompt_v9', '')

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

