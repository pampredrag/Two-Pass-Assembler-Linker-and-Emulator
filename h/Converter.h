#pragma once
#include <iostream>
#include <string>


class Converter {
public:
	static std::string DecimalToHexa(short int);
	static unsigned int HexaToDecimal(std::string);
	static std::string DecimalToBinary(int d);
	static void BinaryToDecimal();

	static std::string BinaryToHexa(std::string);
	static std::string HexaToBinary(std::string);

	static std::string * ugradi(std::string vrednost, int brojBajtova);

private:
	static std::string  GetHexFromBin(std::string);
};