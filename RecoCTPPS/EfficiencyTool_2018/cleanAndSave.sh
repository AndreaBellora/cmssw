if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export CMSSW_BASE=`readlink -f ../../..`
	echo "Removing ReReco/OutputFiles/Run${1}.root"
	eval "rm ReReco/OutputFiles/Run${1}.root"
	echo "Saving OutputFiles/Run${1}.root in /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/Results2018"
	eval "cp OutputFiles/Run${1}.root /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/Results2018/"
fi