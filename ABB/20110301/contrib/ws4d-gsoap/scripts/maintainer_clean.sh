#!/bin/bash

CUR_DIR=$(pwd)

BUILD_DEBUG_DIR=${CUR_DIR}/Build-Debug
INSTALL_DEBUG_DIR=${CUR_DIR}/Install-Debug

BUILD_RELEASE_DIR=${CUR_DIR}/Build-Release
INSTALL_RELEASE_DIR=${CUR_DIR}/Install-Release

BUILD_RELDEBUG_DIR=${CUR_DIR}/Build-RelWithDebInfo
INSTALL_RELDEBUG_DIR=${CUR_DIR}/Install-RelWithDebInfo

BUILD_MINSIZE_DIR=${CUR_DIR}/Build-MinSizeRel
INSTALL_MINSIZE_DIR=${CUR_DIR}/Install-MinSizeRel

if [ -d "${BUILD_DEBUG_DIR}" ]; then
  echo -n "Cleaning ${BUILD_DEBUG_DIR} ... "
  rm -r "${BUILD_DEBUG_DIR}"
  echo "done"
fi

if [ -d "${INSTALL_DEBUG_DIR}" ]; then
  echo -n "Cleaning ${INSTALL_DEBUG_DIR} ... "
  rm -r "${INSTALL_DEBUG_DIR}"
  echo "done"
fi

if [ -d "${BUILD_RELEASE_DIR}" ]; then
  echo -n "Cleaning ${BUILD_RELEASE_DIR} ... "
  rm -r "${BUILD_RELEASE_DIR}"
  echo "done"
fi

if [ -d "${INSTALL_RELEASE_DIR}" ]; then
  echo -n "Cleaning ${INSTALL_RELEASE_DIR} ... "
  rm -r "${INSTALL_RELEASE_DIR}"
  echo "done"
fi

if [ -d "${BUILD_RELDEBUG_DIR}" ]; then
  echo -n "Cleaning ${BUILD_RELDEBUG_DIR} ... "
  rm -r "${BUILD_RELDEBUG_DIR}"
  echo "done"
fi

if [ -d "${INSTALL_RELDEBUG_DIR}" ]; then
  echo -n "Cleaning ${INSTALL_RELDEBUG_DIR} ... "
  rm -r "${INSTALL_RELDEBUG_DIR}"
  echo "done"
fi

if [ -d "${BUILD_MINSIZE_DIR}" ]; then
  echo -n "Cleaning ${BUILD_MINSIZE_DIR} ... "
  rm -r "${BUILD_MINSIZE_DIR}"
  echo "done"
fi

if [ -d "${INSTALL_MINSIZE_DIR}" ]; then
  echo -n "Cleaning ${INSTALL_MINSIZE_DIR} ... "
  rm -r "${INSTALL_MINSIZE_DIR}"
  echo "done"
fi
