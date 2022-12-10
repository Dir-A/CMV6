#include <iostream>
#include "..\CMV6PackEditor\CMV6PackEditor.h"
#include "..\libs\libwebp\encode.h";
#include "..\libs\libwebp\decode.h";
#pragma comment(lib,"..\\libs\\libwebp\\libwebp.lib")

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

void DecodeWebP()
{
	size_t szFile = 0x1401C;
	std::ifstream ifsWebP(L"1.webp", std::ios::binary);
	if (ifsWebP.is_open())
	{
		char* pWebP = new char[szFile];
		ifsWebP.read(pWebP, szFile);

		int width = 1280;
		int hight = 720;
		uint8_t* pBitMap = WebPDecodeBGRA((uint8_t*)pWebP, szFile, &width, &hight);
		WebPFree(pBitMap);

		std::ofstream ofsBitMap(L"1.raw",std::ios::binary);
		ofsBitMap.write((char*)pBitMap, 1280 * 4 * 720);
		ofsBitMap.flush();
		ofsBitMap.close();


		delete[] pWebP;
		ifsWebP.close();
	}
}

void PackCMV()
{
	size_t resRawSize = 0x2A3000;
	size_t resCount = 2907;
	std::wstring resPath = L"Dump\\";

	//Create Pack OBJ
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
	std::wstring resName;
	std::ifstream ifsRes;
	descriptor.uiResType = 2;
	//Processing JBPX
	for (size_t iteFile = 0; iteFile < resCount; iteFile++)
	{
		resName = resPath + newPack.MakeFileName(iteFile, 1);
		ifsRes.open(resName);

		if (ifsRes.is_open())
		{
			descriptor.uiSequence = iteFile;
			descriptor.uiCmpSize = newPack.GetFileSize(ifsRes);
			descriptor.uiOrgSize = resRawSize;
			newPack.AddRes(descriptor, resName);
			ifsRes.close();
		}
	}

	//Processing Ogg
	resName = resPath + newPack.MakeFileName(resCount, 0);
	ifsRes.open(resName);
	if (ifsRes.is_open())
	{
		descriptor.uiResType = 0;
		descriptor.uiSequence = resCount;
		descriptor.uiCmpSize = newPack.GetFileSize(ifsRes);
		descriptor.uiOrgSize = descriptor.uiCmpSize;

		newPack.AddRes(descriptor, resName);
		ifsRes.close();
	}


	//MakePack
	newPack.MakeNewPack();
}

int main()
{
	DecodeWebP();
	//EncodeWebP();
	//PackCMV();
}