if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	export CMSSW_BASE=`readlink -f ../../..`
	echo ""
	echo "***Cleaning Up***"
	echo "Removing /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/ReRecoOutputTmp/Run${1}.root and its link"
	eval "rm /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/ReRecoOutputTmp/Run${1}.root ReReco/OutputFiles/Run${1}.root"
	echo "***Saving in Archive***"
	echo "Saving OutputFiles/Run${1}.root in /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/Results2018"
	eval "cp OutputFiles/Run${1}.root /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2018/Results2018/"
	echo ""
fi