#!/bin/bash

version_file="./buildversion"

if [ -z "$1" ]
then
  echo "Usage: `basename $0` [version string]"
  exit $E_NOARGS
fi

version_string="$1"

if [ ! -e "${version_file}" ]
then
  echo "Version file \"$1\" doesn't exist!"
  exit 1
fi

echo "Set version to \"${version_string}\""
echo -n "${version_string}" > ${version_file}
