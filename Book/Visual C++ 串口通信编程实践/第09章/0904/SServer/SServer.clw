; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSServerDlg
LastTemplate=CSocket
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "SServer.h"

ClassCount=5
Class1=CSServerApp
Class2=CSServerDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CListenSocket
Class5=CAcceptSocket
Resource3=IDD_SSERVER_DIALOG

[CLS:CSServerApp]
Type=0
HeaderFile=SServer.h
ImplementationFile=SServer.cpp
Filter=N

[CLS:CSServerDlg]
Type=0
HeaderFile=SServerDlg.h
ImplementationFile=SServerDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_CHECK_SENDDATA

[CLS:CAboutDlg]
Type=0
HeaderFile=SServerDlg.h
ImplementationFile=SServerDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SSERVER_DIALOG]
Type=1
Class=CSServerDlg
ControlCount=15
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_PORTNO,edit,1350639744
Control5=IDC_BUTTON_START,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_NETMSG,edit,1350631556
Control8=IDC_BUTTON_STOP,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_COMBO_COMPORT,combobox,1344340226
Control11=IDC_BUTTON_OPEN,button,1342242816
Control12=IDC_BUTTON_CLOSE,button,1342242816
Control13=IDC_STATIC,static,1342308352
Control14=IDC_EDIT_COMMSG,edit,1350631556
Control15=IDC_CHECK_SENDDATA,button,1342242819

[CLS:CListenSocket]
Type=0
HeaderFile=ListenSocket.h
ImplementationFile=ListenSocket.cpp
BaseClass=CSocket
Filter=N
LastObject=CListenSocket
VirtualFilter=uq

[CLS:CAcceptSocket]
Type=0
HeaderFile=AcceptSocket.h
ImplementationFile=AcceptSocket.cpp
BaseClass=CSocket
Filter=N
VirtualFilter=uq
LastObject=CAcceptSocket

