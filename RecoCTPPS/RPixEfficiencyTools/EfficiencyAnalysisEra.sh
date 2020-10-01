#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u93252
	if true 
	then	
		addJSON="useJsonFile=True jsonFileName=/eos/project-c/ctpps/Operations/DataExternalConditions/2018/CMSgolden_2RPGood_anyarms_Era${1}.json"
		# addJSON="maxTracksInTagPot=1"
		fileSuffix="_reMiniAOD"
	fi

	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/
	eval `scramv1 runtime -sh`
	set -x
	eval "cmsRun python/EfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Era${1}${fileSuffix}.dat outputFileName=OutputFiles/Era${1}${fileSuffix}.root bunchSelection=NoSelection useJsonFile=True jsonFileName=/eos/project/c/ctpps/Operations/DataExternalConditions/2018/CMSgolden_2RPGood_anyarms_Era${1}.json"
	eval "cmsRun python/RefinedEfficiencyAnalysis_cfg.py sourceFileList=InputFiles/EraA${fileSuffix}.dat efficiencyFileName=OutputFiles/Era${1}.root outputFileName=OutputFiles/Era${1}_refinedEfficiency${fileSuffix}.root ${addJSON}"
	eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Era${1}${fileSuffix}.dat outputFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root ${addJSON}"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraA${fileSuffix}.dat efficiencyFileName=OutputFiles/Era${1}_refinedEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi${fileSuffix}.root"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraA${fileSuffix}.dat efficiencyFileName=OutputFiles/Era${1}_refinedEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi${fileSuffix}_useMultiRPProtons.root useMultiRPProtons=True"
	# eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraA${fileSuffix}.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromInterpot${fileSuffix}.root useInterPotEfficiency=True"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraA${fileSuffix}.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromMultiRP${fileSuffix}.root useMultiRPEfficiency=True"
	set +x
fi