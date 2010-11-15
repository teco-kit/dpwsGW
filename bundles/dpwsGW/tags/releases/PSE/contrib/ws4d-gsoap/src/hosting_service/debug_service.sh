#/bin/bash

sh=/bin/bash

# Check for first parameter
if [ -z "$1" ]
then
  echo "Usage: `basename $0` [service-name]"
  exit 1
fi

confdir="`dirname $0`/conf.d"
if [ ! -e "${confdir}/start-stop-service" ]
then
  echo "Can't find configuration directory"
  exit 1
fi

. ${confdir}/start-stop-service

servicename="$1"

if [ ! -e "${META_DIR}/$1.xml" ]
then
	echo "Can't find metadata file of service $1 in ${META_DIR}"
	exit 1
fi

if [ -e "${ANN_DIR}/$1.xml" ]
then
	echo "Service $1 is already running"
	exit 1
fi

if [ ! -e "${INIT_DIR}/$1" ]
then
	echo "Can't find script to start service $1 in ${INIT_DIR}"
	exit 1
fi

echo "Will announce service in 5 seconds"
(sleep 5; ln -sf ${META_DIR}/$1.xml ${ANN_DIR}/$1.xml) &

$sh "${INIT_DIR}/$1" debug
error=$?

rm ${ANN_DIR}/$1.xml





