#!/bin/bash
if [ $# -ne 1 ]
then
    echo "Command required. Nothing done."
else
	runs=( 	
			# Add Runs here
			315512 
			315713 
			315840 
			316114 
			316199 
			316240 
			316505 
			# 316666 
			316758 
			316944 
			317182 
			317320 
			317435 
			317527 
			317641 
			317696 
			# TS1
			319337 
			319450 
			319579 
			319756 
			319991 
			320038 
			320804 
			320917 
			321051 
			321149 
			# 321233 
			321396 
			# 321457 
			# 321755 
			# 321831 
			321909 
			# 321988 
			322106 
			# 322252 
			322356
			)

	export CMSSW_BASE=`readlink -f ../../..`
	for run in ${runs[@]}
	do
		eval $1 $run
	done
fi