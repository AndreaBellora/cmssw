if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export X509_USER_PROXY=x509up_u93252
	export SCRAM_ARCH=slc6_amd64_gcc530
	export CMSSW_BASE=`readlink -f ../../..`
	echo ""
	bsub -q 1nd -N -o "$CMSSW_BASE/src/RecoCTPPS/EfficiencyTool_2018/LogFiles/Run${1}.log" -e "$CMSSW_BASE/src/RecoCTPPS/EfficiencyTool_2018/LogFiles/Run${1}.err" -J "Run${1}" "sh jobEfficiencyAnalysis.sh ${1} $CMSSW_BASE"
	echo "Log and Error files are saved in LogFiles/Run${1}.log and LogFiles/Run${1}.err"
	echo "At the end of the job the output will be saved in OutputFiles/Run${1}.root"
	echo ""
	echo "Check the output overview with: root -l 'ViewPlots.C(${1})'"
	echo "If the analysis went well run: ./cleanAndSave.sh ${1}"
	echo ""
fi