confdir="`dirname $0`/../conf.d"
if [ ! -e "${confdir}/start-stop-service" ]
then
  echo "Can't find configuration directory"
  exit 1
fi

. ${confdir}/start-stop-service

if [ ! -e "${BIN_DIR}/life-cycle-manager" ]
then
  echo "Can't find life-cycle-manager executable in ${BIN_DIR}"
  exit 1
fi

if [ ! -e "${confdir}/hosting_ip" ]
then
	echo "Hosting service must be initialized first! Can't find ${confdir}/hosting_ip"
	exit 1
fi

. ${confdir}/hosting_ip

lcm="${BIN_DIR}/life-cycle-manager"

case "$1" in
  start)
        echo -n "Starte LCM ..."
        ${lcm} -h ${HOSTING_IP} -l ${ANN_DIR} 2>&1 >/dev/null &
        pid=$!
        echo "${pid}" > ${RUN_DIR}/lcm.pid 
        echo "PID=${pid} done"
        ;;
  stop)
        echo -n "Stopping LCM ..."
        kill -SIGINT `cat ${RUN_DIR}/lcm.pid`
        rm ${RUN_DIR}/lcm.pid
        echo "done"
        ;;
  debug)
        echo "Debugging LCM ..."
        gdb --args ${lcm} -h ${HOSTING_IP} -l ${ANN_DIR}
        ;;
  *)
        echo "Unknown option $1"
        exit 1
esac