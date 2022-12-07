#include "JBPX.h"
#include "..\Detours\detours.h"
#include "..\CMV6PackEditor\CMV6PackEditor.h"
#include "..\libwebp\encode.h"
#pragma comment(lib,"..\\libwebp\\libwebp.lib")


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

WCHAR g_lpFilePathBuffer[0xFF] = { 0 };
PCHAR g_pBitMapBuffer = (PCHAR)malloc(0x384000);
PCHAR g_pJBPDBuffer = (PCHAR)malloc(0x384000);

typedef PBYTE(__fastcall* pGetJBPX)(PBYTE* pTHIS, DWORD dwEDX, DWORD dwSequence, PDWORD szJBPD);
pGetJBPX orgGetJBPX = (pGetJBPX)0x004333D0;

typedef DWORD(__thiscall* pTransFrame)(PDWORD pThis, DWORD a2, char* pJBPX, PDWORD szJBPX, DWORD a5);
pTransFrame orgTransFrame = (pTransFrame)0x004311E0;

typedef DWORD(WINAPI* pDecJBPD)(ImageSecData* lpImageSecData, ImageSecWidthBites* lpImageSecWidthBites, PBYTE pJBPD, PDWORD szJBPD, DWORD dwUnknow);
pDecJBPD orgDecJBPD = (pDecJBPD)0x00518490;

typedef DWORD(WINAPI* pDecJBP1)(ImageSecData* lpImageSecData, ImageSecWidthBites* lpImageSecWidthBites, PBYTE pJBP1, PDWORD szJBP1, DWORD dwUnknow);
pDecJBP1 orgDecJBP1 = (pDecJBP1)0x00431401;

//For 1280 x 720
//MergeImage Frome 15 Sectors
//PerLineBytes = 0x400
//ImageHight 1,2,3,4,5,6,7,8,9 = 0x100; A,B,C,D,E = 0xD0
VOID MergeImage720(ImageSecData* lpImageSecData, PCHAR pMergeBuffer)
{
	for (size_t iteHight = 0; iteHight < 0x100; iteHight++)
	{
		memcpy(&pMergeBuffer[0x400 * 0], &lpImageSecData->pSec0[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 1], &lpImageSecData->pSec1[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 2], &lpImageSecData->pSec2[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 3], &lpImageSecData->pSec3[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 4], &lpImageSecData->pSec4[0x400 * iteHight], 0x400);
		pMergeBuffer += 0x400 * 5;
	}

	for (size_t iteHight = 0; iteHight < 0x100; iteHight++)
	{
		memcpy(&pMergeBuffer[0x400 * 0], &lpImageSecData->pSec5[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 1], &lpImageSecData->pSec6[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 2], &lpImageSecData->pSec7[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 3], &lpImageSecData->pSec8[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 4], &lpImageSecData->pSec9[0x400 * iteHight], 0x400);
		pMergeBuffer += 0x400 * 5;
	}

	for (size_t iteHight = 0; iteHight < 0xD0; iteHight++)
	{
		memcpy(&pMergeBuffer[0x400 * 0], &lpImageSecData->pSecA[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 1], &lpImageSecData->pSecB[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 2], &lpImageSecData->pSecC[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 3], &lpImageSecData->pSecD[0x400 * iteHight], 0x400);
		memcpy(&pMergeBuffer[0x400 * 4], &lpImageSecData->pSecE[0x400 * iteHight], 0x400);
		pMergeBuffer += 0x400 * 5;
	}
}

//Fast Use To Dynamic Dump Frame
VOID SaveAsBmp(LPCWSTR lpFilePath, PCHAR pBitMapBuffer, DWORD szFile)
{
	lstrcatW((LPWSTR)lpFilePath, L".bmp");
	HANDLE hFile = CreateFileW(lpFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, &g_bFile, sizeof(BITMAPFILEHEADER), NULL, NULL);
		WriteFile(hFile, &g_bInfo, sizeof(BITMAPINFOHEADER), NULL, NULL);
		WriteFile(hFile, pBitMapBuffer, szFile, NULL, NULL);

		FlushFileBuffers(hFile);
		CloseHandle(hFile);
	}
}

//Lossless WebP Format But Slow Encoding Speed Use To Decode JBPD By Calling DecJBPD
VOID SaveAsWebP(LPCWSTR lpFilePath, PUCHAR pBitMapBuffer, DWORD szFile)
{
	lstrcatW((LPWSTR)lpFilePath, L".webp");
	HANDLE hFile = CreateFileW(lpFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		//Destroy Alpha Channel
		for (size_t iteBitMap = 3; iteBitMap < szFile; iteBitMap++)
		{
			pBitMapBuffer[iteBitMap] = 0xFF;
			iteBitMap += 3;
		}

		uint8_t* pWebP = nullptr;
		size_t szWebP = WebPEncodeLosslessBGRA((uint8_t*)pBitMapBuffer, 1280, 720, 1280 * 4, &pWebP);

		WriteFile(hFile, pWebP, szWebP, NULL, NULL);

		FlushFileBuffers(hFile);
		CloseHandle(hFile);
		WebPFree(pWebP);

		std::wcout << L"Dump:" << lpFilePath << std::endl;
	}
}

//Make Full Path Of Dump File
VOID MakeFileName(DWORD dwSequence)
{
	swprintf_s(g_lpFilePathBuffer, L"Dump\\%08d", dwSequence);
}

PBYTE __fastcall newGetJBPD(PBYTE* pTHIS, DWORD dwEDX, DWORD dwSequence, PDWORD szJBPD)
{
	MakeFileName(dwSequence);
	return orgGetJBPX(pTHIS, dwEDX, dwSequence, szJBPD);
}

DWORD WINAPI newDecJBPD(ImageSecData* lpImageSecData, ImageSecWidthBites* lpImageSecWidthBites, PBYTE pJBPD, PDWORD szJBPD, DWORD dwUnknow)
{
	MergeImage720(lpImageSecData, g_pBitMapBuffer);
	SaveAsBmp(g_lpFilePathBuffer, g_pBitMapBuffer, g_bInfo.biSizeImage);
	return orgDecJBPD(lpImageSecData, lpImageSecWidthBites, pJBPD, szJBPD, dwUnknow);
}

//Set BMP Format File Info
VOID InitBMPInfo(BITMAPFILEHEADER* lpBitMapFile, BITMAPINFOHEADER* lpBitMapInfo)
{
	lpBitMapFile->bfType = 0x4D42;
	lpBitMapFile->bfSize = 0x384036;
	lpBitMapFile->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	lpBitMapInfo->biSize = 40;
	lpBitMapInfo->biWidth = 1280;
	lpBitMapInfo->biHeight = 720;
	lpBitMapInfo->biPlanes = 1;
	lpBitMapInfo->biBitCount = 32;
	lpBitMapInfo->biCompression = 0;
	lpBitMapInfo->biSizeImage = 0x384000;
}

//Set JBPD Fromat Processing Conditions
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
	InitBMPInfo(&g_bFile, &g_bInfo);
}

BOOL LoadJBPD(LPCWSTR lpJBPDFileName, PCHAR pJBPDBuffer, PDWORD szJBPDFile)
{
	*szJBPDFile = 0;
	errno_t err = 0;
	FILE* fpJBPD = nullptr;

	err = _wfopen_s(&fpJBPD, lpJBPDFileName, L"rb");
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

VOID JBPDDecodeToWebP(LPCWSTR lpBMPFileName, PCHAR pJBPD, DWORD szJBPD)
{
	orgDecJBPD(&g_lpSecData, &g_lpSecBites, (PBYTE)pJBPD, &szJBPD, 0);
	MergeImage720(&g_lpSecData, g_pBitMapBuffer);
	SaveAsWebP(lpBMPFileName, (PUCHAR)g_pBitMapBuffer, g_bInfo.biSizeImage);
}

VOID JBPDDecodeFromeFile(std::wstring strJBPDFile)
{
	static DWORD szJBPD = 0;

	LoadJBPD(strJBPDFile.c_str(), g_pJBPDBuffer, &szJBPD);
	JBPDDecodeToWebP(strJBPDFile.c_str(), g_pJBPDBuffer, szJBPD);
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
	CreateDirectoryW(L"Unpack", NULL);
	CreateDirectoryW(L"Dump", NULL);

	while (true)
	{
		BOOL isSingle = FALSE;
		BOOL isDecode = FALSE;
		std::wstring cmvFile;
		std::wstring jbpdFile;

		std::wcout
			<< L"Decode Single JBPD Input 1" << '\n'
			<< L"Process CMV File Input 0" << std::endl;
		std::wcin >> isSingle;

		if (isSingle)
		{
			JBPDToBMPThread();
			return;
		}

		std::wcout
			<< "Waitting For Command" << '\n'
			<< "Input File Name" << std::endl;
		std::wcin >> cmvFile;

		std::wcout << "Is Decode JBPD (1 = T / 0 = F) (If Not Will Unpack All Resources(.JBPD / .Ogg)" << std::endl;
		std::wcin >> isDecode;

		if (isDecode)
		{
			std::wcout << L"Dump Start Will Save File In Dump Folder. It May Take A Few Minutes\n" << std::endl;
		}
		else
		{
			std::wcout << L"Unpack Start Will Save File In Unpack Folder.\n" << std::endl;
		}

		CMV6File::CMV6Unpack cmv(cmvFile, L"Unpack\\");

		if (isDecode)
		{
			for (CMV6File::CMV6IndexDescriptor& descriptor : cmv.m_vecDescriptor)
			{
				if (descriptor.uiType == 0)
				{
					cmv.UnPackSingleRes(descriptor.uiSequence);
					continue;
				}

				MakeFileName(descriptor.uiSequence);

				JBPDDecodeToWebP(g_lpFilePathBuffer, cmv.GetResToBuffer(descriptor.uiOffset + cmv.m_Header.uiResSecOffset, descriptor.uiCmpSize), descriptor.uiCmpSize);
			}
		}
		else
		{
			cmv.UnPackAllRes();
		}

	}
}

VOID CMV6FrameDump()
{
	InitBMPInfo(&g_bFile, &g_bInfo);

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID*)&orgGetJBPX, newGetJBPD);
	DetourAttach((PVOID*)&orgDecJBPD, newDecJBPD);
	DetourTransactionCommit();
}

//Set Console Input .JBPD File Path Decode To WebP
VOID JBPDToBMPThread()
{
	InitDecodeInfo();
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ReDoDecodeJBPDFromeFile, NULL, NULL, NULL);
}

//Set Console Input .CMV File Path Unpack All Resources And Decode JBPD
VOID UnPackCMVThread()
{
	InitDecodeInfo();
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)UnPackCMV, NULL, NULL, NULL);
}