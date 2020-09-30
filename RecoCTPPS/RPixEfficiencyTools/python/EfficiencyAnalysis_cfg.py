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
options.register('jsonFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "JSON file list name")
options.register('injectionSchemeFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.bool,
                "Injection scheme file name")
options.parseArguments()

if options.sourceFileList != '':
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
            reportEvery = cms.untracked.int32(10000),
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
process.MessageLogger.statistics = cms.untracked.vstring()

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = inputFiles
)

if options.useJsonFile == True:
    print("Using JSON file...")
    import FWCore.PythonUtilities.LumiList as LumiList
    if options.jsonFileName == '':
        jsonFileName = 'test/JSONFiles/Run'+str(options.runNumber)+'.json'
    else:
        jsonFileName = options.jsonFileName
    print(jsonFileName)
    process.source.lumisToProcess = LumiList.LumiList(filename = jsonFileName).getVLuminosityBlockRange()
    
runToScheme = {}
with open("data/RunToScheme2017.csv") as runToSchemeFile:
    firstcycle = True
    next(runToSchemeFile)
    for line in runToSchemeFile:
       (run, fill, injectionScheme) = line.split(", ")
       runToScheme[int(run)] = injectionScheme.rstrip()

if options.bunchSelection != 'NoSelection' and options.bunchSelection != '':
    if options.runNumber in runToScheme.keys():
        injectionSchemeFileName = 'data/2017_FillingSchemes/'+runToScheme[options.runNumber]+'.csv'
    else:
        injectionSchemeFileName = options.injectionSchemeFileName
    print("Using filling scheme: "+injectionSchemeFileName)
else:
    injectionSchemeFileName = ''

# Fiducial region for tracks
# RP order 0_0, 0_2, 1_0, 1_2 at the top left angle of the RP track map (for tilted pots)
# cuts
# fiducialXLow = [0,2.0,0,2.4]
# fiducialYLow = [-99,-11.,-99,-10.9]
# fiducialYHigh = [99,4.2,99,4.7]

# no cuts
fiducialXLow = [0,0,0,0]
fiducialYLow = [-99.,-99.,-99.,-99.]
fiducialYHigh = [99.,99.,99.,99.]

# fiducialXLow = [0,4,0,4]
# fiducialYLow = [99,1.5,-99.,1.5]
# fiducialYHigh = [99.,3.5,99.,3.5]

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
        print
elif runNumber > lastRunOfTheYear:
    print("This run doesn't belong to 2017 data taking")

process.demo = cms.EDAnalyzer('EfficiencyTool_2017',
    # outputFileName=cms.untracked.string("RPixAnalysis_RecoLocalTrack_ReferenceRunAfterTS2.root"),
    outputFileName=cms.untracked.string(options.outputFileName),
    minNumberOfPlanesForEfficiency=cms.int32(3),
    minNumberOfPlanesForTrack=cms.int32(3),
    maxNumberOfPlanesForTrack=cms.int32(6),
    isCorrelationPlotEnabled=cms.bool(False),                       #Only enable if the estimation of the correlation between Strips and Pixel tracks is under study 
                                                                    #(disables filling of TGraph, reducing the output file size)
    minTracksPerEvent=cms.int32(0),
    maxTracksPerEvent=cms.int32(99),
    supplementaryPlots=cms.bool(True),
    bunchSelection=cms.untracked.string(options.bunchSelection),
    bunchListFileName=cms.untracked.string(injectionSchemeFileName),
    binGroupingX=cms.untracked.int32(1),
    binGroupingY=cms.untracked.int32(1),
    fiducialXLow=cms.untracked.vdouble(fiducialXLow),
    fiducialYLow=cms.untracked.vdouble(fiducialYLow),
    fiducialYHigh=cms.untracked.vdouble(fiducialYHigh),
    producerTag=cms.untracked.string("ReMiniAOD")
)

process.p = cms.Path(process.demo)