#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u$UID
	if false
	then	
		# addJSON=" useJsonFile=true jsonFileName=/eos/project-c/ctpps/Operations/DataExternalConditions/2017/combined_RPIN_CMS_EraF2_TimingIn.json"
		# addJSON=" useJsonFile=true jsonFileName=/afs/cern.ch/user/a/abellora/Work/CT-PPS/2017_EfficiencyTool/CMSSW_10_6_10/src/RecoCTPPS/RPixEfficiencyTools/test/JSONFiles/EraB_simMultiRP_realDataCheck.json"
		
		fileSuffix="_try"
	fi

	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/
	eval `scramv1 runtime -sh`
	set -x
	# eval "cmsRun python/EfficiencyAnalysis_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}${fileSuffix}.root bunchSelection=NoSelection $addJSON"
	# eval "cmsRun python/RefinedEfficiencyAnalysis_cfg.py sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}.root"
	eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/Era${1}.dat outputFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root $addJSON"
	# eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_refinedEfficiency.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi${fileSuffix}.root"
	# eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_refinedEfficiency.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi${fileSuffix}_useMultiRPProtons.root useMultiRPProtons=True"
	# eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromInterpot${fileSuffix}.root useInterPotEfficiency=True"
	# eval "cmsRun python/EfficiencyVsXi_cfg.py era=${1} sourceFileList=InputFiles/EraB.dat efficiencyFileName=OutputFiles/Era${1}_interpotEfficiency${fileSuffix}.root outputFileName=OutputFiles/Era${1}_efficiencyVsXi_fromMultiRP${fileSuffix}.root useMultiRPEfficiency=True"
	set +x
fi