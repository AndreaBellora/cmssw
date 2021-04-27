#!/bin/bash
if [ $# -ne 2 ]
then
    echo "Two arguments required. Nothing done."
    # ${1} -> Era
    # ${2} -> CMSSW_BASE
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/Lumi/x509up_u$UID
	cd $CMSSW_BASE/src/RecoCTPPS/Lumi/
	eval `scramv1 runtime -sh`
	eval "cmsRun python/LumiAnalysis_cfg.py sourceFileList=data/Era${1}_forLumi.dat outputFileName=Era${1}_lumi.root"
fi