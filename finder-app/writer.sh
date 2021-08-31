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
# echo "$directory_name"

# extract file name
file_name=$(basename $writefile)
# echo "file_name"

# if not a directory - create one and print status
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

# write in the file

echo "$writestr" > "$directory_name/$file_name"

if [ $? -eq 0 ] 
then
	echo "file creation successfull"
	exit 0
else
	echo "file creation unsuccessful"
	exit 1
fi

