#!/bin/bash
if [ $# -ne 3 ]
then
    echo "Two arguments required. Nothing done."
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u$UID
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/
	eval `scramv1 runtime -sh`
	# eval "cmsRun python/EfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}_xangle${3}.root bunchSelection=NoSelection useJsonFile=True jsonFileName=/eos/project/c/ctpps/Operations/DataExternalConditions/2017/combined_RPIN_CMS_Era${1}_${3}.json"
	# eval "cmsRun python/RefinedEfficiencyAnalysis_cfg.py sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_xangle${3}.root useJsonFile=True jsonFileName=/eos/project/c/ctpps/Operations/DataExternalConditions/2017/combined_RPIN_CMS_EraB_${3}.json"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_xangle${3}_refinedEfficiency.root useJsonFile=True jsonFileName=/eos/project/c/ctpps/Operations/DataExternalConditions/2017/combined_RPIN_CMS_EraB_${3}.json"
	# eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}_interpotEfficiency.root"
fi