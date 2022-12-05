#pragma once
#include <Windows.h>
#include <iostream>

VOID InitDecodeInfo();
VOID JBPDDecodeToWebP(LPCWSTR lpBMPFileName, PCHAR pJBPD, DWORD szJBPD);
VOID JBPDDecodeFromeFile(std::wstring strJBPDFile);

VOID CMV6FrameDump();
VOID JBPDToBMPThread();
VOID UnPackCMVThread();
