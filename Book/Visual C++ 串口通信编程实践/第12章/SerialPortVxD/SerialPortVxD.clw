; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSerialPortVxDDlg
LastTemplate=CListCtrl
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "serialportvxd.h"
LastPage=0

ClassCount=4
Class1=CSerialPortVxDDlg
Class2=CSerialPortVxDApp
Class3=CAboutDlg

ResourceCount=2
Resource1=IDD_ABOUTBOX
Class4=CMyListCtrl
Resource2=IDD_SERIALPORTVXD_DIALOG

[CLS:CSerialPortVxDApp]
Type=0
BaseClass=CWinApp
HeaderFile=SerialPortVxD.h
ImplementationFile=SerialPortVxD.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=SerialPortVxDDlg.cpp
ImplementationFile=SerialPortVxDDlg.cpp

[CLS:CSerialPortVxDDlg]
Type=0
BaseClass=CDialog
HeaderFile=SerialPortVxDDlg.h
ImplementationFile=SerialPortVxDDlg.cpp
LastObject=CSerialPortVxDDlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SERIALPORTVXD_DIALOG]
Type=1
Class=CSerialPortVxDDlg
ControlCount=16
Control1=IDOK,button,1342242817
Control2=IDC_BUTTON_STARTSTOP,button,1342242816
Control3=IDC_LIST_DISPDATA,SysListView32,1342292993
Control4=IDC_EDIT_READS,edit,1350631553
Control5=IDC_EDIT_WRITES,edit,1350631553
Control6=IDC_EDIT_READBYTES,edit,1350631552
Control7=IDC_EDIT_WRITEBYTES,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_BUTTON_STATRESET,button,1342242816
Control13=IDC_BUTTON_SAVEDATA,button,1342242816
Control14=IDC_BUTTON_CLEARDISP,button,1342242816
Control15=IDC_EDIT_PORT,edit,1350631552
Control16=IDC_STATIC,static,1342308352

[CLS:CMyListCtrl]
Type=0
HeaderFile=MyListCtrl.h
ImplementationFile=MyListCtrl.cpp
BaseClass=CListCtrl
Filter=W
VirtualFilter=FWC
LastObject=CMyListCtrl

