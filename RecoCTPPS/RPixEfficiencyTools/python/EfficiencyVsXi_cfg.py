import FWCore.ParameterSet.Config as cms
import sys

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
options.register('efficiencyFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "efficiency ROOT file name - output of the EfficiencyTool_2017")
options.register('sourceFileList',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "source file list name")
options.register('era',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "PPS era to be analyzed")
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
options.register('jsonFileName',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.string,
                "JSON file list name")
options.register('useMultiRPEfficiency',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.bool,
                "Use multiRP efficiency instead of InterpotEfficiencyMap")
options.register('useInterPotEfficiency',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.bool,
                "Use interpot efficiency instead of InterpotEfficiencyMap")
options.register('useMultiRPProtons',
                '',
                VarParsing.VarParsing.multiplicity.singleton,
                VarParsing.VarParsing.varType.bool,
                "Use multiRP protons")
options.useInterPotEfficiency = False
options.useMultiRPEfficiency = False
options.useMultiRPProtons = False
options.parseArguments()

if (options.useInterPotEfficiency & options.useMultiRPEfficiency):
    print("ERROR: useMultiRPEfficiency and useInterPotEfficiency cannot be true at the same time!")
    sys.exit(1)

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

# Fiducial region for tracks
# RP order 0_0, 0_2, 1_0, 1_2 at the top left angle of the RP track map (for tilted pots)
# cuts for DPS note plots
# fiducialXLow = [0,2.0,0,2.4]
# fiducialXHigh = [99,99,99,99]
# fiducialYLow = [-99,-11.,-99,-10.9]
# fiducialYHigh = [99,4.2,99,4.7]

# cuts for POG plots
fiducialXLow = [0,0,0,0]
# fiducialXLow = [14,14,14,14]
# fiducialXHigh = [99,99,99,99]
# fiducialXHigh = [17,17,17,17]
# fiducialYLow = [-99,-99,-99,-99]
# fiducialYHigh = [99,99,99,99]

if options.era == 'B':
    fiducialXLow = [0,1.995,0,2.422]
    fiducialXHigh = [99,24.479,99,24.62]
    fiducialYLow = [-99,-11.098,-99,-10.698]
    fiducialYHigh = [99,4.398,99,4.698]
    print('Era '+options.era+' cuts set')
elif options.era == 'C1':
    fiducialXLow = [0,1.86,0,2.422]
    fiducialXHigh = [99,24.334,99,24.62]
    fiducialYLow = [-99,-11.098,-99,-10.698]
    fiducialYHigh = [99,4.298,99,4.698]
    print('Era '+options.era+' cuts set')
elif options.era == 'C2': 
    fiducialXLow = [0,2.137,0,2.282]
    fiducialXHigh = [99,24.334,99,24.62]
    fiducialYLow = [-99,-11.098,-99,-10.698]
    fiducialYHigh = [99,4.198,99,4.698]
    print('Era '+options.era+' cuts set')
elif options.era == 'D': 
    fiducialXLow = [0,1.86,0,2.282]
    fiducialXHigh = [99,24.334,99,24.62]
    fiducialYLow = [-99,-11.098,-99,-10.698]
    fiducialYHigh = [99,4.298,99,4.698]
    print('Era '+options.era+' cuts set')
elif options.era == 'E':
    fiducialXLow = [0,1.995,0,2.422]
    fiducialXHigh = [99,24.479,99,24.62]
    fiducialYLow = [-99,-10.098,-99,-9.698]
    fiducialYHigh = [99,4.998,99,5.498]
    print('Era '+options.era+' cuts set')
elif options.era == 'F1':
    fiducialXLow = [0,1.995,0,2.422]
    fiducialXHigh = [99,24.479,99,24.62]
    fiducialYLow = [-99,-10.098,-99,-9.798]
    fiducialYHigh = [99,4.998,99,5.398]
    print('Era '+options.era+' cuts set')
elif options.era == 'F2': 
    fiducialXLow = [0,1.86,0,2.280]
    fiducialXHigh = [99,24.479,99,24.62]
    fiducialYLow = [-99,-10.098,-99,-9.798]
    fiducialYHigh = [99,4.998,99,5.398]
    print('Era '+options.era+' cuts set')
elif options.era == 'F3': 
    fiducialXLow = [0,1.86,0,2.280]
    fiducialXHigh = [99,24.479,99,24.62]
    fiducialYLow = [-99,-10.098,-99,-9.798]
    fiducialYHigh = [99,4.998,99,5.398]
    print('Era '+options.era+' cuts set')
elif options.era != '':
    print('Era not found! Exiting...')
    sys.exit(1)

# no cuts
# fiducialXLow = [0,0,0,0]
# fiducialXHigh = [99,99,99,99]
# fiducialYLow = [-99.,-99.,-99.,-99.]
# fiducialYHigh = [99.,99.,99.,99.]

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
    # for i in range(4):
        # fiducialYLow[i] += 1
        # fiducialYHigh[i] += 1
elif runNumber > lastRunOfTheYear:
    print("This run doesn't belong to 2017 data taking")

process.demo = cms.EDAnalyzer('EfficiencyVsXi_2017',
    # outputFileName=cms.untracked.string("RPixAnalysis_RecoLocalTrack_ReferenceRunAfterTS2.root"),
    efficiencyFileName=cms.untracked.string(options.efficiencyFileName),
    outputFileName=cms.untracked.string(options.outputFileName),
    minNumberOfPlanesForEfficiency=cms.int32(3),
    minNumberOfPlanesForTrack=cms.int32(3),
    minTracksPerEvent=cms.int32(0),
    maxTracksPerEvent=cms.int32(99),
    binGroupingX=cms.untracked.int32(1),
    binGroupingY=cms.untracked.int32(1),
    fiducialXLow=cms.untracked.vdouble(fiducialXLow),
    fiducialXHigh=cms.untracked.vdouble(fiducialXHigh),
    fiducialYLow=cms.untracked.vdouble(fiducialYLow),
    fiducialYHigh=cms.untracked.vdouble(fiducialYHigh),
    useMultiRPEfficiency=cms.untracked.bool(options.useMultiRPEfficiency),
    useInterPotEfficiency=cms.untracked.bool(options.useInterPotEfficiency),
    useMultiRPProtons=cms.untracked.bool(options.useMultiRPProtons),
    lhcInfoLabel = cms.string(""),
    producerTag=cms.untracked.string("ReMiniAOD")
)

process.p = cms.Path(process.demo)