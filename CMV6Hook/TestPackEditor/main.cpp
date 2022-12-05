#include <iostream>
#include "..\CMV6PackEditor\CMV6PackEditor.h"

int main()
{
	CMV6Pack cmv6(L"c1.cmv",L"Unpack");
	cmv6.UnPackSingleRes(10000000);
}