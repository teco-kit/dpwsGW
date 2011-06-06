@echo off
set CUR_DIR="%CD%"

set TEST_SUBMISSION_MODE=Experimental

set VC_VARS_DIR="C:\Programme\Microsoft Visual Studio 8\VC"

set CMAKE_GENERATOR="NMake Makefiles"

call %VC_VARS_DIR%\vcvarsall.bat

set BUILD_DEBUG_DIR=%CUR_DIR%\Build-Debug
set INSTALL_DEBUG_DIR=%CUR_DIR%\Install-Debug

set BUILD_RELEASE_DIR=%CUR_DIR%\Build-Release
set INSTALL_RELEASE_DIR=%CUR_DIR%\Install-Release

set BUILD_RELDEBUG_DIR=%CUR_DIR%\Build-RelWithDebInfo
set INSTALL_RELDEBUG_DIR=%CUR_DIR%\Install-RelWithDebInfo

set BUILD_MINSIZE_DIR=%CUR_DIR%\Build-MinSizeRel
set INSTALL_MINSIZE_DIR=%CUR_DIR%\Install-MinSizeRel

md %BUILD_DEBUG_DIR%
md %INSTALL_DEBUG_DIR%

cd %BUILD_DEBUG_DIR%

cmake -G %CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE:STRING="Debug" -DGSOAP_PATH:PATH="%GSOAP_PATH%" -DCMAKE_INSTALL_PREFIX:PATH="%INSTALL_DEBUG_DIR%" ..
if "%errorlevel%"=="0" (
            nmake %TEST_SUBMISSION_MODE%
        )

cd %CUR_DIR%

md %BUILD_RELEASE_DIR%
md %INSTALL_RELEASE_DIR%

cd %BUILD_RELEASE_DIR%

cmake -G %CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE:STRING="Release" -DGSOAP_PATH:PATH="%GSOAP_PATH%" -DCMAKE_INSTALL_PREFIX:PATH="%INSTALL_DEBUG_DIR%" ..
if "%errorlevel%"=="0" (
            nmake %TEST_SUBMISSION_MODE%
        )

cd %CUR_DIR%

md %BUILD_RELDEBUG_DIR%
md %INSTALL_RELDEBUG_DIR%

cd %BUILD_RELDEBUG_DIR%

cmake -G %CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE:STRING="RelWithDebInfo" -DGSOAP_PATH:PATH="%GSOAP_PATH%" -DCMAKE_INSTALL_PREFIX:PATH="%INSTALL_DEBUG_DIR%" ..
if "%errorlevel%"=="0" (
            nmake %TEST_SUBMISSION_MODE%
        )

cd %CUR_DIR%

md %BUILD_MINSIZE_DIR%
md %INSTALL_MINSIZE_DIR%

cd %BUILD_MINSIZE_DIR%

cmake -G %CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE:STRING="MinSizeRel" -DGSOAP_PATH:PATH="%GSOAP_PATH%" -DCMAKE_INSTALL_PREFIX:PATH="%INSTALL_DEBUG_DIR%" ..
if "%errorlevel%"=="0" (
            nmake %TEST_SUBMISSION_MODE%
        )

cd %CUR_DIR%