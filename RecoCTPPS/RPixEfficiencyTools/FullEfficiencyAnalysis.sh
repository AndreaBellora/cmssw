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
	eval "cmsRun python/EfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Run${1}.dat outputFileName=OutputFiles/Run${1}.root runNumber=${1} bunchSelection=NoSelection"
	eval "cmsRun python/RefinedEfficiencyAnalysis_cfg.py sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Run${1}.root outputFileName=OutputFiles/Era${1}_refinedEfficiency.root runNumber=${1}"
	eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Run${1}.dat outputFileName=OutputFiles/Run${1}_interpotEfficiency.root runNumber=${1} recoInfo=-1 maxPixelTracks=99"
	eval "cmsRun python/EfficiencyVsXi_cfg.py sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Run${1}_refinedEfficiency.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi.root runNumber=${1}"
	eval "cmsRun python/EfficiencyVsXi_cfg.py sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Run${1}_interpotEfficiency.root runNumber=${1} outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromInterpot.root"
fi