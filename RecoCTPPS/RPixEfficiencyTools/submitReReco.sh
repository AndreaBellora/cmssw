if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export X509_USER_PROXY=x509up_u93252
	export SCRAM_ARCH=slc7_amd64_gcc700
	export CMSSW_BASE=`readlink -f ../../..`
	if [ -f "test/Jobs/Run${1}.sub" ]
	then
		echo ""
		echo "The job file already exists"
		echo "To submit the analysis with the already existing file do:"
		echo "condor_submit test/Jobs/Run${1}.sub"
		echo "Otherwise delete the file and rerun this script to generate a new one"
		echo ""
	else
		echo ""
		eval 'echo "executable			= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/ReReco.sh"					>> test/Jobs/Run${1}.sub'
		eval 'echo "arguments  			= ${1} $CMSSW_BASE" 														>> test/Jobs/Run${1}.sub'
		eval 'echo "output 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/Run${1}.out" 	>> test/Jobs/Run${1}.sub'
		eval 'echo "error 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/Run${1}.err" 	>> test/Jobs/Run${1}.sub'
		eval 'echo "log 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/Run${1}.log" 	>> test/Jobs/Run${1}.sub'
		eval 'echo "notify_user			= andrea.bellora@cern.ch"													>> test/Jobs/Run${1}.sub'
		eval 'echo "notification		= Always"																	>> test/Jobs/Run${1}.sub'
		eval 'echo "use_x509userproxy   = true"																		>> test/Jobs/Run${1}.sub'
		eval 'echo "+JobFlavour			= \"nextweek\""																>> test/Jobs/Run${1}.sub'
		eval 'echo "queue"																							>> test/Jobs/Run${1}.sub'
		
		condor_submit "test/Jobs/Run${1}.sub" 
		echo "stdout and stderr files are saved in test/LogFiles/Run${1}.out and test/LogFiles/Run${1}.err"
		echo "At the end of the job the output will be saved in /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/ReRecoOutputTmp_CMSSW_10_6_0_pre3/Run${1}.root"
		echo "The output is linked in test/OutputFiles/Run${1}.root"
		echo ""
		echo "Run at the end of the job: ./submitEfficiencyAnalysis.sh ${1}"
		echo ""
	fi
fi
