import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '101X_dataRun2_Prompt_v9', '')

# Loading Geometry from DB for refitting tracks
#from Geometry.VeryForwardGeometry.geometryRPFromDB_cfi import *

process.load("Geometry.VeryForwardGeometry.geometryRPFromDB_cfi")
# process.load("Geometry.VeryForwardGeometry.geometryRPFromDD_2018_cfi")

import FWCore.ParameterSet.VarParsing as VarParsing
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
    FailPath = cms.untracked.vstring('ProductNotFound','Type Mismatch')
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100000) )

options = VarParsing.VarParsing ()
options.register('outputFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "output ROOT file name")
options.register('sourceFileList',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "source file list name")
options.register('runNumber',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "CMS Run Number")
options.register('useJsonFile',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.bool,
                "Do not use JSON file")
options.register('minPointsForFit',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Minimum number of points used for the track fit")
options.register('maxPointsForFit',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Minimum number of points used for the track fit")
options.register('minNumberOfCls2',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Minimum number of points having cls2")
options.register('maxNumberOfCls2',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Maximum number of points having cls2")
options.useJsonFile = False;
options.minPointsForFit = 0;
options.maxPointsForFit = 6;
options.minNumberOfCls2 = 0;
options.maxNumberOfCls2 = 99;
options.parseArguments()

import FWCore.Utilities.FileUtils as FileUtils
fileList = FileUtils.loadListFromFile (options.sourceFileList) 
inputFiles = cms.untracked.vstring( *fileList)

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
            # reportEvery = cms.untracked.int32(1),
            limit = cms.untracked.int32(50000000)
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

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = inputFiles,
    skipBadFiles = cms.untracked.bool(True),
)

if options.useJsonFile == True:
    print("Using JSON file...")
    import FWCore.PythonUtilities.LumiList as LumiList
    jsonFile = 'test/JSONFiles/Run'+str(options.runNumber)+'.json'
    process.source.lumisToProcess = LumiList.LumiList(filename = jsonFile).getVLuminosityBlockRange()

#WARNING: hardcoded magic numbers
#Coefficients of the correlation relations, coming from fits with isCorrelationPlotEnabled=True, computed on Reference runs Pre/Post TS1/2
#For each pot: pixelCoordinate = Coefficient*stripsCoordinate + Constant +- (Tolerance) 
#Every vector takes in order values for: arm0_st0_X0, arm0_st0_Y0, arm0_st2_X0, arm0_st2_Y0, arm1_st0_X0, arm1_st0_Y0, arm1_st2_X0, arm1_st2_Y0

nSigmaTolerance = 5.
firstRunOfTheYear = 297050
lastRunPreTs1     = 297469
lastRunPreTs2     = 302663
lastRunOfTheYear  = 307082

runNumber=options.runNumber
if runNumber < firstRunOfTheYear:
    print("This run belongs to before 2017 data taking")
elif runNumber <= lastRunPreTs1:
    print("Analyzing Pre-TS1 data")
elif runNumber <= lastRunPreTs2:
    print("Analyzing data taken between TS1 and TS2")
elif runNumber <= lastRunOfTheYear:
    print("Analyzing Post-TS2 data")
    for i in range(4):
        fiducialYLow[i] += 1
        fiducialYHigh[i] += 1
elif runNumber > lastRunOfTheYear:
    print("This run doesn't belong to 2017 data taking")

process.demo = cms.EDAnalyzer('ResolutionTool_2017',
    # outputFileName=cms.untracked.string("RPixAnalysis_RecoLocalTrack_ReferenceRunAfterTS2.root"),
    outputFileName=cms.untracked.string(options.outputFileName),
    minNumberOfPlanesForEfficiency=cms.int32(3),
    minTracksPerEvent=cms.int32(0),
    maxTracksPerEvent=cms.int32(99),
    binGroupingX=cms.untracked.int32(1),
    binGroupingY=cms.untracked.int32(1),
    minPointsForFit=cms.untracked.int32(options.minPointsForFit),
    maxPointsForFit=cms.untracked.int32(options.maxPointsForFit),
    minNumberOfCls2=cms.untracked.int32(options.minNumberOfCls2),
    maxNumberOfCls2=cms.untracked.int32(options.maxNumberOfCls2)
)

process.p = cms.Path(process.demo)