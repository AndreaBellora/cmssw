#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc6_amd64_gcc530
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u93252
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test
	eval `scramv1 runtime -sh`
	eval "cmsRun $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/ReReco.py RunNumber=${1}"
fi