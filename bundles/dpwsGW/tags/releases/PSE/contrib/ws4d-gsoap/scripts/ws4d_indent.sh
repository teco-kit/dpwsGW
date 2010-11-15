#!/bin/bash

INDENT_OPTIONS="-gnu -nut -sc" # -hnl

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

indent_file=`which indent`
if [ ! -e "${indent_file}" ]
then
  echo "Can't find indent!"
  exit 1
fi

${indent_file} -npro ${INDENT_OPTIONS} ${convertfile}
if [ $? -ne 0 ]
then
  echo "Error calling indent!"
  exit 0
fi

