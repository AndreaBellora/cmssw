#!/bin/bash
if [ $# -ne 4 ]
then
    echo "Four arguments required. Nothing done."
    # ${1} -> Run Number
    # ${2} -> CMSSW_BASE
    # ${3} -> RecoInfo
    # ${4} -> maxPixelTracks
else
	nameTag=Run${1}_130
	suffix1=''
	suffix2=''
	if [ ${3} -eq '1' ];then
		suffix1='_allShiftedPlanes'
	fi 
	if [ ${3} -eq '2' ];then
		suffix1='_noShiftedPlanes'
	fi 
	if [ ${3} -eq '3' ];then
		suffix1='_mixedPlanes'
	fi 
	if [ ${4} -eq '1' ];then
		suffix2='_oneTrackInPixels'
	fi 

	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u$UID
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/
	eval `scramv1 runtime -sh`
	eval "cmsRun python/InterpotEfficiency_cfg.py sourceFileList=InputFiles/$nameTag.dat outputFileName=OutputFiles/${nameTag}_interpotEfficiency${suffix1}${suffix2}.root runNumber=${1} recoInfo=${3} maxPixelTracks=${4}"
fi