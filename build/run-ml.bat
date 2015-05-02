@setlocal
@set TMPEXE=F:\Projects\terrain\build\Release\ML_Terrain.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@set TMPRT=..\src\ML_Terrain
@if NOT EXIST %TMPRT%\nul goto NORT

cd %TMPRT%

%TMPEXE%


@goto END

:NORT
@echo Error: Can NOT locate %TMPRT%! *** FIX ME ***
@goto END

:NOEXE
@echo Error: Can NOT locate %TMPEXE%! *** FIX ME ***
@goto END


:END
