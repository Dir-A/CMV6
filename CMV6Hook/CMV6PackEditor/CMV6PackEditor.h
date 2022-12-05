#pragma once
#include <iostream>
#include <fstream>
#include <vector>

struct CMV6Header
{
	char aSignature[4];
	unsigned int uiResSecOffset;
	unsigned int uiFileSize;
	unsigned int uiUnknow0;
	unsigned int uiResCount;
	unsigned int uiFrameRate;
	unsigned int uiUnknow1;
	unsigned int uiWide;
	unsigned int uiHigh;
	unsigned int uiBitCount;
	unsigned int uiUnknow2;
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
	bool WriteRes(std::wstring wsRes, std::streampos posRes, size_t szRes);

public:
	CMV6Pack();
	CMV6Pack(std::wstring wsCMV, std::wstring wsPath);
	~CMV6Pack();
	bool UnPackAllRes();
	bool UnPackSingleRes(unsigned int uiSequence);
	static std::wstring MakeFileName(unsigned int uiSequence, unsigned int uiType);
	char* GetResToBuffer(std::streampos posRes, size_t szRes);
};