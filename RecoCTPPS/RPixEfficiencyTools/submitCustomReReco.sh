#!/bin/bash
if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export X509_USER_PROXY=x509up_u93252
	export SCRAM_ARCH=slc6_amd64_gcc530
	export CMSSW_BASE=`readlink -f ../../..`
	if [ -f "test/Jobs/ResolutionSample_Fill6629.sub" ]
	then
		echo ""
		echo "The job file already exists"
		echo "To submit the analysis with the already existing file do:"
		echo "condor_submit test/Jobs/ResolutionSample_Fill6629.sub"
		echo "Otherwise delete the file and rerun this script to generate a new one"
		echo ""
	else
		echo ""
		eval 'echo "executable			= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/CustomReReco.sh"			>> test/Jobs/ResolutionSample_Fill6629.sub'
		eval 'echo "arguments  			= ${1} $CMSSW_BASE" 														>> test/Jobs/ResolutionSample_Fill6629.sub'
		eval 'echo "output 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/ResolutionSample_Fill6629.out" 	>> test/Jobs/ResolutionSample_Fill6629.sub'
		eval 'echo "error 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/ResolutionSample_Fill6629.err" 	>> test/Jobs/ResolutionSample_Fill6629.sub'
		eval 'echo "log 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/ResolutionSample_Fill6629.log" 	>> test/Jobs/ResolutionSample_Fill6629.sub'
		eval 'echo "notify_user			= andrea.bellora@cern.ch"													>> test/Jobs/ResolutionSample_Fill6629.sub'
		eval 'echo "notification		= Always"																	>> test/Jobs/ResolutionSample_Fill6629.sub'
		eval 'echo "+JobFlavour			= \"tomorrow\""																>> test/Jobs/ResolutionSample_Fill6629.sub'
		eval 'echo "queue"																							>> test/Jobs/ResolutionSample_Fill6629.sub'
		
		condor_submit "test/Jobs/ResolutionSample_Fill6629.sub" 
		echo "stdout and stderr files are saved in test/LogFiles/ResolutionSample_Fill6629.out and test/LogFiles/ResolutionSample_Fill6629.err"
		echo "At the end of the job the output will be saved in /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2017/ReRecoOutputTmp/ResolutionSample_Fill6629.root"
		echo "The output is linked in test/OutputFiles/ResolutionSample_Fill6629.root"
		echo ""
		echo "Run at the end of the job: ./submitEfficiencyAnalysis.sh ${1}"
		echo ""
	fi
fi
