#pragma once
#include <iostream>
#include <fstream>

class CMV6
{
private:
	char* m_pRes;
	size_t m_szAllocMax;
	std::wstring& m_wsCMV;
	std::ifstream m_ifsCMV;
	std::ofstream m_ofsRES;

public:
	CMV6(std::wstring wsCMV);
	~CMV6();
	bool ExtractRes(std::wstring& wsCMV, bool isDecode);

private:
	bool WriteResFile(std::wstring& wsRes, std::streampos posRes, size_t szRes);

};