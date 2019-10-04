//Copyright (c) 2019 KEYENCE CORPORATION. All rights reserved.
// CombinationFuncDlg.cpp : Implementation File
//

#include "stdafx.h"
#include "LJX_DllSampleAll.h"
#include "CombinationFuncDlg.h"
#include "LJX8_ErrorCode.h"
#include <vector>
#include "Define.h"
#include "fstream"
#include "DeviceData.h"
#include "DataExport.h"
#include "ThreadSafeBuffer.h"
#include "ProgressDlg.h"

// CCombinationFuncDlg Dialogue

IMPLEMENT_DYNAMIC(CCombinationFuncDlg, CDialog)

const LONG DEVICE_ID = 0;
LJX8IF_PROFILE_INFO CCombinationFuncDlg::m_profileInfo;

CCombinationFuncDlg::CCombinationFuncDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCombinationFuncDlg::IDD, pParent)
	, m_nConnection(0)
	, m_nSelectedProgram(2)
	, m_nCommandPort(24691)
	, m_nHighSpeedPort(24692)
	, m_nReceiveProfileNum(123456789)
	, m_nSendPosition(2)
	, m_nCallbackFrequency(10)
	, m_strDataFilePath(_T(""))
{

}

CCombinationFuncDlg::~CCombinationFuncDlg()
{
}

void CCombinationFuncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_SELECTPROGRAM, m_cmbSelectedProgram);
	DDX_CBIndex(pDX, IDC_CMB_SELECTPROGRAM, m_nSelectedProgram);
	DDX_Text(pDX, IDC_EDIT_COMMANDPORT, m_nCommandPort);
	DDX_Text(pDX, IDC_EDIT_RECEIVEPROFILECOUNT, m_nReceiveProfileNum);
	DDX_Text(pDX, IDC_EDIT_HIGHSPEEDPORT, m_nHighSpeedPort);
	DDX_Text(pDX, IDC_EDIT_STARTPROFILENO, m_nSendPosition);
	DDX_Text(pDX, IDC_EDIT_CALLBACKFREQUENCY, m_nCallbackFrequency);
	DDX_Control(pDX, IDC_IPADDRESSCOMB, m_iacIPAddressComb);

	DDX_Text(pDX, IDC_EDIT_SAVEPATH, m_strDataFilePath);
}


BEGIN_MESSAGE_MAP(CCombinationFuncDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_COMMUNICATIONESTABLISHMENT, &CCombinationFuncDlg::OnBnClickedBtnEstablishcommunication)
	ON_BN_CLICKED(IDC_BTN_COMMUNICATIONFINALIZATION, &CCombinationFuncDlg::OnBnClickedBtnFinalizecommunication)
	ON_BN_CLICKED(IDC_BTN_UPLOADPROGRAM, &CCombinationFuncDlg::OnBnClickedBtnuploadprogram)
	ON_BN_CLICKED(IDC_BTN_DOWNLOADPROGRAM, &CCombinationFuncDlg::OnBnClickedBtndownloadprogram)
	ON_BN_CLICKED(IDC_BTN_REFERENCESAVEPATH, &CCombinationFuncDlg::OnBnClickedBtnreferencesavepath)
	ON_BN_CLICKED(IDC_BTN_GETPROFILEDATA, &CCombinationFuncDlg::OnBnClickedBtnGetprofiledata)
	ON_BN_CLICKED(IDC_BTN_GETBATCHPROFILEDATA, &CCombinationFuncDlg::OnBnClickedBtnGetbatchprofiledata)
	ON_BN_CLICKED(IDC_BTN_BEGINHIGHSPEEDDATACOMMUNICATION, &CCombinationFuncDlg::OnBnClickedBtnBeginhighspeeddatacommunication)
	ON_BN_CLICKED(IDC_BTN_TERMINATEHIGHSPEEDCOMMUNICATION, &CCombinationFuncDlg::OnBnClickedBtnTerminatehighspeedcommunication)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CCombinationFuncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_iacIPAddressComb.SetAddress(192, 168, 0, 1);
	
	UpdateData(FALSE);

	CWnd* pWndCommandPort = GetDlgItem(IDC_EDIT_COMMANDPORT);
	CWnd* pWndHighSpeedPort = GetDlgItem(IDC_EDIT_HIGHSPEEDPORT);
	CWnd* pWndSendPosition = GetDlgItem(IDC_EDIT_STARTPROFILENO);
	CWnd* pWndCallbackFrequency = GetDlgItem(IDC_EDIT_CALLBACKFREQUENCY);
	CEdit *editCommandPort = static_cast<CEdit*>(pWndCommandPort);
	CEdit *editHighSpeedPort = static_cast<CEdit*>(pWndHighSpeedPort);
	CEdit *editSendPosition = static_cast<CEdit*>(pWndSendPosition);
	CEdit *editCallbackFrequency = static_cast<CEdit*>(pWndCallbackFrequency);
	editCommandPort->SetLimitText(5);
	editHighSpeedPort->SetLimitText(5);
	editSendPosition->SetLimitText(3);
	editCallbackFrequency->SetLimitText(10);

	return TRUE;
}

// CCombinationFuncDlg Message handler

/*
 Check return code
 ����Ƿ�����ȷ�ķ���ֵ
 @param Return code
 @return Is return code OK?(True:OK)
*/
BOOL CCombinationFuncDlg::CheckReturnCode(LONG lRc)
{
	if (lRc == LJX8IF_RC_OK) return true;
	
	CString strMessage;
	strMessage.Format(_T("Error : 0x%08X"), lRc);
	AfxMessageBox(strMessage);

	return false;
}

/*
 Get Selected Program Target Setting
 @return LJX8IF_TARGET_SETTING
 ��ȡѡ���ĳ���Ŀ������
*/
LJX8IF_TARGET_SETTING CCombinationFuncDlg::GetSelectedProgramTargetSetting()
{
	//Set target parameter by Combobox selection.
	UpdateData(TRUE);

	LJX8IF_TARGET_SETTING target;
	//Set target as "all parameter"
	target.byCategory = 0xFF;
	target.byItem     = 0x00;
	target.byTarget1  = 0;
	target.byTarget2  = 0;
	target.byTarget3  = 0;
	target.byTarget4  = 0;

	if(m_nSelectedProgram == 0)
	{
		target.byType = LJX8IFEX_CONFIG_TYPE_SYS;
	}
	else if(m_nSelectedProgram == 1)
	{
		target.byType = LJX8IFEX_CONFIG_TYPE_MEASURE_COMMON;

	}
	else
	{
		target.byType = LJX8IFEX_CONFIG_TYPE_PROGRAM_00 + (m_nSelectedProgram - 2);
	}

	return target;
}

/*
 Get Selected Program Data Size
 ��ȡѡ���ĳ������ݴ�С
 @return data size
*/
DWORD CCombinationFuncDlg::GetSelectedProgramDataSize()
{
	if (m_nSelectedProgram == 0) return LJX8IFEX_CONFIG_SIZE_SYSTEM;
	if (m_nSelectedProgram == 1) return LJX8IFEX_CONFIG_SIZE_COMMON;
	return LJX8IFEX_CONFIG_SIZE_PROGRAM;
}

/*
 �ص����������������ļ����ݣ�
 @param�����ļ����ݵ�ָ��
 @paramһ�������ļ����ݵĴ�С
 @param�����ļ�������
 @param֪ͨ
 @param UserID
*/
void CCombinationFuncDlg::ReceiveHighSpeedData(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser)
{
	vector<PROFILE_DATA> vecProfileData;

	int nProfileDataCount  = (dwSize - sizeof(LJX8IF_PROFILE_HEADER) - sizeof(LJX8IF_PROFILE_FOOTER)) / sizeof(DWORD);

	for(DWORD i = 0; i < dwCount; i++)
	{
		BYTE *pbyBlock = pBuffer + dwSize * i;

		LJX8IF_PROFILE_HEADER* pHeader = (LJX8IF_PROFILE_HEADER*)pbyBlock;
		int* pnProfileData             = (int*)(pbyBlock + sizeof(LJX8IF_PROFILE_HEADER));
		LJX8IF_PROFILE_FOOTER* pFooter = (LJX8IF_PROFILE_FOOTER*)(pbyBlock + dwSize - sizeof(LJX8IF_PROFILE_FOOTER));

		vecProfileData.push_back(PROFILE_DATA(m_profileInfo, pHeader, pnProfileData, pFooter));
	}
	CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
		
	threadSafeBuf->Add(dwUser, vecProfileData, dwNotify);
}

/*
 "Communication establishment" button clicked
  ͨ����·������ť
*/
void CCombinationFuncDlg::OnBnClickedBtnEstablishcommunication()
{
	UpdateData(TRUE);

	// ��ʼ�����ӵ�DLL
	LONG lRc = LJX8IF_Initialize();
	if(!CheckReturnCode(lRc)) return;

	//��ͨѶ·��
	//������̫��ͨ�ŵ�����
	LJX8IF_ETHERNET_CONFIG config;
	m_iacIPAddressComb.GetAddress(config.abyIpAddress[0], config.abyIpAddress[1], config.abyIpAddress[2], config.abyIpAddress[3]);
	config.wPortNo = m_nCommandPort;
	config.reserve[0] = 0;
	config.reserve[1] = 0;
	
	//��̫����·��
	lRc = LJX8IF_EthernetOpen(DEVICE_ID, &config);

	CheckReturnCode(lRc);
}

/*
 "Communication finalization" button clicked
 ͨ�����
*/
void CCombinationFuncDlg::OnBnClickedBtnFinalizecommunication()
{
	UpdateData(TRUE);

	// Close the communication
	LONG lRc = LJX8IF_CommunicationClose(DEVICE_ID);
	if(!CheckReturnCode(lRc)) return;

	// Finalize the DLL
	lRc = LJX8IF_Finalize();
	CheckReturnCode(lRc);
}

/*
 Browse ("...") button clicked
 ��ѡ��·��
*/
void CCombinationFuncDlg::OnBnClickedBtnreferencesavepath()
{
	UpdateData(TRUE);

	CFileDialog dlg(FALSE, _T("csv"), m_strDataFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Profile (*.csv)|*.csv|"));

	if(dlg.DoModal() != IDOK) return;

	m_strDataFilePath = dlg.GetPathName();
	UpdateData(FALSE);
}

/*
 "Get profiles" button clicked
  ��ȡ������ť����¼�
*/
void CCombinationFuncDlg::OnBnClickedBtnGetprofiledata()
{
	//����CSV��ʽ����˳��
	/*
	1. ��������LJX8IF_GET_PROFILE_RESPONSEӦ��ʽ
	2. ����LJX8IF_GetProfile������ȡ����
	3. ������һ���Ľ����������ѹջ����
	4. CDataExport::ExportProfileEx�ڶ��庯���������ݵ�ѡ���ĵ�ַ��CSV�ļ���
	*/
	UpdateData(TRUE);
	
	CProgressDlg* progressDlg = new CProgressDlg();
	progressDlg->Create(IDD_PROGRESS_DLG, this);
	progressDlg->SetStatus(PROFILE_PROCESSING_STATUS_COMMUNICATING);	//�����õĲ�����ʾ����־����

	LJX8IF_GET_PROFILE_REQUEST request;
	request.byTargetBank = (BYTE)PROFILEBANK_ACTIVE;
	request.byPositionMode = (BYTE)PROFILEPOS_CURRENT;
	request.dwGetProfileNo  = 0;
	request.byGetProfileCount = 10;
	request.byErase      = 0;
	
	LJX8IF_GET_PROFILE_RESPONSE response;
	LJX8IF_PROFILE_INFO profileInfo;
	DWORD dwOneDataSize = GetMaxProfileDataSize();
	DWORD dwAllDataSize = GetAllDataSize(dwOneDataSize, request.byGetProfileCount);
	std::vector<int> vecProfileData(dwAllDataSize);

	// Get profiles.
	// ��ȡ��������
	LONG lRc = LJX8IF_GetProfile(DEVICE_ID, &request, &response, &profileInfo, (DWORD*)&vecProfileData.at(0), dwAllDataSize * sizeof(int));

	if(!CheckReturnCode(lRc))
	{
		progressDlg->DestroyWindow();
		delete progressDlg;
		progressDlg = NULL;
		return;
	}
	
	// Output the data of each profile
	// ���ÿ������������
	int nMultipleValue = GetIsLuminanceOutput(profileInfo) ? MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT : 1;
	int nDataUnitSize = (sizeof(LJX8IF_PROFILE_HEADER) + sizeof(int) * profileInfo.wProfileDataCount * profileInfo.byProfileCount  * nMultipleValue + sizeof(LJX8IF_PROFILE_FOOTER)) / sizeof(int);
	std::vector<PROFILE_DATA> vecReceiveProfileData;

	for(int i = 0; i < response.byGetProfileCount; i++)
	{
		int *pnBlock = &vecProfileData.at(nDataUnitSize * i);
			
		LJX8IF_PROFILE_HEADER* pHeader = (LJX8IF_PROFILE_HEADER*)pnBlock;
		int* pnProfileData			   = pnBlock + (sizeof(LJX8IF_PROFILE_HEADER) / sizeof(DWORD));
		LJX8IF_PROFILE_FOOTER* pFooter = (LJX8IF_PROFILE_FOOTER*)(pnProfileData + profileInfo.wProfileDataCount * profileInfo.byProfileCount);

		vecReceiveProfileData.push_back(PROFILE_DATA(profileInfo, pHeader, pnProfileData, pFooter));
	}

	progressDlg->SetStatus(PROFILE_PROCESSING_STATUS_SAVING);

	// Save the file
	// �����ļ�����ʽΪCSV��ʽ
	if (response.byGetProfileCount != 0) {
		CDataExport::ExportProfileEx(&(vecReceiveProfileData.at(0)), m_strDataFilePath, response.byGetProfileCount);
	}

	progressDlg->DestroyWindow();
	delete progressDlg;
	progressDlg = NULL;
}

/*
 "Get batch profiles" button clicked
  ��ȡ����������
*/
void CCombinationFuncDlg::OnBnClickedBtnGetbatchprofiledata()
{
	/*
	�����ǰ�淽����ͬ����ѭ�������յ����ź�
	*/

	UpdateData(TRUE);

	CProgressDlg* progressDlg = new CProgressDlg();
	progressDlg->Create(IDD_PROGRESS_DLG, this);
	progressDlg->SetStatus(PROFILE_PROCESSING_STATUS_COMMUNICATING);

	LJX8IF_GET_BATCH_PROFILE_REQUEST request;
	request.byTargetBank = (BYTE)PROFILEBANK_ACTIVE;
	request.byPositionMode = (BYTE)BATCHPOS_COMMITED;
	request.dwGetBatchNo = 0;
	request.dwGetProfileNo  = 0;
	request.byGetProfileCount = MAXBYTE;
	request.byErase      = 0;
		
	LJX8IF_GET_BATCH_PROFILE_RESPONSE response;
	LJX8IF_PROFILE_INFO profileInfo;
	DWORD dwOneDataSize = GetMaxProfileDataSize();
	DWORD dwAllDataSize = GetAllDataSize(dwOneDataSize, request.byGetProfileCount);
	std::vector<int> vecProfileData(dwAllDataSize);
	
	// Get profiles
	LONG lRc = LJX8IF_GetBatchProfile(DEVICE_ID, &request, &response, &profileInfo, (DWORD*)&vecProfileData.at(0), dwAllDataSize * sizeof(int));
	if (!CheckReturnCode(lRc))
	{
		progressDlg->DestroyWindow();
		delete progressDlg;
		progressDlg = NULL;
		return;
	}

	// @Point
		//���ӵ������ζ�ȡ���������ļ�ʱ�������޷���ȡָ�������������ļ���
		//��Ҫ�ڶ�ȡ��һ�������ļ����ȡ���������ļ����뽫ָ��������byPositionMode������Ϊ0x02��
		//ָ�����α�ţ�dwGetBatchNo����Ȼ�����ñ���Կ�ʼ�ӣ�dwGetProfileNo����
		//Ҫ��ȡ�ĸ�Ҫ�ļ�������byGetProfileCount������ֵָ������δ��ȡ�԰�˳���ȡ��Ҫ�ļ��ĸ�Ҫ�ļ���Χ��
		//������ϸ��˵���˹�����Ҫ��
		// *��������������ʾ������Ȼ���ٴε��ô˺�����
		// byPositionMode = LJX8IF_BATCH_POSITION_SPEC
		// dwGetBatchNo =�Ѷ�ȡ������
		// byGetProfileCount =������δ���������ļ����
		// *���⣬���δ��ȡ�������е����������ļ�������¶�ȡ�����ļ�����ʼλ�ã�request.dwGetProfileNo����Ȼ��
		//Ҫ��ȡ�������ļ�����request.byGetProfileCount����Ȼ���ٴε���LJX8IF_GetBatchProfile�� ���ظ��˹��̣�ֱ���Ѷ�ȡ���������ļ�����

		//���ÿ�������ļ�������
	int nMultipleValue = GetIsLuminanceOutput(profileInfo) ? MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT : 1;
	int nDataUnitSize = (sizeof(LJX8IF_PROFILE_HEADER) + sizeof(int) * profileInfo.wProfileDataCount * profileInfo.byProfileCount * nMultipleValue + sizeof(LJX8IF_PROFILE_FOOTER)) / sizeof(int);
	std::vector<PROFILE_DATA> vecReceiveProfileData;
	for(int i = 0; i < response.byGetProfileCount; i++)
	{
		int *pnBlock = &vecProfileData.at(nDataUnitSize * i);
			
		LJX8IF_PROFILE_HEADER* pHeader = (LJX8IF_PROFILE_HEADER*)pnBlock;
		int* pnProfileData             = pnBlock + (sizeof(LJX8IF_PROFILE_HEADER) / sizeof(DWORD));
		LJX8IF_PROFILE_FOOTER* pFooter = (LJX8IF_PROFILE_FOOTER*)(pnProfileData + profileInfo.wProfileDataCount * profileInfo.byProfileCount );

		vecReceiveProfileData.push_back(PROFILE_DATA(profileInfo, pHeader, pnProfileData, pFooter));
	}

	// Get all profiles within the batch.
	// ��ȡ�������е����������ļ�
	request.byPositionMode = (BYTE)BATCHPOS_SPEC;
	request.dwGetBatchNo = response.dwGetBatchNo;

	while(response.dwGetBatchProfileCount != (response.dwGetBatchTopProfileNo + response.byGetProfileCount))
	{
		// Update the get profile position
		request.dwGetProfileNo = response.dwGetBatchTopProfileNo + response.byGetProfileCount;
		request.byGetProfileCount = (BYTE)min((UINT)(MAXBYTE), (response.dwCurrentBatchProfileCount - request.dwGetProfileNo));

		lRc = LJX8IF_GetBatchProfile(DEVICE_ID, &request, &response, &profileInfo, (DWORD*)&vecProfileData.at(0), dwAllDataSize * sizeof(int));
		if (!CheckReturnCode(lRc))
		{
			progressDlg->DestroyWindow();
			delete progressDlg;
			progressDlg = NULL;
			return;
		}

		for(int i = 0; i < response.byGetProfileCount; i++)
		{
			int *pnBlock = &vecProfileData.at(nDataUnitSize * i);
			
			LJX8IF_PROFILE_HEADER* pHeader = (LJX8IF_PROFILE_HEADER*)pnBlock;
			int* pnProfileData             = pnBlock + (sizeof(LJX8IF_PROFILE_HEADER) / sizeof(DWORD));
			LJX8IF_PROFILE_FOOTER* pFooter = (LJX8IF_PROFILE_FOOTER*)(pnProfileData + profileInfo.wProfileDataCount * profileInfo.byProfileCount);

			vecReceiveProfileData.push_back(PROFILE_DATA(profileInfo, pHeader, pnProfileData, pFooter));
		}
	}

	progressDlg->SetStatus(PROFILE_PROCESSING_STATUS_SAVING);

	// Save the file
	if (response.dwGetBatchProfileCount != 0)
	{
		CDataExport::ExportProfileEx(&(vecReceiveProfileData.at(0)), m_strDataFilePath, response.dwGetBatchProfileCount);
	}

	progressDlg->DestroyWindow();
	delete progressDlg;
	progressDlg = NULL;
}

/*
 "Sending settings (PC -> LJ)" button clicked
*/
void CCombinationFuncDlg::OnBnClickedBtnuploadprogram()
{
	UpdateData(TRUE);
	CFileDialog dialog(TRUE);
	if (dialog.DoModal() != IDOK) return;

	// �趨����
	DWORD dwSizeData = GetSelectedProgramDataSize();
	
	// �����ڴ�
	std::vector<BYTE> data;
	data.resize(dwSizeData);

	// ���س�������
	CStringA strPathAnsi(dialog.GetPathName());
	std::ifstream infile(strPathAnsi, std::ios_base::in|std::ios_base::binary);	

	// ��֤�ļ� 
	infile.seekg(0, std::ios::end);
	size_t sizeFile = (size_t)infile.tellg();
	if(sizeFile != dwSizeData){
		AfxMessageBox(_T("File size is not match."));
		ShowNgMessage();
		return;
	}
	infile.seekg(0, std::ios::beg);
	infile.read((char*)&data.at(0), dwSizeData); 

	// ����
	DWORD dwError;
	// ������������
	LJX8IF_TARGET_SETTING target = GetSelectedProgramTargetSetting();

	LONG lRc = LJX8IF_SetSetting(DEVICE_ID, LJX8IF_SETTING_DEPTH_RUNNING, target, &data.at(0), dwSizeData, &dwError);
	ShowProcessingResultMessage(lRc);
}

/*
 "Receiving settings (LJ -> PC)" button clicked.
*/
void CCombinationFuncDlg::OnBnClickedBtndownloadprogram()
{
	UpdateData(TRUE);
	CFileDialog dialog(FALSE);
	if (dialog.DoModal() != IDOK) return;


	DWORD dwSizeData = GetSelectedProgramDataSize(); // Environment:60, Common:20, Program number:528
	// Allocate buffer
	std::vector<BYTE> data;
	data.resize(dwSizeData);
	
	// ��ȡ����
	LJX8IF_TARGET_SETTING target = GetSelectedProgramTargetSetting();
	// ����
	LONG lRc = LJX8IF_GetSetting(DEVICE_ID, LJX8IF_SETTING_DEPTH_RUNNING, target, &data.at(0), dwSizeData);
	if (lRc != LJX8IF_RC_OK)
	{
		ShowNgMessage();
		return;
	}

	// �洢�������������
	CStringA strPathAnsi(dialog.GetPathName());
	std::ofstream outfile(strPathAnsi, std::ios_base::out|std::ios_base::binary);
	outfile.write((char*)&data.at(0), dwSizeData); 
	ShowSuccessMessage();
}

/*
 High-speed data communication "Start" button clicked
*/
void CCombinationFuncDlg::OnBnClickedBtnBeginhighspeeddatacommunication()
{
	UpdateData(TRUE);
	KillTimer(TIMER_EX_ID);

	// Stop and finalize high-speed data communication.
	LONG lRc = LJX8IF_StopHighSpeedDataCommunication(DEVICE_ID);
	if (!CheckReturnCode(lRc)) return;
	lRc = LJX8IF_FinalizeHighSpeedDataCommunication(DEVICE_ID);
	if (!CheckReturnCode(lRc)) return;

	CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
	threadSafeBuf->ClearBuffer(DEVICE_ID);

	// Generate the settings for Ethernet communication.
	LJX8IF_ETHERNET_CONFIG config;
	m_iacIPAddressComb.GetAddress(config.abyIpAddress[0], config.abyIpAddress[1], config.abyIpAddress[2], config.abyIpAddress[3]);
	config.wPortNo = m_nCommandPort;
	config.reserve[0] = 0;
	config.reserve[1] = 0;
	
	DWORD dwProfileCount = m_nCallbackFrequency;
	DWORD dwThreadId = (DWORD)DEVICE_ID;

	// ��ʼ��������̫������ͨ��
	lRc = LJX8IF_InitializeHighSpeedDataCommunication(DEVICE_ID, &config, m_nHighSpeedPort, ReceiveHighSpeedData, dwProfileCount, dwThreadId);
	if(!CheckReturnCode(lRc)) return;

	// ��������ͨ�ſ�ʼ׼��
	LJX8IF_HIGH_SPEED_PRE_START_REQ request;
	request.bySendPosition = m_nSendPosition;
	lRc = LJX8IF_PreStartHighSpeedDataCommunication(DEVICE_ID, &request, &m_profileInfo);
	if(!CheckReturnCode(lRc)) return;

	// ����������̫��ͨ��
	lRc = LJX8IF_StartHighSpeedDataCommunication(DEVICE_ID);
	if (!CheckReturnCode(lRc)) return;
	
	m_nReceiveProfileNum = 0;
	SetTimer(TIMER_EX_ID, TIMER_ELASP, NULL);
}

/*
 High-speed data communication "Finalize" button clicked
*/
void CCombinationFuncDlg::OnBnClickedBtnTerminatehighspeedcommunication()
{
	// Stop high-speed data communication.
	LONG lRc = LJX8IF_StopHighSpeedDataCommunication(DEVICE_ID);
	if (!CheckReturnCode(lRc)) return;

	// Finalize high-speed data communication.
	lRc = LJX8IF_FinalizeHighSpeedDataCommunication(DEVICE_ID);
	CheckReturnCode(lRc);
}

/*
 Timer Control
*/
void CCombinationFuncDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_EX_ID)
	{
		int nBatchNo   = 0;
		DWORD dwNotify = 0;

		CThreadSafeBuffer* threadSafeBuf = CThreadSafeBuffer::getInstance();
		vector<PROFILE_DATA> vecProfileData;
		threadSafeBuf->Get((DWORD)DEVICE_ID, &dwNotify, &nBatchNo, vecProfileData);

		DWORD dwCount = (DWORD)vecProfileData.size();
		m_nReceiveProfileNum += dwCount;

		if((dwNotify & 0xFFFF) != 0)
		{
			// If the lower 16 bits of the notification are not 0, high-speed communication was interrupted, so stop the timer.
			KillTimer(TIMER_EX_ID);
			CString strMessage;
			strMessage.Format(_T("Finalize communication :Notify = 0x%08X"), dwNotify);
			AfxMessageBox(strMessage);
		}

		if((dwNotify & 0x1000) != 0)
		{
			// A descriptor is included here if processing when batch measurement is finished is required.
		}

		UpdateData(FALSE);
	}

	CDialog::OnTimer(nIDEvent);
}

/*
 Get is luminance output
 ��ȡ��������ֵ���
*/
BOOL CCombinationFuncDlg::GetIsLuminanceOutput(LJX8IF_PROFILE_INFO profileInfo)
{
	return profileInfo.byLuminanceOutput == LUMINANCE_OUTPUT_ON_VALUE;
}

/*
Show NG message
*/
void CCombinationFuncDlg::ShowNgMessage()
{
	AfxMessageBox(_T("NG!!"));
}

/*
Show success message
*/
void CCombinationFuncDlg::ShowSuccessMessage()
{
	AfxMessageBox(_T("Success!!"));
}

/*
Show processing result message
*/
void CCombinationFuncDlg::ShowProcessingResultMessage(LONG lRc)
{
	if (lRc == LJX8IF_RC_OK)
	{
		ShowSuccessMessage();
	}
	else
	{
		ShowNgMessage();
	}
}

/*
Get max profile data size
*/
DWORD CCombinationFuncDlg::GetMaxProfileDataSize()
{
	return (MAX_PROFILE_COUNT * MULTIPLE_VALUE_FOR_LUMINANCE_OUTPUT) + (sizeof(LJX8IF_PROFILE_HEADER) + sizeof(LJX8IF_PROFILE_FOOTER)) / sizeof(DWORD);
}

/*
Get whole data data size
*/
DWORD CCombinationFuncDlg::GetAllDataSize(DWORD dwOneDataSize, BYTE byDataCount)
{
	return dwOneDataSize * byDataCount;
}
