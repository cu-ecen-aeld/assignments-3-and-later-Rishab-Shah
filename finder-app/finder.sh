#!/bin/bash
filesdir=$1
searchstr=$2

if [ $# -ne 2 ]
then
	echo "ERROR: Invalid number of arguments"
	echo "Total number of arguments should be 2"
	echo "The order of the arguments should be:"
	echo "	1)File Directory path"
	echo "	2)String to be searched in the specified directory path"	
	exit 1
fi
	
if [ ! -d "$filesdir" ]
then
	echo "Please enter a correct directory path"
	exit 1
fi
	

# calculates the total number of files	
filesnum=$(grep "$searchstr" -rl $filesdir | wc -l )	

# finds the number of occurances of the word in the files
no_of_occurances=$(grep "$searchstr" $(find $filesdir -type f) | wc -l)

echo "The number of files are ${filesnum} and the number of matching lines are ${no_of_occurances}"




