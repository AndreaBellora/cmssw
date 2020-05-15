#!/bin/bash
if [ $# -ne 1 ]
then
	echo "Run Number required. Nothing done."
else
	root -l -q -b "CustomViewPlots.C($1,true)"
fi