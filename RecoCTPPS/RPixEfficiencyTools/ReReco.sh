#!/bin/bash
if [ $# -ne 6 ]
then
    echo "Six arguments required. Nothing done."
    # ${1} -> nameTag
	# ${2} -> CMSSW_BASE
	# ${3} -> Events to be skipped
	# ${4} -> Events to be analyzed
	# ${5} -> Output file name
	# ${6} -> Input file name
	exit 1
else
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u$UID
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test
	eval `scramv1 runtime -sh`
	eval "cmsRun $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/ReReco.py nameTag=${1} skipEvents=${3} maxEvents=${4} \
	outputFileName=${5} sourceFileName=${6}"
fi