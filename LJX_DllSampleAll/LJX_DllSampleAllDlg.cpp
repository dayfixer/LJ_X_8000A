//Copyright (c) 2019 KEYENCE CORPORATION. All rights reserved.
// LJX_DllSampleAllDlg.cpp : Implementation File
//

#include "stdafx.h"
#include "LJX_DllSampleAll.h"
#include "LJX_DllSampleAllDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogue Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLJX_DllSampleAllDlg Dialogue


CLJX_DllSampleAllDlg::CLJX_DllSampleAllDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLJX_DllSampleAllDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLJX_DllSampleAllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_xcTab);
}

BEGIN_MESSAGE_MAP(CLJX_DllSampleAllDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CLJX_DllSampleAllDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()



BOOL CLJX_DllSampleAllDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	{
		CRect rectLabel;
		GetDlgItem(IDC_ADJUST_LABEL)->GetWindowRect(rectLabel);
		ScreenToClient(rectLabel);

		// Creat the Dialog
		m_singleFuncDlg.Create(CSingleFuncDlg::IDD, this);
		m_combinationFuncDlg.Create(CCombinationFuncDlg::IDD, this);

		// Movement of child Dialogue
		m_singleFuncDlg.MoveWindow(&rectLabel, FALSE);
		m_combinationFuncDlg.MoveWindow(&rectLabel, FALSE);

		// Tab making
		//m_xcTab.InsertItem(0, _T("[DLL functions] Simple function sample"));
		//m_xcTab.InsertItem(1, _T("[DLL functions] Combination function sample"));

		m_xcTab.InsertItem(0, _T("[DLL函数]简单函数示例"));
		m_xcTab.InsertItem(1, _T("[DLL函数]组合函数示例"));

		OnTcnSelchangeTab(0,0);
	}

	return TRUE;
}

void CLJX_DllSampleAllDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CLJX_DllSampleAllDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CLJX_DllSampleAllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLJX_DllSampleAllDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nCurrentTab = m_xcTab.GetCurSel();

	m_singleFuncDlg.ShowWindow(nCurrentTab == 0 ? SW_SHOW : SW_HIDE);
	m_combinationFuncDlg.ShowWindow(nCurrentTab == 1 ? SW_SHOW : SW_HIDE);

	if (pResult) *pResult = 0;
}
