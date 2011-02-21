confdir="`dirname $0`/../conf.d"
if [ ! -e "${confdir}/start-stop-service" ]
then
  echo "Can't find configuration directory"
  exit 1
fi

. ${confdir}/start-stop-service

if [ ! -e "${BIN_DIR}/acs_service" ]
then
  echo "Can't find acs_service executable in ${BIN_DIR}"
  exit 1
fi

if [ ! -e "${confdir}/hosting_ip" ]
then
	echo "Hosting service must be initialized first! Can't find ${confdir}/hosting_ip"
	exit 1
fi

. ${confdir}/hosting_ip

acs_service="${BIN_DIR}/acs_service"

case "$1" in
  start)
        echo -n "Starte acs_service ..."
        ${acs_service} -h ${HOSTING_IP} 2>&1 >/dev/null &
        pid=$!
        echo "${pid}" > ${RUN_DIR}/acs_service.pid 
        echo "PID=${pid} done"
        ;;
  stop)
        echo -n "Stopping acs_service ..."
        kill -SIGINT `cat ${RUN_DIR}/acs_service.pid`
        rm ${RUN_DIR}/acs_service.pid
        echo "done"
        ;;
  debug)
        echo "Debugging acs_service ..."
        gdb --args ${acs_service} -h ${HOSTING_IP}
        ;;
  *)
        echo "Unknown option $1"
        exit 1
esac