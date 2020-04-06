#!/bin/bash

function find_newest_set {
	local datasets=$1
	local dates=`echo "$1" | grep -o '\-.*\-'`
	local dates=`eval echo '${dates}' | sed 's/-//g'`
	if [[ $dates == *"_"* ]]; 
	then 
		dates=`eval echo $dates | awk -F_ '{print $1}'`
	fi
	local newest_date=`eval echo $dates | awk '{print $1}'`
	local newest_date_conv=`eval date -d "$newest_date" +%Y%m%d`
	for date in ${dates[@]}
	do
		local date_conv=`eval date -d \"$date\" +%Y%m%d`
		if [ $date_conv -ge $newest_date_conv ]
		then
			local newest_date=$date
		fi
	done
	for dataset in ${datasets[@]}
	do
		newest_dataset=`eval echo $dataset | grep ${newest_date}`
		if [[ -z $newest_dataset ]]
		then
			continue
		else
			break
		fi
	done
}

if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else

	# bash getGridCredentials.sh

	export X509_USER_PROXY=x509up_u$UID
	export CMSSW_BASE=`readlink -f ../../..`

	datasets=(
		ZeroBias
		SingleMuon
		DoubleEG
		)

	nameTag=Run${1}
	jsonFile=/eos/project/c/ctpps/Operations/DataExternalConditions/2017/combined_RPIN_CMS.json
	outputDir=/eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2017/ReRecoOutputTmp_CMSSW_10_6_0_patch1/

	
	echo ""
	echo "*****Setting up for Run ${1}*****"
	eval "mkdir -p -v InputFiles OutputFiles OutputFiles/PlotsRun${1} LogFiles Jobs test/JSONFiles test/InputFiles test/OutputFiles test/LogFiles test/Jobs"
	echo ""
	echo "***Creating file list for ReReco***"

	if [ -f "test/InputFiles/${nameTag}.dat" ] 
	then
		rm test/InputFiles/${nameTag}.dat
		touch test/InputFiles/${nameTag}.dat
	else
		touch test/InputFiles/${nameTag}.dat
	fi

	for dataset in ${datasets[@]}
	do
		echo "Preparing files for dataset \"${dataset}\""
		set=`eval dasgoclient -query=\"dataset run=${1}\" | grep -i /${dataset}/.*/AOD`
		nsets=`echo "${set}" | wc -l`
		if [[ -z ${set} ]]
		then
			echo "Dataset not found"
			echo ""
		else
			if [ ${nsets} -eq 1 ] 
			then
				echo "Using dataset: ${set}"
			else
				set=`echo "${set}" | grep -v PromptReco`
				nsets=`echo "${set}" | wc -l`
				
				if [ "$set" == "" ]
				then
					echo "Only PromptReco sets found"
					set=`eval dasgoclient -query=\"dataset run=${1}\" | grep -i /${dataset}/.*/AOD | grep 'Run201[0-9][[:upper:]]-PromptReco'`
					nsets=`echo "${set}" | wc -l`
					if [ "${nsets}" -eq 1 ]
					then
						echo "Using dataset: ${set}"
					else
						set=`echo $set | awk '{print $1;}'`
					fi
				else
				if [ "${nsets}" -eq 1 ]
					then
						echo "Using dataset: ${set}"
					else
						echo "More than one dataset was found"
						find_newest_set "${set}"
						set=$newest_dataset
						echo "Using dataset: ${set}"
					fi
				fi
			fi
		
			if [ ${set} == "" ]
			then
				echo "ERROR: NO DATASET FOUND!"
				exit 1
			fi

			fnprev=`eval "more test/InputFiles/${nameTag}.dat | wc -l"`
			eval "dasgoclient -query=\"file dataset=${set} run=${1}\" >> test/InputFiles/${nameTag}.dat"
			fn=`eval "more test/InputFiles/${nameTag}.dat | wc -l"`
			echo "ReReco input files found for dataset \"${dataset}\": $(($fn-$fnprev))"
			echo ""
		fi
	done
	fn=`eval "more test/InputFiles/${nameTag}.dat | wc -l"`
	echo "Total ReReco input files found: ${fn}"
	echo "Saved in test/InputFiles/${nameTag}.dat"
	echo ""
	echo "***Creating JSON file***"
	echo "Selecting from JSON file: $jsonFile"
	jsonline=`eval "sed -n -e 's/^.*${1}/\"${1}/p' ${jsonFile}"`
	echo "{${jsonline}}" | sed -n -e 's/,}/}/p' > "test/JSONFiles/${nameTag}.json"
	echo "JSON File contains:"
	eval "more test/JSONFiles/${nameTag}.json"
	echo "Saved as test/JSONFiles/${nameTag}.json"
	echo ""
	echo "***Setting up Input File for Efficiency Analysis***"
	echo "file:$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/OutputFiles/${nameTag}.root" > "InputFiles/${nameTag}.dat"
	echo "Saved as InputFiles/${nameTag}.dat"
	echo ""
	echo "***Setting up links to ReReco OutputFiles***"
	eval "ln -s $outputDir/${nameTag}.root test/OutputFiles/${nameTag}.root"
	echo ""
	echo "Run: ./submitReReco.sh ${1}"
	echo ""
fi