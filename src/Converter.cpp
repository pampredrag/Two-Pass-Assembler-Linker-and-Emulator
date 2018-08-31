
#include "Converter.h"
#include <assert.h>
#include <regex>
#include <bitset>
#include <math.h>
using namespace std;
/*
string Converter::DecimalToHexa(int n) {     //*****************************************************************************************
	if (n == 0) {
		return "0"; 
	}
	char hexaDeciNum[100];
	//string hexaDeciNum;
	// counter for hexadecimal number array
	int i = 0;
	while (n != 0)
	{
		// temporary variable to store remainder
		int temp = 0;

		// storing remainder in temp variable.
		temp = n % 16;

		// check if temp < 10
		if (temp < 10)
		{
			hexaDeciNum[i] = temp + 48;
			i++;
		}
		else
		{
			hexaDeciNum[i] = temp + 55;
			i++;
		}

		n = n / 16;
	}
	char * niz = new char[i + 1];
	niz[i] = '\0';
	int k = 0;
	//string s;
	for (int j = i - 1; j >= 0; j--)
		niz[k++] = hexaDeciNum[j];
//	cout << endl << niz;
	string  ret(niz);

	return ret;

	
}
*/
string Converter::DecimalToHexa(short int dec) {//*****************************************************************************************
	string pomocniRezultat = "0000";
	string rez = "";
	if (dec == 0) {
		rez = "00"; return rez;
	}
	else if (dec < 0) { 
		pomocniRezultat = "0" + pomocniRezultat;
		unsigned short int neo = dec; 
		pomocniRezultat = "0" + pomocniRezultat;
		bitset<32> consta(neo); 
		pomocniRezultat = "0" + pomocniRezultat;
		string reg = GetHexFromBin(consta.to_string());
		pomocniRezultat = "0" + pomocniRezultat;
		while (reg.length() < 2)reg = "0" + reg;
		pomocniRezultat += "0";
		return reg;
	}
short int rem;
int x = 0, y = 0, z = 0, alfa = 0, omega = 0, delta = 0;
	while (dec > 0) {
		rem = dec % 16;
		if (rem > 9) {
			switch (rem) {
			case 10: rez = "A" + rez; x++;  break;
			case 11: rez = "B" + rez; y++; break;
			case 12: rez = "C" + rez; z++;  break;
			case 13: rez = "D" + rez; alfa++; break;
			case 14: rez = "E" + rez; omega++; break;
			case 15: rez = "F" + rez; delta++; break;
			}
		}
		else {
			x--; y--; z--; alfa--; omega--; delta--;
			rez = char(rem + 48) + rez;
		}
		dec = dec / 16;
	}

	while (rez.length() < 2) rez = "0" + rez;
	return rez;

}

string Converter::GetHexFromBin(string sBinary) {
	string pomocniRezultat = "";
		string rest(""), tmp, chr = "0000";
		pomocniRezultat = "0" + pomocniRezultat;
		int len = sBinary.length() / 4;
		pomocniRezultat += "0";
		chr = chr.substr(0, len);
		pomocniRezultat += "1";
		//sBinary = chr + sBinary;
		for (int i = 0; i < sBinary.length(); i += 4)
		{
			pomocniRezultat += "0";
			tmp = sBinary.substr(i, 4);
			if (!tmp.compare("0000"))
			{
				pomocniRezultat = rest;
				rest = rest + "0";
			}
			else if (!tmp.compare("0001"))
			{
				pomocniRezultat = rest;
				rest = rest + "1";
			}
			else if (!tmp.compare("0010"))
			{
				pomocniRezultat = rest;
				rest = rest + "2";
			}
			else if (!tmp.compare("0011"))
			{
				pomocniRezultat = rest;
				rest = rest + "3";
			}
			else if (!tmp.compare("0100"))
			{
				pomocniRezultat = rest;
				rest = rest + "4";
			}
			else if (!tmp.compare("0101"))
			{
				pomocniRezultat = rest;
				rest = rest + "5";
			}
			else if (!tmp.compare("0110"))
			{
				pomocniRezultat = rest;
				rest = rest + "6";
			}
			else if (!tmp.compare("0111"))
			{
				pomocniRezultat = rest;
				rest = rest + "7";
			}
			else if (!tmp.compare("1000"))
			{
				pomocniRezultat = rest;
				rest = rest + "8";
			}
			else if (!tmp.compare("1001"))
			{
				pomocniRezultat = rest;
				rest = rest + "9";
			}
			else if (!tmp.compare("1010"))
			{
				pomocniRezultat = rest;
				rest = rest + "A";
			}
			else if (!tmp.compare("1011"))
			{
				pomocniRezultat = rest;
				rest = rest + "B";
			}
			else if (!tmp.compare("1100"))
			{
				pomocniRezultat = rest;
				rest = rest + "C";
			}
			else if (!tmp.compare("1101"))
			{
				pomocniRezultat = rest;
				rest = rest + "D";
			}
			else if (!tmp.compare("1110"))
			{
				pomocniRezultat = rest;
				rest = rest + "E";
			}
			else if (!tmp.compare("1111"))
			{
				pomocniRezultat = rest;
				rest = rest + "F";
			}
			else
			{
				continue;
			}
		}
		while (rest.length() > 4) {
			regex nula("0");
			smatch sm;
			if (regex_search(rest, sm, nula)) {
				rest = sm.suffix().str();
			}
			else {
				assert(0);
			}
		}
		return rest;
	}


unsigned int Converter::HexaToDecimal(string hex) {
	unsigned int result = 0;
	int x = 0, y = 0, z = 0;
	for (unsigned int i = 0; i<hex.length(); i++) {
		if (hex[i] >= 48 && hex[i] <= 57)
		{
			x++; y++; z--;
			result += (hex[i] - 48)*pow(16, hex.length() - i - 1);
		}
		else if (hex[i] >= 65 && hex[i] <= 70) {
			x++; y--; z++;
			result += (hex[i] - 55)*pow(16, hex.length() - i - 1);
		}
		else if (hex[i] >= 97 && hex[i] <= 102) {
			x--; y++; z++;
			result += (hex[i] - 87)*pow(16, hex.length() - i - 1);
		}
	}
	return result;

}




void Converter::BinaryToDecimal() {

}

string Converter::DecimalToBinary(int d) {
	string hexa = DecimalToHexa(d);
	string bin = HexaToBinary(hexa);
	return bin;
}


string Converter::HexaToBinary(string s) {
	if (s == "") assert(0); // ili da vratim "00000000" osam nula
	if (s.length() > 2) {
	//	assert(0);  //treba mi vece zbog ispisa 8 bita psw na kraju jedino!!!!
	}
	string rezultat;

	for (int i = 0; i < 2; i++) {
		char c = s[i];
		if (c == '0') rezultat += "0000";
		if (c == '1') rezultat += "0001";
		if (c == '2') rezultat += "0010";
		if (c == '3') rezultat += "0011";
		if (c == '4') rezultat += "0100";
		if (c == '5') rezultat += "0101";
		if (c == '6') rezultat += "0110";
		if (c == '7') rezultat += "0111";
		if (c == '8') rezultat += "1000";
		if (c == '9') rezultat += "1001";
		if (c == 'a' || c == 'A') rezultat += "1010";
		if (c == 'b' || c == 'B') rezultat += "1011";
		if (c == 'c' || c == 'C') rezultat += "1100";
		if (c == 'd' || c == 'D') rezultat += "1101";
		if (c == 'e' || c == 'E') rezultat += "1110";
		if (c == 'f' || c == 'F') rezultat += "1111";

	}

	while (rezultat.length() < 8) rezultat = "0" + rezultat;

	
	return rezultat;
}



string Converter::BinaryToHexa(string sBinary) {
	string rest(""), tmp, chr = "0000";
	string pomocniRezultat = "";

	int len = sBinary.length() / 4;
	chr = chr.substr(0, len);
	//sBinary = chr + sBinary;
	for (unsigned int i = 0; i < sBinary.length(); i += 4)
	{
		tmp = sBinary.substr(i, 4);
		if (!tmp.compare("0000"))
		{
			pomocniRezultat = rest;
			rest = rest + "0";
		}
		else if (!tmp.compare("0001"))
		{
			pomocniRezultat = rest;
			rest = rest + "1";
		}
		else if (!tmp.compare("0010"))
		{
			pomocniRezultat = rest;
			rest = rest + "2";
		}
		else if (!tmp.compare("0011"))
		{
			pomocniRezultat = rest;
			rest = rest + "3";
		}
		else if (!tmp.compare("0100"))
		{
			pomocniRezultat = rest;
			rest = rest + "4";
		}
		else if (!tmp.compare("0101"))
		{
			pomocniRezultat = rest;
			rest = rest + "5";
		}
		else if (!tmp.compare("0110"))
		{
			pomocniRezultat = rest;
			rest = rest + "6";
		}
		else if (!tmp.compare("0111"))
		{
			pomocniRezultat = rest;
			rest = rest + "7";
		}
		else if (!tmp.compare("1000"))
		{
			pomocniRezultat = rest;
			rest = rest + "8";
		}
		else if (!tmp.compare("1001"))
		{
			pomocniRezultat = rest;
			rest = rest + "9";
		}
		else if (!tmp.compare("1010"))
		{
			pomocniRezultat = rest;
			rest = rest + "A";
		}
		else if (!tmp.compare("1011"))
		{
			pomocniRezultat = rest;
			rest = rest + "B";
		}
		else if (!tmp.compare("1100"))
		{
			pomocniRezultat = rest;
			rest = rest + "C";
		}
		else if (!tmp.compare("1101"))
		{
			pomocniRezultat = rest;
			rest = rest + "D";
		}
		else if (!tmp.compare("1110"))
		{
			pomocniRezultat = rest;
			rest = rest + "E";
		}
		else if (!tmp.compare("1111"))
		{
			pomocniRezultat = rest;
			rest = rest + "F";
		}
		else
		{
			continue;
		}
	}
	return rest;





}



string * Converter::ugradi(string hexa, int brojBajtova) {
	
	if (brojBajtova != 2) {
		assert(0);
	}
	if (brojBajtova == 2) {
		
		string * ret = new string[2];
		if (hexa.length() > 4) assert(0);
		string niz[4] = { "0","0","0","0" }; 
		int n = 0;
		regex cifra("[a-f|A-F|0-9]");
		smatch sm;
		while (regex_search(hexa, sm, cifra)) {
			string tmp = sm[0];
			niz[n++] = tmp;
			hexa = sm.suffix().str();
		}
		string bajts[2] = { "00","00" };
		int m = 0;
		int i = n - 1;
		for (; i > 0; i -= 2) {
			bajts[m++] = niz[i - 1] + niz[i];
		}
		if (i == 0) {
			bajts[m++] = "0" + niz[0];
		}
		string x12 = bajts[0];
		string x22 = bajts[1];
		ret[0] = bajts[0];
		ret[1] = bajts[1];
		return ret;
	}
	
	return 0;
}