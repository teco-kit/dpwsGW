#!/bin/bash

INDENT_OPTIONS="-gnu -nut -sc"

# Check for file

if [ -z "$1" ]
then
  echo "Usage: `basename $0` [file]"
  exit $E_NOARGS
fi

if [ ! -e "$1" ]
then
  echo "File \"$1\" doesn't exist!"
  exit 1
fi

convertfile="$1"

sed_file=`which sed`
if [ ! -e "${sed_file}" ]
then
  echo "Can't find sed!"
  exit 1
fi

${sed_file} -i -e 's/[ ]*$//' ${convertfile}
if [ $? -ne 0 ]
then
  echo "Error calling sed!"
  exit 0
fi

