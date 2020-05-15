import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

import FWCore.ParameterSet.VarParsing as VarParsing
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
    FailPath = cms.untracked.vstring('ProductNotFound','Type Mismatch')
    )
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
options.register('bunchSelection',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "bunches to be analyzed")
options.register('useJsonFile',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.bool,
                "Do not use JSON file")
options.register('injectionSchemeFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.bool,
                "Injection scheme file name")
options.parseArguments()

import FWCore.Utilities.FileUtils as FileUtils
fileList = FileUtils.loadListFromFile (options.sourceFileList) 
inputFiles = cms.untracked.vstring( *fileList)

process.load("FWCore.MessageService.MessageLogger_cfi")

process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('ERROR'))
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000000) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = inputFiles
)

if options.useJsonFile == True:
    print("Using JSON file...")
    import FWCore.PythonUtilities.LumiList as LumiList
    jsonFile = 'test/JSONFiles/Run'+str(options.runNumber)+'.json'
    process.source.lumisToProcess = LumiList.LumiList(filename = jsonFile).getVLuminosityBlockRange()

runToScheme = {}
with open("data/RunToScheme.csv") as runToSchemeFile:
    firstcycle = True
    next(runToSchemeFile)
    for line in runToSchemeFile:
       (run, fill, injectionScheme) = line.split(", ")
       runToScheme[int(run)] = injectionScheme.rstrip()

if options.bunchSelection != 'NoSelection':
    if options.runNumber in runToScheme.keys():
        injectionSchemeFileName = 'data/2018_FillingSchemes/'+runToScheme[options.runNumber]+'.csv'
    else:
        injectionSchemeFileName = options.injectionSchemeFileName
    print("Using filling scheme: "+injectionSchemeFileName)
else:
    injectionSchemeFileName = ''

#WARNING: hardcoded magic numbers
#Coefficients of the correlation relations, coming from fits with isCorrelationPlotEnabled=True, computed on Reference runs Pre/Post TS1/2
#For each pot: pixelCoordinate = Coefficient*stripsCoordinate + Constant +- (Tolerance) 
#Every vector takes in order values for: arm0_st0_X0, arm0_st0_Y0, arm0_st2_X0, arm0_st2_Y0, arm1_st0_X0, arm1_st0_Y0, arm1_st2_X0, arm1_st2_Y0

nSigmaTolerance = 5.
firstRunOfTheYear = 314247
lastRunPreTs1     = 317696
lastRunPreTs2     = 322633
lastRunOfTheYear  = 324897

runNumber=options.runNumber
if runNumber < firstRunOfTheYear:
    print("This run doesn't belong to 2018 data taking")
elif runNumber <= lastRunPreTs1:
    print("Analyzing Pre-TS1 data")
    correlationCoefficients = [0.986511,0.922616,1.00923,1.0524,0.993892,0.93067,1.00467,1.06942]
    correlationConstants = [-37.6006,1.58184,38.165,-1.55599,-39.2032,1.84624,39.4468,-1.95123]
    sigmas = [0.24397,0.0916029,0.16667,0.116173,0.315105,0.10588,0.231927,0.1195]
elif runNumber <= lastRunPreTs2:
    print("Analyzing data taken between TS1 and TS2")
    correlationCoefficients = [0.986828,0.920718,1.00917,1.08219,0.997005,0.926529,0.998374,1.07058]
    correlationConstants = [-37.4915,0.7017,38.036,-0.7345,-38.8277,0.671142,38.9841,-0.680128]
    sigmas = [0.2124,0.0712,0.17727,0.09359,0.34288,0.081469,0.221028,0.0978877]
elif runNumber <= lastRunOfTheYear:
    print("Analyzing Post-TS2 data")
    correlationCoefficients = [0.986511,0.922616,1.00923,1.0524,0.993892,0.93067,1.00467,1.06942]
    correlationConstants = [-37.6006,1.58184,38.165,-1.55599,-39.2032,1.84624,39.4468,-1.95123]
    sigmas = [0.24397,0.0916029,0.16667,0.116173,0.315105,0.10588,0.231927,0.1195]
elif runNumber > lastRunOfTheYear:
    print("This run doesn't belong to 2018 data taking")

correlationTolerances = [nSigmaTolerance * x for x in sigmas]

process.demo = cms.EDAnalyzer('EfficiencyTool_2018',
    # outputFileName=cms.untracked.string("RPixAnalysis_RecoLocalTrack_ReferenceRunAfterTS2.root"),
    outputFileName=cms.untracked.string(options.outputFileName),
    minNumberOfPlanesForEfficiency=cms.int32(3),
    minNumberOfPlanesForTrack=cms.int32(3),
    isCorrelationPlotEnabled=cms.bool(False),                       #Only enable if the estimation of the correlation between Strips and Pixel tracks is under study 
                                                                    #(disables filling of TGraph, reducing the output file size)
    correlationCoefficients=cms.vdouble(correlationCoefficients),
    correlationConstants=cms.vdouble(correlationConstants),
    correlationTolerances=cms.vdouble(correlationTolerances),
    interPotEffMinTracksStart=cms.untracked.int32(0),
    interPotEffMaxTracksStart=cms.untracked.int32(2),
    minTracksPerEvent=cms.int32(0),
    maxTracksPerEvent=cms.int32(99),
    supplementaryPlots=cms.bool(True),
    bunchSelection=cms.untracked.string(options.bunchSelection),
    bunchListFileName=cms.untracked.string(injectionSchemeFileName),
    binGroupingX=cms.untracked.int32(1),
    binGroupingY=cms.untracked.int32(1),
)

process.p = cms.Path(process.demo)