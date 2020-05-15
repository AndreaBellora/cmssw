#!/bin/bash
if [ $# -ne 1 ]
then
    echo "Command required. Nothing done."
else
	runs=( 	
			# Add Runs here
			# END of EraA
			# 297050 # DONE
			# 297219 # DONE
			# # TS1
			# 299061 # DONE
			# # END of EraB
			# 299370 # DONE
			# 299597 # DONE
			# 300155 # DONE
			# 300280 # DONE
			# 300401 # DONE
			# 300558 # DONE
			# 300576 # DONE
			# # From 300802 45-220F 3+3
			# 301161 # low stat
			# 301323 
			# 301627 
			# 301998 # DONE
			# END of EraC
			# 302225
			# 302388
			# 302485 
			# 302573
			# 302663
			# END of EraD
			# # TS2
			# # 45-220F again in normal mode
			# 303832 # DONE 
			# 303885 # DONE
			# 304062 # DONE
			# 304144 # DONE
			# 304204 # DONE
			# 304333 # DONE
			# 304447 # DONE
			# 304616 # DONE
			# 304671 # DONE
			# 304778 # DONE <- lowstat
			# # END of EraE	
			# 305045 # DONE
			# 305081 # DONE
			# # From 305169 45-220F 3+3
			# 305202
			# 305237
			# 305310
			# 305365
			# 305377
			# 305518
			# 305636
			# 305814
			# 305842 # DONE
			# # From 305965 56-220 3+3
			# 306092
			# 306125
			# 306154
			# 306256
			# 306459
			# END of EraF
			# FOR 3+3 Analysis
			297057
			# 300576
			301283
			# 305081
			305202
			# 306092
			)

	export CMSSW_BASE=`readlink -f ../../..`
	for run in ${runs[@]}
	do
		echo "$1 $run"
		eval $1 $run
	done
fi