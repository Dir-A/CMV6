#pragma once
#include <Windows.h>

VOID InitDecodeInfo();
VOID JBPDDecodeToBitMap(LPCWSTR lpBMPFileName, PCHAR pJBPD, DWORD szJBPD);

VOID UnPackCMVThread();
VOID CvtJBPDToBMP();
VOID CMV6FrameDump();