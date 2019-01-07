#!/bin/bash 
# 
# Pipe a file into this and ist spits out a C++ fragment containing the
# contents of the file.
# 
# set -x

if [ $# -eq 0 ]; then
	echo "Need a variable name"
	exit 1
fi

printf "string $1 = "
while IFS= read -r var; do
	TMP=$(echo $var | sed -e 's/^[ \t]*//;s/[ \t]*$//')
	echo "\"$TMP\""
done 
echo ";"
