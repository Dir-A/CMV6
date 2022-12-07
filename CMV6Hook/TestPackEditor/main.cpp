#include <iostream>
#include "..\CMV6PackEditor\CMV6PackEditor.h"
#include "..\libwebp\encode.h";
#pragma comment(lib,"..\\libwebp\\libwebp.lib")

void EncodeWebP()
{
	std::fstream ifsBitMap(L"00000299.bmp", std::ios::binary);
	if (ifsBitMap.is_open())
	{
		size_t szBitMap = 0;
		ifsBitMap.seekg(0x22);
		ifsBitMap.read((char*)&szBitMap, 0x4);

		ifsBitMap.seekg(0x36);
		char* pRawData = new char[szBitMap];
		if (pRawData)
		{
			ifsBitMap.read(pRawData, szBitMap);
		}
		else
		{
			return;
		}

		//Destroy Alpha Channel
		for (size_t iteBitMap = 3; iteBitMap < szBitMap; iteBitMap++)
		{
			pRawData[iteBitMap] = 0xFF;
			iteBitMap += 3;
		}

		uint8_t* pWebP = nullptr;
		size_t sz = WebPEncodeBGRA((uint8_t*)pRawData, 1280, 720, 0x1400, 70, &pWebP);

		std::fstream oWebp(L"1.webp", std::ios::binary);
		if (oWebp.is_open())
		{
			oWebp.write((char*)pWebP, sz);

			oWebp.flush();
			oWebp.close();
		}

		WebPFree(pWebP);
		delete[] pRawData;
		ifsBitMap.close();
	}
}

unsigned int GetFileSize(std::ifstream& fsFile)
{
	std::streamoff szFile = 0;
	std::streamoff oldOff = fsFile.tellg();

	fsFile.seekg(0, std::ios::end);
	szFile = fsFile.tellg();
	fsFile.seekg(oldOff, std::ios::beg);

	return (unsigned int)szFile;
}

void PackCMV()
{
	size_t imagePerRawSize = 0x2A3000;
	size_t imageFileCount = 2907;
	wchar_t oggFileName[] = L"00002907.ogg";
	wchar_t imageFileNameFormat[] = L"%08d.jbpd";
	wchar_t imageFilePath[] = L"Unpack\\";



	CMV6File::CMV6Pack newPack(L"new.cmv");


	//Init File Header
	newPack.m_Header.aSignature[0] = 'C';
	newPack.m_Header.aSignature[1] = 'M';
	newPack.m_Header.aSignature[2] = 'V';
	newPack.m_Header.aSignature[3] = '6';

	newPack.m_Header.uiFrameRate = 24;
	newPack.m_Header.uiFrameRateTime = 1;
	newPack.m_Header.uiImageWidth = 1280;
	newPack.m_Header.uiImageHight = 720;
	newPack.m_Header.uiBitCount = 24;
	newPack.m_Header.uiAudioFlag = 2;

	//AddRes
	CMV6File::CMV6IndexDescriptor descriptor = { 0 };
	descriptor.uiType = 2;
	wchar_t buffer[0xFF] = { 0 };
	std::wstring fileName = imageFilePath;
	std::ifstream iFile;
	for (size_t iteFile = 0; iteFile < imageFileCount; iteFile++)
	{
		swprintf_s(buffer, imageFileNameFormat, iteFile);
		iFile.open(fileName + buffer);

		if (iFile.is_open())
		{
			descriptor.uiSequence = iteFile;
			descriptor.uiCmpSize = GetFileSize(iFile);
			descriptor.uiOrgSize = imagePerRawSize;
			newPack.AddRes(&descriptor, fileName + buffer);
			iFile.close();
		}
	}

	descriptor.uiSequence = imageFileCount;
	iFile.open(fileName + oggFileName);
	descriptor.uiType = 0;
	descriptor.uiCmpSize = GetFileSize(iFile);
	descriptor.uiOrgSize = descriptor.uiCmpSize;
	newPack.AddRes(&descriptor, fileName + oggFileName);
	iFile.close();

	//MakePack
	newPack.MakeNewPack();
}

int main()
{
	PackCMV();
}