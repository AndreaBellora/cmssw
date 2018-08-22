if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export X509_USER_PROXY=x509up_u93252
	export SCRAM_ARCH=slc6_amd64_gcc530
	export CMSSW_BASE=`readlink -f ../../..`
	bsub -q 1nd -N -o "$CMSSW_BASE/src/RecoCTPPS/EfficiencyTool_2018/LogFiles/Run${1}.log" -e "$CMSSW_BASE/src/RecoCTPPS/EfficiencyTool_2018/LogFiles/Run${1}.err" -J "Run${1}" "sh jobEfficiencyTool_2018.sh ${1} $CMSSW_BASE"
fi