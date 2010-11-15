#!/bin/bash

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

expand_file=`which expand`
if [ ! -e "${expand_file}" ]
then
  echo "Can't find expand!"
  exit 1
fi

cmp_file=`which cmp`
if [ ! -e "${cmp_file}" ]
then
  echo "Can't find cmp!"
  exit 1
fi

spacesfile=`mktemp /tmp/spaces.XXXXXXXXXX` || exit 1
convertfile="$1"

${expand_file} -t 2 ${convertfile} > ${spacesfile}
if [ $? -ne 0 ]
then
  echo "Error calling expand!"
  exit 0
fi

${cmp_file} -s ${convertfile} ${spacesfile}
if [ $? -eq 1 ]
then
  mv ${spacesfile} ${convertfile}
else
  rm ${spacesfile}
fi
