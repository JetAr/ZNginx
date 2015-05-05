; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMySocket
LastTemplate=CSocket
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "SClient.h"

ClassCount=4
Class1=CSClientApp
Class2=CSClientDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CMySocket
Resource3=IDD_SCLIENT_DIALOG

[CLS:CSClientApp]
Type=0
HeaderFile=SClient.h
ImplementationFile=SClient.cpp
Filter=N

[CLS:CSClientDlg]
Type=0
HeaderFile=SClientDlg.h
ImplementationFile=SClientDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CSClientDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=SClientDlg.h
ImplementationFile=SClientDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SCLIENT_DIALOG]
Type=1
Class=CSClientDlg
ControlCount=13
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_IPADDR,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_PORTNO,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_SENDDATA,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_RECVDATA,edit,1350631552
Control11=IDC_BUTTON_LINK,button,1342242816
Control12=IDC_BUTTON_UNLINK,button,1342242816
Control13=IDC_BUTTON_SEND,button,1342242816

[CLS:CMySocket]
Type=0
HeaderFile=MySocket.h
ImplementationFile=MySocket.cpp
BaseClass=CSocket
Filter=N
VirtualFilter=uq
LastObject=CMySocket

