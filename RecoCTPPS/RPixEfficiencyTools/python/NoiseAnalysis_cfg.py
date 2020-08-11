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
options.register('useJsonFile',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.bool,
                "Do not use JSON file")
options.register('recoInfo',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "CTPPSpixelLocalTrackReconstructionInfo proton variable - -1 for no selection")
options.register('jsonFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "JSON file list name")
options.register('maxPixelTracks',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.int,
                "Maximum pixel tracks in RP")
options.register('maxChi2Prob',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.float,
                "Maximum chi2 probability of the track")
options.maxChi2Prob = 0.999999
options.maxPixelTracks = 99
options.recoInfo = -1
options.parseArguments()

print("Chi2 cut: "+str(options.maxChi2Prob))

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

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000000) )

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

# Fiducial region for tracks
# RP order 0_0, 0_2, 1_0, 1_2 at the top left angle of the RP track map (for tilted pots)
#No Cuts
# fiducialXLow = [0,0,0,0]
# fiducialXHigh = [99,99,99,99]
# fiducialYLow = [-99,-99.,-99,-99]
# fiducialYHigh = [99,99,99,99]

# Cuts 2
# fiducialXLow = [0,0,0,0]
# fiducialXHigh = [99,13,99,13]
# fiducialYLow = [-99,-2.,-99,-2]
# fiducialYHigh = [99,3,99,3]

# Cuts 3
# fiducialXLow = [0,3.5,0,3.5]
# fiducialXHigh = [99,13,99,13]
# fiducialYLow = [-99,-2.,-99,-2]
# fiducialYHigh = [99,3,99,3]

# Cuts 4
# fiducialXLow = [0,2.5,0,2.5]
# fiducialXHigh = [99,13,99,13]
# fiducialYLow = [-99,-2.,-99,-2]
# fiducialYHigh = [99,3,99,3]

# Cuts 5
fiducialXLow = [0,0,0,0]
fiducialXHigh = [99,8.5,99,8.5]
fiducialYLow = [-99,-1.,-99,-1]
fiducialYHigh = [99,3,99,3]

# Cuts 6
# fiducialXLow = [0,0,0,0]
# fiducialXHigh = [99,3.5,99,3.5]
# fiducialYLow = [-99,-1.,-99,-1]
# fiducialYHigh = [99,3,99,3]

# Cuts 7
# fiducialXLow = [0,0,0,0]
# fiducialXHigh = [99,3.5,99,3.5]
# fiducialYLow = [-99,-1.,-99,-1]
# fiducialYHigh = [99,3,99,3]

# Good cuts
# fiducialXLow = [0,3.5,0,3.5]
# fiducialXHigh = [99,8.5,99,8.5]
# fiducialYLow = [-99,-1.,-99,-1]
# fiducialYHigh = [99,3,99,3]

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

process.demo = cms.EDAnalyzer('NoiseAnalyzer_2017',
    # outputFileName=cms.untracked.string("RPixAnalysis_RecoLocalTrack_ReferenceRunAfterTS2.root"),
    outputFileName=cms.untracked.string(options.outputFileName),
    minNumberOfPlanesForTrack=cms.int32(3),
    maxNumberOfPlanesForTrack=cms.int32(3),
    maxChi2Prob=cms.untracked.double(options.maxChi2Prob),
    minTracksPerEvent=cms.int32(0), # this affects only the proton part
    maxTracksPerEvent=cms.int32(options.maxPixelTracks), # this affects only the proton part
    binGroupingX=cms.untracked.int32(1),
    binGroupingY=cms.untracked.int32(1),
    fiducialXLow=cms.untracked.vdouble(fiducialXLow),    
    fiducialXHigh=cms.untracked.vdouble(fiducialXHigh),
    fiducialYLow=cms.untracked.vdouble(fiducialYLow),
    fiducialYHigh=cms.untracked.vdouble(fiducialYHigh),
    recoInfo=cms.untracked.int32(options.recoInfo),
    maxProtonsInPixelRP=cms.untracked.int32(options.maxPixelTracks), # this affects only the interpot efficiency part
    debug=cms.untracked.bool(False), 
)

process.p = cms.Path(process.demo)