#include "JBPD.h"
#include <iostream>
#include "detours.h"
#include "CMV6.h"


struct ImageSecData
{
	unsigned char* pSec0;
	unsigned char* pSec1;
	unsigned char* pSec2;
	unsigned char* pSec3;
	unsigned char* pSec4;
	unsigned char* pSec5;
	unsigned char* pSec6;
	unsigned char* pSec7;
	unsigned char* pSec8;
	unsigned char* pSec9;
	unsigned char* pSecA;
	unsigned char* pSecB;
	unsigned char* pSecC;
	unsigned char* pSecD;
	unsigned char* pSecE;
};

struct ImageSecWidthBites
{
	unsigned long szSec0;
	unsigned long szSec1;
	unsigned long szSec2;
	unsigned long szSec3;
	unsigned long szSec4;
	unsigned long szSec5;
	unsigned long szSec6;
	unsigned long szSec7;
	unsigned long szSec8;
	unsigned long szSec9;
	unsigned long szSecA;
	unsigned long szSecB;
	unsigned long szSecC;
	unsigned long szSecD;
	unsigned long szSecE;
};

BITMAPFILEHEADER g_bFile = { 0 };
BITMAPINFOHEADER g_bInfo = { 0 };

ImageSecData g_lpSecData = { 0 };
ImageSecWidthBites g_lpSecBites = { 0 };

WCHAR g_lpFileName[0xFF] = { 0 };
WCHAR g_lpFilePath[0xFF] = { 0 };
PCHAR g_pBitMapBuffer = (PCHAR)malloc(0x384000);
PCHAR g_pJBPDBuffer = (PCHAR)malloc(0x384000);

typedef PBYTE(__fastcall* pGetJBPD)(PBYTE* pTHIS, DWORD dwEDX, DWORD dwSequence, PDWORD szJBPD);
pGetJBPD orgGetJBPD = (pGetJBPD)0x004333D0;

typedef DWORD(WINAPI* pDecJBPD)(ImageSecData* lpImageSecData, ImageSecWidthBites* lpImageSecWidthBites, PBYTE pJBPD, PDWORD szJBPD, DWORD dwUnknow);
pDecJBPD orgDecJBPD = (pDecJBPD)0x00518490;

//perLineBytes = 0x400
//Hight = 0x100; b c d e = 0xD0
PCHAR MergeImage(ImageSecData* lpImageSecData)
{
	PCHAR pImage = g_pBitMapBuffer;

	for (size_t iteHight = 0; iteHight < 0x100; iteHight++)
	{
		memcpy(&pImage[0x400 * 0], &lpImageSecData->pSec0[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 1], &lpImageSecData->pSec1[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 2], &lpImageSecData->pSec2[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 3], &lpImageSecData->pSec3[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 4], &lpImageSecData->pSec4[0x400 * iteHight], 0x400);
		pImage += 0x400 * 5;
	}

	for (size_t iteHight = 0; iteHight < 0x100; iteHight++)
	{
		memcpy(&pImage[0x400 * 0], &lpImageSecData->pSec5[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 1], &lpImageSecData->pSec6[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 2], &lpImageSecData->pSec7[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 3], &lpImageSecData->pSec8[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 4], &lpImageSecData->pSec9[0x400 * iteHight], 0x400);
		pImage += 0x400 * 5;
	}

	for (size_t iteHight = 0; iteHight < 0xD0; iteHight++)
	{
		memcpy(&pImage[0x400 * 0], &lpImageSecData->pSecA[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 1], &lpImageSecData->pSecB[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 2], &lpImageSecData->pSecC[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 3], &lpImageSecData->pSecD[0x400 * iteHight], 0x400);
		memcpy(&pImage[0x400 * 4], &lpImageSecData->pSecE[0x400 * iteHight], 0x400);
		pImage += 0x400 * 5;
	}

	return g_pBitMapBuffer;
}

VOID DumpImage(ImageSecData* lpImageSecData, LPCWSTR lpFilePath)
{
	MergeImage(lpImageSecData);

	HANDLE hFile = CreateFileW(lpFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, &g_bFile, sizeof(BITMAPFILEHEADER), NULL, NULL);
		WriteFile(hFile, &g_bInfo, sizeof(BITMAPINFOHEADER), NULL, NULL);
		WriteFile(hFile, g_pBitMapBuffer, 0x384000, NULL, NULL);
		FlushFileBuffers(hFile);
		CloseHandle(hFile);
	}
}

VOID MakeFileName(DWORD dwSequence)
{
	wcscpy_s(g_lpFilePath, L"dump\\");
	_itow_s(dwSequence, g_lpFileName, 10);
	wcscat_s(g_lpFileName, L".bmp");
	wcscat_s(g_lpFilePath, g_lpFileName);
}

PBYTE __fastcall newGetJBPD(PBYTE* pTHIS, DWORD dwEDX, DWORD dwSequence, PDWORD szJBPD)
{
	MakeFileName(dwSequence);
	return orgGetJBPD(pTHIS, dwEDX, dwSequence, szJBPD);
}

DWORD WINAPI newDecJBPD(ImageSecData* lpImageSecData, ImageSecWidthBites* lpImageSecWidthBites, PBYTE pJBPD, PDWORD szJBPD, DWORD dwUnknow)
{
	DumpImage(lpImageSecData, g_lpFilePath);
	return orgDecJBPD(lpImageSecData, lpImageSecWidthBites, pJBPD, szJBPD, dwUnknow);
}

VOID InitBMPInfo()
{
	g_bFile.bfType = 0x4D42;
	g_bFile.bfSize = 0x384036;
	g_bFile.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	g_bInfo.biSize = 40;
	g_bInfo.biWidth = 1280;
	g_bInfo.biHeight = 720;
	g_bInfo.biPlanes = 1;
	g_bInfo.biBitCount = 32;
	g_bInfo.biCompression = 0;
	g_bInfo.biSizeImage = 0x384000;
}

VOID InitJBPDInfo(ImageSecData* lpImageSecData, ImageSecWidthBites* lpImageSecWidthBites)
{
	for (size_t iteSec = 0; iteSec < 0xF; iteSec++)
	{
		((PBYTE*)lpImageSecData)[iteSec] = (PBYTE)malloc(0x40000);
		((DWORD*)lpImageSecWidthBites)[iteSec] = 0x400;
	}
}

VOID InitDecodeInfo()
{
	InitJBPDInfo(&g_lpSecData, &g_lpSecBites);
	InitBMPInfo();
}

BOOL LoadJBPD(LPCWSTR lpJBPDFile, PCHAR pJBPDBuffer ,PDWORD szJBPDFile)
{
	errno_t err = 0;
	FILE* fpJBPD = nullptr;

	err = _wfopen_s(&fpJBPD, lpJBPDFile, L"rb");
	if (fpJBPD && !err)
	{
		fseek(fpJBPD, 0, SEEK_END);
		*szJBPDFile = ftell(fpJBPD);
		fseek(fpJBPD, 0, SEEK_SET);
		if (!(*szJBPDFile))
		{
			return FALSE;
		}

		fread(pJBPDBuffer, 1, *szJBPDFile, fpJBPD);

		fflush(fpJBPD);
		fclose(fpJBPD);

		return TRUE;
	}

	return FALSE;
}

VOID JBPDDecodeToBitMap(LPCWSTR lpBMPFileName, PCHAR pJBPD, DWORD szJBPD)
{
	orgDecJBPD(&g_lpSecData, &g_lpSecBites, (PBYTE)pJBPD, &szJBPD, 0);
	DumpImage(&g_lpSecData, lpBMPFileName);
}

VOID JBPDDecodeFromeFile(std::wstring strJBPDFile)
{
	static DWORD szJBPD = 0;

	LoadJBPD(strJBPDFile.c_str(), g_pJBPDBuffer, &szJBPD);
	JBPDDecodeToBitMap((strJBPDFile + L".bmp").c_str(), g_pJBPDBuffer, szJBPD);
}

VOID ReDoDecodeJBPDFromeFile()
{
	std::wstring jbpdFile;
	while (true)
	{
		std::wcout << L"Input JBPD File Name" << std::endl;
		std::wcin >> jbpdFile;
		JBPDDecodeFromeFile(jbpdFile);
	}
}

VOID UnPackCMV()
{
	while (true)
	{
		std::wstring cmvFile;
		std::wcout << "Waitting For Command" << std::endl;
		std::wcin >> cmvFile;
		CMV6 cmv6(cmvFile, true);
	}
}

VOID CMV6FrameDump()
{
	InitBMPInfo();

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID*)&orgGetJBPD, newGetJBPD);
	DetourAttach((PVOID*)&orgDecJBPD, newDecJBPD);
	DetourTransactionCommit();
}

VOID CvtJBPDToBMP()
{
	InitDecodeInfo();
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ReDoDecodeJBPDFromeFile, NULL, NULL, NULL);
}

VOID UnPackCMVThread()
{
	InitDecodeInfo();
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)UnPackCMV, NULL, NULL, NULL);
}