cd
echo %0
echo $(IntDir) = %1
echo $(TargetPath) = %2
cd ..\Translations\Frhed
cscript CreateMasterPotFile.vbs
cscript UpdatePoFilesFromPotFile.vbs
cd ..\..\FRHED
rc /fo%1\lang.res /i.. ..\Translations\Frhed\heksedit.rc
mkdir %2\..\Languages
link /DLL /NOENTRY /MACHINE:IX86 /OUT:%2\..\Languages\heksedit.lng %1\lang.res
copy ..\Translations\Frhed\*.po %2\..\Languages
