//Copyright (c) 2019 KEYENCE CORPORATION. All rights reserved.
/** @file
@brief	LJX8_IF Header
*/

#pragma once
#pragma managed(push, off)

#ifdef LJX8_IF_EXPORT
#define LJX8_IF_API __declspec(dllexport)
#else
#define LJX8_IF_API __declspec(dllimport)
#endif

/// Setting value storage level designation
typedef enum {
	LJX8IF_SETTING_DEPTH_WRITE		= 0x00,		// ������д���� Write settings area
	LJX8IF_SETTING_DEPTH_RUNNING	= 0x01,		// �������趨���� Running settings area
	LJX8IF_SETTING_DEPTH_SAVE		= 0x02,		// �������� Save area
} LJX8IF_SETTING_DEPTH;

/// Initialization target setting item designation
typedef enum {
	LJX8IF_INIT_SETTING_TARGET_PRG0		= 0x00,		// Program 0
	LJX8IF_INIT_SETTING_TARGET_PRG1		= 0x01,		// Program 1
	LJX8IF_INIT_SETTING_TARGET_PRG2		= 0x02,		// Program 2
	LJX8IF_INIT_SETTING_TARGET_PRG3		= 0x03,		// Program 3
	LJX8IF_INIT_SETTING_TARGET_PRG4		= 0x04,		// Program 4
	LJX8IF_INIT_SETTING_TARGET_PRG5		= 0x05,		// Program 5
	LJX8IF_INIT_SETTING_TARGET_PRG6		= 0x06,		// Program 6
	LJX8IF_INIT_SETTING_TARGET_PRG7		= 0x07,		// Program 7
	LJX8IF_INIT_SETTING_TARGET_PRG8		= 0x08,		// Program 8
	LJX8IF_INIT_SETTING_TARGET_PRG9		= 0x09,		// Program 9
	LJX8IF_INIT_SETTING_TARGET_PRG10	= 0x0A,		// Program 10
	LJX8IF_INIT_SETTING_TARGET_PRG11	= 0x0B,		// Program 11
	LJX8IF_INIT_SETTING_TARGET_PRG12	= 0x0C,		// Program 12
	LJX8IF_INIT_SETTING_TARGET_PRG13	= 0x0D,		// Program 13
	LJX8IF_INIT_SETTING_TARGET_PRG14	= 0x0E,		// Program 14
	LJX8IF_INIT_SETTING_TARGET_PRG15	= 0x0F,		// Program 15
} LJX8IF_INIT_SETTING_TARGET;

/// Get profile target buffer designation
typedef enum {
	LJX8IF_PROFILE_BANK_ACTIVE		= 0x00,		// ������� Active surface
	LJX8IF_PROFILE_BANK_INACTIVE	= 0x01,		// �ǻ���� Inactive surface
} LJX8IF_PROFILE_BANK;

/// Get profile position specification method designation (batch measurement: off)
typedef enum {
	LJX8IF_PROFILE_POSITION_CURRENT	= 0x00,		// From current
	LJX8IF_PROFILE_POSITION_OLDEST	= 0x01,		// From oldest
	LJX8IF_PROFILE_POSITION_SPEC	= 0x02,		// Specify position
} LJX8IF_PROFILE_POSITION;

/// Get profile batch data position specification method designation (batch measurement: on)
typedef enum {
	LJX8IF_BATCH_POSITION_CURRENT		= 0x00,		// From current
	LJX8IF_BATCH_POSITION_SPEC			= 0x02,		// Specify position
	LJX8IF_BATCH_POSITION_COMMITED		= 0x03,		// From current after batch commitment
	LJX8IF_BATCH_POSITION_CURRENT_ONLY	= 0x04,		// Current only
} LJX8IF_BATCH_POSITION;

/// Version info structure
typedef struct {
	INT	nMajorNumber;		// ���� Major number
	INT	nMinorNumber;		// ���� Minor number
	INT	nRevisionNumber;	// �޶��� Revision number
	INT	nBuildNumber;		// �ڲ��޶��� Buiid number
} LJX8IF_VERSION_INFO;

/// Ethernet settings structure
typedef struct {
	BYTE	abyIpAddress[4];	// The IP address of the controller to connect to.
	WORD	wPortNo;			// The port number of the controller to connect to.
	BYTE	reserve[2];			// Reserved
} LJX8IF_ETHERNET_CONFIG;

/// Setting item designation structure
typedef struct {
	BYTE	byType;			// Setting type
	BYTE	byCategory;		// Category
	BYTE	byItem;			// Setting item
	BYTE	reserve;		// Reserved
	BYTE	byTarget1;		// Setting Target 1
	BYTE	byTarget2;		// Setting Target 2
	BYTE	byTarget3;		// Setting Target 3
	BYTE	byTarget4;		// Setting Target 4
} LJX8IF_TARGET_SETTING;

/// Profile information
typedef struct {
	BYTE	byProfileCount;		// �洢���������� The amount of profile data stored
	BYTE	reserve1;			// Reserved
	BYTE	byLuminanceOutput;	// ��������Ƿ�� Whether luminance output is on.
	BYTE	reserve2;			// Reserved
	WORD	wProfileDataCount;	// ������ Profile data count
	BYTE	reserve3[2];		// Reserved
	LONG	lXStart;			// ��һ�����X���� 1st point X coordinate.
	LONG	lXPitch;			// ��������X������ Profile data X direction interval.
} LJX8IF_PROFILE_INFO;

/// Profile header information structure
typedef struct {
	DWORD	reserve;		// Reserved
	DWORD	dwTriggerCount;	// The trigger count when the trigger was issued.
	LONG	lEncoderCount;	// The encoder count when the trigger was issued.
	DWORD	reserve2[3];	// Reserved
} LJX8IF_PROFILE_HEADER;

/// Profile footer information structure
typedef struct {
	DWORD	reserve;	// Reserved
} LJX8IF_PROFILE_FOOTER;

/// Get profile request structure (batch measurement: off)
typedef struct {
	BYTE	byTargetBank;		// ��ȡĿ����� The target surface to read.
	BYTE	byPositionMode;		// ��ȡ����λ��ָ������ The get profile position specification method.
	BYTE	reserve[2];			// Ԥ��λ�� Reserved
	DWORD	dwGetProfileNo;		// �������н�Ҫ��õ������� The profile number for the profile to get.
	BYTE	byGetProfileCount;	// Ҫ��ȡ���������� The number of profiles to read.
	BYTE	byErase;			// �����ӿ�������ȡ������ Erase the profiles that were read from the controller.
	BYTE	reserve2[2];		// Reserved
} LJX8IF_GET_PROFILE_REQUEST;

/// Get profile request structure (batch measurement: on)
typedef struct {
	BYTE	byTargetBank;		// The target surface to read.
	BYTE	byPositionMode;		// The get profile position specification method
	BYTE	reserve[2];			// Reserved
	DWORD	dwGetBatchNo;		// The batch number for the profile to get
	DWORD	dwGetProfileNo;		// The profile number to start getting profiles from in the specified batch number.
	BYTE	byGetProfileCount;	// The number of profiles to read.
	BYTE	byErase;			// Erase the profiles that were read from the controller.
	BYTE	reserve2[2];		// Reserved
} LJX8IF_GET_BATCH_PROFILE_REQUEST;

/// Get profile response structure (batch measurement: off)
typedef struct {
	DWORD	dwCurrentProfileNo;		// ��ǰʱ���ĸ����������.
	DWORD	dwOldestProfileNo;		// �������������������ļ��������ļ����.
	DWORD	dwGetTopProfileNo;		// ��ζ�ȡ����������������ϵ����ϱ��.
	BYTE	byGetProfileCount;		// ��ζ�ȡ��������.	
	BYTE	reserve[3];				// Ԥ���� Reserved
} LJX8IF_GET_PROFILE_RESPONSE;

/// Get profile response structure (batch measurement: on)
typedef struct {
	DWORD	dwCurrentBatchNo;			// The batch number at the current point in time.
	DWORD	dwCurrentBatchProfileCount;	// The number of profiles in the newest batch.
	DWORD	dwOldestBatchNo;			// The batch number for the oldest batch held by the controller.
	DWORD	dwOldestBatchProfileCount;	// The number of profiles in the oldest batch held by the controller.
	DWORD	dwGetBatchNo;				// The batch number that was read this time.
	DWORD	dwGetBatchProfileCount;		// The number of profiles in the batch that was read this time.
	DWORD	dwGetBatchTopProfileNo;		// The oldest profile number in the batch out of the profiles that were read this time.
	BYTE	byGetProfileCount;			// The number of profiles that were read this time.
	BYTE	byCurrentBatchCommited;		// The batch measurements for the newest batch number has finished.
	BYTE	reserve[2];					// Reserved
} LJX8IF_GET_BATCH_PROFILE_RESPONSE;

/// High-speed communication prep start request structure
typedef struct {
	BYTE	bySendPosition;			// Send start position
	BYTE	reserve[3];				// Reserved
} LJX8IF_HIGH_SPEED_PRE_START_REQ;


/**
Callback function interface for high-speed data communication
@param	pBuffer		A pointer to the buffer that stores the profile data.
@param	dwSize		The size in BYTEs per single unit of the profile.
@param	dwCount		The number of profiles stored in pBuffer.
@param	dwNotify	Notification of an interruption in high-speed communication or a break in batch measurements.
@param	dwUser		User information
*/
typedef void(_cdecl *LJX8IF_CALLBACK)(BYTE* pBuffer, DWORD dwSize, DWORD dwCount, DWORD dwNotify, DWORD dwUser);

/**
Callback function interface for high-speed data communication
@param	pProfileHeaderArray		A pointer to the buffer that stores the header data array.
@param	pHeightProfileArray		A pointer to the buffer that stores the profile data array.
@param	pLuminanceProfileArray		A pointer to the buffer that stores the luminance profile data array.
@param	dwLuminanceEnable		The value indicating whether luminance data output is enable or not.
@param	dwProfileDataCount		The data count of one profile.
@param	dwCount		The number of profile or header data stored in buffer.
@param	dwNotify	Notification of an interruption in high-speed communication or a break in batch measurements.
@param	dwUser		User information
*/
typedef void(_cdecl *LJX8IF_CALLBACK_SIMPLE_ARRAY)(LJX8IF_PROFILE_HEADER* pProfileHeaderArray, WORD* pHeightProfileArray, WORD* pLuminanceProfileArray, DWORD dwLuminanceEnable, DWORD dwProfileDataCount, DWORD dwCount, DWORD dwNotify, DWORD dwUser);



extern "C"
{
	// Functions
	// Operations for the DLL
	/**
	Initializes the DLL
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_Initialize(void);

	/**
	Finalize DLL
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_Finalize(void);

	/**
	Get DLL version
	@return	DLL version
	*/
	LJX8_IF_API LJX8IF_VERSION_INFO WINAPI LJX8IF_GetVersion(void);

	/**
	��̫��ͨѶ����
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@param pEthernetConfig��̫��ͨѶ����
	@return������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_EthernetOpen(LONG lDeviceId, LJX8IF_ETHERNET_CONFIG* pEthernetConfig);
	
	/**
	Disconnect communication path
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_CommunicationClose(LONG lDeviceId);


	// System control
	/**
	Reboot the controller
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_RebootController(LONG lDeviceId);

	/**
	Return to factory state
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_ReturnToFactorySetting(LONG lDeviceId);

	/**
	Control Laser
	@param	lDeviceId	The communication device to communicate with.
	@param	byState		Laser state
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_ControlLaser(LONG lDeviceId, BYTE byState);

	/**
	��ȡϵͳ������Ϣ
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@param byReceivedMaxҪ���յ����ϵͳ������Ϣ��
	@param pbyErrCount�����������ڽ���ϵͳ������Ϣ����
	@param pwErrCode����ϵͳ������Ϣ�Ļ�������
	@return������
	Get system error information
	@param	lDeviceId		The communication device to communicate with.
	@param	byReceivedMax	The maximum amount of system error information to receive
	@param	pbyErrCount		The buffer to receive the amount of system error information.
	@param	pwErrCode		The buffer to receive the system error information.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetError(LONG lDeviceId, BYTE byReceivedMax, BYTE* pbyErrCount, WORD* pwErrCode);

	/**
	Clear system error
	@param	lDeviceId	The communication device to communicate with.
	@param	wErrCode	The error code for the error you wish to clear.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_ClearError(LONG lDeviceId, WORD wErrCode);

	/**
	���� trg_error �Ĵ�״̬
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_TrgErrorReset(LONG lDeviceId);

	/**
	��ȡ�����ͱ������������
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@param pdwTriggerCount���մ��������Ļ�����
	@param plEncoderCount���ձ�������������Ļ�����
	@return������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetTriggerAndPulseCount(LONG lDeviceId, DWORD* pdwTriggerCount, LONG* plEncoderCount);

	/**
	Get head temperature
	@param	lDeviceId				The communication device to communicate with.
	@param	pnSensorTemperature		The buffer to receive sensor Temperature.
	@param	pnProcessorTemperature	The buffer to receive processor Temperature.
	@param	pnCaseTemperature		The buffer to receive case Temperature.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetHeadTemperature(LONG lDeviceId, SHORT* pnSensorTemperature, SHORT* pnProcessorTemperature, SHORT* pnCaseTemperature);

	/**
	Get serial Number
	@param	lDeviceId			The communication device to communicate with.
	@param	pControllerSerialNo	The buffer to receive serial number of the controller
	@param	pHeadSerialNo		The buffer to receive serial number of the head
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetSerialNumber(LONG lDeviceId, CHAR* pControllerSerialNo, CHAR* pHeadSerialNo);

	/**
	Get current attention status value
	��ȡ trg_error / mem_full / trg_pass ��״̬��
	@param	lDeviceId		The communication device to communicate with.
	@param	pwAttentionStatus	The buffer to receive attention status
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetAttentionStatus(LONG lDeviceId, WORD* pwAttentionStatus);


	// Measurement control
	/**
	Trigger
	0x8080������ģʽ���ǡ��ⲿ������
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_Trigger(LONG lDeviceId);

	/**
	Start batch measurements
	��ʼ��������
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	��ע��
	0x8080��������������Ϊ OFF��
	0x80A0������ LASER_ON ����Ϊ OFF������޷�ִ������������ʼ����
	��ʼ��������������Ѿ���ʼ�����������򲻻�����κδ���Ҳ���ᷢ������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_StartMeasure(LONG lDeviceId);

	/**
	Stop batch measurements
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	��ע��
	0x8080��������������Ϊ OFF��
	0x80A0������ LASER_ON ����Ϊ OFF������޷�ִ������������������
	�������β��������δ��ʼ�����������򲻻�����κδ���Ҳ���ᷢ������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_StopMeasure(LONG lDeviceId);

	/**
	Clear memory
	�ڲ��洢�����ۻ��������ļ����ݽ������
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_ClearMemory(LONG lDeviceId);


	// ���޸Ļ��ȡ�����йصĹ���
	/**
	��������
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@param byDepth��ӳ����ֵ�ļ���
	@param TargetSetting��Ŀ�����Ŀ
	@param pData�洢�������ݵĻ�����
	@param dwDataSize�������ݵ��ֽڴ�С
	@param pdwError��ϸ���ô���
	@return������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_SetSetting(LONG lDeviceId, BYTE byDepth, LJX8IF_TARGET_SETTING TargetSetting, void* pData, DWORD dwDataSize, DWORD* pdwError);

	/**
	Get setting
	@param	lDeviceId		The communication device to communicate with.
	@param	byDepth			The level of the setting value to get.
	@param	TargetSetting	The item that is the target
	@param	pData			The buffer to receive the setting data
	@param	dwDataSize		The size of the buffer to receive the acquired data in BYTEs.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetSetting(LONG lDeviceId, BYTE byDepth, LJX8IF_TARGET_SETTING TargetSetting, void* pData, DWORD dwDataSize);

	/**
	Initialize setting
	@param	lDeviceId	The communication device to communicate with.
	@param	byDepth		The level to reflect the initialized setting.
	@param	byTarget	The setting that is the target for initialization.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_InitializeSetting(LONG lDeviceId, BYTE byDepth, BYTE byTarget);

	/**
	Request to reflect settings in the write settings area
	@param	lDeviceId	The communication device to communicate with.
	@param	byDepth		The level to reflect the setting value
	@param	pdwError	Detailed setting error
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_ReflectSetting(LONG lDeviceId, BYTE byDepth, DWORD* pdwError);

	/**
	Update write settings area
	@param	lDeviceId	The communication device to communicate with.
	@param	byDepth		The level of the settings to update the write settings area with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_RewriteTemporarySetting(LONG lDeviceId, BYTE byDepth);

	/**
	��鱣�浽���������״̬
	@param lDeviceId ��֮ͨ�ŵ�ͨ���豸��
	@param pbyBusy 0���⣺���ʱ�������0���޷���Ȩ�ޡ�
	@return������
	Check the status of saving to the save area
	@param	lDeviceId	The communication device to communicate with.
	@param	pbyBusy		Other than 0: Accessing the save area, 0: no access.
	@return	Return code
	�������Ƿ����ڷ��ʴ洢���Խ�������ֵ�洢����ȡ�
	ȷ����LJX8IF_ReturnToFactorySetting ����/ LJX8IF_SetSetting ����/
	ʹ�� LJX8IF_InitializeSetting ����/ LJX8IF_ReflectSetting ��������ֵ�Ĵ洢����
	���ָʾ�˹��ܱ��棬��ʹ�ô˹�����ɶԱ�������ķ��ʡ� ȷ�Ϻ���رյ�Դ��
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_CheckMemoryAccess(LONG lDeviceId, BYTE* pbyBusy);

	/**
	Change program
	@param	lDeviceId	The communication device to communicate with.
	@param	byProgramNo	Program number after the change.
	@return	Return code
	�л������š�
	��ͨ�� ProgramNo ָ�����������ͬ�ı��ʱ������
	���ָ������Ч�ĳ���ţ�������л��������ڲ�����¼�����ǻ����Ų��䡣
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_ChangeActiveProgram(LONG lDeviceId, BYTE byProgramNo);

	/**
	Get the active program number
	@param	lDeviceId		The communication device to communicate with.
	@param	pbyProgramNo	The buffer to receive the active program number.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetActiveProgram(LONG lDeviceId, BYTE* pbyProgramNo);


	// ��ȡ�������
	/**
	Get profiles
	��ȡ����
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@param pReqҪ��ȡ��������λ�õȡ�
	@param pRspʵ�ʻ�ȡ�������ļ���λ�õȡ�
	@param pProfileInfo��ȡ�������ļ��������ļ���Ϣ��
	@param pdwProfileData���ڻ�ȡ�����ļ����ݵĻ�������
	@param dwDataSize pdwProfileData�ֽڴ�С
	@return ������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetProfile(LONG lDeviceId, LJX8IF_GET_PROFILE_REQUEST* pReq, LJX8IF_GET_PROFILE_RESPONSE* pRsp, LJX8IF_PROFILE_INFO* pProfileInfo, DWORD* pdwProfileData, DWORD dwDataSize);

	/**
	Get batch profiles
	��ȡ���������ļ�
	@param	lDeviceId		The communication device to communicate with.
	@param	pReq			The position, etc., of the profiles to get.
	@param	pRsp			The position, etc., of the profiles that were actually acquired.
	@param	pProfileInfo	The profile information for the acquired profiles.
	@param	pdwBatchData	The buffer to get the profile data.
	@param	dwDataSize		pdwProfileData size in BYTEs
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetBatchProfile(LONG lDeviceId, LJX8IF_GET_BATCH_PROFILE_REQUEST* pReq, LJX8IF_GET_BATCH_PROFILE_RESPONSE* pRsp, LJX8IF_PROFILE_INFO * pProfileInfo, DWORD* pdwBatchData, DWORD dwDataSize);

	/**
	Get batch profiles by simple array format
	ͨ���򵥵������ʽ��ȡ�����������ļ�
	@param	lDeviceId				The communication device to communicate with.
	@param	pReq					The position, etc., of the profiles to get.
	@param	pRsp					The position, etc., of the profiles that were actually acquired.
	@param	pProfileInfo			The profile information for the acquired profiles.
	@param  pProfileHeaderArray		The buffer to get array of header.
	@param  pHeightProfileArray		The buffer to get array of profile data.
	@param  pLuminanceProfileArray	The buffer to get array of luminance profile data.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_GetBatchSimpleArray(LONG lDeviceId, LJX8IF_GET_BATCH_PROFILE_REQUEST* pReq, LJX8IF_GET_BATCH_PROFILE_RESPONSE* pRsp, LJX8IF_PROFILE_INFO* pProfileInfo, LJX8IF_PROFILE_HEADER* pProfileHeaderArray, WORD* pHeightProfileArray, WORD* pLuminanceProfileArray);

	// High-speed data communication related
	/**
	��ʼ����̫����������ͨ��
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@param pEthernetConfig����ͨ����ʹ�õ���̫�����á�
	@param wHighSpeedPortNo����ͨ����ʹ�õĶ˿ںš�
	@param pCallBack����ͨ�Ž��յ�����ʱҪ���õĻص�������
	@param dwProfileCount���ûص�������Ƶ�ʡ�
	@param dwThreadId�߳�ID��
	@return ������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_InitializeHighSpeedDataCommunication(LONG lDeviceId, LJX8IF_ETHERNET_CONFIG* pEthernetConfig, WORD wHighSpeedPortNo,
		LJX8IF_CALLBACK pCallBack, DWORD dwProfileCount, DWORD dwThreadId);

	/**
	��ʼ����̫����������ͨ����ʵ�ּ�����
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@param pEthernetConfig����ͨ����ʹ�õ���̫�����á�
	@param wHighSpeedPortNo����ͨ����ʹ�õĶ˿ںš�
	@param pCallBackSimpleArray����ͨ�Ž��յ�����ʱҪ���õĻص�������
	@param dwProfileCount���ûص�������Ƶ�ʡ�
	@param dwThreadId�߳�ID��
	@return������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_InitializeHighSpeedDataCommunicationSimpleArray(LONG lDeviceId, LJX8IF_ETHERNET_CONFIG* pEthernetConfig, WORD wHighSpeedPortNo,
		LJX8IF_CALLBACK_SIMPLE_ARRAY pCallBackSimpleArray, DWORD dwProfileCount, DWORD dwThreadId);

	/**
	��ʼ��������ͨ��֮ǰҪ��׼��
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@param pReq����Щ���ݷ��͸���ͨ�š�
	@param pProfileInfo�洢�����ļ���Ϣ��
	@return������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_PreStartHighSpeedDataCommunication(LONG lDeviceId, LJX8IF_HIGH_SPEED_PRE_START_REQ* pReq, LJX8IF_PROFILE_INFO* pProfileInfo);

	/**
	��ʼ��������ͨѶ
	@param lDeviceId��֮ͨ�ŵ�ͨ���豸��
	@return������
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_StartHighSpeedDataCommunication(LONG lDeviceId);

	/**
	Stop high-speed data communication
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_StopHighSpeedDataCommunication(LONG lDeviceId);

	/**
	Finalize high-speed data communication
	@param	lDeviceId	The communication device to communicate with.
	@return	Return code
	*/
	LJX8_IF_API LONG WINAPI LJX8IF_FinalizeHighSpeedDataCommunication(LONG lDeviceId);


};
#pragma managed(pop)