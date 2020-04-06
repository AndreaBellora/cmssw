#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u$UID
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/
	eval `scramv1 runtime -sh`
	# eval "cmsRun python/EfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}.root bunchSelection=NoSelection"
	# eval "cmsRun python/RefinedEfficiencyAnalysis_cfg.py sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}.root"
	# eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}_interpotEfficiency_noChi2Cut_Chi2OverNDF.root recoInfo=-1 maxPixelTracks=99"
	# eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}_interpotEfficiency_noChi2Cut_noROC0.root recoInfo=0 maxPixelTracks=99"
	eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}_interpotEfficiency_noChi2Cut_ROC0_Chi2OverNDF.root recoInfo=2 maxPixelTracks=99"
	# eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_refinedEfficiency.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi.root"
	# eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency_noChi2Cut.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromInterpot_noChi2Cut.root"
fi