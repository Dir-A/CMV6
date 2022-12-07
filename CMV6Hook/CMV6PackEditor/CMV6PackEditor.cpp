#include "CMV6PackEditor.h"
#include <Windows.h>
#include <string>


namespace CMV6File
{
	//*************************
	//*      CMV6Editor       *
	//*************************
	CMV6Editor::CMV6Editor() :
		m_szData(0),
		m_szAllocMax(0),
		m_pRes(nullptr),
		m_Header({ 0 })
	{

	}

	CMV6Editor::~CMV6Editor()
	{
		if (m_fsCMV.is_open())
		{
			m_fsCMV.close();
		}

		if (m_pRes)
		{
			delete[] m_pRes;
		}
	}

	//*************************
	//*      CMV6Unpack       *
	//*************************
	CMV6Unpack::CMV6Unpack(std::wstring wsCMV, std::wstring wsPath)
	{
		m_wsCMV = wsCMV;
		m_wsPath = wsPath;
		if (m_wsPath[m_wsPath.size()] != L'\\')
		{
			m_wsPath.append(L"\\");
		}

		m_fsCMV.open(wsCMV, std::ios::in | std::ios::binary);
		if (m_fsCMV.is_open())
		{
			InitPackInfo();
		}
	}

	void CMV6Unpack::InitPackInfo()
	{
		//Read File Header
		m_fsCMV.read((char*)&m_Header, sizeof(CMV6FileHeader));

		//Read Resources Index
		size_t szIndex = (m_Header.uiResMaxSequence + 1) * sizeof(CMV6IndexDescriptor);
		CMV6IndexDescriptor* pDescriptor = nullptr;
		char* pIndex = new char[szIndex];
		m_fsCMV.read(pIndex, szIndex);
		pDescriptor = (CMV6IndexDescriptor*)pIndex;
		if (pDescriptor)
		{
			for (size_t iteIndex = 0; iteIndex <= m_Header.uiResMaxSequence; iteIndex++)
			{
				m_szData += (pDescriptor + iteIndex)->uiCmpSize;
				m_vecDescriptor.push_back(*(pDescriptor + iteIndex));
			}
			pDescriptor = nullptr;
			delete[] pIndex;
		}
	}

	inline std::wstring CMV6Unpack::MakeFileName(unsigned int uiSequence, unsigned int uiType)
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

	bool CMV6Unpack::UnPackAllRes()
	{
		if (m_fsCMV.is_open())
		{
			std::wstring fileName;
			for (CMV6IndexDescriptor& descriptor : m_vecDescriptor)
			{
				fileName = MakeFileName(descriptor.uiSequence, descriptor.uiType);
				WriteRes(fileName, descriptor.uiOffset + m_Header.uiResSecOffset, descriptor.uiCmpSize);

			}
			return true;
		}

		return false;
	}

	bool CMV6Unpack::UnPackSingleRes(unsigned int uiSequence)
	{
		if (uiSequence <= m_Header.uiResMaxSequence)
		{
			std::wstring fileName = MakeFileName(uiSequence, m_vecDescriptor[uiSequence].uiType);

			return WriteRes(fileName, m_vecDescriptor[uiSequence].uiOffset + m_Header.uiResSecOffset, m_vecDescriptor[uiSequence].uiCmpSize);
		}

		return false;
	}

	char* CMV6Unpack::GetResToBuffer(size_t posRes, size_t szRes)
	{
		if (m_fsCMV.is_open())
		{
			m_fsCMV.seekg(posRes);

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
				m_fsCMV.read(m_pRes, szRes);
			}
			else
			{
				return nullptr;
			}

			return m_pRes;
		}

		return nullptr;
	}


	bool CMV6Unpack::WriteRes(std::wstring wsRes, size_t posRes, size_t szRes)
	{
		m_fsRES.open(m_wsPath + wsRes, std::ios::out | std::ios::trunc | std::ios::binary);
		if (m_fsRES.is_open())
		{
			m_fsRES.write(GetResToBuffer(posRes, szRes), szRes);

			m_fsRES.flush();
			m_fsRES.close();

			return true;
		}

		return false;
	}


	//*************************
	//*       CMV6Pack        *
	//*************************
	CMV6Pack::CMV6Pack(std::wstring wsPath)
	{
		m_wsPath = wsPath;
		m_fsCMV.open(m_wsPath, std::ios::out | std::ios::trunc | std::ios::binary);
		m_pRes = new char[0x374000];
	}

	void CMV6Pack::AddRes(CMV6IndexDescriptor* lpDescriptor, std::wstring wsResPath)
	{
		m_szData += lpDescriptor->uiCmpSize;
		m_vecAddResInfo.push_back({ *lpDescriptor, wsResPath });
	}

	void CMV6Pack::MakeNewPack()
	{
		if (m_fsCMV.is_open() && m_pRes)
		{
			m_Header.uiResMaxSequence = m_vecAddResInfo.size() - 1;
			m_Header.uiResSecOffset = (m_Header.uiResMaxSequence + 1) * 0x14 + sizeof(CMV6FileHeader);
			m_Header.uiCMVFileSize = m_Header.uiResSecOffset + m_szData;

			m_fsCMV.write((char*)&m_Header, sizeof(CMV6FileHeader));

			size_t posResSecOffset = 0;
			for (AddResInfo& iteInfo : m_vecAddResInfo)
			{
				iteInfo.Descriptor.uiOffset = posResSecOffset;
				posResSecOffset += iteInfo.Descriptor.uiCmpSize;

				m_fsCMV.write((char*)&iteInfo.Descriptor, sizeof(CMV6IndexDescriptor));
			}

			for (AddResInfo& iteInfo : m_vecAddResInfo)
			{
				m_fsRES.open(iteInfo.wsResPath, std::ios::in | std::ios::binary);
				if (m_fsRES.is_open())
				{
					m_fsRES.read(m_pRes, iteInfo.Descriptor.uiCmpSize);
					m_fsCMV.write(m_pRes, iteInfo.Descriptor.uiCmpSize);

					m_fsRES.close();
				}
			}


			m_fsCMV.flush();
		}
	}
}
