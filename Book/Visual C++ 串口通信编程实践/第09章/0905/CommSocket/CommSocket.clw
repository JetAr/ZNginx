; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCommSocketDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "CommSocket.h"

ClassCount=3
Class1=CCommSocketApp
Class2=CCommSocketDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_COMMSOCKET_DIALOG

[CLS:CCommSocketApp]
Type=0
HeaderFile=CommSocket.h
ImplementationFile=CommSocket.cpp
Filter=N

[CLS:CCommSocketDlg]
Type=0
HeaderFile=CommSocketDlg.h
ImplementationFile=CommSocketDlg.cpp
Filter=D
LastObject=IDC_EDIT_SERVERPORT
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=CommSocketDlg.h
ImplementationFile=CommSocketDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_COMMSOCKET_DIALOG]
Type=1
Class=CCommSocketDlg
ControlCount=19
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_BUTTON_START,button,1342242817
Control5=IDC_BUTTON_STOP,button,1342242816
Control6=IDC_EDIT_SERVERPORT,edit,1350639744
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,button,1342177287
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_EDIT_CONNPORT,edit,1350639744
Control12=IDC_BUTTON_CONNECT,button,1342242816
Control13=IDC_EDIT_SENDDATA,edit,1350631552
Control14=IDC_BUTTON_SEND,button,1342242816
Control15=IDC_EDIT_RECVDATA,edit,1350635520
Control16=IDC_BUTTON_DISCONNECT,button,1342242816
Control17=IDC_EDIT_CONNIP,edit,1350631552
Control18=IDC_EDIT_SERVERDATA,edit,1353781444
Control19=IDC_STATIC,static,1342308352

