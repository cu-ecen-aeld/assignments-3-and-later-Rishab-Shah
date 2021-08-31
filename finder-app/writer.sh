#!/bin/bash
writefile=$1
writestr=$2

# Checking if the number of parameters are two
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
DIRECTORY_NAME=$(dirname $writefile)
# echo "$DIRECTORY_NAME"

# extract file name
FILE_NAME=$(basename $writefile)
# echo "$FILE_NAME"

# if not a directory - create one and print status
if [ ! -d $DIRECTORY_NAME ]
then
	echo "Directory does not exist yet"
	mkdir -p "$DIRECTORY_NAME"
	
	if [ -d "$DIRECTORY_NAME" ]
	then
		echo "$DIRECTORY_NAME created"
	else
		echo "$DIRECTORY_NAME failed to create"
		exit 1
	fi
fi

# write in the file
echo "$writestr" > "$DIRECTORY_NAME/$FILE_NAME"

# Check if the file was created sucessfully
if [ $? -eq 0 ] 
then
	echo "file creation successfull"
	exit 0
else
	echo "file creation unsuccessful"
	exit 1
fi


