#include "CBigStorage.h"

int main(int argc, char *argv[])
{
	BigStorage::CArray<double> oValue1(3);

	oValue1[0] = 15.467;
	oValue1[1] = 63423.132336;
	oValue1[2] = 43453.123;

	std::wcout << oValue1[1] << std::endl;

	BigStorage::CMatrix<double> oValue2(3, 3);

	oValue2[2][1] = 125.516;
	oValue2[0][6] = 31.161;

	std::wcout << oValue2[2][1] << std::endl;
	std::wcout << oValue2[0][6] << std::endl;

	return 0;
}
