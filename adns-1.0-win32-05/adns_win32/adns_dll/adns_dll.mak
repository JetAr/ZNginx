# Microsoft Developer Studio Generated NMAKE File, Based on adns_dll.dsp
!IF "$(CFG)" == ""
CFG=adns_dll - Win32 Debug
!MESSAGE No configuration specified. Defaulting to adns_dll - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "adns_dll - Win32 Release" && "$(CFG)" != "adns_dll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "adns_dll.mak" CFG="adns_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "adns_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "adns_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "adns_dll - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\lib\adns_dll.dll"


CLEAN :
	-@erase "$(INTDIR)\adns_dll.obj"
	-@erase "$(INTDIR)\adns_dll.res"
	-@erase "$(INTDIR)\adns_unix_calls.obj"
	-@erase "$(INTDIR)\check.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\general.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\query.obj"
	-@erase "$(INTDIR)\reply.obj"
	-@erase "$(INTDIR)\setup.obj"
	-@erase "$(INTDIR)\transmit.obj"
	-@erase "$(INTDIR)\types.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\adns_dll.pdb"
	-@erase "..\lib\adns_dll.dll"
	-@erase "..\lib\adns_dll.exp"
	-@erase "..\lib\adns_dll.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "../" /I "../../src" /D "NDEBUG" /D "ADNS_DLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADNS_DLL_EXPORTS" /D "ADNS_JGAA_WIN32" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x414 /fo"$(INTDIR)\adns_dll.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\adns_dll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib Ws2_32.lib Iphlpapi.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\adns_dll.pdb" /debug /machine:I386 /out:"../lib/adns_dll.dll" /implib:"../lib/adns_dll.lib" 
LINK32_OBJS= \
	"$(INTDIR)\adns_dll.obj" \
	"$(INTDIR)\adns_unix_calls.obj" \
	"$(INTDIR)\check.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\general.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\query.obj" \
	"$(INTDIR)\reply.obj" \
	"$(INTDIR)\setup.obj" \
	"$(INTDIR)\transmit.obj" \
	"$(INTDIR)\types.obj" \
	"$(INTDIR)\adns_dll.res"

"..\lib\adns_dll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\lib\adns_db_dll.dll" "$(OUTDIR)\adns_dll.bsc"


CLEAN :
	-@erase "$(INTDIR)\adns_dll.obj"
	-@erase "$(INTDIR)\adns_dll.res"
	-@erase "$(INTDIR)\adns_dll.sbr"
	-@erase "$(INTDIR)\adns_unix_calls.obj"
	-@erase "$(INTDIR)\adns_unix_calls.sbr"
	-@erase "$(INTDIR)\check.obj"
	-@erase "$(INTDIR)\check.sbr"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\event.sbr"
	-@erase "$(INTDIR)\general.obj"
	-@erase "$(INTDIR)\general.sbr"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\parse.sbr"
	-@erase "$(INTDIR)\query.obj"
	-@erase "$(INTDIR)\query.sbr"
	-@erase "$(INTDIR)\reply.obj"
	-@erase "$(INTDIR)\reply.sbr"
	-@erase "$(INTDIR)\setup.obj"
	-@erase "$(INTDIR)\setup.sbr"
	-@erase "$(INTDIR)\transmit.obj"
	-@erase "$(INTDIR)\transmit.sbr"
	-@erase "$(INTDIR)\types.obj"
	-@erase "$(INTDIR)\types.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\adns_db_dll.pdb"
	-@erase "$(OUTDIR)\adns_dll.bsc"
	-@erase "..\lib\adns_db_dll.dll"
	-@erase "..\lib\adns_db_dll.exp"
	-@erase "..\lib\adns_db_dll.ilk"
	-@erase "..\lib\adns_db_dll.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../" /I "../../src" /D "_DEBUG" /D "ADNS_DLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADNS_DLL_EXPORTS" /D "ADNS_JGAA_WIN32" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x414 /fo"$(INTDIR)\adns_dll.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\adns_dll.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adns_dll.sbr" \
	"$(INTDIR)\adns_unix_calls.sbr" \
	"$(INTDIR)\check.sbr" \
	"$(INTDIR)\event.sbr" \
	"$(INTDIR)\general.sbr" \
	"$(INTDIR)\parse.sbr" \
	"$(INTDIR)\query.sbr" \
	"$(INTDIR)\reply.sbr" \
	"$(INTDIR)\setup.sbr" \
	"$(INTDIR)\transmit.sbr" \
	"$(INTDIR)\types.sbr"

"$(OUTDIR)\adns_dll.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib Ws2_32.lib Iphlpapi.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\adns_db_dll.pdb" /debug /machine:I386 /out:"../lib/adns_db_dll.dll" /implib:"../lib/adns_db_dll.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\adns_dll.obj" \
	"$(INTDIR)\adns_unix_calls.obj" \
	"$(INTDIR)\check.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\general.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\query.obj" \
	"$(INTDIR)\reply.obj" \
	"$(INTDIR)\setup.obj" \
	"$(INTDIR)\transmit.obj" \
	"$(INTDIR)\types.obj" \
	"$(INTDIR)\adns_dll.res"

"..\lib\adns_db_dll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("adns_dll.dep")
!INCLUDE "adns_dll.dep"
!ELSE 
!MESSAGE Warning: cannot find "adns_dll.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "adns_dll - Win32 Release" || "$(CFG)" == "adns_dll - Win32 Debug"
SOURCE=.\adns_dll.cpp

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\adns_dll.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\adns_dll.obj"	"$(INTDIR)\adns_dll.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\adns_dll.rc

"$(INTDIR)\adns_dll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\adns_unix_calls.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\adns_unix_calls.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\adns_unix_calls.obj"	"$(INTDIR)\adns_unix_calls.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\check.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\check.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\check.obj"	"$(INTDIR)\check.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\event.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\event.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\event.obj"	"$(INTDIR)\event.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\general.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\general.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\general.obj"	"$(INTDIR)\general.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\parse.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\parse.obj"	"$(INTDIR)\parse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\query.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\query.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\query.obj"	"$(INTDIR)\query.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\reply.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\reply.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\reply.obj"	"$(INTDIR)\reply.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\setup.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\setup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\setup.obj"	"$(INTDIR)\setup.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\transmit.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\transmit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\transmit.obj"	"$(INTDIR)\transmit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\types.c

!IF  "$(CFG)" == "adns_dll - Win32 Release"


"$(INTDIR)\types.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "adns_dll - Win32 Debug"


"$(INTDIR)\types.obj"	"$(INTDIR)\types.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

