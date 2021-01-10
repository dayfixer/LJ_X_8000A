//Copyright (c) 2019 KEYENCE CORPORATION. All rights reserved.
#include "StdAfx.h"
#include "ProfileSimpleArrayStore.h"
#include "DataExport.h"
#include "Define.h"

CProfileSimpleArrayStore::CProfileSimpleArrayStore() : m_dwCount(0),
													   m_dwNotify(0),
													   m_vecProfileData(0),
													   m_vecLuminanceData(0),
													   m_bIsLuminanceEnable(false),
													   m_nDataWidth(0),
													   m_nBatchNo(0)
{
}

CProfileSimpleArrayStore::~CProfileSimpleArrayStore()
{
}

/*
 @note��ȡ�洢�������ļ�����
 @������������
 @note Get stored profile count
 @return profile count
*/
DWORD CProfileSimpleArrayStore::GetCount()
{
	CSingleLock singleLock(&m_csDataAccess);
	singleLock.Lock();

	DWORD value = m_dwCount;

	singleLock.Unlock();

	return value;
}

/*
@note Add stored profile count
@param profile count
*/
void CProfileSimpleArrayStore::AddCount(DWORD dwCount)
{
	CSingleLock singleLock(&m_csDataAccess);
	singleLock.Lock();

	m_dwCount = dwCount;

	singleLock.Unlock();
}

/*
@note Get latest notify flag
@return notify flag
*/
DWORD CProfileSimpleArrayStore::GetNotify()
{
	CSingleLock singleLock(&m_csDataAccess);
	singleLock.Lock();

	DWORD value = m_dwNotify;
	m_dwNotify = 0;

	singleLock.Unlock();

	return value;
}

/*
@note Set notify flag
@param notify flag
@note����֪ͨ��־
@param֪ͨ��־
*/
void CProfileSimpleArrayStore::AddNotify(DWORD dwNotify)
{
	CSingleLock singleLock(&m_csDataAccess);
	singleLock.Lock();

	if ((dwNotify & (0x1 << BATCH_FINALIZE_FLAG_BIT_COUNT)) != 0)
		m_nBatchNo++;
	m_dwNotify |= dwNotify;

	singleLock.Unlock();
}

/*
@note Add received simple array data from device
@param A pointer to the buffer that stores the profile data array
@param A pointer to the buffer that stores the luminance profile data array
@param The number of profile data stored in buffer
@return True if buffer full.
@note���豸��ӽ��յ��ļ���������
@paramָ��洢�����ļ���������Ļ�������ָ��
@paramָ��洢���ȷֲ���������Ļ�������ָ��
@param�������д洢�������ļ����ݵ�����
@return�����������������ΪTrue��
*/
bool CProfileSimpleArrayStore::AddReceivedData(WORD *pwProfileBuffer, WORD *pwLuminanceBuffer, DWORD dwCount)
{
	CSingleLock singleLock(&m_csDataAccess);
	singleLock.Lock();

	if (m_nDataWidth <= 0)
		return m_dwCount >= BUFFER_FULL_COUNT;

	DWORD dwCopyCount = min(BUFFER_FULL_COUNT - m_dwCount, dwCount);
	if (dwCopyCount == 0)
		return m_dwCount >= BUFFER_FULL_COUNT;

	m_dwCount += dwCopyCount;
	auto dataSize = dwCopyCount * m_nDataWidth;
	m_vecProfileData.insert(m_vecProfileData.end(), &pwProfileBuffer[0], &pwProfileBuffer[dataSize]);

	if (m_bIsLuminanceEnable)
	{
		m_vecLuminanceData.insert(m_vecLuminanceData.end(), &pwLuminanceBuffer[0], &pwLuminanceBuffer[dataSize]);
	}

	singleLock.Unlock();

	return m_dwCount >= BUFFER_FULL_COUNT;
}

/*
@note Clear all data and property to default
*/
void CProfileSimpleArrayStore::Clear()
{
	CSingleLock singleLock(&m_csDataAccess);
	singleLock.Lock();
	m_dwCount = 0;
	m_dwNotify = 0;
	m_vecProfileData.clear();
	m_vecProfileData.shrink_to_fit();
	m_vecLuminanceData.clear();
	m_vecLuminanceData.shrink_to_fit();
	m_bIsLuminanceEnable = false;
	m_nDataWidth = 0;
	m_nBatchNo = 0;

	singleLock.Unlock();
}

/*
@note����ǰ�����ļ����ݺ��������ݱ��浽ͼ���ļ�
@paramҪ������ļ�·��
@paramҪ����������ļ�������
@paramҪ����������ļ�����
@return�������ɹ�����ΪTrue
*/
bool CProfileSimpleArrayStore::SaveDataAsImages(CString strFilePath, DWORD dwIndex, DWORD dwCount)
{
	/*
	ʹ��C
	ͨ���Ի���Ŀ��ƴﵽ������Դ�Ļ�����ʣ����һ���߳�ռ���ڷ��ʻ��⣬��ôϵͳ������ǰ�ĵ����̣߳�
	ֱ��������ⱻ�ͷ�Ϊֹ����ʱ����������߳̽������Ѳ�ȡ�öԻ���Ŀ��ƣ�һ����˵ʹ������ҪLock,Unlock��������ʹ��CSingleLockʱ��
	ֻ��Ҫһ�����ɣ����ں����ڲ�������һ��CSingleLock��ͬʱָ����������ָ�룬�ں���ִ����ϵ�ʱ�򣬻��Զ�ִ��CSingleLock�������������Ӷ�����
	*/

	CSingleLock singleLock(&m_csDataAccess);
	singleLock.Lock();

	if (strFilePath.IsEmpty())
		return false;
	if (m_nDataWidth <= 0)
		return false;
	if (dwCount <= 0 || dwIndex > m_dwCount || dwIndex + dwCount > m_dwCount)
		return false;

	SaveBitmap(dwIndex, dwCount, strFilePath);
	SaveTiff(dwIndex, dwCount, strFilePath);

	singleLock.Unlock();

	return true;
}

//�洢bmpͼ����ú���
void CProfileSimpleArrayStore::SaveBitmap(DWORD dwIndex, DWORD dwCount, CString strPathBase)
{
	const CString heightSuffix("_height.bmp");
	const CString luminanceSuffix("_luminance.bmp");

	CString strHeightFilePath = CDataExport::GetDeterminantFilePath(strPathBase, heightSuffix);
	SaveBitmapCore(strHeightFilePath, &m_vecProfileData[dwIndex * m_nDataWidth], m_nDataWidth, dwCount);

	if (m_bIsLuminanceEnable)
	{
		CString strLuminanceFilePath = CDataExport::GetDeterminantFilePath(strPathBase, luminanceSuffix);
		SaveBitmapCore(strLuminanceFilePath, &m_vecLuminanceData[dwIndex * m_nDataWidth], m_nDataWidth, dwCount);
	}
}

//�洢tiff������ú���
void CProfileSimpleArrayStore::SaveTiff(DWORD dwIndex, DWORD dwCount, CString strPathBase)
{
	const CString heightSuffix("_height.tif");
	const CString luminanceSuffix("_luminance.tif");

	CString strHeightFilePath = CDataExport::GetDeterminantFilePath(strPathBase, heightSuffix);
	SaveTiffCore(strHeightFilePath, &m_vecProfileData[dwIndex * m_nDataWidth], m_nDataWidth, dwCount);

	if (m_bIsLuminanceEnable)
	{
		CString strLuminanceFilePath = CDataExport::GetDeterminantFilePath(strPathBase, luminanceSuffix);
		SaveTiffCore(strLuminanceFilePath, &m_vecLuminanceData[dwIndex * m_nDataWidth], m_nDataWidth, dwCount);
	}
}

//�洢λͼ���ķ���
void CProfileSimpleArrayStore::SaveBitmapCore(CString strFilePath, WORD *data, DWORD dwWidth, DWORD dwHeight)
{
	int imageSize = dwWidth * dwHeight * sizeof(WORD);
	int headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 3;

	BITMAPFILEHEADER bmpHead;
	memset(&bmpHead, 0, sizeof(BITMAPFILEHEADER));
	memcpy(&bmpHead.bfType, "BM", 2);
	bmpHead.bfSize = headerSize + imageSize;
	bmpHead.bfOffBits = headerSize;

	BITMAPINFOHEADER bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.biSize = 40;
	bmpInfo.biWidth = dwWidth;
	bmpInfo.biHeight = dwHeight;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 16;
	bmpInfo.biCompression = BI_BITFIELDS;
	bmpInfo.biSizeImage = 0;
	bmpInfo.biXPelsPerMeter = 3780; // 96dpi
	bmpInfo.biYPelsPerMeter = 3780; // 96dpi
	bmpInfo.biClrUsed = 0;
	bmpInfo.biClrImportant = 0;
	DWORD dwBitField[3]{0x0000F800, 0x000007E0, 0x0000001F};

	CStringA astrFilePath(strFilePath);
	FILE *fBmp;
	fopen_s(&fBmp, astrFilePath, "wb");
	fseek(fBmp, 0L, SEEK_SET);
	fwrite(&bmpHead, sizeof(BITMAPFILEHEADER), 1, fBmp);
	fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fBmp);
	fwrite(dwBitField, sizeof(DWORD), 3, fBmp);

	for (int i = dwHeight - 1; i >= 0; i--)
	{
		fwrite(data + (dwWidth * i), sizeof(WORD), dwWidth, fBmp);
	}

	// @Point
	//��λͼ������������ݸ�ʽ�����¶��ϵġ�
	//�����ļ������һ�д洢����������Ķ����������ļ��ĵ�һ�д洢�����

	fclose(fBmp);
}

//�洢tiff���Ĵ���
void CProfileSimpleArrayStore::SaveTiffCore(CString strFilePath, WORD *data, DWORD dwWidth, DWORD dwHeight)
{
	CStringA astrFilePath(strFilePath);
	FILE *fTif;
	fopen_s(&fTif, astrFilePath, "wb");
	fseek(fTif, 0L, SEEK_SET);

	WriteTiffHeader(fTif, dwWidth, dwHeight);

	fwrite(data, sizeof(WORD), dwWidth * dwHeight, fTif);

	fclose(fTif);
}

void CProfileSimpleArrayStore::WriteTiffHeader(FILE *fTif, DWORD dwWidth, DWORD dwHeight)
{
	// <header(8)> + <��ǩ����(2)> + <tag(12)> * 12 + <next IFD(4)> + <�ֱ��ʣ���λ�� unit(8)> * 2
	const unsigned int stripOffset = 174;

	// Header (little endian)
	byte header[8] = {0x49, 0x49, 0x2A, 0x00, 0x08, 0x00, 0x00, 0x00};
	fwrite(header, sizeof(byte), 8, fTif);

	// ��ǩ����
	byte tagCount[2] = {0x0C, 0x00};
	fwrite(tagCount, sizeof(byte), 2, fTif);

	// ͼ����
	WriteTiffTag(fTif, 0x0100, 3, 1, dwWidth);

	// ͼ�񳤶�
	WriteTiffTag(fTif, 0x0101, 3, 1, dwHeight);

	// ÿ��������λ��  Bits per sample
	WriteTiffTag(fTif, 0x0102, 3, 1, 16);

	// ѹ������ѹ����  Compression (no compression)
	WriteTiffTag(fTif, 0x0103, 3, 1, 1);

	// ��Ƚ��ͣ���ɫģʽ�͵�ɫ�� Photometric interpretation (white mode & monochrome)
	WriteTiffTag(fTif, 0x0106, 3, 1, 1);

	// ��ƫ���� Strip offsets
	WriteTiffTag(fTif, 0x0111, 3, 1, stripOffset);

	// ��ÿ��  Rows per strip
	WriteTiffTag(fTif, 0x0116, 3, 1, dwHeight);

	// ���ֽ��� strip byte counts
	WriteTiffTag(fTif, 0x0117, 4, 1, dwWidth * dwHeight * 2);

	// X�����ַ X resolusion address
	WriteTiffTag(fTif, 0x011A, 5, 1, stripOffset - 16);

	// Y�����ַ Y resolusion address
	WriteTiffTag(fTif, 0x011B, 5, 1, stripOffset - 8);

	// �ֱ��ʵ�λ��Ӣ�磩 Resolusion unit (inch)
	WriteTiffTag(fTif, 0x0128, 3, 1, 2);

	// ��ɫ��ͼ(��ʹ�ò�ɫ��ͼ) Color map (not use color map)
	WriteTiffTag(fTif, 0x0140, 3, 1, 0);

	// Next IFD
	int nextIfd = 0;
	fwrite(&nextIfd, sizeof(int), 1, fTif);

	// X�ֱ��ʺ�Y�ֱ���  X resolusion and Y resolusion
	int xResolusion[2] = {96, 1};
	fwrite(&xResolusion, sizeof(int), 2, fTif);
	int yResolusion[2] = {96, 1};
	fwrite(&yResolusion, sizeof(int), 2, fTif);
}

void CProfileSimpleArrayStore::WriteTiffTag(FILE *fTif, unsigned short kind, unsigned short dataType, unsigned int dataSize, unsigned int data)
{
	fwrite(&kind, sizeof(unsigned short), 1, fTif);
	fwrite(&dataType, sizeof(unsigned short), 1, fTif);
	fwrite(&dataSize, sizeof(unsigned int), 1, fTif);
	fwrite(&data, sizeof(unsigned int), 1, fTif);
}