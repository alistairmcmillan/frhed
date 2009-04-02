cd
echo %0
echo $(IntDir) = %1
echo $(TargetPath) = %2
cd Languages
cscript CreateMasterPotFile.vbs
cscript UpdatePoFilesFromPotFile.vbs
cd ..
rc /fo%1\lang.res /i.. Languages\heksedit.rc
mkdir %2\..\Languages
link /DLL /NOENTRY /MACHINE:IX86 /OUT:%2\..\Languages\heksedit.lng %1\lang.res
copy Languages\*.po %2\..\Languages
