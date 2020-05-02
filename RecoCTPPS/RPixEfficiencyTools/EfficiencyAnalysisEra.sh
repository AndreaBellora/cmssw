#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u93252
	if [ false ]
	then	
		addJSON="useJsonFile=True jsonFileName=/eos/project/c/ctpps/Operations/DataExternalConditions/2018/CMSgolden_2RPGood_anyarms_BeginningOfDataTakingSample.json"
		# fileSuffix="_TimingOut"
	fi

	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/
	eval `scramv1 runtime -sh`
	# eval "cmsRun python/EfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}.root bunchSelection=NoSelection useJsonFile=True jsonFileName=/eos/project/c/ctpps/Operations/DataExternalConditions/2018/CMSgolden_2RPGood_anyarms_Era${1}.json"
	# eval "cmsRun python/RefinedEfficiencyAnalysis_cfg.py sourceFileList=InputFiles/EraA.dat efficiencyFileName=OutputFiles/Era${1}.root ${addJSON}"
	# eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Era${1}_UL_MINIAOD.dat outputFileName=OutputFiles/Era${1}_interpotEfficiency.root"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/Era${1}_UL_MINIAOD.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromInterpot${fileSuffix}.root"
	eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/Era${1}_UL_MINIAOD.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_multiRP${fileSuffix}.root useMultiRP=True"
fi