#pragma once
#include <iostream>
#include <fstream>
#include <vector>

//Some As CMV4 CMV3
struct CMV6Header
{
	char aSignature[4]; //CMV6 CMV4 CMV3
	unsigned int uiResSecOffset;
	unsigned int uiFileSize;
	unsigned int uiUnknow0; //Not Used
	unsigned int uiResCount;
	unsigned int uiFrameRate;
	unsigned int uiFrameRateTime; // Per Second
	unsigned int uiWide;
	unsigned int uiHigh;
	unsigned int uiBitCount;
	unsigned int uiAudio; // About Audio File Load If 0x0 Game Will Crash In MOG_INputMemory
	//CMV6IndexDescriptor[uiResCount + 1];
};

struct CMV6IndexDescriptor
{
	unsigned int uiSequence; // max = uiResCount
	unsigned int uiCmpSize;
	unsigned int uiOrgSize;
	unsigned int uiType; // 0 = ogg, 02 = jbp
	unsigned int uiOffset; // dataOffset = uiOffset + uiResOffset
};

class CMV6Pack
{
private:
	char* m_pRes;
	size_t m_szAllocMax;
	std::wstring m_wsCMV;
	std::wstring m_wsPath;
	std::ifstream m_ifsCMV;
	std::ofstream m_ofsRES;

public:
	size_t m_szData;
	CMV6Header m_Header;
	std::vector<CMV6IndexDescriptor> m_vecDescriptor;

private:
	void InitPackInfo();
	bool WriteRes(std::wstring wsRes, size_t posRes, size_t szRes);

public:
	CMV6Pack();
	CMV6Pack(std::wstring wsCMV, std::wstring wsPath);
	~CMV6Pack();
	bool UnPackAllRes();
	bool UnPackSingleRes(unsigned int uiSequence);
	static std::wstring MakeFileName(unsigned int uiSequence, unsigned int uiType);
	char* GetResToBuffer(size_t posRes, size_t szRes);
};