#!/bin/bash
if [ $# -ne 4 ]
then
    echo "Four arguments required. Nothing done."
	# ${1} -> nameTag
	# ${2} -> Output directory
	# ${3} -> CMSSW_BASE
	# ${4} -> maxEvents
else
	# read files belonging to one run, print file: in front of them, get them on a single line, separated by commas
	filesToMerge=`eval 'ls ${2} | grep ${1}_"[0-9]"*.root | sed -e "s~^~file:${2}/~" | awk "{print}" ORS=","' `
	# one terabyte file size limit
	cd ${3}/src
	eval `scramv1 runtime -sh`
	edmCopyPickMerge "inputFiles=${filesToMerge::-1}" "outputFile=${2}/${1}.root maxEvents=${4}"
	if [ $? -eq 0 ]
	then
		filesToDelete=`eval 'ls ${2} | grep ${1}_"[0-9]"*.root | sed -e "s~^~${2}/~"'`
		rm $filesToDelete
		outputFile=`ls "${2}" | grep -e "${1}_numEvent.*.root"`
		mv "${2}/$outputFile" "${2}/${1}.root"
		exit 0
	else
		echo "An error during edmCopyPickMerge occurred!"
		exit 1
	fi
fi