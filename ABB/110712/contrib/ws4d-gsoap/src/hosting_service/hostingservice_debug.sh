#/bin/bash

confdir="`dirname $0`/conf.d"
if [ ! -e "${confdir}/start-stop-service" ]
then
  echo "Can't find configuration directory"
  exit 1
fi

. ${confdir}/start-stop-service

if [ ! -e "${BIN_DIR}/hosting_service" ]
then
  echo "Can't find hosting_service executable in ${BIN_DIR}"
  exit 1
fi

hosting="${BIN_DIR}/hosting_service"

if [ ! -e "${confdir}/hosting_ip" ]
then
  echo "${confdir}/hosting_ip doesn't exist. Using eth0 to generate new file"
  ip=`env LC_ALL=C /sbin/ifconfig eth0 | sed -n '/addr:/s/ [^r]*..//gp'`
  echo "HOSTING_IP=\"${ip}\"" >> ${confdir}/hosting_ip
fi

. ${confdir}/hosting_ip

if [ ! -e "${confdir}/hosting_uuid" ]
then
  echo "${confdir}/hosting_uuid doesn't exist. Generating new file"
  uuid=`uuidgen`
  echo "HOSTING_UUID=\"urn:uuid:${uuid}\"" >> ${confdir}/hosting_uuid
fi

. ${confdir}/hosting_uuid

echo -n "Starting Hosting Service ..."
gdb --args ${hosting} -i ${HOSTING_IP} -u ${HOSTING_UUID}
