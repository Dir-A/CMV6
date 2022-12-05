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
	std::wstring& m_wsCMV;
	std::ifstream m_ifsCMV;
	std::ofstream m_ofsRES;
	CMV6Header m_Header;
	std::vector<CMV6IndexDescriptor> m_vecDescriptor;

public:
	CMV6Pack(std::wstring wsCMV);
	~CMV6Pack();
	bool ExtractRes(std::wstring& wsCMV, bool isDecode);

private:
	bool WriteResFile(std::wstring& wsRes, std::streampos posRes, size_t szRes);

};