#include "CMV6PackEditor.h"
#include <Windows.h>
#include <string>

CMV6Pack::CMV6Pack():
	m_szData(0),
	m_szAllocMax(0),
	m_pRes(nullptr),
	m_Header({ 0 })
{

}

CMV6Pack::CMV6Pack(std::wstring wsCMV, std::wstring wsPath) :
	m_wsCMV(wsCMV),
	m_wsPath(wsPath),
	m_szData(0),
	m_szAllocMax(0),
	m_pRes(nullptr),
	m_Header({0})
{
	if (m_wsPath[m_wsPath.size()] != L'\\')
	{
		m_wsPath.append(L"\\");
	}

	m_ifsCMV.open(wsCMV, std::ios::binary);
	if (m_ifsCMV.is_open())
	{
		InitPackInfo();
		return;
	}
}

CMV6Pack::~CMV6Pack()
{
	if (m_ifsCMV.is_open())
	{
		m_ifsCMV.close();
	}

	if (m_pRes)
	{
		delete[] m_pRes;
	}
}

void CMV6Pack::InitPackInfo()
{
	//Read File Header
	m_ifsCMV.read((char*)&m_Header, sizeof(CMV6Header));
	
	//Read Resources Index
	size_t szIndex = (m_Header.uiResCount + 1) * sizeof(CMV6IndexDescriptor);
	CMV6IndexDescriptor* pDescriptor = nullptr;
	char* pIndex = new char[szIndex];
	m_ifsCMV.read(pIndex, szIndex);
	pDescriptor = (CMV6IndexDescriptor*)pIndex;
	if (pDescriptor)
	{
		for (size_t iteIndex = 0; iteIndex <= m_Header.uiResCount; iteIndex++)
		{
			m_szData += (pDescriptor + iteIndex)->uiCmpSize;
			m_vecDescriptor.push_back(*(pDescriptor + iteIndex));
		}
		pDescriptor = nullptr;
		delete[] pIndex;
	}
}

inline std::wstring CMV6Pack::MakeFileName(unsigned int uiSequence, unsigned int uiType)
{
	wchar_t fileNameBuffer[20] = { 0 };

	switch (uiType)
	{
	case 2:
		swprintf_s(fileNameBuffer, L"%08d.jbpd", uiSequence);
		break;

	case 0:
		swprintf_s(fileNameBuffer, L"%08d.ogg", uiSequence);
		break;
	}

	return fileNameBuffer;
}

bool CMV6Pack::UnPackAllRes()
{
	if (m_ifsCMV.is_open())
	{
		std::wstring fileName;
		for (CMV6IndexDescriptor& descriptor: m_vecDescriptor)
		{
			fileName = MakeFileName(descriptor.uiSequence,descriptor.uiType);
			WriteRes(fileName, descriptor.uiOffset + m_Header.uiResSecOffset, descriptor.uiCmpSize);

		}
		return true;
	}

	return false;
}

bool CMV6Pack::UnPackSingleRes(unsigned int uiSequence)
{
	if (uiSequence <= m_Header.uiResCount)
	{
		std::wstring fileName = MakeFileName(uiSequence, m_vecDescriptor[uiSequence].uiType);

		return WriteRes(fileName, m_vecDescriptor[uiSequence].uiOffset + m_Header.uiResSecOffset, m_vecDescriptor[uiSequence].uiCmpSize);
	}

	return false;
}

char* CMV6Pack::GetResToBuffer(size_t posRes, size_t szRes)
{
	if (m_ifsCMV.is_open())
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
			return nullptr;
		}

		return m_pRes;
	}

	return nullptr;
}

bool CMV6Pack::WriteRes(std::wstring wsRes, size_t posRes, size_t szRes)
{
	m_ofsRES.open(m_wsPath + wsRes, std::ios::binary);
	if (m_ofsRES.is_open())
	{
		m_ofsRES.write(GetResToBuffer(posRes, szRes), szRes);

		m_ofsRES.flush();
		m_ofsRES.close();

		return true;
	}

	return false;
}