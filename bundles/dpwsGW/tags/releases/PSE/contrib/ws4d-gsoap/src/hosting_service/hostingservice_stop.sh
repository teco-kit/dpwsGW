#/bin/bash

confdir="`dirname $0`/conf.d"
if [ ! -e "${confdir}/start-stop-service" ]
then
  echo "Can't find configuration directory"
  exit 1
fi

. ${confdir}/start-stop-service

echo -n "Stopping Hosting Service ..."
kill -SIGINT `cat ${RUN_DIR}/hosting.pid`
rm ${RUN_DIR}/hosting.pid
echo "done"