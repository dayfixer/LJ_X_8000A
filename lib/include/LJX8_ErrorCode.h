//Copyright (c) 2019 KEYENCE CORPORATION. All rights reserved.
/** @file
@brief	LJX8_ErrorCode Header
*/

#define LJX8IF_RC_OK						0x0000	// ������ֹ
#define LJX8IF_RC_ERR_OPEN					0x1000	// ��ͨѶ·��ʧ��
#define LJX8IF_RC_ERR_NOT_OPEN				0x1001	// ͨѶ·��δ����
#define LJX8IF_RC_ERR_SEND					0x1002	// ��������ʧ��.
#define LJX8IF_RC_ERR_RECEIVE				0x1003	// δ���յ���ȷӦ��.
#define LJX8IF_RC_ERR_TIMEOUT				0x1004	// �ȴ���Ӧ�ڼ䷢����ʱ
#define LJX8IF_RC_ERR_NOMEMORY				0x1005	// �����ڴ�ʧ��
#define LJX8IF_RC_ERR_PARAMETER				0x1006	// ��������Ч�Ĳ���
#define LJX8IF_RC_ERR_RECV_FMT				0x1007	// �յ�����Ӧ������Ч

#define LJX8IF_RC_ERR_HISPEED_NO_DEVICE		0x1009	// �޷�ִ�и���ͨ�ų�ʼ��
#define LJX8IF_RC_ERR_HISPEED_OPEN_YET		0x100A	// ����ͨѶ�ѳ�ʼ��
#define LJX8IF_RC_ERR_HISPEED_RECV_YET		0x100B	// ����ͨ�����Ѿ������������ڸ���ͨ�ţ�
#define LJX8IF_RC_ERR_BUFFER_SHORT			0x100C	// ��Ϊ�������ݵĻ�������С���� 
