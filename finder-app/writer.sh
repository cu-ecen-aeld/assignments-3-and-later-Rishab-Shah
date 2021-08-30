#!/bin/bash
writefile=$1
writestr=$2

if [ $# -ne 2 ]
then
	echo "ERROR: Invalid number of arguments"
	echo "Total number of arguments should be 2"
	echo "The order of the arguments should be:"
	echo "	1)Name of the file alo0ng with the directory. Ex - /home/rishab/abcd.txt"
	echo "	2)String to be written inside the file mentioned in arguement 1 i.e. inside abcd.txt"	
	exit 1
fi

# extract directory name to test the directory
directory_name=$(dirname $writefile)
# echo "The dir name is ${directory_name}"

# extract file name
file_name=$(basename $writefile)
# echo "The file name is ${file_name}"

if [ ! -d $directory_name ]
then
	echo "Directory does not exist yet"
	mkdir -p "$directory_name"
	
	if [ -d "$directory_name" ]
	then
		echo "$directory_name created"
	else
		echo "$directory_name failed to create"
		exit 1
	fi
fi

#if [ -d $directory_name ]
#then
#	echo "correct directory path"
#fi	

touch "$directory_name/$file_name"
echo "$writestr" >> "$directory_name/$file_name"
echo "completed writer execution"






