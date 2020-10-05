#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u$UID
	addJSON_B=" useJsonFile=true jsonFileName=/eos/project-c/ctpps/Operations/DataExternalConditions/2017/combined_RPIN_CMS_EraB.json"
	if true
	then	
		addJSON=" useJsonFile=true jsonFileName=/eos/project-c/ctpps/Operations/DataExternalConditions/2017/combined_RPIN_CMS_Era${1}.json"
		# addJSON=" useJsonFile=true jsonFileName=/afs/cern.ch/user/a/abellora/Work/CT-PPS/2017_EfficiencyTool/CMSSW_10_6_10/src/RecoCTPPS/RPixEfficiencyTools/test/JSONFiles/EraB_simMultiRP_realDataCheck.json"
		fileSuffix="_reMiniAOD"
	fi

	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/
	eval `scramv1 runtime -sh`
	set -x
	eval "cmsRun python/EfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Era${1}${fileSuffix}.dat outputFileName=OutputFiles/Era${1}${fileSuffix}.root bunchSelection=NoSelection $addJSON"
	eval "cmsRun python/RefinedEfficiencyAnalysis_cfg.py sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}${fileSuffix}.root outputFileName=OutputFiles/Era${1}_refinedEfficiency${fileSuffix}.root ${addJSON_B}"
	eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Era${1}${fileSuffix}.dat outputFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root $addJSON"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_refinedEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi${fileSuffix}.root ${addJSON_B}"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_refinedEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi${fileSuffix}_useMultiRPProtons.root useMultiRPProtons=True ${addJSON_B}"
	# eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromInterpot${fileSuffix}.root useInterPotEfficiency=True"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromMultiRP${fileSuffix}.root useMultiRPEfficiency=True ${addJSON_B}"
	set +x
fi