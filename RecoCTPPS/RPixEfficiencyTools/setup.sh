#!/bin/bash
if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export X509_USER_PROXY=x509up_u93252
	export CMSSW_BASE=`readlink -f ../../..`
	echo ""
	echo "*****Setting up for Run ${1}*****"
	eval "mkdir -p -v InputFiles OutputFiles OutputFiles/PlotsRun${1} LogFiles test/JSONFiles test/InputFiles test/OutputFiles test/LogFiles"
	echo ""
	echo "***Creating file list for ReReco***"
	set=`eval dasgoclient -query=\"dataset run=${1}\" | grep -i /ZeroBias/.*/AOD`
	nsets=`echo "${set}" | wc -l`
	if [ ${nsets} -eq 1 ] 
	then
		echo "Using dataset: ${set}"
	else
		set=`echo "${set}" | grep -i /ZeroBias/Run2018.*2018-v.*/AOD`
		nsets=`echo "${set}" | wc -l`
		if [ ${nsets} -eq 1 ]
		then
			echo "More than one dataset was found"
			echo "Using dataset: ${set}"
		else
			echo "More than one dataset was found"
			set=`echo "${set}" | tail -1`
			echo "Using dataset: ${set}"
		fi
	fi
	eval "dasgoclient -query=\"file dataset=${set} run=${1}\" > test/InputFiles/Run${1}.dat"
	fn=`eval "more test/InputFiles/Run${1}.dat | wc -l"`
	echo "ReReco input files were found: ${fn}"
	echo "Saved in test/InputFiles/Run${1}.dat"
	echo ""
	echo "***Creating JSON file***"
	jsonline=`eval "sed -n -e 's/^.*${1}/\"${1}/p' /eos/project/c/ctpps/Operations/DataExternalConditions/2018/2018_2RPGood_anyarms_ls.json"`
	echo "{${jsonline}}" | sed -n -e 's/,}/}/p' > "test/JSONFiles/Run${1}.json"
	echo "JSON File contains:"
	eval "more test/JSONFiles/Run${1}.json"
	echo "Saved as test/JSONFiles/Run${1}.json"
	echo ""
	echo "***Setting up Input File for Efficiency Analysis***"
	echo "file:$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/OutputFiles/Run${1}.root" > "InputFiles/Run${1}.dat"
	echo "Saved as InputFiles/Run${1}.dat"
	echo ""
	echo "***Setting up links to ReReco OutputFiles***"
	eval "ln -s /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/ReRecoOutputTmp/Run${1}.root test/OutputFiles/Run${1}.root"
	echo ""
	echo "Run: ./submitReReco.sh ${1}"
	echo ""
fi