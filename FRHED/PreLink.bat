@echo OFF
echo Starting %0 ...
REM Remove enclosing double quotes and trailing dir backslash.
set ID=%~1
set TD=%~dp2
set TD=%TD:~0,-1%
echo Temp dir: $(IntDir) = %ID%
echo Target dir: $(TargetPath) = %TD%
cd
cd ..\Translations\Frhed
cscript /NOLOGO CreateMasterPotFile.vbs
cscript /NOLOGO UpdatePoFilesFromPotFile.vbs
cd ..\..\FRHED
rc "/fo%ID%\lang.res" /i.. ..\Translations\Frhed\heksedit.rc
mkdir "%TD%\..\Languages"
link /NOLOGO /DLL /NOENTRY /MACHINE:IX86 "/OUT:%TD%\..\Languages\heksedit.lng" "%ID%\lang.res"
copy ..\Translations\Frhed\*.po "%TD%\..\Languages"
