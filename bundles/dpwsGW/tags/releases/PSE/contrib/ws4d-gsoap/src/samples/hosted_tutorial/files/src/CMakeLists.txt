IF(COMMAND CMAKE_POLICY)
   CMAKE_POLICY(SET CMP0003 OLD)
ENDIF(COMMAND CMAKE_POLICY)


################################################################################
# Don't change this !
################################################################################

# directory to put all generated files in
SET(gen_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)

# If the directory for the generated sources does not
# already exists, create it.
IF(NOT EXISTS ${gen_DIR})
	FILE(MAKE_DIRECTORY ${gen_DIR})
ENDIF(NOT EXISTS ${gen_DIR})

# set include paths
INCLUDE_DIRECTORIES(${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES} ${gen_DIR})

###############################################################################
# hosting service specific directories
###############################################################################

#TODO create cmake-module for this
SET(HOSTINGSERVICE_CONFIG_FILE ${DPWS_PATH}/etc/ws4d-gSOAP/conf.d/hosting.xml CACHE STRING "" FORCE)
SET(HOSTEDSERVICE_META_DIR ${DPWS_PATH}/etc/ws4d-gSOAP/meta.d CACHE STRING "" FORCE)
SET(HOSTEDSERVICE_CONF_DIR ${DPWS_PATH}/etc/ws4d-gSOAP/conf.d CACHE STRING "" FORCE)
SET(HOSTEDSERVICE_INIT_DIR ${DPWS_PATH}/etc/ws4d-gSOAP/init.d CACHE STRING "" FORCE)
SET(HOSTEDSERVICE_ANNC_DIR ${DPWS_PATH}/etc/ws4d-gSOAP/announce.d CACHE STRING "" FORCE)
SET(HOSTEDSERVICE_BIN_DIR ${DPWS_PATH}/bin CACHE STRING "" FORCE)
SET(HOSTEDSERVICE_RUN_DIR ${DPWS_PATH}/var/run CACHE STRING "" FORCE)


###############################################################################
# Code generation - for custom projects this must be modified
###############################################################################

# generate code and C data binding operations of service
GSOAP_GENERATE(acs.gsoap acs1 ${gen_DIR})   # normal service

# generate code and C data binding for events of service
GSOAP_GENERATE(acs_inv.gsoap acs_inv1 ${gen_DIR}) # event service

# generate device description and setup code
DPWS_METADATA_GENERATE(metadata.xml acs ${gen_DIR} hosted)

# embed wsdl files into c code
SET(AIRCONDITIONER_WSDL ${CMAKE_CURRENT_SOURCE_DIR}/AirConditioner.wsdl)
DPWS_EMBEDD_WSDL(${gen_DIR} acs ${AIRCONDITIONER_WSDL})

################################################################################
# create config.h
################################################################################

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${gen_DIR}/config.h
               @ONLY IMMEDIATE)

################################################################################
# set options for gSOAP runtime
################################################################################

# stdsoap2.c
SET_SOURCE_FILES_PROPERTIES(
	PROPERTIES COMPILE_FLAGS "-DWITH_NONAMESPACES -DWITH_UDP")

################################################################################
# build AirConditioner device - for custom projects this must be modified
################################################################################

# create airconditioner executable und define dependencies
# the device includes the airconditioner.c and event_worker.c, the
# gsoap runtime (), the server lib by gsoap
# (${gen_DIR}/acs1ServerLib.c), the client lib of the inverted service
# by gsoap for eventing (${gen_DIR}/acs_inv1ClientLib.c), the device
# description and setup code (${gen_DIR}/acs_metadata.c) and the embedded
# wsdl files (${gen_DIR}/acs_wsdl.c)

ADD_EXECUTABLE(acs_service
	airconditioner.c event_worker.c  ${gen_DIR}/acs1ServerLib.c ${gen_DIR}/acs_inv1ClientLib.c ${gen_DIR}/acs_metadata.c ${gen_DIR}/acs_wsdl.c)

EXTEND_TGT_COMPILE_FLAGS(acs_service FLAGS "-DDPWS_DEVICE -DDPWS_HOSTED_SERVICE -DWITH_MUTEXES")

# link the device specific dpws libraries and the pthread library into
# the airconditioner executable
TARGET_LINK_LIBRARIES(acs_service ${DPWS_LIBRARIES} ${DPWS-HMT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

IF(LCM_PACKAGE)

	# directory to put all generated files in
	SET(PACKAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/package)
	SET(PACKAGE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lcm_package)

	# If the directory for the generated sources does not
	# already exists, create it.
	IF(NOT EXISTS ${PACKAGE_DIR})
		FILE(MAKE_DIRECTORY ${PACKAGE_DIR})
	ENDIF(NOT EXISTS ${PACKAGE_DIR})

	SET(PACKAGE_FILES
		${PACKAGE_DIR}/acs_service
		${PACKAGE_DIR}/acs_service.xml)
		
	GET_TARGET_PROPERTY(acs_service_LOCATION acs_service LOCATION)
		
	SET(PACKAGE_FILES_DEPENDS
		acs_service
		${CMAKE_CURRENT_SOURCE_DIR}/metadata.xml)

	ADD_CUSTOM_COMMAND(OUTPUT ${PACKAGE_FILES}
					   COMMAND ${CMAKE_COMMAND} -E copy ${acs_service_LOCATION} ${PACKAGE_DIR}/acs_service
				       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/metadata.xml ${PACKAGE_DIR}/acs_service.xml
				       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PACKAGE_SRC_DIR} ${PACKAGE_DIR}
				       DEPENDS ${PACKAGE_FILES_DEPENDS})

	ADD_CUSTOM_COMMAND(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/acs_service.zip
				       COMMAND zip ${EXECUTABLE_OUTPUT_PATH}/acs_service.zip *
				       WORKING_DIRECTORY ${PACKAGE_DIR}
				       DEPENDS ${PACKAGE_FILES})
				       
	ADD_CUSTOM_TARGET(lcm_package ALL 
	                  DEPENDS ${EXECUTABLE_OUTPUT_PATH}/acs_service.zip)

ELSEIF(LCM_PACKAGE)
INSTALL(TARGETS acs_service RUNTIME DESTINATION ${DPWS_PATH}/bin)

INSTALL(FILES metadata.xml RENAME acs_service.xml DESTINATION ${HOSTEDSERVICE_META_DIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/init_script.sh RENAME acs_service
		DESTINATION ${HOSTEDSERVICE_INIT_DIR}
		PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)
ENDIF(LCM_PACKAGE)
