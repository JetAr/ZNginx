; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTermDoc
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "term.h"
LastPage=0

ClassCount=6
Class1=CMainFrame
Class2=CSetupDlg
Class3=CTermApp
Class4=CAboutDlg
Class5=CTermDoc
Class6=CTermView

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_DIALOG_COMSETTINGS

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CSetupDlg]
Type=0
BaseClass=CDialog
HeaderFile=SetupDlg.h
ImplementationFile=SetupDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_BAUD

[CLS:CTermApp]
Type=0
BaseClass=CWinApp
HeaderFile=Term.h
ImplementationFile=Term.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Term.cpp
ImplementationFile=Term.cpp
LastObject=CAboutDlg

[CLS:CTermDoc]
Type=0
BaseClass=CDocument
HeaderFile=TermDoc.h
ImplementationFile=TermDoc.cpp
Filter=N
VirtualFilter=DC
LastObject=ID_COMM_DISCONNECT

[CLS:CTermView]
Type=0
BaseClass=CEditView
HeaderFile=TermView.h
ImplementationFile=TermView.cpp
Filter=C
VirtualFilter=VWC
LastObject=CTermView

[DLG:IDD_DIALOG_COMSETTINGS]
Type=1
Class=CSetupDlg
ControlCount=20
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,button,1342177287
Control5=IDC_PORT,combobox,1344339970
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BAUD,combobox,1344339970
Control8=IDC_STATIC,static,1342308352
Control9=IDC_DATABITS,combobox,1344339970
Control10=IDC_STATIC,static,1342308352
Control11=IDC_PARITY,combobox,1344339970
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STOPBITS,combobox,1344339970
Control14=IDC_STATIC,static,1342308352
Control15=IDC_NEWLINE,button,1342242819
Control16=IDC_ECHO,button,1342242819
Control17=IDC_STATIC,button,1342177287
Control18=IDC_FLOWCTRL,button,1342308361
Control19=IDC_RADIO2,button,1342177289
Control20=IDC_RADIO3,button,1342177289

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_COMM_SETTINGS
Command11=ID_COMM_CONNECT
Command12=ID_COMM_DISCONNECT
Command13=ID_APP_ABOUT
CommandCount=13

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

