#!/bin/bash
if [ $# -ne 1 ]
then
    echo "Run Number required. Nothing done."
else
	filename=`eval "grep -i \"Failed to open\" LogFiles/Run${1}.log | tail -1 | sed -n -e 's/^.*\/store/\/store/p' | sed -n -e 's/\.root.*/\.root/p'"`
	filename=`echo ${filename}  | sed -n -e 's/\//\\\\\//pg'`
	eval "sed -i -e 's/${filename}/\#${filename}/g' InputFiles/Run${1}.dat"
fi
