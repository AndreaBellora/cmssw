#!/bin/bash
if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export X509_USER_PROXY=x509up_u93252
	export CMSSW_BASE=`readlink -f ../../..`
	echo "*****Setting up for Run ${1}*****"
	echo "***Creating file list for ReReco***"
	set=`eval dasgoclient -query=\"dataset run=${1}\" | grep -i /ZeroBias/.*/AOD`
	nsets=`echo "${set}" | wc -l`
	if [ ${nsets} -eq 1 ] 
	then
		echo "Using dataset: ${set}"
	else
		set=`echo "${set}" | grep -i /ZeroBias/Run2018.*2018.*/AOD`
		echo "More than one dataset was found"
		echo "Using dataset: ${set}"
	fi
	eval "dasgoclient -query=\"file dataset=${set} run=${1}\" > ReReco/InputFiles/Run${1}.dat"
	fn=`eval "more ReReco/InputFiles/Run${1}.dat | wc -l"`
	echo "ReReco input files were found: ${fn}"
	echo "Saved in ReReco/InputFiles/Run${1}.dat"

	echo "***Creating JSON file***"
	jsonline=`eval "sed -n -e 's/^.*${1}/\"${1}/p' /eos/project/c/ctpps/Operations/DataExternalConditions/2018/test2018.json"`
	echo "{${jsonline}}" | sed -n -e 's/,}/}/p' > "ReReco/JSONFiles/Run${1}.json"
	echo "JSON File contains: ${jsonline}"
	echo "Saved as ReReco/JSONFiles/Run${1}.json"

	echo "***Setting up Input File for Efficiency Analysis***"
	echo "file:$CMSSW_BASE/src/RecoCTPPS/EfficiencyTool_2018/ReReco/OutputFiles/Run${1}.root" > "InputFiles/Run${1}.dat"
	echo "Saved as InputFiles/Run${1}.dat"
	echo "Run: ./submitReReco.sh ${1}"
fi