# Microsoft Developer Studio Project File - Name="RAWIO32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=RAWIO32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RAWIO32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RAWIO32.mak" CFG="RAWIO32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RAWIO32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RAWIO32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RAWIO32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Build\Release"
# PROP Intermediate_Dir "..\BuildTmp\RAWIO32\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_CRT_SECURE_NO_DEPRECATE" /FR /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib thunk32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /dll /machine:I386 /ALIGN:16384
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "RAWIO32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\Debug"
# PROP Intermediate_Dir "..\BuildTmp\RAWIO32\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "STRICT" /D "_CRT_SECURE_NO_DEPRECATE" /FR /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 thunk32.lib shell32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /pdbtype:sept /ALIGN:16384

!ENDIF 

# Begin Target

# Name "RAWIO32 - Win32 Release"
# Name "RAWIO32 - Win32 Debug"
# Begin Group "Sources"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\precomp.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# Begin Source File

SOURCE=.\RAWIO32.cpp
# End Source File
# End Group
# Begin Group "Includes"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\precomp.h
# End Source File
# Begin Source File

SOURCE=.\RAWIO32.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\RAWIO32.def
# End Source File
# Begin Source File

SOURCE=..\thunk\thk32.obj
# End Source File
# End Target
# End Project
