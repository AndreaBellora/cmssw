#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
else
	function runReReco
	{
		eval "cmsRun $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/ReReco.py RunNumber=${1}"
		if [ $? -eq 0 ] 
		then
			touch "$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/OutputFiles/Run${1}_success"
			nfiles=`eval "wc -l $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/InputFiles/Run${1}.dat" | awk '{ print $1 }'`
			nexcluded=`eval "grep -i "skipped" $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/Run${1}.log | wc -l" | awk '{ print $1}'`
			echo "***********************************"
			echo "${nexcluded}/${nfiles} input files were excluded"
			echo "***********************************"
		fi
	}
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc6_amd64_gcc530
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u93252
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test
	eval `scramv1 runtime -sh`
	runReReco $1 $2
fi