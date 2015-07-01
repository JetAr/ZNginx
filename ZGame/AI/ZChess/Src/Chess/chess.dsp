# Microsoft Developer Studio Project File - Name="chess" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=chess - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chess.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chess.mak" CFG="chess - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chess - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "chess - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/chess", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chess - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "chess - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "chess - Win32 Release"
# Name "chess - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Alphabeta_HH.cpp
# End Source File
# Begin Source File

SOURCE=.\AlphaBetaAndTT.cpp
# End Source File
# Begin Source File

SOURCE=.\AlphaBetaEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\AspirationSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\chess.cpp
# End Source File
# Begin Source File

SOURCE=.\chess.rc
# End Source File
# Begin Source File

SOURCE=.\chessDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Eveluation.cpp
# End Source File
# Begin Source File

SOURCE=.\FAlphaBetaEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoryHeuristic.cpp
# End Source File
# Begin Source File

SOURCE=.\IDAlphabeta.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\MTD_f.cpp
# End Source File
# Begin Source File

SOURCE=.\NegamaxEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGame.cpp
# End Source File
# Begin Source File

SOURCE=.\PVS_Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TranspositionTable.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Alphabeta_HH.h
# End Source File
# Begin Source File

SOURCE=.\AlphaBetaAndTT.h
# End Source File
# Begin Source File

SOURCE=.\AlphaBetaEngine.h
# End Source File
# Begin Source File

SOURCE=.\AspirationSearch.h
# End Source File
# Begin Source File

SOURCE=.\chess.h
# End Source File
# Begin Source File

SOURCE=.\chessDlg.h
# End Source File
# Begin Source File

SOURCE=.\define.h
# End Source File
# Begin Source File

SOURCE=.\Eveluation.h
# End Source File
# Begin Source File

SOURCE=.\FAlphaBetaEngine.h
# End Source File
# Begin Source File

SOURCE=.\HistoryHeuristic.h
# End Source File
# Begin Source File

SOURCE=.\IDAlphabeta.h
# End Source File
# Begin Source File

SOURCE=.\MoveGenerator.h
# End Source File
# Begin Source File

SOURCE=.\MTD_f.h
# End Source File
# Begin Source File

SOURCE=.\NegamaxEngine.h
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH.h
# End Source File
# Begin Source File

SOURCE=.\NewGame.h
# End Source File
# Begin Source File

SOURCE=.\PVS_Engine.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SearchEngine.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TranspositionTable.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\chess.ico
# End Source File
# Begin Source File

SOURCE=.\res\chess.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
