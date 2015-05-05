; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMSCommMultiPortDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MSCommMultiPort.h"

ClassCount=3
Class1=CMSCommMultiPortApp
Class2=CMSCommMultiPortDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MSCOMMMULTIPORT_DIALOG

[CLS:CMSCommMultiPortApp]
Type=0
HeaderFile=MSCommMultiPort.h
ImplementationFile=MSCommMultiPort.cpp
Filter=N

[CLS:CMSCommMultiPortDlg]
Type=0
HeaderFile=MSCommMultiPortDlg.h
ImplementationFile=MSCommMultiPortDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BUTTON_COM1SEND

[CLS:CAboutDlg]
Type=0
HeaderFile=MSCommMultiPortDlg.h
ImplementationFile=MSCommMultiPortDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MSCOMMMULTIPORT_DIALOG]
Type=1
Class=CMSCommMultiPortDlg
ControlCount=13
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_COM1RXDATA,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_COM1TXDATA,edit,1350631552
Control6=IDC_BUTTON_COM1SEND,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_COM2RXDATA,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_COM2TXDATA,edit,1350631552
Control11=IDC_BUTTON_COM2SEND,button,1342242816
Control12=IDC_MSCOMM_COM1,{648A5600-2C6E-101B-82B6-000000000014},1342242816
Control13=IDC_MSCOMM_COM2,{648A5600-2C6E-101B-82B6-000000000014},1342242816

