#include <Windows.h>
#include "Tools.h"
#include "JBPX.h"

VOID StartHook()
{
	UnPackCMVThread();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		SetConsole(L"CMV6");
		StartHook();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

EXTERN_C VOID _declspec(dllexport) DirA(){}

