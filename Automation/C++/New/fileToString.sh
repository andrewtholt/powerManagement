#!/bin/bash 

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
