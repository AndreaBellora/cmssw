if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	# Maximum number of events to be analyzed
	maxEvents=3000000
	filesPerEvent=20
	nameTag=Run${1}
	outputDir=/eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2017/ReRecoOutputTmp_CMSSW_10_6_0_patch1

	# bash getGridCredentials.sh

	export X509_USER_PROXY=x509up_u$UID
	export SCRAM_ARCH=slc7_amd64_gcc700
	export CMSSW_BASE=`readlink -f ../../..`

	if [ -f "test/Jobs/${nameTag}.dag" ]
	then
		eval 'ls test/Jobs/ | grep "${nameTag}.*" | sed -e "s/^/test\/Jobs\//" | xargs rm' # removes all Job files related to DAG
	fi
	
	inputFile=$CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/InputFiles/${nameTag}.dat
	linesInInputFile=`cat $inputFile | wc -l`
	njobs=$(((linesInInputFile+filesPerEvent-1)/filesPerEvent))

	echo "Input files: $linesInInputFile"
	echo "Dividing in $njobs jobs..."

	eval 'echo "executable			= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/mergeFiles.sh"							>> test/Jobs/${nameTag}_mergeJob.sub'
	eval 'echo "arguments  			= ${nameTag} ${outputDir} $CMSSW_BASE $maxEvents"											>> test/Jobs/${nameTag}_mergeJob.sub'
	eval 'echo "output 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_mergeJob.out"	>> test/Jobs/${nameTag}_mergeJob.sub'
	eval 'echo "error 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_mergeJob.err"	>> test/Jobs/${nameTag}_mergeJob.sub'
	eval 'echo "log 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_mergeJob.log"	>> test/Jobs/${nameTag}_mergeJob.sub'
	eval 'echo "notify_user			= andrea.bellora@cern.ch"																>> test/Jobs/${nameTag}_mergeJob.sub'
	eval 'echo "notification		= Always"																				>> test/Jobs/${nameTag}_mergeJob.sub'
	eval 'echo "+JobFlavour			= \"workday\""																			>> test/Jobs/${nameTag}_mergeJob.sub'
	eval 'echo "queue"																										>> test/Jobs/${nameTag}_mergeJob.sub'

	eval 'echo "JOB mergeJob test/Jobs/${nameTag}_mergeJob.sub"																>> test/Jobs/${nameTag}.dag'

	i=0
	while [ $i -lt $njobs ] 
	do
		# Create separated input file lists for each job
		awk "NR >= $(($filesPerEvent*($i)+1)) && NR <= $(($filesPerEvent*($i+1)))" $inputFile > test/InputFiles/${nameTag}_${i}.dat

		# Create submit files for each job, with their test
		# eval 'echo "executable			= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/testFiles.sh"								>> test/Jobs/${nameTag}_${i}_testJob.sub'
		# eval 'echo "arguments  			= ${1} $CMSSW_BASE $(($filesPerEvent*100000)) ${i}"											>> test/Jobs/${nameTag}_${i}_testJob.sub'
		# eval 'echo "output 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_${i}_testJob.out"	>> test/Jobs/${nameTag}_${i}_testJob.sub'
		# eval 'echo "error 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_${i}_testJob.err"	>> test/Jobs/${nameTag}_${i}_testJob.sub'
		# eval 'echo "log 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_${i}_testJob.log"	>> test/Jobs/${nameTag}_${i}_testJob.sub'
		# eval 'echo "notify_user			= andrea.bellora@cern.ch"																	>> test/Jobs/${nameTag}_${i}_testJob.sub'
		# eval 'echo "notification		= Always"																					>> test/Jobs/${nameTag}_${i}_testJob.sub'
		# eval 'echo "+JobFlavour			= \"nextweek\""																				>> test/Jobs/${nameTag}_${i}_testJob.sub'
		# eval 'echo "queue"																											>> test/Jobs/${nameTag}_${i}_testJob.sub'

		eval 'echo "executable			= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/skimFiles.sh"						>> test/Jobs/${nameTag}_${i}.sub'
		eval 'echo "arguments  			= ${nameTag} $CMSSW_BASE 0 -1 $outputDir/${nameTag}_${i}.root $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/InputFiles/${nameTag}_${i}.dat"						>> test/Jobs/${nameTag}_${i}.sub'
		eval 'echo "output 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_${i}.out" 	>> test/Jobs/${nameTag}_${i}.sub'
		eval 'echo "error 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_${i}.err" 	>> test/Jobs/${nameTag}_${i}.sub'
		eval 'echo "log 				= $CMSSW_BASE/src/RecoCTPPS/RPixEfficiencyTools/test/LogFiles/${nameTag}_${i}.log" 	>> test/Jobs/${nameTag}_${i}.sub'
		eval 'echo "notify_user			= andrea.bellora@cern.ch"															>> test/Jobs/${nameTag}_${i}.sub'
		eval 'echo "notification		= Always"																			>> test/Jobs/${nameTag}_${i}.sub'
		eval 'echo "+JobFlavour			= \"tomorrow\""																		>> test/Jobs/${nameTag}_${i}.sub'
		eval 'echo "queue"																									>> test/Jobs/${nameTag}_${i}.sub'
		
		eval 'echo "JOB job_${i} test/Jobs/${nameTag}_${i}.sub"																>> test/Jobs/${nameTag}.dag'
		# eval 'echo "JOB testJob_${i} test/Jobs/${nameTag}_${i}_testJob.sub"													>> test/Jobs/${nameTag}.dag'

		# eval 'echo "PARENT testJob_${i} CHILD job_${i}"																		>> test/Jobs/${nameTag}.dag'
		eval 'echo "PARENT job_${i} CHILD mergeJob"																			>> test/Jobs/${nameTag}.dag'
		eval 'echo "RETRY job_${i} 3"																						>> test/Jobs/${nameTag}.dag'

		i=$(($i+1))
	done

	condor_submit_dag -notification Always -batch-name "${nameTag}" "test/Jobs/${nameTag}.dag" 
	echo "stdout and stderr files are saved in test/LogFiles/${nameTag}_*.out and test/LogFiles/${nameTag}_*.err"
	echo "At the end of the job the output will be saved in /eos/project/c/ctpps/subsystems/Pixel/RPixTracking/EfficiencyCalculation2017/ReRecoOutputTmp_CMSSW_10_6_0_patch1/${nameTag}.root"
	echo "The output is linked in test/OutputFiles/${nameTag}.root"
	echo ""
	echo "Run at the end of the job: ./submitEfficiencyAnalysis.sh ${1}"
	echo ""

fi