#!/bin/bash
if [ $# -ne 4 ]
then
	echo "Four arguments required. Nothing done."
	# ${1} -> Run Number
	# ${2} -> CMSSW_BASE
	# ${3} -> Events to be checked
	# ${4} -> Job number
else
	
	function checkAndComment
	{	
		sourceFileName="${2}/src/RecoCTPPS/RPixEfficiencyTools/test/InputFiles/Run${1}_${4}.dat"
		cmsRun ${2}/src/RecoCTPPS/RPixEfficiencyTools/test/ReReco.py runNumber=${1} skipEvents=${3} maxEvents=1 sourceFileName=$sourceFileName\
		outputFileName=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/Run${1}_${4}_test.root \
		| tee ${2}/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/Run${1}_test.out
		exit_code=$PIPESTATUS
		echo "Exit code of cmsRun is: $exit_code"
		if [ $exit_code -ne 0 ]
		then
			lastline=`eval "tail -1 ${2}/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/Run${1}_${4}_testJob.out | grep Closed"`
			if [ -z "$lastline" ]
			then 
				fail_line=`eval "tac ${2}/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/Run${1}_${4}_testJob.out | grep -m 1 -o /store/.*.root"`
				echo "Excluding file: ${fail_line}"
				fail_line=`echo "$fail_line" | sed -e 's,\\/,\\\\/,g'`
				sed -i -e "s/$fail_line/#$fail_line/" ${2}/src/RecoCTPPS/RPixEfficiencyTools/test/InputFiles/Run${1}_${4}.dat
				checkAndComment ${1} ${2} ${3} ${4}
			else
				echo "Some error occurred, but testFiles.sh was not able to solve it."
				exit 1
			fi
		else
			echo "All files ok."
		fi
	}
	
	export CMSSW_BASE=${2}
	export SCRAM_ARCH=slc7_amd64_gcc700
	export X509_USER_PROXY=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/x509up_u$UID
	cd $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test
	eval `scramv1 runtime -sh`
	checkAndComment ${1} ${2} ${3} ${4}

	if [ -f $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/Run${1}_${4}_test.root ]
	then
		rm $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/Run${1}_${4}_test.root
	fi
fi