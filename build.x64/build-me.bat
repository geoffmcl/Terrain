@setlocal
@set TMPBGN=%TIME%

@REM Setup
@set TMPPRJ=terrain
@echo Building %TMPPRJ% in 64-bits
@set TMPDRV=F:\Projects

@REM ############################################
@REM NOTE: MSVC 10 INSTALL LOCATION
@REM Adjust to suit your environment
@REM ##########################################
@set GENERATOR=Visual Studio 10 Win64
@set VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0
@set VC_BAT=%VS_PATH%\VC\vcvarsall.bat
@if NOT EXIST "%VS_PATH%" goto NOVS
@if NOT EXIST "%VC_BAT%" goto NOBAT
@set BUILD_BITS=%PROCESSOR_ARCHITECTURE%
@IF /i %BUILD_BITS% EQU x86_amd64 (
    @set "RDPARTY_ARCH=x64"
    @set TMPINST=%TMPDRV%\software.x64
) ELSE (
    @IF /i %BUILD_BITS% EQU amd64 (
        @set "RDPARTY_ARCH=x64"
        @set TMPINST=%TMPDRV%\software.x64
    ) ELSE (
        @echo Appears system is NOT 'x86_amd64', nor 'amd64'
        @echo Can NOT build the 64-bit version! Aborting
        @exit /b 1
    )
)

@set CURRDIR=%CD%
@ECHO Setting environment - CALL "%VC_BAT%" %BUILD_BITS%
@CALL "%VC_BAT%" %BUILD_BITS%
@if ERRORLEVEL 1 goto NOSETUP

@cd %CURRDIR%

@set TMPLOG=bldlog-1.txt
@set TMPSRC=..
@set TMP3RD=X:\3rdParty.x64

@REM set BOOST_ROOT=%TMPDRV%\boost_1_53_0
@set ADDINST=0

@REM NOthing below need be touched..
@REM if NOT EXIST X:\nul goto NOXD
@if NOT EXIST %TMPSRC%\nul goto NOSRC
@REM if NOT EXIST %TMP3RD%\nul goto NO3RD
@REM if NOT EXIST %BOOST_ROOT%\nul goto NOBOOST
@if NOT EXIST %TMPSRC%\CMakeLists.txt goto NOSRC2

@set TMPOPTS=-G "%GENERATOR%"
@set TMPOPTS=%TMPOPTS% -DCMAKE_INSTALL_PREFIX=%TMPINST%
@if EXIST %TMP3RD%\nul (
    @set TMPOPTS=%TMPOPTS% -DCMAKE_PREFIX_PATH=%TMP3RD%
) else (
    @echo.
    @echo WARNING: %TMP3RD% not found! So no prefix path
    @echo.
)

:RPT
@if "%~1x" == "x" goto GOTCMD
@set TMPOPTS=%TMPOPTS% %1
@shift
@goto RPT
:GOTCMD

@call chkmsvc %TMPPRJ%

@echo Building %TMPPRJ% in 64-bits Begin %TMPBGN% > %TMPLOG%
@echo All output to %TMPLOG%...

@REM echo Set BOOST_ROOT=%BOOST_ROOT% >> %TMPLOG%

@echo Doing: 'cmake %TMPSRC% %TMPOPTS%'
@echo Doing: 'cmake %TMPSRC% %TMPOPTS%' >> %TMPLOG%
@cmake %TMPSRC% %TMPOPTS% >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1

@echo Doing: 'cmake --build . --config Debug'
@echo Doing: 'cmake --build . --config Debug' >> %TMPLOG%
@cmake --build . --config Debug  >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR2

@echo Doing: 'cmake --build . --config Release'
@echo Doing: 'cmake --build . --config Release' >> %TMPLOG%
@cmake --build . --config Release  >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3

@REM type %TMPLOG%
@fa4 "***" %TMPLOG%
@echo.
@echo Appears a successful build... see %TMPLOG%
@call elapsed %TMPBGN%
@if "%ADDINST%x" == "1x" goto DOINST
@echo.
@echo Install presently OFF
@echo.
@goto END

:DOINST
@echo Proceed with an install - Debug then Release
@pause *** CONTINUE? ***

@echo Doing: 'cmake --build . --config Debug --target INSTALL'
@echo Doing: 'cmake --build . --config Debug --target INSTALL' >> %TMPLOG%
@cmake --build . --config Debug --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR4

@echo Doing: 1cmake --build . --config Release --target INSTALL`
@echo Doing: 1cmake --build . --config Release --target INSTALL` >> %TMPLOG%
@cmake --build . --config Release --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR5

@fa4 " -- " %TMPLOG%

@call elapsed %TMPBGN%
@echo All done...
@goto END

:ERR1
@echo ERROR: Cmake config or geneation FAILED!
@goto ISERR

:ERR2
@echo ERROR: Cmake build Debug FAILED!
@goto ISERR

:ERR3
@echo ERROR: Cmake build Release FAILED!
@goto ISERR

:ERR4
@echo ERROR: Debug install FAILED!
@goto ISERR

:ERR5
@echo ERROR: Release install FAILED!
@goto ISERR

:NOXD
@echo Error: X:\ drive NOT found!
@goto ISERR
 
:NOSRC
@echo Error: No %TMPSRC% found!
@goto ISERR

:NO3RD
@echo Erro: No directory %TMP3RD% found!
@goto ISERR

:NOBOOST
@echo Error: Boost directory %BOOST_ROOT% not found!
@goto ISERR
 
:NOSRC2
@echo Error: File %TMPSRC%\CMakeLists.txt not found!
@goto ISERR

:NOVS
@echo Error: Can NOT locate Visual Studio path "%VS_PATH%"! *** FIX ME ***
@goto ISERR

:NOSETUP
@echo Error: In setting up the Visual Studio 64-bit environment! *** FIX ME ***
@goto ISERR

:NOBAT
@echo Error: Can NOT locate the batch "%VC_BAT%"! *** FIX ME ***
@goto ISERR

:ISERR
@echo See %TMPLOG%
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof

