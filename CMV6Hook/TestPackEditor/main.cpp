#include <iostream>
#include "..\CMV6PackEditor\CMV6PackEditor.h"
#include "..\libwebp\encode.h";
#pragma comment(lib,"..\\libwebp\\libwebp.lib")

void EncodeWebP()
{
	std::ifstream ifsBitMap(L"00000299.bmp", std::ios::binary);
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

		std::ofstream oWebp(L"1.webp", std::ios::binary);
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

int main()
{
	//CMV6Pack cmv6(L"c1.cmv",L"Unpack");
	//cmv6.UnPackSingleRes(10000000);
}