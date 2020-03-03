import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing ('analysis')

# add a list of strings for events to process
options.register ('eventsToProcess',
				  '',
				  VarParsing.multiplicity.list,
				  VarParsing.varType.string,
				  "Events to process")
options.register ('maxSize',
				  0,
				  VarParsing.multiplicity.singleton,
				  VarParsing.varType.int,
				  "Maximum (suggested) file size (in Kb)")
options.parseArguments()

process = cms.Process("PickEvent")
process.source = cms.Source ("PoolSource",
	  fileNames = cms.untracked.vstring (options.inputFiles),
)

if options.eventsToProcess:
    process.source.eventsToProcess = \
           cms.untracked.VEventRange (options.eventsToProcess)


process.maxEvents = cms.untracked.PSet(
	    input = cms.untracked.int32 (options.maxEvents)
)


process.Out = cms.OutputModule("PoolOutputModule",
        fileName = cms.untracked.string (options.outputFile)
)

if options.maxSize:
    process.Out.maxSize = cms.untracked.int32 (options.maxSize)

# MY STUFF
process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( 
        optionalPSet = cms.untracked.bool(True),
        categories = cms.untracked.vstring(
            "FwkReport"
        ),
        INFO = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        WARNING = cms.untracked.PSet(
            limit = cms.untracked.int32(10)
            ),
        noTimeStamps = cms.untracked.bool(False),
        FwkReport = cms.untracked.PSet(
            optionalPSet = cms.untracked.bool(True),
            reportEvery = cms.untracked.int32(1000),
            limit = cms.untracked.int32(10000000),
        ),
        default = cms.untracked.PSet(
            limit = cms.untracked.int32(10000000)
        ),
        threshold = cms.untracked.string('INFO')
        ),

)


process.end = cms.EndPath(process.Out)
