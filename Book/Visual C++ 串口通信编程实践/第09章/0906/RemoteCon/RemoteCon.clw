; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CRemoteConDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "RemoteCon.h"

ClassCount=3
Class1=CRemoteConApp
Class2=CRemoteConDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_REMOTECON_DIALOG

[CLS:CRemoteConApp]
Type=0
HeaderFile=RemoteCon.h
ImplementationFile=RemoteCon.cpp
Filter=N

[CLS:CRemoteConDlg]
Type=0
HeaderFile=RemoteConDlg.h
ImplementationFile=RemoteConDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BUTTON_BLUESEND

[CLS:CAboutDlg]
Type=0
HeaderFile=RemoteConDlg.h
ImplementationFile=RemoteConDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_REMOTECON_DIALOG]
Type=1
Class=CRemoteConDlg
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_MSCOMM1,{648A5600-2C6E-101B-82B6-000000000014},1342242816
Control4=IDC_EDIT_REDSCORE,edit,1350631552
Control5=IDC_EDIT_BLUESCORE,edit,1350631552
Control6=IDC_BUTTON_REDSEND,button,1342242816
Control7=IDC_BUTTON_BLUESEND,button,1342242816
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_BUTTONNEXT,button,1342242816

