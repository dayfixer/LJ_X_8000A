//Copyright (c) 2019 KEYENCE CORPORATION. All rights reserved.

// SingleFuncDlg.cpp :Implementation file
//
#include "stdafx.h"
#include <afx.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <Windows.h>
#include <sstream>
#include <string>
#include <iomanip>
#include "LJX_DllSampleAll.h"
#include "SingleFuncDlg.h"
#include "DataExport.h"
#include "OpenEthernetDlg.h"
#include "GetErrorDlg.h"
#include "ClearErrorDlg.h"
#include "ControlLaserDlg.h"
#include "SettingDlg.h"
#include "DepthProgramSelectDlg.h"
#include "GetProfileDlg.h"
#include "GetBatchProfileDlg.h"
#include "ThreadSafeBuffer.h"
#include "HighSpeedEthernetInitializeDlg.h"
#include "PreStartHighSpeedDlg.h"
#include "Define.h"

// CSingleFuncDlg Dialog

IMPLEMENT_DYNAMIC(CSingleFuncDlg, CDialog)

CDeviceData CSingleFuncDlg::m_aDeviceData[LJX8IF_GUI_DEVICE_COUNT];
LJX8IF_PROFILE_INFO CSingleFuncDlg::m_aProfileInfo[LJX8IF_GUI_DEVICE_COUNT];
BOOL CSingleFuncDlg::m_bIsBufferFull[LJX8IF_GUI_DEVICE_COUNT];
BOOL CSingleFuncDlg::m_bIsStopCommunicationByError[LJX8IF_GUI_DEVICE_COUNT];

CSingleFuncDlg::CSingleFuncDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CSingleFuncDlg::IDD, pParent), m_nCurrentDeviceID(0), m_nXRange(0), m_nXThinning(0), m_strSaveFilePath(_T("")), m_strHighSpeedSaveFilePath(_T("")), m_bIsCountOnly(FALSE), m_bIsUseSimpleArray(TRUE), m_bIsStartTimer(FALSE), m_nSamplingPeriod(6), m_nLuminanceOutput(0), m_nLjvMeasureX(0), m_nLjvThinning(0), m_nLjvBinning(0), m_nHeadType(0), m_strSpinProfIndex(_T("0")), m_strSpinProfCount(_T("1"))
{
	// Save file path
	CString strFilePath = NULL;
	TCHAR tcPath[MAX_PATH];
	if (::GetModuleFileName(NULL, tcPath, MAX_PATH))
	{
		TCHAR tcDrive[_MAX_DRIVE];	  // Drive name
		TCHAR tcDir[_MAX_DIR];		  // Directory name
		TCHAR tcFileName[_MAX_FNAME]; // File name
		TCHAR tcExtension[_MAX_PATH]; // Extension name

		_tsplitpath_s(tcPath, tcDrive, _MAX_DRIVE, tcDir, _MAX_DIR, tcFileName, _MAX_FNAME, tcExtension, _MAX_EXT);
		strFilePath += tcDrive;
		strFilePath += tcDir;
		strFilePath += "outputfile.txt";
	}

	InitializeAllConnectionInfo();
}

CSingleFuncDlg::~CSingleFuncDlg()
{
}

void CSingleFuncDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_COMMANDLOG, m_strCommandLog);
	DDX_Control(pDX, IDC_COMMANDLOG, m_txtCommandLog);
	DDX_Control(pDX, IDC_STATIC_DEVICESTATUS0, m_sttConnectionStatus[0]);
	DDX_Control(pDX, IDC_STATIC_DEVICESTATUS1, m_sttConnectionStatus[1]);
	DDX_Control(pDX, IDC_STATIC_DEVICESTATUS2, m_sttConnectionStatus[2]);
	DDX_Control(pDX, IDC_STATIC_DEVICESTATUS3, m_sttConnectionStatus[3]);
	DDX_Control(pDX, IDC_STATIC_DEVICESTATUS4, m_sttConnectionStatus[4]);
	DDX_Control(pDX, IDC_STATIC_DEVICESTATUS5, m_sttConnectionStatus[5]);

	DDX_Radio(pDX, IDC_RD_STATE0, m_nCurrentDeviceID);
	DDX_Control(pDX, IDC_CMB_LJXMEASUREX, m_cmbXRange);
	DDX_CBIndex(pDX, IDC_CMB_LJXMEASUREX, m_nXRange);

	DDX_Text(pDX, IDC_EDIT_STARTTIMER, m_strSpinStartTimer);
	DDX_Control(pDX, IDC_SPIN_STARTTIMER, m_sbcSpinStartTimer);
	DDX_Control(pDX, IDC_CMB_LJXTHINNING, m_cmbXThinning);
	DDX_CBIndex(pDX, IDC_CMB_LJXTHINNING, m_nXThinning);
	DDX_Text(pDX, IDC_EDIT_PROFILEFILEPATH, m_strSaveFilePath);
	DDX_Text(pDX, IDC_EDIT_HIGHSPEEDSAVEFILEPATH, m_strHighSpeedSaveFilePath);
	DDX_Check(pDX, IDC_CHK_USESIMPLEARRAY, m_bIsUseSimpleArray);
	DDX_Check(pDX, IDC_CHK_COUNTONLY, m_bIsCountOnly);
	DDX_Check(pDX, IDC_CHK_TIMER, m_bIsStartTimer);
	DDX_Control(pDX, IDC_CMB_LJXSAMPLINGPERIOD, m_cmbSamplingPeriod);
	DDX_CBIndex(pDX, IDC_CMB_LJXSAMPLINGPERIOD, m_nSamplingPeriod);
	DDX_Control(pDX, IDC_CMB_LJXLUMINANCEOUTPUT, m_cmbLuminanceOutput);
	DDX_CBIndex(pDX, IDC_CMB_LJXLUMINANCEOUTPUT, m_nLuminanceOutput);
	DDX_Control(pDX, IDC_CMB_LJVTHINNING, m_cmbLjvThinning);
	DDX_CBIndex(pDX, IDC_CMB_LJVTHINNING, m_nLjvThinning);
	DDX_Control(pDX, IDC_CMB_LJVBINNIG, m_cmbLjvBinning);
	DDX_CBIndex(pDX, IDC_CMB_LJVBINNIG, m_nLjvBinning);
	DDX_Control(pDX, IDC_CMB_LJVMEASUREX, m_cmbLjvMeasureX);
	DDX_CBIndex(pDX, IDC_CMB_LJVMEASUREX, m_nLjvMeasureX);

	DDX_Control(pDX, IDC_STATIC_BUFFERSIZEVALUE, m_sttBufferSizeValue);
	DDX_Radio(pDX, IDC_RADIO_LJX, m_nHeadType);
	DDX_Control(pDX, IDC_SPIN_PROFILEINDEX, m_sbcSpinProfIndex);
	DDX_Text(pDX, IDC_EDIT_PROFILEINDEX, m_strSpinProfIndex);
	DDX_Control(pDX, IDC_SPIN_PROFILECOUNT, m_sbcSpinProfCount);
	DDX_Text(pDX, IDC_EDIT_PROFILECOUNT, m_strSpinProfCount);
}

BEGIN_MESSAGE_MAP(CSingleFuncDlg, CDialog)
ON_BN_CLICKED(IDC_BTN_INITIALIZE, &CSingleFuncDlg::OnBnClickedBtnInitialize)
ON_BN_CLICKED(IDC_CHK_TIMER, &CSingleFuncDlg::OnBnClickedChkTimer)
ON_BN_CLICKED(IDC_BTN_HIGHSPEEDSAVE_DESTINATION, &CSingleFuncDlg::OnBnClickedBtnHighSpeedSavedestination)
ON_BN_CLICKED(IDC_BTN_GETVERSION, &CSingleFuncDlg::OnBnClickedBtnGetversion)
ON_BN_CLICKED(IDC_BTN_CLEARLOG, &CSingleFuncDlg::OnBnClickedBtnClearlog)
ON_BN_CLICKED(IDC_SAVE_PROFILE, &CSingleFuncDlg::OnBnClickedSaveprofile)
ON_BN_CLICKED(IDC_BTN_FINALIZE, &CSingleFuncDlg::OnBnClickedBtnFinalize)
ON_BN_CLICKED(IDC_BTN_ETHERNETOPEN, &CSingleFuncDlg::OnBnClickedBtnEthernetopen)
ON_BN_CLICKED(IDC_BTN_COMMCLOSE, &CSingleFuncDlg::OnBnClickedBtnCommunicationclose)
ON_BN_CLICKED(IDC_BTN_REBOOTCONTROLLER, &CSingleFuncDlg::OnBnClickedBtnRebootcontroller)
ON_BN_CLICKED(IDC_BTN_RETURNTOFACTORYSETTIG, &CSingleFuncDlg::OnBnClickedBtnReturntofactorysettig)
ON_BN_CLICKED(IDC_BTN_GETERROR, &CSingleFuncDlg::OnBnClickedBtnGeterror)
ON_BN_CLICKED(IDC_BTN_CLEARERROR, &CSingleFuncDlg::OnBnClickedBtnClearerror)
ON_BN_CLICKED(IDC_BTN_TRIGGER, &CSingleFuncDlg::OnBnClickedBtnTrigger)
ON_BN_CLICKED(IDC_BTN_STARTMEASURE, &CSingleFuncDlg::OnBnClickedBtnStartmeasure)
ON_BN_CLICKED(IDC_BTN_STOPMEASURE, &CSingleFuncDlg::OnBnClickedBtnStopmeasure)
ON_BN_CLICKED(IDC_BTN_CLEARMEMORY, &CSingleFuncDlg::OnBnClickedBtnClearmemory)
ON_BN_CLICKED(IDC_BTN_SETSETTIG, &CSingleFuncDlg::OnBnClickedBtnSetsetting)
ON_BN_CLICKED(IDC_BTN_GETSETTING, &CSingleFuncDlg::OnBnClickedBtnGetsetting)
ON_BN_CLICKED(IDC_BTN_INITIALIZESETTING, &CSingleFuncDlg::OnBnClickedBtnInitializesetting)
ON_BN_CLICKED(IDC_BTN_REFLECTSETTING, &CSingleFuncDlg::OnBnClickedBtnReflectsetting)
ON_BN_CLICKED(IDC_BTN_REWRITETEMPORARYSETTING, &CSingleFuncDlg::OnBnClickedBtnRewritetemporarysetting)
ON_BN_CLICKED(IDC_BTN_CHECKMEMORYACCESS, &CSingleFuncDlg::OnBnClickedBtnCheckmemoryaccess)
ON_BN_CLICKED(IDC_BTN_CHANGEACTIVEPROGRAM, &CSingleFuncDlg::OnBnClickedBtnChangeactiveprogram)
ON_BN_CLICKED(IDC_BTN_GETACTIVEPROGRAM, &CSingleFuncDlg::OnBnClickedBtnGetactiveprogram)
ON_BN_CLICKED(IDC_BTN_GETPROFILE, &CSingleFuncDlg::OnBnClickedBtnGetprofile)
ON_BN_CLICKED(IDC_BTN_GETBATCHPROFILEE, &CSingleFuncDlg::OnBnClickedBtnGetbatchprofile)
ON_BN_CLICKED(IDC_BTN_HIGHSPEEDDATAETHERNETCOMMUNICATIONINITIALIZE, &CSingleFuncDlg::OnBnClickedBtnInitializeHighspeeddatacommunication)
ON_BN_CLICKED(IDC_BTN_PRESTARTHIGHSPEEDDATACOMMUNICATION, &CSingleFuncDlg::OnBnClickedBtnPrestarthighspeeddatacommunication)
ON_BN_CLICKED(IDC_BTN_STARTHIGHSPEEDDATACOMMUNICATION, &CSingleFuncDlg::OnBnClickedBtnStarthighspeeddatacommunication)
ON_BN_CLICKED(IDC_BTN_STOPHIGHSPEEDDATACOMMUNICATION, &CSingleFuncDlg::OnBnClickedBtnStophighspeeddatacommunication)
ON_BN_CLICKED(IDC_BTN_HIGHSPEEDDATACOMMUNICATIONFINALIZE, &CSingleFuncDlg::OnBnClickedBtnFinalizehighspeeddatacommunication)
ON_WM_TIMER()
ON_CBN_SELCHANGE(IDC_CMB_LJXMEASUREX, &CSingleFuncDlg::OnCbnSelchangeCmbLjxmeasurex)
ON_CBN_SELCHANGE(IDC_CMB_LJXTHINNING, &CSingleFuncDlg::OnCbnSelchangeCmbLjxthinning)
ON_CBN_SELCHANGE(IDC_CMB_LJXLUMINANCEOUTPUT, &CSingleFuncDlg::OnCbnSelchangeCmbLjxluminanceoutput)
ON_CBN_SELCHANGE(IDC_CMB_LJXSAMPLINGPERIOD, &CSingleFuncDlg::OnCbnSelchangeCmbLjxsamplingperiod)
ON_CBN_SELCHANGE(IDC_CMB_LJVMEASUREX, &CSingleFuncDlg::OnCbnSelchangeCmbLjvmeasurex)
ON_CBN_SELCHANGE(IDC_CMB_LJVTHINNING, &CSingleFuncDlg::OnCbnSelchangeCmbLjvthinning)
ON_CBN_SELCHANGE(IDC_CMB_LJVBINNIG, &CSingleFuncDlg::OnCbnSelchangeCmbLjvbinning)
ON_BN_CLICKED(IDC_RADIO_LJX, &CSingleFuncDlg::OnBnClickedRadioLjx)
ON_BN_CLICKED(IDC_RADIO_LJV, &CSingleFuncDlg::OnBnClickedRadioLjv)
ON_BN_CLICKED(IDC_RADIO_LJVB, &CSingleFuncDlg::OnBnClickedRadioLjvb)
ON_BN_CLICKED(IDC_BTN_CONTROLLASER, &CSingleFuncDlg::OnBnClickedBtnControllaser)
ON_BN_CLICKED(IDC_BTN_TRGERRORRESET, &CSingleFuncDlg::OnBnClickedBtnTrginterrorreset)
ON_BN_CLICKED(IDC_BTN_GETTRIGGERANDPULSECOUNT, &CSingleFuncDlg::OnBnClickedBtnGettriggerandpulsecount)
ON_BN_CLICKED(IDC_BTN_GETHEADTEMPERATURE, &CSingleFuncDlg::OnBnClickedBtnGetheadtemperature)
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BTN_GETSERIALNUMBER, &CSingleFuncDlg::OnBnClickedBtnGetserialnumber)
ON_BN_CLICKED(IDC_BTN_GETATTENTIONSTATUS, &CSingleFuncDlg::OnBnClickedBtnGetattentionstatus)
ON_BN_CLICKED(IDC_CHK_COUNTONLY, &CSingleFuncDlg::OnBnClickedChkCountOnly)
ON_BN_CLICKED(IDC_BTN_SAVE_DESTINATION, &CSingleFuncDlg::OnBnClickedBtnSavedestination)
ON_BN_CLICKED(IDC_BTN_GETBATCHSIMPLEARRAY, &CSingleFuncDlg::OnBnClickedBtnGetbatchsimplearray)
ON_BN_CLICKED(IDC_BTN_SAVEASBITMAPFILE, &CSingleFuncDlg::OnBnClickedBtnSaveasbitmapfile)
ON_BN_CLICKED(IDC_CHK_USESIMPLEARRAY, &CSingleFuncDlg::OnBnClickedChkUsesimplearray)
ON_BN_CLICKED(IDC_BTN_INITIALIZEHIGHSPEEDDATACOMMUNICATIONSIMPLEARRAY, &CSingleFuncDlg::OnBnClickedBtnInitializehighspeeddatacommunicationSimplearray)
ON_BN_CLICKED(IDC_BTN_HIGHSPEEDSAVEASBITMAPFILE, &CSingleFuncDlg::OnBnClickedBtnHispeedsaveasbitmapfile)
ON_EN_KILLFOCUS(IDC_EDIT_STARTTIMER, &CSingleFuncDlg::OnEnKillfocusEditStarttimer)
ON_EN_KILLFOCUS(IDC_EDIT_PROFILEINDEX, &CSingleFuncDlg::OnEnKillfocusEditProfileindex)
ON_EN_KILLFOCUS(IDC_EDIT_PROFILECOUNT, &CSingleFuncDlg::OnEnKillfocusEditProfilecount)
ON_BN_CLICKED(IDC_BUT_FLOWWORK, &CSingleFuncDlg::OnBnClickedButFlowwork)
ON_BN_CLICKED(IDC_BUT_OPENCAM, &CSingleFuncDlg::OnBnClickedButOpencam)
ON_BN_CLICKED(IDC_BUT_STARTCAM_MEASURE, &CSingleFuncDlg::OnBnClickedButStartcamMeasure)
ON_BN_CLICKED(IDC_BUT_CLOSECAM, &CSingleFuncDlg::OnBnClickedButClosecam)
END_MESSAGE_MAP()

// CSingleFuncDlg Message handler
BOOL CSingleFuncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set Spin control parameter
	m_sbcSpinProfIndex.SetRange32(0, 60000);
	m_sbcSpinProfCount.SetRange32(0, 60000);
	m_sbcSpinStartTimer.SetPos32(0);
	m_sbcSpinStartTimer.SetRange32(0, 1000);
	m_sbcSpinStartTimer.SetPos32(500);

	for (int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++)
	{
		m_anProfReceiveCount[i] = 0;
	}

	m_sendCommand = SENDCOMMAND_NONE;

	UpdateBufferSizeSettingItemEnabled();
	UpdateHighSpeedGetSimpleArrayItemEnabled();

	UpdateHighSpeedProfileSaveSettingItemEnabled();

	SetTimer(BUFFER_FULL_TIMER_ID, BUFFER_FULL_TIMER_INTERVAL, NULL);
	return TRUE;
}

BOOL CSingleFuncDlg::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CEdit *startTimerEdit = (CEdit *)GetDlgItem(IDC_EDIT_STARTTIMER);

		if (pMsg->hwnd == startTimerEdit->m_hWnd)
		{
			ConfirmTimerElapse();
			return TRUE;
		}

		CEdit *profileIndexEdit = (CEdit *)GetDlgItem(IDC_EDIT_PROFILEINDEX);
		if (pMsg->hwnd == profileIndexEdit->m_hWnd)
		{
			ConfirmProfileindex();
			return TRUE;
		}

		CEdit *profileCountEdit = (CEdit *)GetDlgItem(IDC_EDIT_PROFILECOUNT);
		if (pMsg->hwnd == profileCountEdit->m_hWnd)
		{
			ConfirmProfilecount();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSingleFuncDlg::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer(BUFFER_FULL_TIMER_ID);
}

/*
 "Clear" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnClearlog()
{
	UpdateData(TRUE);

	m_strCommandLog = "";
	int scrollPos = m_txtCommandLog.GetFirstVisibleLine();
	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(scrollPos);
}

/*
"..." button clicked
 普通
*/
void CSingleFuncDlg::OnBnClickedBtnSavedestination()
{
	UpdateData(TRUE);

	CFileDialog selDlg(FALSE, _T("csv"), m_strSaveFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Profile (*.csv)|*.csv|"));

	if (selDlg.DoModal() != IDOK)
		return;

	m_strSaveFilePath = selDlg.GetPathName();
	int scrollPos = m_txtCommandLog.GetFirstVisibleLine();
	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(scrollPos);
}

/*
 "..." button clicked
  高速事件——选择保存文件按钮的点击事件
*/
void CSingleFuncDlg::OnBnClickedBtnHighSpeedSavedestination()
{
	UpdateData(TRUE);

	CFileDialog selDlg(FALSE, _T("csv"), m_strHighSpeedSaveFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					   _T("Profile (*.csv)|*.csv|Bitmap (*.bmp)|*.bmp|TIFF (*.tif;*.tiff)|*.tif;*.tiff|All Files (*.*)|*.*||"));

	if (selDlg.DoModal() != IDOK)
		return;

	m_strHighSpeedSaveFilePath = selDlg.GetPathName();
	int scrollPos = m_txtCommandLog.GetFirstVisibleLine();
	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(scrollPos);
}

/*
 "Save the profile" button clicked
*/
void CSingleFuncDlg::OnBnClickedSaveprofile()
{
	UpdateData(TRUE);

	unsigned int startIndex = m_sbcSpinProfIndex.GetPos32(); // The limit is 2^16-1
	unsigned int dataCount = m_sbcSpinProfCount.GetPos32();	 // The limit is 2^16-1

	unsigned int dataListCount = (unsigned int)m_aDeviceData[m_nCurrentDeviceID].m_vecProfileDataHighSpeed.size();
	if (!(dataListCount > startIndex && dataCount <= (dataListCount - startIndex)))
	{
		ShowMessage(_T("There is no profile specified."));
		return;
	}

	if (m_strHighSpeedSaveFilePath.IsEmpty())
	{
		ShowMessage(_T("Failed in exporting file. (File path is empty.)"));
		return;
	}

	if (CDataExport::ExportProfileEx(&(m_aDeviceData[m_nCurrentDeviceID].m_vecProfileDataHighSpeed.at(startIndex)), m_strHighSpeedSaveFilePath, dataCount))
	{
		ShowMessage(_T("Saved"));
		return;
	}

	ShowMessage(_T("Failed in exporting file."));
}

/*
 "Save As Image File" button clicked
  作为图片保存按钮的点击事件
*/
void CSingleFuncDlg::OnBnClickedBtnHispeedsaveasbitmapfile()
{
	UpdateData(TRUE);

	if (m_strHighSpeedSaveFilePath.IsEmpty())
	{
		ShowMessage(_T("Failed in exporting file. (File path is empty.)"));
		return;
	}

	CWaitCursor waitCursor;

	//GetPos32 const以32位的精度检索上下控件的当前位置
	DWORD startIndex = m_sbcSpinProfIndex.GetPos32(); // 限制是 2^16-1
	DWORD dataCount = m_sbcSpinProfCount.GetPos32();  // 限制是 2^16-1

	if (m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStoreHighSpeed.SaveDataAsImages(m_strHighSpeedSaveFilePath, startIndex, dataCount))
	{
		ShowMessage(_T("Saved"));
		return;
	}

	ShowMessage(_T("Failed in exporting file."));
}

/*
 Index of the profile to save control focus out.
*/
void CSingleFuncDlg::OnEnKillfocusEditProfileindex()
{
	ConfirmProfileindex();
}

void CSingleFuncDlg::ConfirmProfileindex()
{
	DWORD startIndex = m_sbcSpinProfIndex.GetPos32();
	m_strSpinProfIndex.Format(_T("%d"), startIndex);
	int scrollPos = m_txtCommandLog.GetFirstVisibleLine();
	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(scrollPos);
}

/*
 Number of the profiles to save control focus out.
 保存控制焦点的配置文件数
*/
void CSingleFuncDlg::OnEnKillfocusEditProfilecount()
{
	ConfirmProfilecount();
}

void CSingleFuncDlg::ConfirmProfilecount()
{
	DWORD dataCount = m_sbcSpinProfCount.GetPos32();
	m_strSpinProfCount.Format(_T("%d"), dataCount);
	int scrollPos = m_txtCommandLog.GetFirstVisibleLine();
	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(scrollPos);
}

/*
 "Initialize" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnInitialize()
{
	UpdateData(TRUE);

	LONG lRc = LJX8IF_Initialize();
	DisplayCommandLog(lRc, IDS_INITIALIZE);

	for (int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++)
	{
		m_aDeviceData[i].m_deviceStatus = CDeviceData::DEVICESTATUS_NO_CONNECTION;
		InitializeConnectionInfo(i);
		SetDlgItemText(m_sttConnectionStatus[i].GetDlgCtrlID(), m_aDeviceData[i].GetStatusString());
	}
}

/*
"Finalize" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnFinalize()
{
	UpdateData(TRUE);

	LONG lRc = LJX8IF_Finalize();
	DisplayCommandLog(lRc, IDS_FINALIZE);
	for (int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++)
	{
		m_aDeviceData[i].m_deviceStatus = CDeviceData::DEVICESTATUS_NO_CONNECTION;
		InitializeConnectionInfo(i);
		SetDlgItemText(m_sttConnectionStatus[i].GetDlgCtrlID(), m_aDeviceData[i].GetStatusString());
	}
}

/*
 "GetVersion" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGetversion()
{
	UpdateData(TRUE);

	DisplayCommandLog(LJX8IF_RC_OK, IDS_GET_VERSION);
	LJX8IF_VERSION_INFO versionInfo = LJX8IF_GetVersion();
	CString versionText;
	versionText.Format(_T("%d.%d.%d.%d"), versionInfo.nMajorNumber, versionInfo.nMinorNumber, versionInfo.nRevisionNumber, versionInfo.nBuildNumber);
	AddResult(_T("Version"), versionText);
}

/*
 "EthernetOpen" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnEthernetopen()
{
	// @Point
	//＃将在此处为通讯设置设置的“ m_nCurrentDeviceID”输入到每个DLL函数的参数中。
	//＃如果要从多个控制器获取数据，请准备并设置多个“ m_nCurrentDeviceID”值，
	//将这些值输入DLL函数的参数，然后使用这些函数。

	UpdateData(TRUE);

	COpenEthernetDlg communicaionSettinDlg;
	if (communicaionSettinDlg.DoModal() != IDOK)
		return;

	LJX8IF_ETHERNET_CONFIG ethernetConfig = communicaionSettinDlg.GetEthernetConfig();
	LONG lRc = LJX8IF_EthernetOpen((LONG)m_nCurrentDeviceID, &ethernetConfig);
	DisplayCommandLog(lRc, IDS_EHTERNET_OPEN);

	CDeviceData::DEVICESTATUS status = (lRc == LJX8IF_RC_OK) ? CDeviceData::DEVICESTATUS_ETHERNET : CDeviceData::DEVICESTATUS_NO_CONNECTION;
	m_aDeviceData[m_nCurrentDeviceID].m_deviceStatus = status;

	if (lRc == LJX8IF_RC_OK)
	{
		m_aDeviceData[m_nCurrentDeviceID].m_ethernetConfig = ethernetConfig;
	}

	SetDlgItemText(m_sttConnectionStatus[m_nCurrentDeviceID].GetDlgCtrlID(), m_aDeviceData[m_nCurrentDeviceID].GetStatusString());
}

/*
 "CommunicationClose" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnCommunicationclose()
{
	UpdateData(TRUE);

	LONG lRc = LJX8IF_CommunicationClose((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_COMM_CLOSE);

	m_aDeviceData[m_nCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_NO_CONNECTION;
	InitializeConnectionInfo(m_nCurrentDeviceID);

	SetDlgItemText(m_sttConnectionStatus[m_nCurrentDeviceID].GetDlgCtrlID(), m_aDeviceData[m_nCurrentDeviceID].GetStatusString());

	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount()); //LineScroll指示多行文本滚动
}

/*
 "RebootController" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnRebootcontroller()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_REBOOT_CONTROLLER;
	LONG lRc = LJX8IF_RebootController((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_REBOOT_CONTROLLER);
}

/*
 "Return To Factory Setting" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnReturntofactorysettig()
{
	UpdateData(TRUE);

	LONG lRc = LJX8IF_ReturnToFactorySetting((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_RETURN_TO_FACTORY_SETTING);
}

/*
 "ControlLaser" button clicked
 状态指定（0：禁止照明，非零：允许照明）
*/
void CSingleFuncDlg::OnBnClickedBtnControllaser()
{
	UpdateData(TRUE);

	CControlLaserDlg controlLaserDlg;
	if (controlLaserDlg.DoModal() != IDOK)
		return;

	LONG lRc = LJX8IF_ControlLaser((LONG)m_nCurrentDeviceID, controlLaserDlg.GetLaserStatus());
	DisplayCommandLog(lRc, IDS_CONTROL_LASER);
}

/*
 "GetError" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGeterror()
{
	UpdateData(TRUE);

	CGetErrorDlg getErrorDlg;
	if (getErrorDlg.DoModal() != IDOK)
		return;

	BYTE byErrCount = 0;
	BYTE byReceiveMax = getErrorDlg.m_byReceiveMax;
	int errCodeLength = max<int>(1, byReceiveMax);
	vector<WORD> errCode(errCodeLength);

	LONG lRc = LJX8IF_GetError((LONG)m_nCurrentDeviceID, byReceiveMax, &byErrCount, (WORD *)&errCode.at(0));
	DisplayCommandLog(lRc, IDS_GET_ERROR);

	// Display Error Code
	if (lRc != LJX8IF_RC_OK)
		return;

	CString strErrCountValue;
	strErrCountValue.Format(_T("%d"), byErrCount);
	AddResult(_T("ErrCount"), strErrCountValue);

	for (int i = 0; i < byReceiveMax; i++)
	{
		CString strItem;
		strItem.Format(_T("ErrCode[%d]"), i);
		CString strText;
		strText.Format(_T("0x%04X"), errCode[i]);
		AddResult(strItem, strText);
	}
}

/*
 "ClearError" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnClearerror()
{
	UpdateData(TRUE);

	CClearErrorDlg clearErrorDlg;
	if (clearErrorDlg.DoModal() != IDOK)
		return;

	LONG lRc = LJX8IF_ClearError((LONG)m_nCurrentDeviceID, clearErrorDlg.GetErrCode());
	DisplayCommandLog(lRc, IDS_CLEAR_ERROR);
}

/*
"TRG_ERROR_RESET" button clicked
 清除 trg_error 的打开状态
*/
void CSingleFuncDlg::OnBnClickedBtnTrginterrorreset()
{
	UpdateData(TRUE);

	LONG lRc = LJX8IF_TrgErrorReset((LONG)m_nCurrentDeviceID);

	DisplayCommandLog(lRc, IDS_TRG_ERROR_RESET);
}

/*
"GetTriggerAndPulseCount" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGettriggerandpulsecount()
{
	UpdateData(TRUE);

	DWORD dwTriggerCount = 0;
	LONG lEncoderCount = 0;
	LONG lRc = LJX8IF_GetTriggerAndPulseCount((LONG)m_nCurrentDeviceID, &dwTriggerCount, &lEncoderCount);
	DisplayCommandLog(lRc, IDS_GET_TRIGGER_AND_PULSE_COUNT);

	if (lRc != LJX8IF_RC_OK)
		return;

	CString strTriggerCount;
	strTriggerCount.Format(_T("%d"), dwTriggerCount);
	CString strEncoderCount;
	strEncoderCount.Format(_T("%d"), lEncoderCount);

	AddResult(_T("TriggerCount"), strTriggerCount);
	AddResult(_T("EncoderCount"), strEncoderCount);
}

/*
"GetHeadTemperature" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGetheadtemperature()
{
	UpdateData(TRUE);

	SHORT nSensorTemperature = 0;
	SHORT nProcessorTemperature = 0;
	SHORT nCaseTemperature = 0;
	LONG lRc = LJX8IF_GetHeadTemperature((LONG)m_nCurrentDeviceID, &nSensorTemperature, &nProcessorTemperature, &nCaseTemperature);
	DisplayCommandLog(lRc, IDS_GET_HEAD_TEMPERATURE);

	if (lRc != LJX8IF_RC_OK)
		return;
	ShowHeadTemperature(nSensorTemperature, nProcessorTemperature, nCaseTemperature);
}

/*
"GetSerialNumber" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGetserialnumber()
{
	UpdateData(TRUE);

	// serial number data length
	const int SERIAL_NUMBER_DATA_LENGTH = 16;

	CHAR szControllerSerialNumber[SERIAL_NUMBER_DATA_LENGTH];
	CHAR szHeadSerialNumber[SERIAL_NUMBER_DATA_LENGTH];

	LONG lRc = LJX8IF_GetSerialNumber((LONG)m_nCurrentDeviceID, &szControllerSerialNumber[0], &szHeadSerialNumber[0]);
	DisplayCommandLog(lRc, IDS_GET_SERIAL_NUMBER);

	if (lRc != LJX8IF_RC_OK)
		return;
	CString strControllerSerialNumber;
	strControllerSerialNumber = szControllerSerialNumber;
	CString strHeadSerialNumber;
	strHeadSerialNumber = szHeadSerialNumber;
	AddResult(_T("Controller serial number"), strControllerSerialNumber);
	AddResult(_T("Head serial number"), strHeadSerialNumber);
}

/*
"GetAttentionStatus" button clicked
 获取 trg_error / mem_full / trg_pass 的状态
*/
void CSingleFuncDlg::OnBnClickedBtnGetattentionstatus()
{
	UpdateData(TRUE);

	WORD pwAttentionStatus = 0;
	LONG lRc = LJX8IF_GetAttentionStatus((LONG)m_nCurrentDeviceID, &pwAttentionStatus);
	DisplayCommandLog(lRc, IDS_GET_ATTENTION_STATUS);

	if (lRc != LJX8IF_RC_OK)
		return;

	WORD attentionStatus = pwAttentionStatus;
	CString binaryString;
	binaryString = "";
	const int attentionStatusBitLength = 16;
	for (int i = 0; i < attentionStatusBitLength; i++)
	{
		BYTE bit = attentionStatus % 2;
		CString bitString;
		if (i % 2 == 0)
		{
			bitString.Format(_T("%d"), bit);
		}
		else
		{
			bitString.Format(_T(" %d"), bit);
		}
		binaryString = bitString + binaryString;
		attentionStatus /= 2;
	}

	AddResult(_T("AttentionStatus"), binaryString);
}

/*
 "Trigger" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnTrigger()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_TRIGGER;
	LONG lRc = LJX8IF_Trigger((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_TRIGGER);
}

/*
 "StartMeasure" button clicked
 开始收集亮度按钮
*/
void CSingleFuncDlg::OnBnClickedBtnStartmeasure()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_START_MEASURE;
	LONG lRc = LJX8IF_StartMeasure((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_START_MEASURE);
}

/*
 "StopMeasure" button clicked
 停止收集亮度按钮
*/
void CSingleFuncDlg::OnBnClickedBtnStopmeasure()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_STOP_MEASURE;
	LONG lRc = LJX8IF_StopMeasure((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_STOP_MEASURE);
}

/*
 "ClearMemory" button clicked
  清除内存按钮点击事件
*/
void CSingleFuncDlg::OnBnClickedBtnClearmemory()
{
	UpdateData(TRUE);

	LONG lRc = LJX8IF_ClearMemory((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_CLEAR_MEMORY);
}

/*
 "SetSetting" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnSetsetting()
{
	UpdateData(TRUE);

	CSettingDlg setSettingDlg(TRUE);
	if (setSettingDlg.DoModal() != IDOK)
		return;

	BYTE byDepth = setSettingDlg.GetDepth();
	LJX8IF_TARGET_SETTING targetSetting = setSettingDlg.GetTargetSetting();
	vector<BYTE> vecData = setSettingDlg.GetSendData();
	DWORD dwDataSize = setSettingDlg.m_nDataAmount;
	DWORD dwError = 0;

	LONG lRc = LJX8IF_SetSetting((LONG)m_nCurrentDeviceID, byDepth, targetSetting, &vecData.at(0), dwDataSize, &dwError);

	//＃共有三个设置区域：a）写设置区域，b）运行区域，以及c）保存区域。
	// *如果要更改多个设置，请为设置级别指定a）。 但是，要在LJ-X操作中反映设置，您必须调用LJX8IF_ReflectSetting。
	// *当您要更改一个设置时，将b）指定为设置级别，但是您不介意在关闭电源时是否将此设置恢复为更改之前的值。
	// *如果要更改一个设置并且即使关闭电源也要保留此新值，请在设置级别中指定c）。

	// @Point
	//作为一个使用示例，我们将展示如何使用SettingForm配置设置，以便通过SettingForm使用其初始值发送设置，
	//会将运行区域中的采样周期更改为“ 100 Hz”。
	//另请参见GetSetting函数。

	DisplayCommandLog(lRc, IDS_SET_SETTING);
	if ((lRc == LJX8IF_RC_OK) && (dwError != NO_ERROR_VALUE))
	{
		AddSettingErrorLog(dwError);
	}
}

/*
 "GetSetting" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGetsetting()
{
	UpdateData(TRUE);

	CSettingDlg getSettingDlg(FALSE);
	vector<BYTE> vecData;
	if (getSettingDlg.DoModal() != IDOK)
		return;

	BYTE byDepth = getSettingDlg.GetDepth();
	LJX8IF_TARGET_SETTING targetSetting = getSettingDlg.GetTargetSetting();
	DWORD dwDataSize = getSettingDlg.m_nDataAmount;
	DWORD dwError = 0;

	for (DWORD i = 0; i < dwDataSize; i++)
	{
		vecData.push_back(0);
	}
	LONG lRc = LJX8IF_GetSetting((LONG)m_nCurrentDeviceID, byDepth, targetSetting, &vecData.at(0), dwDataSize);
	// @Point
	//  We have prepared an object for reading the sampling period into the setting's initial value.
	//  Also see the SetSetting function.

	DisplayCommandLog(lRc, IDS_GET_SETTING);
	if (lRc != LJX8IF_RC_OK)
		return;

	CString strLog;
	m_strCommandLog += _T("\t   0   1   2   3   4   5   6   7  \r\n");
	for (UINT i = 0; i < vecData.size(); i++)
	{
		if ((i % DATA_COUNT_IN_ONE_LINE) == 0)
		{
			strLog.Format(_T("[0x%04X] "), i);
			m_strCommandLog += (LPCTSTR)strLog;
		}
		strLog.Format(_T(" %02X"), vecData.at(i));
		m_strCommandLog += (LPCTSTR)strLog;

		if ((i % DATA_COUNT_IN_ONE_LINE) == (DATA_COUNT_IN_ONE_LINE - 1) || i == dwDataSize - 1)
		{
			strLog.Format(_T("\r\n"));
			m_strCommandLog += (LPCTSTR)strLog;
		}
	}

	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
 "InitializeSetting" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnInitializesetting()
{
	UpdateData(TRUE);

	CDepthProgramSelectDlg depthProgramSelectDlg(TRUE, TRUE);
	if (depthProgramSelectDlg.DoModal() != IDOK)
		return;

	LONG lRc = LJX8IF_InitializeSetting((LONG)m_nCurrentDeviceID, depthProgramSelectDlg.GetDepth(), depthProgramSelectDlg.GetTarget());
	DisplayCommandLog(lRc, IDS_INITIALIZE_SETTING);
}

/*
 "ReflectSetting" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnReflectsetting()
{
	UpdateData(TRUE);

	CDepthProgramSelectDlg depthSelectDlg(FALSE, TRUE);
	if (depthSelectDlg.DoModal() != IDOK)
		return;

	DWORD dwError = 0;
	LONG lRc = LJX8IF_ReflectSetting((LONG)m_nCurrentDeviceID, depthSelectDlg.GetDepth(), &dwError);
	DisplayCommandLog(lRc, IDS_REFLECT_SETTING);
	if ((lRc == LJX8IF_RC_OK) && (dwError != NO_ERROR_VALUE))
	{
		AddSettingErrorLog(dwError);
	}
}

/*
 "RewriteTemporarySettig" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnRewritetemporarysetting()
{
	UpdateData(TRUE);

	CDepthProgramSelectDlg depthSelectDlg(FALSE, TRUE);
	if (depthSelectDlg.DoModal() != IDOK)
		return;

	LONG lRc = LJX8IF_RewriteTemporarySetting((LONG)m_nCurrentDeviceID, depthSelectDlg.GetDepth());
	DisplayCommandLog(lRc, IDS_REWRITE_TEMPORARY_SETTING);
}

/*
 "CheckMemoryAccess" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnCheckmemoryaccess()
{
	UpdateData(TRUE);

	BYTE byBusy = 0;
	LONG lRc = LJX8IF_CheckMemoryAccess((LONG)m_nCurrentDeviceID, &byBusy);
	DisplayCommandLog(lRc, IDS_CHECK_MEMORY_ACCESS);

	if (lRc != LJX8IF_RC_OK)
		return;

	CString strLog;
	strLog = byBusy != NOT_ACCESS_VALUE ? _T("  Accessing the save area\r\n") : _T("  No access\r\n");
	m_strCommandLog += strLog;

	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
 "ChangeActiveProgram" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnChangeactiveprogram()
{
	UpdateData(TRUE);

	CDepthProgramSelectDlg programSelectDlg(TRUE, FALSE);
	if (programSelectDlg.DoModal() != IDOK)
		return;

	LONG lRc = LJX8IF_ChangeActiveProgram((LONG)m_nCurrentDeviceID, programSelectDlg.GetTarget());
	DisplayCommandLog(lRc, IDS_CHANGE_ACTIVE_PROGRAM);
}

/*
 "GetActiveProgram" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGetactiveprogram()
{
	UpdateData(TRUE);

	BYTE programNo = 0;
	LONG lRc = LJX8IF_GetActiveProgram((LONG)m_nCurrentDeviceID, &programNo);
	DisplayCommandLog(lRc, IDS_GET_ACTIVE_PROGRAM);
	if (lRc != LJX8IF_RC_OK)
		return;

	CString strProgramNoText;
	strProgramNoText.Format(_T("%d"), programNo);
	AddResult(_T("ProgramNo"), strProgramNoText);
}

/*
 "GetProfile" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGetprofile()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_GET_PROFILE;
	CGetProfileDlg getProfileDlg;
	if (getProfileDlg.DoModal() != IDOK)
		return;

	m_aDeviceData[m_nCurrentDeviceID].m_vecProfileData.clear();

	LJX8IF_GET_PROFILE_REQUEST request = getProfileDlg.GetProfileReq();

	DWORD dwOneProfileDataBufferSize = GetOneProfileDataSize();
	DWORD dwDataSize = dwOneProfileDataBufferSize * request.byGetProfileCount;
	vector<int> vecProfileData(dwDataSize / sizeof(DWORD));

	LJX8IF_GET_PROFILE_RESPONSE response;
	LJX8IF_PROFILE_INFO profileInfo;
	LONG lRc = LJX8IF_GetProfile((LONG)m_nCurrentDeviceID, &request, &response, &profileInfo, (DWORD *)&vecProfileData.at(0), dwDataSize);
	DisplayCommandLog(lRc, IDS_GET_PROFILE);

	if (lRc != LJX8IF_RC_OK)
		return;

	// Show response
	ShowResponse(response);
	ShowProfileInfo(profileInfo);

	AnalyzeProfileData(response.byGetProfileCount, profileInfo, vecProfileData);

	// Profile output(one profile only)
	ExportProfile();
}

/*
 "GetBatchProfile" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGetbatchprofile()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_GET_BATCH_PROFILE;
	CGetBatchProfileDlg getBatchProfile;
	if (getBatchProfile.DoModal() != IDOK)
		return;

	m_aDeviceData[m_nCurrentDeviceID].m_vecProfileData.clear();

	LJX8IF_GET_BATCH_PROFILE_REQUEST request = getBatchProfile.GetBatchProfileReq();

	DWORD dwOneDataSize = GetOneProfileDataSize();
	DWORD dwDataSize = dwOneDataSize * request.byGetProfileCount;
	vector<int> vecBatchData(dwDataSize / sizeof(int));

	LJX8IF_GET_BATCH_PROFILE_RESPONSE rsp;
	LJX8IF_PROFILE_INFO profileInfo;
	// Send Command
	LONG lRc = LJX8IF_GetBatchProfile((LONG)m_nCurrentDeviceID, &request, &rsp, &profileInfo, (DWORD *)&vecBatchData.at(0), dwDataSize);

	// @Point
	//＃从单个批次读取所有配置文件时，可能无法读取指定数量的配置文件。
	//＃要在读取第一组配置文件后读取其余配置文件，
	//将指定方法（byPosMode）设置为0x02，指定批号（dwGetBatchNo），
	//，然后设置编号以开始从（dwGetProfileNo）读取配置文件，并将要读取的配置文件数量（byGetProfileCount）设置为值
	//，它们指定尚未读取的概要文件范围，无法按顺序读取概要文件。
	//＃有关基本代码，请参见“ CCombinationFuncDlg :: OnBnClickedBtnGetBatchProfileData”

	// Result output
	DisplayCommandLog(lRc, IDS_GET_BATCH_PROFILE);

	if (lRc != LJX8IF_RC_OK)
		return;

	// Show Response
	ShowResponse(rsp);
	ShowProfileInfo(profileInfo);

	AnalyzeProfileData(rsp.byGetProfileCount, profileInfo, vecBatchData);

	// Profile output(one profile only)
	ExportProfile();
}

/*
 "GetBatchSimpleArray" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnGetbatchsimplearray()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_GET_BATCH_PROFILE;
	CGetBatchProfileDlg getBatchProfile;
	if (getBatchProfile.DoModal() != IDOK)
		return;

	m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStore.Clear();

	LJX8IF_GET_BATCH_PROFILE_REQUEST request = getBatchProfile.GetBatchProfileReq();

	DWORD dwDataSize = MAX_PROFILE_COUNT * request.byGetProfileCount;

	// @Point
	//＃＃“ LJX8IF_GetBatchSimpleArray” API没有像“ LJX8IF_GetBatchProfile”这样的“ dwDataSize”参数。
	//当缓冲区大小小于收到的批处理数据时，将发生内存访问冲突。
	//＃此示例每次都会分配最大轮廓宽度，以简化代码。
	//为了减少内存使用，请记住最新的配置文件信息结构，并使用“ nProfileDataCount”属性来分配缓冲区。

	vector<LJX8IF_PROFILE_HEADER> vecHeader(request.byGetProfileCount);
	vector<WORD> vecHeightData(dwDataSize);
	vector<WORD> vecLuminanceData(dwDataSize);

	LJX8IF_GET_BATCH_PROFILE_RESPONSE rsp;
	LJX8IF_PROFILE_INFO profileInfo;
	// Send Command
	LONG lRc = LJX8IF_GetBatchSimpleArray((LONG)m_nCurrentDeviceID, &request, &rsp, &profileInfo, &vecHeader[0], &vecHeightData[0], &vecLuminanceData[0]);
	// @Point
	//＃从单个批次读取所有配置文件时，可能无法读取指定数量的配置文件。
	//＃要在读取第一组配置文件后读取其余配置文件，
	//将指定方法（byPosMode）设置为0x02，指定批号（dwGetBatchNo），
	//，然后设置编号以开始从（dwGetProfileNo）读取配置文件，并将要读取的配置文件数量（byGetProfileCount）设置为值
	//，它们指定尚未读取的概要文件范围，无法按顺序读取概要文件。
	//＃有关基本代码，请参见“ CCombinationFuncDlg :: OnBnClickedBtnGetBatchProfileData”

	// Result output
	DisplayCommandLog(lRc, IDS_GET_BATCH_PROFILE_SIMPLE_ARRAY);

	if (lRc != LJX8IF_RC_OK)
		return;

	// Show Response
	ShowResponse(rsp);
	ShowProfileInfo(profileInfo);

	m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStore.m_nDataWidth = profileInfo.wProfileDataCount;
	m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStore.m_bIsLuminanceEnable = profileInfo.byLuminanceOutput == 1;
	m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStore.AddReceivedData(&vecHeightData[0], &vecLuminanceData[0], rsp.byGetProfileCount);
}

/*
"Save As Image File" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnSaveasbitmapfile()
{
	CString strFilePath;
	//CFileDialog selDlg第一个参数true为打开文件框，false为保存文件对话框
	CFileDialog selDlg(FALSE, _T("bmp"), strFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					   _T("Bitmap (*.bmp)|*.bmp|TIFF (*.tif;*.tiff)|*.tif;*.tiff|All Files (*.*)|*.*||"));

	/*
	CString CFileDialog::GetPathName( ) 得到完整的文件名，包括目录名和扩展名如：c: est est1.txt 
    CString CFileDialog::GetFileName( ) 得到完整的文件名，包括扩展名如：test1.txt 
    CString CFileDialog::GetExtName( ) 得到完整的文件扩展名，如：txt 
    CString CFileDialog::GetFileTitle ( ) 得到完整的文件名，不包括目录名和扩展名如：test1 
    POSITION CFileDialog::GetStartPosition( ) 对于选择了多个文件的情况得到第一个文件位置。 
	*/

	if (selDlg.DoModal() != IDOK)
		return;

	DWORD dwCount = m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStore.GetCount();
	if (m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStore.SaveDataAsImages(selDlg.GetPathName(), 0, dwCount))
	{
		ShowMessage(_T("Saved"));
		return;
	}

	ShowMessage(_T("Failed in exporting file."));
}

/*
 "InitializeHighSpeedDataCommunication" button clicked
  初始化高速以太网连接按钮点击事件
*/
void CSingleFuncDlg::OnBnClickedBtnInitializeHighspeeddatacommunication()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_INITIALIZE_HIGH_SPEED_DATA_ETHERNET_COMMUNICATION;
	CHighSpeedEthernetInitializeDlg highSpeedEthernetInitializeDlg;
	highSpeedEthernetInitializeDlg.ethernetConfig = m_aDeviceData[m_nCurrentDeviceID].m_ethernetConfig;

	if (highSpeedEthernetInitializeDlg.DoModal() != IDOK)
		return;

	// Clear the data
	ClearAllHighspeedBuffer(m_nCurrentDeviceID);

	LJX8IF_ETHERNET_CONFIG config = highSpeedEthernetInitializeDlg.GetEthernetConfig();

	LONG lRc = LJX8IF_InitializeHighSpeedDataCommunication((LONG)m_nCurrentDeviceID, &config,
														   (WORD)highSpeedEthernetInitializeDlg.m_dwHighSpeedPortNo,
														   (m_bIsCountOnly ? CountProfileReceive : ReceiveHighSpeedData),
														   highSpeedEthernetInitializeDlg.m_dwProfileCount, m_nCurrentDeviceID);

	// @Point
	//＃当调用频率较低时，可能无法按指定数量的配置文件调用一次回调函数。
	//＃当同时满足以下两个条件时，将调用回调函数。
	// *有一个接收数据包。
	// *达到呼叫频率时已收到指定数量的配置文件。

	DisplayCommandLog(lRc, IDS_INITIALIZE_HIGH_SPEED_DATA_COMMUNICATION);

	if (lRc == LJX8IF_RC_OK)
	{
		m_aDeviceData[m_nCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_ETHERNET_FAST;
		m_aDeviceData[m_nCurrentDeviceID].m_ethernetConfig = config;
	}

	SetDlgItemText(m_sttConnectionStatus[m_nCurrentDeviceID].GetDlgCtrlID(), m_aDeviceData[m_nCurrentDeviceID].GetStatusString());
	m_anProfReceiveCount[m_nCurrentDeviceID] = 0;

	UpDateReceiveCount();
}

/*
"InitializeHighSpeed (SimpleArray)" button clicked
 初始化高速以太网连接（简单数组）按钮的点击事件
*/
void CSingleFuncDlg::OnBnClickedBtnInitializehighspeeddatacommunicationSimplearray()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_INITIALIZE_HIGH_SPEED_DATA_ETHERNET_COMMUNICATION;
	CHighSpeedEthernetInitializeDlg highSpeedEthernetInitializeDlg;
	highSpeedEthernetInitializeDlg.ethernetConfig = m_aDeviceData[m_nCurrentDeviceID].m_ethernetConfig;
	if (highSpeedEthernetInitializeDlg.DoModal() != IDOK)
		return;

	// Clear the data
	ClearAllHighspeedBuffer(m_nCurrentDeviceID);

	LJX8IF_ETHERNET_CONFIG config = highSpeedEthernetInitializeDlg.GetEthernetConfig();

	LONG lRc = LJX8IF_InitializeHighSpeedDataCommunicationSimpleArray(
		(LONG)m_nCurrentDeviceID,
		&config,
		(WORD)highSpeedEthernetInitializeDlg.m_dwHighSpeedPortNo,
		(m_bIsCountOnly ? CountSimpleArrayReceive : ReceiveHighSpeedSimpleArray),
		highSpeedEthernetInitializeDlg.m_dwProfileCount, m_nCurrentDeviceID);

	// @Point
	//＃当调用频率较低时，可能无法按指定数量的配置文件调用一次回调函数。
	//＃当同时满足以下两个条件时，将调用回调函数。
	// *有一个接收数据包。
	// *达到呼叫频率时已收到指定数量的配置文件。

	DisplayCommandLog(lRc, IDS_INITIALIZE_HIGH_SPEED_DATA_COMMUNICATION_SIMPLE_ARRAY);

	if (lRc == LJX8IF_RC_OK)
	{
		m_aDeviceData[m_nCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_ETHERNET_FAST;
		m_aDeviceData[m_nCurrentDeviceID].m_ethernetConfig = config;
	}

	SetDlgItemText(m_sttConnectionStatus[m_nCurrentDeviceID].GetDlgCtrlID(), m_aDeviceData[m_nCurrentDeviceID].GetStatusString());
	m_anProfReceiveCount[m_nCurrentDeviceID] = 0;

	UpDateReceiveCount();
}

/*
 "PreStartHighSpeedDataCommunication" button clicked
  开始准备高速以太网连接按钮点击事件
  仅为了获取 开始轮廓的启示位置
*/
void CSingleFuncDlg::OnBnClickedBtnPrestarthighspeeddatacommunication()
{
	UpdateData(TRUE);
	m_sendCommand = SENDCOMMAND_PRE_START_HIGH_SPEED_DATA_COMMUNICATION;

	CPreStartHighSpeedDlg preStartHighSpeedDlg;
	if (preStartHighSpeedDlg.DoModal() != IDOK)
		return;

	LJX8IF_HIGH_SPEED_PRE_START_REQ request = preStartHighSpeedDlg.getHighSpeedPreStartReq();

	// @Point
	//＃SendPos用于指定在高速通信期间从哪个配置文件开始发送数据。
	//＃当内存已满且
	//将“ 0：从上一个发送完成位置开始”指定为发送开始位置，
	//如果LJ-X继续累积配置文件，则LJ-X内存将变满，
	//，并且先前发送完成位置的配置文件将被新的配置文件覆盖。
	//在这种情况下，由于未保存先前发送完成位置的配置文件，因此将发生错误。

	LJX8IF_PROFILE_INFO profileInfo;

	LONG lRc = LJX8IF_PreStartHighSpeedDataCommunication((LONG)m_nCurrentDeviceID, &request, &profileInfo);
	DisplayCommandLog(lRc, IDS_PRE_START_HIGH_SPEED_DATA_COMMUNICATION);

	if (lRc != LJX8IF_RC_OK)
		return;

	ShowProfileInfo(profileInfo);
	m_aProfileInfo[m_nCurrentDeviceID] = profileInfo;

	if (m_bIsUseSimpleArray == TRUE)
	{
		m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStoreHighSpeed.m_nDataWidth = profileInfo.wProfileDataCount;
		m_aDeviceData[m_nCurrentDeviceID].m_simpleArrayStoreHighSpeed.m_bIsLuminanceEnable = profileInfo.byLuminanceOutput == 1;
	}
}

/*
 "StartHighSpeedDataCommunication" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnStarthighspeeddatacommunication()
{
	UpdateData(TRUE);

	m_sendCommand = SENDCOMMAND_START_HIGH_SPEED_DATA_COMMUNICATION;

	LONG lRc = LJX8IF_StartHighSpeedDataCommunication((LONG)m_nCurrentDeviceID);

	DisplayCommandLog(lRc, IDS_START_HIGH_SPEED_DATA_COMMUNICATION);
}

/*
 "StopHighSpeedDataCommunication" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnStophighspeeddatacommunication()
{
	UpdateData(TRUE);

	LONG lRc = LJX8IF_StopHighSpeedDataCommunication((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_STOP_HIGH_SPEED_DATA_COMMUNICATION);
}

/*
 "FinalizeHighSpeedDataCommunication" button clicked
*/
void CSingleFuncDlg::OnBnClickedBtnFinalizehighspeeddatacommunication()
{
	UpdateData(TRUE);

	LONG lRc = LJX8IF_FinalizeHighSpeedDataCommunication((LONG)m_nCurrentDeviceID);
	DisplayCommandLog(lRc, IDS_FINALIZE_HIGH_SPEED_DATA_COMMUNICATION);
	LJX8IF_ETHERNET_CONFIG config;

	switch (m_aDeviceData[m_nCurrentDeviceID].m_deviceStatus)
	{

	case CDeviceData::DEVICESTATUS_ETHERNET_FAST:
		config = m_aDeviceData[m_nCurrentDeviceID].m_ethernetConfig;
		m_aDeviceData[m_nCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_ETHERNET;
		m_aDeviceData[m_nCurrentDeviceID].m_ethernetConfig = config;
		break;

	default:
		break;
	}

	SetDlgItemText(m_sttConnectionStatus[m_nCurrentDeviceID].GetDlgCtrlID(), m_aDeviceData[m_nCurrentDeviceID].GetStatusString());
}

/*
Show Head Temperature
@param nSensorTemperature
@param nProcessorTemperature
@param nCaseTemperature
*/
void CSingleFuncDlg::ShowHeadTemperature(SHORT nSensorTemperature, SHORT nProcessorTemperature, SHORT nCaseTemperature)
{
	CAtlString strLog;
	strLog.Format(_T("  SensorTemperature	: %s\r\n"), GetTemperatureText(nSensorTemperature));
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  ProcessorTemperature	: %s\r\n"), GetTemperatureText(nProcessorTemperature));
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  CaseTemperature		: %s\r\n"), GetTemperatureText(nCaseTemperature));
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
Get Temperature Text
@param nTemperature
*/
CString CSingleFuncDlg::GetTemperatureText(SHORT nTemperature)
{
	if ((nTemperature & HEAD_TEMPERATURE_INVALID_VALUE) == HEAD_TEMPERATURE_INVALID_VALUE)
	{
		return _T("----");
	}
	auto value = (double)nTemperature / DIVIDE_VALUE_FOR_HEAD_TEMPARETURE_DISPLAY;
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) << value;
	CString strValue(oss.str().c_str());

	CString result;
	result.Format(_T("%s C"), strValue);
	return result;
}

/*
 （显示轮廓资料信息）Show the profile informaion
 @param LJX8IF_PROFILE_INFO
*/
void CSingleFuncDlg::ShowProfileInfo(LJX8IF_PROFILE_INFO profileInfo)
{
	CAtlString strLog;

	strLog.Format(_T("  Profile Data Num			: %d\r\n"), profileInfo.byProfileCount);
	m_strCommandLog += (LPCTSTR)strLog;

	CString luminanceOutput = profileInfo.byLuminanceOutput == 1 ? _T("ON") : _T("OFF");
	strLog.Format(_T("  Luminance output			: %s\r\n"), luminanceOutput);
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  Profile Data Points			: %d\r\n"), profileInfo.wProfileDataCount);
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  X coordinate of the first point	: %d\r\n"), profileInfo.lXStart);
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  X-direction interval		: %d\r\n"), profileInfo.lXPitch);
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
 Show the profile response
 @param LJX8IF_GET_PROFILE_RESPONSE
*/
void CSingleFuncDlg::ShowResponse(LJX8IF_GET_PROFILE_RESPONSE rsp)
{
	CAtlString strLog;

	strLog.Format(_T("  CurrentProfileNo		: %d\r\n"), rsp.dwCurrentProfileNo);
	m_strCommandLog += (LPCTSTR)strLog;
	strLog.Format(_T("  OldestProfileNo		: %d\r\n"), rsp.dwOldestProfileNo);
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  GetTopProfileNo		: %d\r\n"), rsp.dwGetTopProfileNo);
	m_strCommandLog += (LPCTSTR)strLog;
	strLog.Format(_T("  GetProfileCount		: %d\r\n"), rsp.byGetProfileCount);
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
 Show the batch profile response
 @param LJX8IF_GET_BATCH_PROFILE_RESPONSE
*/
void CSingleFuncDlg::ShowResponse(LJX8IF_GET_BATCH_PROFILE_RESPONSE rsp)
{
	CAtlString strLog;

	strLog.Format(_T("  CurrentBatchNo			: %d\r\n"), rsp.dwCurrentBatchNo);
	m_strCommandLog += (LPCTSTR)strLog;
	strLog.Format(_T("  CurrentBatchProfileCount		: %d\r\n"), rsp.dwCurrentBatchProfileCount);
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  OldestBatchNo			: %d\r\n"), rsp.dwOldestBatchNo);
	m_strCommandLog += (LPCTSTR)strLog;
	strLog.Format(_T("  OldestBatchProfileCount		: %d\r\n"), rsp.dwOldestBatchProfileCount);
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  GetBatchNo			: %d\r\n"), rsp.dwGetBatchNo);
	m_strCommandLog += (LPCTSTR)strLog;
	strLog.Format(_T("  GetBatchProfileCount		: %d\r\n"), rsp.dwGetBatchProfileCount);
	m_strCommandLog += (LPCTSTR)strLog;

	strLog.Format(_T("  GetBatchTopProfileNo		: %d\r\n"), rsp.dwGetBatchTopProfileNo);
	m_strCommandLog += (LPCTSTR)strLog;
	strLog.Format(_T("  GetProfileCount			: %d\r\n"), rsp.byGetProfileCount);
	m_strCommandLog += (LPCTSTR)strLog;
	strLog.Format(_T("  CurrentBatchCommited		: %d\r\n"), rsp.byCurrentBatchCommited);
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
 Display the send command and the result
 显示发送命令和结果
 @param Return code
 @param Command ID
*/
void CSingleFuncDlg::DisplayCommandLog(LONG lRc, int nCommandID)
{
	CString strCommand;
	strCommand.LoadStringW(nCommandID);
	AddLogResult(lRc, strCommand);
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
 Add log and reslut
 @param Return code
 @param Command code
*/
void CSingleFuncDlg::AddLogResult(LONG lRc, CString strCommandCode)
{
	CString strResult;

	if (lRc == LJX8IF_RC_OK)
	{
		strResult.LoadStringW(IDS_RESULT_OK);
		AddLog(strCommandCode, strResult, lRc);
	}
	else
	{
		strResult.LoadStringW(IDS_RESULT_NG);
		AddLog(strCommandCode, strResult, lRc);
		AddErrorLog(lRc);
	}
}

/*
 Add error log
 @param Return code
*/
void CSingleFuncDlg::AddErrorLog(LONG lRc)
{
	if (lRc < 0x8000)
	{
		CommonErrorLog(lRc); // Common return code
	}
	else
	{
		// Controller return code
		if (ControllerErrorLog(lRc))
		{
			return;
		}

		IndividualErrorLog(lRc); // Individual return code
	}
}

/*
 Commoon error log
 普遍错误日志
 @param Return code
*/
void CSingleFuncDlg::CommonErrorLog(LONG lRc)
{
	switch (lRc)
	{
	case (int)LJX8IF_RC_OK:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_OK);
		break;
	case (int)LJX8IF_RC_ERR_OPEN:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_OPEN_DEVICE);
		break;
	case (int)LJX8IF_RC_ERR_NOT_OPEN:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_NO_DEVICE);
		break;
	case (int)LJX8IF_RC_ERR_SEND:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_SEND);
		break;
	case (int)LJX8IF_RC_ERR_RECEIVE:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_RECEIVE);
		break;
	case (int)LJX8IF_RC_ERR_TIMEOUT:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_TIMEOUT);
		m_aDeviceData[m_nCurrentDeviceID].m_deviceStatus = CDeviceData::DEVICESTATUS_NO_CONNECTION;
		InitializeConnectionInfo(m_nCurrentDeviceID);
		SetDlgItemText(m_sttConnectionStatus[m_nCurrentDeviceID].GetDlgCtrlID(), m_aDeviceData[m_nCurrentDeviceID].GetStatusString());
		break;
	case (int)LJX8IF_RC_ERR_PARAMETER:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_PARAMETER);
		break;
	case (int)LJX8IF_RC_ERR_NOMEMORY:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_NOMEMORY);
		break;
	case (int)LJX8IF_RC_ERR_HISPEED_NO_DEVICE:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_HISPEED_NO_DEVICE);
		break;
	case (int)LJX8IF_RC_ERR_HISPEED_OPEN_YET:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_HISPEED_OPEN_YET);
		break;
	case (int)LJX8IF_RC_ERR_BUFFER_SHORT:
		AddAdditionalErrorLog(IDS_RC_FORMAT, IDS_RC_ERR_BUFFER_SHORT);
		break;
	default:
		AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
		break;
	}
}

/*
 Indivisual error Log
 @param Return code
*/
void CSingleFuncDlg::IndividualErrorLog(LONG lRc)
{
	switch (m_sendCommand)
	{
	case SENDCOMMAND_REBOOT_CONTROLLER:
	{
		switch (lRc)
		{
		case 0x80A0:
			AddLog(IDS_RC_FORMAT, _T("Accessing the save area"));
			break;
		default:
			AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
			break;
		}
	}
	break;

	case SENDCOMMAND_TRIGGER:
	{
		switch (lRc)
		{
		case 0x8080:
			AddLog(IDS_RC_FORMAT, _T("The trigger mode is not [external trigger]"));
			break;
		default:
			AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
			break;
		}
	}
	break;

	case SENDCOMMAND_START_MEASURE:
	case SENDCOMMAND_STOP_MEASURE:
	{
		switch (lRc)
		{
		case 0x8080:
			AddLog(IDS_RC_FORMAT, _T("Batch measurements are off or several controller Sync function is Sync Slave"));
			break;
		case 0x80A0:
			AddLog(IDS_RC_FORMAT, _T("Batch measurement start or stop processing could not be performed because laser off by command or the LASER_ON terminal is off"));
			break;
		default:
			AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
			break;
		}
	}
	break;

	case SENDCOMMAND_GET_PROFILE:
	{
		switch (lRc)
		{
		case 0x8081:
			AddLog(IDS_RC_FORMAT, _T("Batch measurements on"));
			break;
		case 0x80A0:
			AddLog(IDS_RC_FORMAT, _T("No profile data"));
			break;
		default:
			AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
			break;
		}
	}
	break;

	case SENDCOMMAND_GET_BATCH_PROFILE:
	{
		switch (lRc)
		{
		case 0x8081:
			AddLog(IDS_RC_FORMAT, _T("Batch measurements off"));
			break;
		case 0x80A0:
			AddLog(IDS_RC_FORMAT, _T("No batch data (batch measurements not run even once)"));
			break;
		default:
			AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
			break;
		}
	}
	break;

	case SENDCOMMAND_INITIALIZE_HIGH_SPEED_DATA_ETHERNET_COMMUNICATION:
	{
		switch (lRc)
		{
		case 0x80A1:
			AddLog(IDS_RC_FORMAT, _T("Already performing high-speed communication error (for high-speed communication)"));
			break;
		default:
			AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
			break;
		}
	}
	break;

	case SENDCOMMAND_PRE_START_HIGH_SPEED_DATA_COMMUNICATION:
	case SENDCOMMAND_START_HIGH_SPEED_DATA_COMMUNICATION:
	{
		switch (lRc)
		{
		case 0x8081:
			AddLog(IDS_RC_FORMAT, _T("The data specified as the send start position does not exist"));
			break;
		case 0x80A0:
			AddLog(IDS_RC_FORMAT, _T("A high-speed data communication connection was not established"));
			break;
		case 0x80A1:
			AddLog(IDS_RC_FORMAT, _T("Already performing high-speed communication error (for high-speed communication)"));
			break;
		case 0x80A2:
			AddLog(IDS_RC_FORMAT, _T("Command code does not match (for high-speed communication)"));
			break;
		case 0x80A3:
			AddLog(IDS_RC_FORMAT, _T("Start code does not match (for high-speed communication"));
			break;
		case 0x80A4:
			AddLog(IDS_RC_FORMAT, _T("The send target data was cleared"));
			break;
		default:
			AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
			break;
		}
	}
	break;

	default:
		AddLog(IDS_NOT_DEFINE_FORMAT, lRc);
		break;
	}
}

/*
Controller error Log
@param Return code
@return Processing continuation condition(TRUE:stop)
*/
BOOL CSingleFuncDlg::ControllerErrorLog(LONG lRc)
{
	switch (lRc)
	{
	case 0x8011:
		return true;
	case 0x8021:
		AddLog(IDS_RC_FORMAT, _T("Controller model difference"));
		return true;
	case 0x8031:
		AddLog(IDS_RC_FORMAT, _T("Undefined command"));
		return true;
	case 0x8032:
		AddLog(IDS_RC_FORMAT, _T("Command length error"));
		return true;
	case 0x8041:
		AddLog(IDS_RC_FORMAT, _T("Controller status error"));
		return true;
	case 0x8042:
		AddLog(IDS_RC_FORMAT, _T("Controller parameter error"));
		return true;
	}

	return false;
}

/*
 Add log
 @param Command code
 @param Result
 @param Return code
*/
void CSingleFuncDlg::AddLog(CString commandCode, CString result, LONG lRc)
{
	CAtlString strLog;

	strLog.Format(_T("[%s]:%s (0x%04X)\r\n"), commandCode, result, lRc);
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
}

/*
 Add Log
 @param Display format
 @param Error message
*/
void CSingleFuncDlg::AddLog(int nDisplayFormat, CString strErrorMessage)
{
	CString strDisplayFormat;
	strDisplayFormat.LoadStringW(nDisplayFormat);

	CAtlString strLog;
	strLog.Format(strDisplayFormat, strErrorMessage);
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
}

/*
 Add Log
 @param Display format
 @param Return Code
*/
void CSingleFuncDlg::AddLog(int nDisplayFormat, LONG lRc)
{
	CString strDisplayFormat;
	strDisplayFormat.LoadStringW(nDisplayFormat);

	CAtlString strLog;
	strLog.Format(strDisplayFormat, lRc);
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
}

/*
 Add Adddirional Error Log
 添加 添加错误到日志框
 @param Display format
 @param Error log
*/
void CSingleFuncDlg::AddAdditionalErrorLog(int nDisplayFormat, int nErrorLog)
{
	CString strDisplayFormat;
	strDisplayFormat.LoadStringW(nDisplayFormat);

	CString strErrorLog;
	strErrorLog.LoadStringW(nErrorLog);

	CAtlString strLog;
	strLog.Format(strDisplayFormat, strErrorLog);
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
}

/*
Add Error Log
添加错误到日志框
@param error
*/
void CSingleFuncDlg::AddSettingErrorLog(DWORD dwError)
{
	CString strError;
	strError.Format(_T("0x%08X"), dwError);
	AddResult(_T("ErrorCode"), strError);
}

/*
Add Result
添加结果显示到日志框
@param item name
@param result text
*/
void CSingleFuncDlg::AddResult(CString item, CString text)
{
	CString strLog;
	strLog.Format(IDS_COMMAND_RESULT_FORMAT, item, text);
	m_strCommandLog += (LPCTSTR)strLog;

	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
 Use Simple Array Control
 使用简单的数组控件
*/
void CSingleFuncDlg::OnBnClickedChkUsesimplearray()
{
	UpdateHighSpeedGetSimpleArrayItemEnabled();
}

/*
 Count only the number of profiles Control
 仅计算轮廓配置文件的数量
*/
void CSingleFuncDlg::OnBnClickedChkCountOnly()
{
	UpdateHighSpeedProfileSaveSettingItemEnabled();
}

/*
 Timer Control
 时间控制函数
*/
void CSingleFuncDlg::OnBnClickedChkTimer()
{
	UpdateData(TRUE);
	UINT nTimerELASP = m_sbcSpinStartTimer.GetPos32();

	// Start Timer or Stop Timer
	// 输入触发间隔同时要确认是否选中触发按钮
	if (m_bIsStartTimer)
	{
		SetTimer(TIMER_ID, nTimerELASP, NULL);
	}
	else
	{
		KillTimer(TIMER_ID);
	}

	// It prevents a callback function from being available
	// 它阻止回调函数可用
	CButton *button = (CButton *)GetDlgItem(IDC_CHK_COUNTONLY);
	CEdit *startTimerEdit = (CEdit *)GetDlgItem(IDC_EDIT_STARTTIMER);
	if (m_bIsStartTimer)
	{
		button->EnableWindow(FALSE);
		startTimerEdit->EnableWindow(FALSE);
	}
	else
	{
		button->EnableWindow(TRUE);
		startTimerEdit->EnableWindow(TRUE);
	}
}

/*
 Start the timer control focus out.
 启用时间控制焦点
*/
void CSingleFuncDlg::OnEnKillfocusEditStarttimer()
{
	ConfirmTimerElapse();
}

void CSingleFuncDlg::ConfirmTimerElapse()
{
	UINT nTimerELASP = m_sbcSpinStartTimer.GetPos32();
	m_strSpinStartTimer.Format(_T("%d"), nTimerELASP);
	int scrollPos = m_txtCommandLog.GetFirstVisibleLine();
	UpdateData(FALSE);
	m_txtCommandLog.LineScroll(scrollPos);
}

/*
 Show the message
 显示消息函数
 @param Message
*/
void CSingleFuncDlg::ShowMessage(CString message)
{
	CAtlString strLog;
	strLog.Format(message + _T("\r\n"));
	m_strCommandLog += (LPCTSTR)strLog;
	UpdateData(FALSE);
	//让滚动条处于最下方位置
	m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
}

/*
Measurement range for LJ-X combobox selected index is changed.
*/
void CSingleFuncDlg::OnCbnSelchangeCmbLjxmeasurex()
{
	UpdateBufferSizeText();
}

/*
Thinning for LJ-X combobox selected index is changed.
*/
void CSingleFuncDlg::OnCbnSelchangeCmbLjxthinning()
{
	UpdateBufferSizeText();
}

/*
 Luminance output for LJ-X combobox selected index is changed.
 LJ-X亮度值输出已经选中。
*/
void CSingleFuncDlg::OnCbnSelchangeCmbLjxluminanceoutput()
{
	UpdateBufferSizeText();
}

/*
Sampling period for LJ-X combobox selected index is changed.
*/
void CSingleFuncDlg::OnCbnSelchangeCmbLjxsamplingperiod()
{
	UpdateBufferSizeText();
}

/*
Measurement range for LJ-V combobox selected index is changed.
*/
void CSingleFuncDlg::OnCbnSelchangeCmbLjvmeasurex()
{
	UpdateBufferSizeText();
}

/*
Thinning for LJ-X combobox selected index is changed.
*/
void CSingleFuncDlg::OnCbnSelchangeCmbLjvthinning()
{
	UpdateBufferSizeText();
}

/*
 Binning for LJ-X combobox selected index is changed.
*/
void CSingleFuncDlg::OnCbnSelchangeCmbLjvbinning()
{
	UpdateBufferSizeText();
}

/*
LJ-X radio button is clicked.
*/
void CSingleFuncDlg::OnBnClickedRadioLjx()
{
	UpdateBufferSizeText();
	UpdateBufferSizeSettingItemEnabled();
}

/*
 LJ-V radio button is clicked.
*/
void CSingleFuncDlg::OnBnClickedRadioLjv()
{
	UpdateBufferSizeText();
	UpdateBufferSizeSettingItemEnabled();
}

/*
 LJ-VB radio button is clicked.
*/
void CSingleFuncDlg::OnBnClickedRadioLjvb()
{
	UpdateBufferSizeText();
	UpdateBufferSizeSettingItemEnabled();
}

/*
 Update buffer size setting item enabled
 更新缓冲区大小设置项已启用
*/
void CSingleFuncDlg::UpdateBufferSizeSettingItemEnabled()
{
	BOOL isLjxSelected = m_nHeadType == LJX_HEAD_INDEX;
	UpdateLjxBufferSizeSettingItemEnabled(isLjxSelected);
	UpdateLjvBufferSizeSettingItemEnabled(!isLjxSelected);
}

/*
 Update High Speed Simple Array item enabled
 选中启用高速数据通信（简单数组）
*/
void CSingleFuncDlg::UpdateHighSpeedGetSimpleArrayItemEnabled()
{
	UpdateData(TRUE);

	CButton *initializeCommunicationButton = (CButton *)GetDlgItem(IDC_BTN_INITIALIZEHIGHSPEEDDATACOMMUNICATION);
	CButton *initializeSimpleArrayButton = (CButton *)GetDlgItem(IDC_BTN_INITIALIZEHIGHSPEEDDATACOMMUNICATIONSIMPLEARRAY);
	CButton *saveProfileButton = (CButton *)GetDlgItem(IDC_SAVE_PROFILE);
	CButton *saveAsBitmapFileButton = (CButton *)GetDlgItem(IDC_BTN_HIGHSPEEDSAVEASBITMAPFILE);

	initializeCommunicationButton->EnableWindow(!m_bIsUseSimpleArray);
	initializeSimpleArrayButton->EnableWindow(m_bIsUseSimpleArray);
	saveProfileButton->EnableWindow(!m_bIsUseSimpleArray && !m_bIsCountOnly);
	saveAsBitmapFileButton->EnableWindow(m_bIsUseSimpleArray && !m_bIsCountOnly);

	//自动显示到文本框的最后一行
	int scrollPos = m_txtCommandLog.GetFirstVisibleLine();

	UpdateData(FALSE);

	m_txtCommandLog.LineScroll(scrollPos);
}

/*
Update High Speed Profile Save setting item enabled
*/
void CSingleFuncDlg::UpdateHighSpeedProfileSaveSettingItemEnabled()
{
	UpdateData(TRUE);
	CEdit *highSpeedSaveFilePathEdit = (CEdit *)GetDlgItem(IDC_EDIT_HIGHSPEEDSAVEFILEPATH);
	CButton *saveDestinationButton = (CButton *)GetDlgItem(IDC_BTN_HIGHSPEEDSAVE_DESTINATION);
	CButton *saveProfileButton = (CButton *)GetDlgItem(IDC_SAVE_PROFILE);
	CButton *saveAsBitmapFileButton = (CButton *)GetDlgItem(IDC_BTN_HIGHSPEEDSAVEASBITMAPFILE);
	CEdit *profileIndexEdit = (CEdit *)GetDlgItem(IDC_EDIT_PROFILEINDEX);
	CEdit *profileCountEdit = (CEdit *)GetDlgItem(IDC_EDIT_PROFILECOUNT);

	highSpeedSaveFilePathEdit->EnableWindow(!m_bIsCountOnly);
	saveDestinationButton->EnableWindow(!m_bIsCountOnly);
	saveProfileButton->EnableWindow(!m_bIsCountOnly && !m_bIsUseSimpleArray);
	saveAsBitmapFileButton->EnableWindow(!m_bIsCountOnly && m_bIsUseSimpleArray);
	profileIndexEdit->EnableWindow(!m_bIsCountOnly);
	profileCountEdit->EnableWindow(!m_bIsCountOnly);

	int scrollPos = m_txtCommandLog.GetFirstVisibleLine();

	UpdateData(FALSE);

	m_txtCommandLog.LineScroll(scrollPos);
}

/*
Update buffer size setting item enabled for LJ-X setting item
*/
void CSingleFuncDlg::UpdateLjxBufferSizeSettingItemEnabled(BOOL isEnabled)
{
	CComboBox *ljxMeasureXComboBox = (CComboBox *)GetDlgItem(IDC_CMB_LJXMEASUREX);
	CComboBox *ljxThinningComboBox = (CComboBox *)GetDlgItem(IDC_CMB_LJXTHINNING);
	CComboBox *ljxLuminanceOutputComboBox = (CComboBox *)GetDlgItem(IDC_CMB_LJXLUMINANCEOUTPUT);
	CComboBox *ljxSamplingPeriodComboBox = (CComboBox *)GetDlgItem(IDC_CMB_LJXSAMPLINGPERIOD);
	CStatic *ljxMeasureXStatic = (CStatic *)GetDlgItem(IDC_STATIC_LJXMEASUREX);
	CStatic *ljxThinningStatic = (CStatic *)GetDlgItem(IDC_STATIC_LJXTHINNING);
	CStatic *ljxLuminanceOutputStatic = (CStatic *)GetDlgItem(IDC_STATIC_LJXLUMINANCEOUTPUT);
	CStatic *ljxSamplingPeriodStatic = (CStatic *)GetDlgItem(IDC_STATIC_LJXSAMPLINGPERIOD);
	CStatic *ljxSamplingPeriodNoteStatic = (CStatic *)GetDlgItem(IDC_STATIC_SAMPLINGPERIODNOTE);

	ljxMeasureXComboBox->EnableWindow(isEnabled);
	ljxThinningComboBox->EnableWindow(isEnabled);
	ljxLuminanceOutputComboBox->EnableWindow(isEnabled);
	ljxSamplingPeriodComboBox->EnableWindow(isEnabled);

	ljxMeasureXStatic->EnableWindow(isEnabled);
	ljxThinningStatic->EnableWindow(isEnabled);
	ljxLuminanceOutputStatic->EnableWindow(isEnabled);
	ljxSamplingPeriodStatic->EnableWindow(isEnabled);
	ljxSamplingPeriodNoteStatic->EnableWindow(isEnabled);
}

/*
Update buffer size setting item enabled for LJ-V setting item
*/
void CSingleFuncDlg::UpdateLjvBufferSizeSettingItemEnabled(BOOL isEnabled)
{
	CComboBox *ljvMeasureXComboBox = (CComboBox *)GetDlgItem(IDC_CMB_LJVMEASUREX);
	CComboBox *ljvThinningComboBox = (CComboBox *)GetDlgItem(IDC_CMB_LJVTHINNING);
	CComboBox *ljvBinningComboBox = (CComboBox *)GetDlgItem(IDC_CMB_LJVBINNING);
	CStatic *ljvMeasureXStatic = (CStatic *)GetDlgItem(IDC_STATIC_LJVMEASUREX);
	CStatic *ljvThinningStatic = (CStatic *)GetDlgItem(IDC_STATIC_LJVTHINNING);
	CStatic *ljvBinningStatic = (CStatic *)GetDlgItem(IDC_STATIC_LJVBINNING);

	ljvMeasureXComboBox->EnableWindow(isEnabled);
	ljvThinningComboBox->EnableWindow(isEnabled);
	ljvBinningComboBox->EnableWindow(isEnabled);

	ljvMeasureXStatic->EnableWindow(isEnabled);
	ljvThinningStatic->EnableWindow(isEnabled);
	ljvBinningStatic->EnableWindow(isEnabled);
}

/*
 Get one profile size for current specified state
 @return one profile data size(byte)
*/
int CSingleFuncDlg::GetOneProfileDataSize()
{
	int nProfileCount = GetXDirectionDataCount();
	// Buffer size (in units of bytes)
	UINT oneProfileBufferSize = 0;

	// Number of headers
	oneProfileBufferSize += (UINT)nProfileCount;

	//in units of bytes
	oneProfileBufferSize *= sizeof(UINT);

	oneProfileBufferSize += sizeof(LJX8IF_PROFILE_HEADER); // Sizes of the header and footer structures
	oneProfileBufferSize += sizeof(LJX8IF_PROFILE_FOOTER);

	return oneProfileBufferSize;
}

/*
Get one profile x direction data count
@return one profile x direction data count
*/
int CSingleFuncDlg::GetXDirectionDataCount()
{
	return m_nHeadType == LJX_HEAD_INDEX ? GetLjxXDirectionDataCount() : GetLjvXDirectionDataCount();
}

/*
Get one profile x direction data count for LJ-V Head
@return one profile x direction data count
*/
int CSingleFuncDlg::GetLjvXDirectionDataCount()
{
	int nMultipleValueForLuminanceOutput = m_nHeadType == LJVB_HEAD_INDEX ? MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT : 1;
	return GetLjvProfileCount() * nMultipleValueForLuminanceOutput;
}

/*
GetLjvProfileCount
@return one profile count
*/
int CSingleFuncDlg::GetLjvProfileCount()
{
	int profileDataCountBeforeThinning = GetLjvProfileCountByMeasureRange() / GetLjvDivideValueByBinning();
	CString strThinning;
	m_cmbLjvThinning.GetLBText(m_nLjvThinning, strThinning);
	if (m_nLjvThinning == LJV_THINNING_OFF_INDEX)
	{
		return profileDataCountBeforeThinning;
	}
	int halfProfileDataCount = profileDataCountBeforeThinning / 2;
	if (m_nLjvThinning == LJV_THINNING_2_INDEX)
	{
		return PROFILE_DATA_MIN_COUNT <= halfProfileDataCount ? halfProfileDataCount : profileDataCountBeforeThinning;
	}
	//In the thinning 4 case
	if (halfProfileDataCount < PROFILE_DATA_MIN_COUNT)
	{
		return profileDataCountBeforeThinning;
	}
	int quarterProfileDataCount = profileDataCountBeforeThinning / 4;
	return PROFILE_DATA_MIN_COUNT <= quarterProfileDataCount ? quarterProfileDataCount : halfProfileDataCount;
}

/*
Get one profile data count by x measure range for LJ-V Head
@return one profile x direction data count
*/
int CSingleFuncDlg::GetLjvProfileCountByMeasureRange()
{
	CString strXRange;
	m_cmbLjvMeasureX.GetLBText(m_nLjvMeasureX, strXRange);
	if (strXRange == _T("FULL"))
	{
		return 800;
	}
	else if (strXRange == _T("MIDDLE"))
	{
		return 600;
	}
	else if (strXRange == _T("SMALL"))
	{
		return 400;
	}
	return 0;
}

/*
Get divide value by Binning for LJ-V Head
@return thinning count for calculating profile data size.
*/
int CSingleFuncDlg::GetLjvDivideValueByBinning()
{
	CString strBinning;
	m_cmbLjvBinning.GetLBText(m_nLjvBinning, strBinning);
	if (strBinning == _T("OFF"))
	{
		return 1;
	}
	else if (strBinning == _T("ON"))
	{
		return 2;
	}
	return 0;
	;
}

/*
 GetLjxXDirectionDataCount
 @return one profile data count(x axis)
*/
int CSingleFuncDlg::GetLjxXDirectionDataCount()
{
	int nMultipleValueForLuminanceOutput = GetLjxLuminanceOutputOn() ? MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT : 1;
	return GetLjxProfileCount() * nMultipleValueForLuminanceOutput;
}

/*
 GetLjxProfileCount
 @return one profile count
*/
int CSingleFuncDlg::GetLjxProfileCount()
{
	bool bIsXBinningOn = GetIsLjxXBinningOn();
	int nDevidedValue = bIsXBinningOn ? 2 : 1;
	int profileDataCount = GetLjxProfileCountByMeasureRange() / GetLjxDivideValueByThinning() / nDevidedValue;
	return max(PROFILE_DATA_MIN_COUNT, profileDataCount);
}

/*
 GetLjxProfileCountByMeasureRange
 @return data count devended range for calculating profile data size. 
*/
int CSingleFuncDlg::GetLjxProfileCountByMeasureRange()
{
	CString strXRange;
	m_cmbXRange.GetLBText(m_nXRange, strXRange);
	if (strXRange == _T("FULL"))
	{
		return 3200;
	}
	else if (strXRange == _T("3/4"))
	{
		return 2400;
	}
	else if (strXRange == _T("1/2"))
	{
		return 1600;
	}
	else if (strXRange == _T("1/4"))
	{
		return 800;
	}
	return 0;
}

/*
 GetLjxDivideValueByThinning
 @return thinning count for calculating profile data size.
*/
int CSingleFuncDlg::GetLjxDivideValueByThinning()
{
	CString strThinning;
	m_cmbXThinning.GetLBText(m_nXThinning, strThinning);
	if (strThinning == _T("OFF"))
	{
		return 1;
	}
	else if (strThinning == _T("1/2"))
	{
		return 2;
	}
	else if (strThinning == _T("1/4"))
	{
		return 4;
	}
	return 0;
	;
}

/*
 GetIsLjxXBinningOn
 @return X binning on is true/ X binning off is false;
 binning按钮的点击事件是否选中
*/
bool CSingleFuncDlg::GetIsLjxXBinningOn()
{
	// Get luminance output
	bool bIsLuminanceOutput = GetLjxLuminanceOutputOn();

	//Get sampling period
	CString strSamplingPeriod;
	m_cmbSamplingPeriod.GetLBText(m_nSamplingPeriod, strSamplingPeriod);

	if ((bIsLuminanceOutput) &&
		(strSamplingPeriod == _T("8kHz")))
	{
		return true;
	}
	else if ((!bIsLuminanceOutput) &&
			 ((strSamplingPeriod == _T("16kHz"))))
	{
		return true;
	}

	return false;
}

/*
 GetLjxLuminanceOutputOn
 @return luminance output on is true/luminance output off is false
 @返回亮度输出为真/亮度输出为假
*/
bool CSingleFuncDlg::GetLjxLuminanceOutputOn()
{
	CString strLuminanceOutput;
	m_cmbLuminanceOutput.GetLBText(m_nLuminanceOutput, strLuminanceOutput);
	return strLuminanceOutput == _T("ON");
}

//更新显示缓存区文本
void CSingleFuncDlg::UpdateBufferSizeText()
{
	UpdateData(TRUE);
	CString bufferSizeText = GetBufferSizeText();
	m_sttBufferSizeValue.SetWindowTextW(bufferSizeText);
}

/*
 GetBufferSizeText
 @return display text
 获取缓冲区大小
 返回要显示的文字内容
*/
CString CSingleFuncDlg::GetBufferSizeText()
{
	CString strSamplingPeriod;
	m_cmbSamplingPeriod.GetLBText(m_nSamplingPeriod, strSamplingPeriod);

	if ((m_nHeadType == LJX_HEAD_INDEX) &&
		(GetLjxLuminanceOutputOn()) &&
		(strSamplingPeriod == _T("16kHz")))
	{
		return _T("-----");
	}

	CString bufferSizeText;
	bufferSizeText.Format(_T("%d"), GetOneProfileDataSize());
	return bufferSizeText;
}

/*
 Calcultate the data size
 @param LJX8IF_PROFILE_INFO
 @return Data size(byte)
*/
int CSingleFuncDlg::CalculateDataSize(const LJX8IF_PROFILE_INFO &profileInfo)
{
	return profileInfo.wProfileDataCount * profileInfo.byProfileCount +
		   (sizeof(LJX8IF_PROFILE_HEADER) + sizeof(LJX8IF_PROFILE_FOOTER)) / sizeof(int);
}

/*
Analize profile data
*/
void CSingleFuncDlg::AnalyzeProfileData(BYTE byGetProfileCount, LJX8IF_PROFILE_INFO profileInfo, vector<int> profileData)
{
	int nMultipleValue = GetIsLuminanceOutput(profileInfo) ? MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT : 1;
	int nDataUnitSize = (sizeof(LJX8IF_PROFILE_HEADER) + sizeof(int) * profileInfo.wProfileDataCount * profileInfo.byProfileCount * nMultipleValue + sizeof(LJX8IF_PROFILE_FOOTER)) / sizeof(int);
	for (int i = 0; i < byGetProfileCount; i++)
	{
		int *pnBlock = &profileData.at(nDataUnitSize * i);

		LJX8IF_PROFILE_HEADER *pHeader = (LJX8IF_PROFILE_HEADER *)pnBlock;
		int *pnProfileData = pnBlock + (sizeof(LJX8IF_PROFILE_HEADER) / sizeof(DWORD));
		LJX8IF_PROFILE_FOOTER *pFooter = (LJX8IF_PROFILE_FOOTER *)(pnProfileData + profileInfo.wProfileDataCount * profileInfo.byProfileCount * nMultipleValue);

		// Store the profile data
		m_aDeviceData[m_nCurrentDeviceID].m_vecProfileData.push_back(PROFILE_DATA(profileInfo, pHeader, pnProfileData, pFooter));
	}
}

/*
Get luminance output is on
获取亮度输出已打开
*/
BOOL CSingleFuncDlg::GetIsLuminanceOutput(LJX8IF_PROFILE_INFO profileInfo)
{
	return profileInfo.byLuminanceOutput == LUMINANCE_OUTPUT_ON_VALUE;
}

/*
Export profile data
*/
void CSingleFuncDlg::ExportProfile()
{
	if (m_strSaveFilePath.IsEmpty())
	{
		ShowMessage(_T("Failed in exporting file. (File path is empty.)"));
		return;
	}

	BOOL bIsSuccess = false;
	if (m_aDeviceData[m_nCurrentDeviceID].m_vecProfileData.size() != 0)
	{
		bIsSuccess = CDataExport::ExportProfileEx(&(m_aDeviceData[m_nCurrentDeviceID].m_vecProfileData.at(0)), m_strSaveFilePath, (int)m_aDeviceData[m_nCurrentDeviceID].m_vecProfileData.size());
	}
	if (!bIsSuccess)
	{
		ShowMessage(_T("Failed in exporting file."));
		return;
	}
	ShowMessage(_T("###Save!!"));
}

/*
回调函数（接收配置文件数据）
@param	配置文件数据的指针
@param	一个配置文件数据的大小
@param	配置文件计数器
@param	通知
@param	UserID
*/
void CSingleFuncDlg::ReceiveHighSpeedData(BYTE *pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
{
	// @Point
	//注意仅在回调函数中实现将配置文件数据存储在线程保存缓冲区中。
	//由于用于调用回调函数的线程与用于接收数据的线程相同，
	//回调函数的处理时间会影响数据接收的速度，
	//并可能在某些环境中使通信无法正常执行。
	//由于它与线程相同，因此回调函数的处理时间会影响数据接收速度和
	//根据边界，可能无法进行正常通信。

	vector<PROFILE_DATA> vecProfileData;

	int nProfileDataCount = (dwSize - sizeof(LJX8IF_PROFILE_HEADER) - sizeof(LJX8IF_PROFILE_FOOTER)) / sizeof(DWORD);

	for (DWORD i = 0; i < dwCount; i++)
	{
		BYTE *pbyBlock = pBuffer + dwSize * i;

		LJX8IF_PROFILE_HEADER *pHeader = (LJX8IF_PROFILE_HEADER *)pbyBlock;
		int *pnProfileData = (int *)(pbyBlock + sizeof(LJX8IF_PROFILE_HEADER));
		LJX8IF_PROFILE_FOOTER *pFooter = (LJX8IF_PROFILE_FOOTER *)(pbyBlock + dwSize - sizeof(LJX8IF_PROFILE_FOOTER));

		vecProfileData.push_back(PROFILE_DATA(m_aProfileInfo[dwUser], pHeader, pnProfileData, pFooter));
	}
	CThreadSafeBuffer *threadSafeBuf = CThreadSafeBuffer::getInstance();

	if (threadSafeBuf->m_vecProfileData[dwUser].size() + vecProfileData.size() < BUFFER_FULL_COUNT)
	{
		threadSafeBuf->Add(dwUser, vecProfileData, dwNotify);
	}
	else
	{
		m_bIsBufferFull[dwUser] = true;
	}
}

/*
回调函数（仅计数）
@param配置文件数据的指针
@param一个配置文件数据的大小
@param配置文件计数器
@param通知
@param UserID
*/
void CSingleFuncDlg::CountProfileReceive(BYTE *pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
{
	// @Point
	//注意仅在回调函数中实现将配置文件数据存储在线程保存缓冲区中。
	//由于用于调用回调函数的线程与用于接收数据的线程相同，
	//回调函数的处理时间会影响数据接收的速度，
	//并可能在某些环境中使通信无法正常执行。

	CThreadSafeBuffer *threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->AddCount(dwUser, dwCount, dwNotify);
}

/**
简单数组的回调函数（接收配置文件数据）
  @param指向存储头数据数组的缓冲区的指针。
  @param指向存储配置文件数据数组的缓冲区的指针。
  @param指向存储亮度分布数据数组的缓冲区的指针。
  @param指示亮度数据输出是否启用的值。
  @param一个配置文件的数据计数。
  @param存储在缓冲区中的配置文件或头数据的数量。
  @param通知
  @param用户ID
*/
void CSingleFuncDlg::ReceiveHighSpeedSimpleArray(LJX8IF_PROFILE_HEADER *pProfileHeaderArray,
												 WORD *pHeightProfileArray,
												 WORD *pLuminanceProfileArray,
												 DWORD dwLuminanceEnable,
												 DWORD dwProfileDataCount,
												 DWORD dwCount,
												 DWORD dwNotify,
												 DWORD dwUser)
{
	// @Point
	//注意仅在回调函数中实现将配置文件数据存储在线程保存缓冲区中。
	//由于用于调用回调函数的线程与用于接收数据的线程相同，
	//回调函数的处理时间会影响数据接收的速度，
	//并可能在某些环境中使通信无法正常执行。
	m_bIsBufferFull[dwUser] = m_aDeviceData[dwUser].m_simpleArrayStoreHighSpeed.AddReceivedData(pHeightProfileArray, pLuminanceProfileArray, dwCount);
	m_aDeviceData[dwUser].m_simpleArrayStoreHighSpeed.AddNotify(dwNotify);
}

/**
  简单数组的回调函数（仅计数）
  @param指向存储头数据数组的缓冲区的指针。
  @param指向存储配置文件数据数组的缓冲区的指针。
  @param指向存储亮度分布数据数组的缓冲区的指针。
  @param指示亮度数据输出是否启用的值。
  @param一个配置文件的数据计数。
  @param存储在缓冲区中的配置文件或头数据的数量。
  @param通知
  @param用户ID
*/
void CSingleFuncDlg::CountSimpleArrayReceive(LJX8IF_PROFILE_HEADER *pProfileHeaderArray, WORD *pHeightProfileArray,
											 WORD *pLuminanceProfileArray, DWORD dwLuminanceEnable, DWORD dwProfileDataCount, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
{
	// @Point
	// Take care to only implement storing profile data in a thread save buffer in the callback function.
	// As the thread used to call the callback function is the same as the thread used to receive data,
	// the processing time of the callback function affects the speed at which data is received,
	// and may stop communication from being performed properly in some environments.
	m_aDeviceData[dwUser].m_simpleArrayStoreHighSpeed.AddCount(dwCount);
	m_aDeviceData[dwUser].m_simpleArrayStoreHighSpeed.AddNotify(dwNotify);
}

/**
 初始化时清除所有缓冲区以提高速度。
 @param the target device id.
*/
void CSingleFuncDlg::ClearAllHighspeedBuffer(int nDeviceId)
{
	CThreadSafeBuffer *threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->ClearBuffer(nDeviceId);
	m_aDeviceData[nDeviceId].m_vecProfileDataHighSpeed.clear();
	m_aDeviceData[nDeviceId].m_vecProfileDataHighSpeed.shrink_to_fit();
	m_aDeviceData[nDeviceId].m_simpleArrayStoreHighSpeed.Clear();
	m_bIsBufferFull[nDeviceId] = false;
	m_bIsStopCommunicationByError[nDeviceId] = false;
}

/*
 Timer function
 时间消息函数
 @param Event ID
*/
void CSingleFuncDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID)
	{
		int nBatchNo = 0;	//批次
		DWORD dwNotify = 0; //通知

		for (int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++)
		{
			if (m_bIsUseSimpleArray)
			{
				m_anProfReceiveCount[i] = m_aDeviceData[i].m_simpleArrayStoreHighSpeed.GetCount();
				dwNotify = m_aDeviceData[i].m_simpleArrayStoreHighSpeed.GetNotify();
				nBatchNo = m_aDeviceData[i].m_simpleArrayStoreHighSpeed.m_nBatchNo;
			}
			else if (m_bIsCountOnly)
			{
				CThreadSafeBuffer *threadSafeBuf = CThreadSafeBuffer::getInstance();
				m_anProfReceiveCount[i] = threadSafeBuf->GetCount(i, &dwNotify, &nBatchNo);
			}
			else
			{
				CThreadSafeBuffer *threadSafeBuf = CThreadSafeBuffer::getInstance();
				vector<PROFILE_DATA> vecProfileData;
				threadSafeBuf->Get(i, &dwNotify, &nBatchNo, vecProfileData);

				if (vecProfileData.size() == 0 && dwNotify == 0)
					continue;

				for (unsigned int j = 0; j < vecProfileData.size(); j++)
				{
					if (m_aDeviceData[i].m_vecProfileDataHighSpeed.size() < BUFFER_FULL_COUNT)
					{
						m_aDeviceData[i].m_vecProfileDataHighSpeed.push_back(vecProfileData.at(j));
					}
					m_anProfReceiveCount[i]++;
				}
			}

			if (dwNotify == 0)
				continue;

			CString strLog;
			strLog.Format(_T("notify[%d] = 0x%08X \tbatch[%d] \r\n"), i, dwNotify, nBatchNo);
			m_strCommandLog += (LPCTSTR)strLog;
			UpdateData(FALSE);
			m_txtCommandLog.LineScroll(m_txtCommandLog.GetLineCount());
		}

		UpDateReceiveCount();
	}

	if (nIDEvent == BUFFER_FULL_TIMER_ID)
	{
		for (int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++)
		{
			if ((m_bIsBufferFull[i]) && (!m_bIsStopCommunicationByError[i]))
			{
				m_bIsStopCommunicationByError[i] = true;
				LJX8IF_StopHighSpeedDataCommunication(i);
				LJX8IF_FinalizeHighSpeedDataCommunication(i);
				AfxMessageBox(_T("receive buffer is full."));
			}
		}
	}

	//计算收集轮廓的总数，当达到要求的轮廓数的时候，自动点击停止测量按钮
	if (nIDEvent == 2012)
	{
		UpdateData(TRUE);

		CString receiveCount;
		GetDlgItem(IDC_STATIC_RECEIVE0)->GetWindowText(receiveCount);

		if (receiveCount == m_strSpinProfCount)
		{
			ReceiveEnough = true;
			OnBnClickedBtnStopmeasure();
			OnBnClickedBtnHispeedsaveasbitmapfile();
			ClearMark = true;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

/*
 Update the number of the profiles
 更新接受到的轮廓数
*/
void CSingleFuncDlg::UpDateReceiveCount()
{
	CString strLog[LJX8IF_GUI_DEVICE_COUNT];

	for (int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++)
	{
		strLog[i].Format(_T("%d"), m_anProfReceiveCount[i]);
	}

	this->SetDlgItemTextW(IDC_STATIC_RECEIVE0, (LPCTSTR)strLog[0]);
	this->SetDlgItemTextW(IDC_STATIC_RECEIVE1, (LPCTSTR)strLog[1]);
	this->SetDlgItemTextW(IDC_STATIC_RECEIVE2, (LPCTSTR)strLog[2]);
	this->SetDlgItemTextW(IDC_STATIC_RECEIVE3, (LPCTSTR)strLog[3]);
	this->SetDlgItemTextW(IDC_STATIC_RECEIVE4, (LPCTSTR)strLog[4]);
	this->SetDlgItemTextW(IDC_STATIC_RECEIVE5, (LPCTSTR)strLog[5]);
}

/*
Initialize all ConnectionInfo
初始化所有连接信息
*/
void CSingleFuncDlg::InitializeAllConnectionInfo()
{
	for (int i = 0; i < LJX8IF_GUI_DEVICE_COUNT; i++)
	{
		InitializeConnectionInfo(i);
	}
}

/*
Initialize target ConnectionInfo
 将设置值还原为默认的操作
*/
void CSingleFuncDlg::InitializeConnectionInfo(int targetDeviceID)
{
	m_aDeviceData[targetDeviceID].m_ethernetConfig.abyIpAddress[0] = 192;
	m_aDeviceData[targetDeviceID].m_ethernetConfig.abyIpAddress[1] = 168;
	m_aDeviceData[targetDeviceID].m_ethernetConfig.abyIpAddress[2] = 0;
	m_aDeviceData[targetDeviceID].m_ethernetConfig.abyIpAddress[3] = 1;

	m_aDeviceData[targetDeviceID].m_ethernetConfig.wPortNo = 24691;
}

// 流程化工作按钮点击事件
void CSingleFuncDlg::OnBnClickedButFlowwork()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ClearMark)
		OnBnClickedBtnFinalizehighspeeddatacommunication();

	//m_bIsStartTimer = TRUE;
	//m_bIsUseSimpleArray = TRUE;

	//默认使用相同数组大小按钮为选中
	((CButton *)GetDlgItem(IDC_CHK_USESIMPLEARRAY))->SetCheck(TRUE);
	m_bIsUseSimpleArray = TRUE;
	OnBnClickedChkUsesimplearray();
	//默认选中启动定时器按钮
	((CButton *)GetDlgItem(IDC_CHK_TIMER))->SetCheck(TRUE);
	m_bIsStartTimer = TRUE;
	OnBnClickedChkTimer();

	//控制触发的时间
	m_strSpinStartTimer = "1";
	//控制收集的轮廓数
	m_strSpinProfCount = "1500";
	UpdateData(FALSE);

	//触发时间的具体相应函数
	OnEnKillfocusEditStarttimer();
	//轮廓数的具体相应函数
	OnEnKillfocusEditProfilecount();

	//开启选择保存路径对话框
	OnBnClickedBtnHighSpeedSavedestination();

	//ConfirmProfilecount();

	//初始化DLL库
	OnBnClickedBtnInitialize();

	//初始化以太网连接地址
	OnBnClickedBtnEthernetopen();

	//初始化以太网高速连接（简单数组）
	OnBnClickedBtnInitializehighspeeddatacommunicationSimplearray();

	//准备以太网高速连接
	OnBnClickedBtnPrestarthighspeeddatacommunication();

	//开始以太网高速连接
	OnBnClickedBtnStarthighspeeddatacommunication();

	//开始测量
	OnBnClickedBtnStartmeasure();

	/*CString ProfileNum;
	BOOL SaveMark;
	this->GetDlgItemTextW(IDC_STATIC_RECEIVE0, ProfileNum);

	while (ProfileNum != 1500)
		SaveMark = 0;

	if (ProfileNum == 1500 && SaveMark != 0)
		OnBnClickedBtnStopmeasure();*/

	//定时刷新接受到的轮廓数，到指定后自动停止测量和存储图片
	SetTimer(2012, 1, NULL);

	//清除定时器
	if (ReceiveEnough)
		KillTimer(2012);
	/*while (ReceiveEnough == 1);
		OnBnClickedBtnStopmeasure();*/

	//OnBnClickedBtnHispeedsaveasbitmapfile();

	//OnBnClickedBtnStophighspeeddatacommunication();

	//OnBnClickedBtnFinalizehighspeeddatacommunication();

	ClearMark = 1;
}

// 开启相机
void CSingleFuncDlg::OnBnClickedButOpencam()
{
	// TODO: 在此添加控件通知处理程序代码

	ClearMark = 0;

	//控制触发的时间
	m_strSpinStartTimer = "500";
	//控制收集的轮廓数
	m_strSpinProfCount = "1500";
	UpdateData(FALSE);

	//默认使用相同数组大小按钮为选中
	((CButton *)GetDlgItem(IDC_CHK_USESIMPLEARRAY))->SetCheck(TRUE);
	m_bIsUseSimpleArray = TRUE;
	//函数功能仅使能某些按钮是否可以被点击
	//并无具体功能性操作
	OnBnClickedChkUsesimplearray();
	//默认选中启动定时器按钮
	((CButton *)GetDlgItem(IDC_CHK_TIMER))->SetCheck(TRUE);
	m_bIsStartTimer = TRUE;
	//以输入的触发间隔来获取数据
	OnBnClickedChkTimer();

	//触发时间的具体相应函数
	//作用好像仅仅是 显示你输入的数字到日志框
	OnEnKillfocusEditStarttimer();
	//轮廓数的具体相应函数
	//作用好像仅仅是 显示你输入的数字到日志框
	OnEnKillfocusEditProfilecount();

	//开启选择保存路径对话框
	//作用选择要保存的文件的路径
	OnBnClickedBtnHighSpeedSavedestination();

	//初始化DLL库
	//连接之前必须的操作
	OnBnClickedBtnInitialize();

	//初始化以太网连接地址
	//可以屏蔽弹窗，但是还有些操作没明白意思
	OnBnClickedBtnEthernetopen();

	//初始化以太网高速连接（简单数组）
	//可以屏蔽弹窗，有些操作没明白意思，不懂界面接受轮廓数的更新机制
	//相比上一个以太网连接，多了一个高速以太网连接端口
	OnBnClickedBtnInitializehighspeeddatacommunicationSimplearray();

	//准备以太网高速连接
	//可以屏蔽弹窗，有些操作现在还是不明确有没有用，不知道能不能省略掉
	//相比上一个，只为了获取轮廓数扫描的起始位置
	OnBnClickedBtnPrestarthighspeeddatacommunication();

	//开始以太网高速连接
	//不弹窗 ，开始准备收集
	OnBnClickedBtnStarthighspeeddatacommunication();
}

// 开始采集
void CSingleFuncDlg::OnBnClickedButStartcamMeasure()
{
	// TODO: 在此添加控件通知处理程序代码

	ClearMark = 0;

	//开始测量
	OnBnClickedBtnStartmeasure();

	//定时刷新接受到的轮廓数，到指定后自动停止测量和存储图片
	SetTimer(2012, 50, NULL);

	//清除定时器
	if (ReceiveEnough)
		KillTimer(2012);
}

// 关闭相机
void CSingleFuncDlg::OnBnClickedButClosecam()
{
	// TODO: 在此添加控件通知处理程序代码
	if (ClearMark)
	{
		OnBnClickedBtnStophighspeeddatacommunication();
		OnBnClickedBtnFinalizehighspeeddatacommunication();
	}

	//OnBnClickedBtnStophighspeeddatacommunication();

	//OnBnClickedBtnFinalizehighspeeddatacommunication();
}
