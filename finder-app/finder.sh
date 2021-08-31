#!/bin/bash
filesdir=$1
searchstr=$2

# Checking for number of parameters
if [ $# -ne 2 ]
then
	echo "ERROR: Invalid number of arguments"
	echo "Total number of arguments should be 2"
	echo "The order of the arguments should be:"
	echo "	1)File Directory path"
	echo "	2)String to be searched in the specified directory path"	
	exit 1
fi
	
# Checking for valid directory
if [ ! -d "$filesdir" ]
then
	echo "Please enter a correct directory path"
	exit 1
fi
	

# Calculates the total number of files in which match is hit	
FILESNUM=$(grep "$searchstr" -rl $filesdir | wc -l )	

# Finds the number of occurances of the word per line in the files in which match was hit
NUM_OCCURANCES=$(grep "$searchstr" $(find $filesdir -type f) | wc -l)

echo "The number of files are ${FILESNUM} and the number of matching lines are ${NUM_OCCURANCES}"


