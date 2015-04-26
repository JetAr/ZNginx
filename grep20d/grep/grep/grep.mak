# Microsoft Developer Studio Generated NMAKE File, Based on grep.dsp
!IF "$(CFG)" == ""
CFG=grep - Win32 Debug
!MESSAGE No configuration specified. Defaulting to grep - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "grep - Win32 Release" && "$(CFG)" != "grep - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "grep.mak" CFG="grep - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "grep - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "grep - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "grep - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\grep.exe" "$(OUTDIR)\grep.bsc"


CLEAN :
	-@erase "$(INTDIR)\alloca.obj"
	-@erase "$(INTDIR)\alloca.sbr"
	-@erase "$(INTDIR)\dfa.obj"
	-@erase "$(INTDIR)\dfa.sbr"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt.sbr"
	-@erase "$(INTDIR)\grep.obj"
	-@erase "$(INTDIR)\grep.sbr"
	-@erase "$(INTDIR)\kwset.obj"
	-@erase "$(INTDIR)\kwset.sbr"
	-@erase "$(INTDIR)\obstack.obj"
	-@erase "$(INTDIR)\obstack.sbr"
	-@erase "$(INTDIR)\regex.obj"
	-@erase "$(INTDIR)\regex.sbr"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\search.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\grep.bsc"
	-@erase "$(OUTDIR)\grep.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\grep.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\grep.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\alloca.sbr" \
	"$(INTDIR)\dfa.sbr" \
	"$(INTDIR)\getopt.sbr" \
	"$(INTDIR)\grep.sbr" \
	"$(INTDIR)\kwset.sbr" \
	"$(INTDIR)\obstack.sbr" \
	"$(INTDIR)\regex.sbr" \
	"$(INTDIR)\search.sbr"

"$(OUTDIR)\grep.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\grep.pdb" /machine:I386 /out:"$(OUTDIR)\grep.exe" 
LINK32_OBJS= \
	"$(INTDIR)\alloca.obj" \
	"$(INTDIR)\dfa.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\grep.obj" \
	"$(INTDIR)\kwset.obj" \
	"$(INTDIR)\obstack.obj" \
	"$(INTDIR)\regex.obj" \
	"$(INTDIR)\search.obj"

"$(OUTDIR)\grep.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "grep - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\grep.exe" "$(OUTDIR)\grep.bsc"


CLEAN :
	-@erase "$(INTDIR)\alloca.obj"
	-@erase "$(INTDIR)\alloca.sbr"
	-@erase "$(INTDIR)\dfa.obj"
	-@erase "$(INTDIR)\dfa.sbr"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt.sbr"
	-@erase "$(INTDIR)\grep.obj"
	-@erase "$(INTDIR)\grep.sbr"
	-@erase "$(INTDIR)\kwset.obj"
	-@erase "$(INTDIR)\kwset.sbr"
	-@erase "$(INTDIR)\obstack.obj"
	-@erase "$(INTDIR)\obstack.sbr"
	-@erase "$(INTDIR)\regex.obj"
	-@erase "$(INTDIR)\regex.sbr"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\search.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\grep.bsc"
	-@erase "$(OUTDIR)\grep.exe"
	-@erase "$(OUTDIR)\grep.ilk"
	-@erase "$(OUTDIR)\grep.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\grep.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\grep.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\alloca.sbr" \
	"$(INTDIR)\dfa.sbr" \
	"$(INTDIR)\getopt.sbr" \
	"$(INTDIR)\grep.sbr" \
	"$(INTDIR)\kwset.sbr" \
	"$(INTDIR)\obstack.sbr" \
	"$(INTDIR)\regex.sbr" \
	"$(INTDIR)\search.sbr"

"$(OUTDIR)\grep.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\grep.pdb" /debug /machine:I386 /out:"$(OUTDIR)\grep.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\alloca.obj" \
	"$(INTDIR)\dfa.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\grep.obj" \
	"$(INTDIR)\kwset.obj" \
	"$(INTDIR)\obstack.obj" \
	"$(INTDIR)\regex.obj" \
	"$(INTDIR)\search.obj"

"$(OUTDIR)\grep.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("grep.dep")
!INCLUDE "grep.dep"
!ELSE 
!MESSAGE Warning: cannot find "grep.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "grep - Win32 Release" || "$(CFG)" == "grep - Win32 Debug"
SOURCE=.\alloca.c

"$(INTDIR)\alloca.obj"	"$(INTDIR)\alloca.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dfa.c

"$(INTDIR)\dfa.obj"	"$(INTDIR)\dfa.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\getopt.c

"$(INTDIR)\getopt.obj"	"$(INTDIR)\getopt.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\grep.c

"$(INTDIR)\grep.obj"	"$(INTDIR)\grep.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\kwset.c

"$(INTDIR)\kwset.obj"	"$(INTDIR)\kwset.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\obstack.c

"$(INTDIR)\obstack.obj"	"$(INTDIR)\obstack.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\regex.c

"$(INTDIR)\regex.obj"	"$(INTDIR)\regex.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\search.c

"$(INTDIR)\search.obj"	"$(INTDIR)\search.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

