#include "CMV6PackEditor.h"
#include <Windows.h>
#include <string>

CMV6Pack::CMV6Pack(std::wstring wsCMV) :
	m_wsCMV(wsCMV),
	m_szAllocMax(0),
	m_pRes(nullptr)
{
	CreateDirectoryW((m_wsCMV + L"_Unpack").c_str(), NULL);
	ExtractRes(m_wsCMV,false);
}

CMV6Pack::~CMV6Pack()
{
	if (m_pRes)
	{
		delete[] m_pRes;
	}
	m_ifsCMV.close();
}

bool CMV6Pack::ExtractRes(std::wstring& wsCMV, bool isDecode)
{
	m_ifsCMV.open(wsCMV, std::ios::binary);
	if (m_ifsCMV.is_open())
	{
		char* pCMV6Header = new char[sizeof(CMV6Header)];
		if (pCMV6Header)
		{
			m_ifsCMV.read(pCMV6Header, sizeof(CMV6Header));
		}
		else
		{
			return false;
		}
		CMV6Header* pHeader = (CMV6Header*)pCMV6Header;

		size_t IndexDescriptorCount = pHeader->uiResCount + 1;
		char* pCMV6Index = new char[IndexDescriptorCount * sizeof(CMV6IndexDescriptor)];
		if (pCMV6Index)
		{
			m_ifsCMV.read(pCMV6Index, IndexDescriptorCount * sizeof(CMV6IndexDescriptor));
		}
		else
		{
			return false;
		}
		CMV6IndexDescriptor* pIndex = (CMV6IndexDescriptor*)pCMV6Index;

		std::streampos posRes = 0;
		std::wstring resName;
		for (size_t iteIndex = 0; iteIndex < IndexDescriptorCount; iteIndex++)
		{
			switch (pIndex[iteIndex].uiType)
			{
			case 2:
				resName = std::to_wstring(pIndex[iteIndex].uiSequence) + L".jbp";
				break;
			case 0:
				resName = std::to_wstring(pIndex[iteIndex].uiSequence) + L".ogg";
				break;
			}

			if (!WriteResFile(resName, pHeader->uiResSecOffset + pIndex[iteIndex].uiOffset, pIndex[iteIndex].uiCmpSize))
			{
				std::cout << "Failed!!" << " Sequence:" << pIndex[iteIndex].uiSequence << std::endl;
			}
		}

		delete[] pCMV6Header;
		delete[] pCMV6Index;
		return true;
	}
	return false;
}

bool CMV6Pack::WriteResFile(std::wstring& wsRes, std::streampos posRes, size_t szRes)
{
	m_ofsRES.open(m_wsCMV + L"_Unpack/" + wsRes, std::ios::binary);
	if (m_ofsRES.is_open())
	{
		m_ifsCMV.seekg(posRes);

		if (!m_szAllocMax)
		{
			m_pRes = new char[szRes];
			m_szAllocMax = szRes;
		}

		if (szRes > m_szAllocMax)
		{
			delete[] m_pRes;
			m_pRes = new char[szRes];
			m_szAllocMax = szRes;
		}

		if (m_pRes)
		{
			m_ifsCMV.read(m_pRes, szRes);
		}
		else
		{
			return false;
		}

		m_ofsRES.write(m_pRes, szRes);

		m_ofsRES.flush();
		m_ofsRES.close();
		return true;
	}
	return false;
}
