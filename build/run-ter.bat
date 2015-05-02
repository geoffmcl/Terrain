@setlocal
@set TMPEXE=Release\terrain.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@REM set TMP3RD=F:\FG\18\3rdParty\bin
@REM set TMP3RD=F:\Projects\software\bin
@REM if NOT EXIST %TMP3RD%\nul goto NO3RD

@REM set PATH=%TMp3RD%;%PATH%

%TMPEXE% ..\src\heightmap.bmp

@goto END

:NOEXE
@echo Can NOT locate %TMPEXE%! *** FIX ME ***
@goto END

:NO3RD
@echo Can NOT locate %TMP3RD%! *** FIX ME ***
@goto END

:END

