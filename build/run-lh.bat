@setlocal
@set TMPEXE=Release\Lighthouse3D.exe
@set TMP3RD=F:\Projects\software\bin
@if NOT EXIST %TMPEXE% goto NOEXE
@if NOT EXIST %TMP3RD%\nul goto NO3RD

@set PATH=%TMp3RD%;%PATH%

%TMPEXE%

@goto END

:NOEXE
@echo Can NOT locate %TMPEXE%! *** FIX ME ***
@goto END

:NO3RD
@echo Can NOT locate %TMP3RD%! *** FIX ME ***
@goto END

:END

