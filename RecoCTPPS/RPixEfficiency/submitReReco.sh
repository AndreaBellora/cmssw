#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export X509_USER_PROXY=~/x509up_u93252
	export SCRAM_ARCH=slc6_amd64_gcc530
	export CMSSW_BASE=/afs/cern.ch/user/a/abellora/Work/CT-PPS/2018_EfficiencyTool/CMSSW_10_2_0
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiency/ReReco
	eval `scramv1 runtime -sh`
	eval "cmsRun $CMSSW_BASE/src/RecoCTPPS/RPixEfficiency/ReReco/ReReco.py RunNumber=${1}"
fi