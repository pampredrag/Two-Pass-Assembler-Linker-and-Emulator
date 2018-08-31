#pragma once

#include "asm.h"
#include <regex>
#include <thread>
#include <mutex>

const int memory_size = 65536;
class GlobSimb {
public:
	string ime;
	string vrednost; //vrednost je u hexa oblikku
	int definisan = 0;

};

class Fajl {
public:
	int pocetak;
	int duzina;
	int kraj;
	Fajl() {};
	Fajl(int p, int d) : pocetak(p), kraj(d) { duzina = kraj - pocetak; }

};

class Maske {
public:
	unsigned int mask1 = 1;
	unsigned int mask2 = 1 << 1;
	unsigned int mask3 = 1 << 2;
	unsigned int mask4 = 1 << 3;
	unsigned int mask5 = 1 << 4;
	unsigned int mask13 = 1 << 12;
	unsigned int mask16 = 1 << 15;
	unsigned int mask32 = 1 << 31;

	
	unsigned int unmask1;
	unsigned int unmask2;
	unsigned int unmask3;
	unsigned int unmask4;
	unsigned int unmask5;
	unsigned int unmask16;
	unsigned int unmask13;
	Maske() {
		unmask1 = mask16 + mask13 + mask5 + 14;
		unmask2 = mask16 + mask13 + mask5 + 13;
		unmask3 = mask16 + mask13 + mask5 + 11;
		unmask4 = mask16 + mask13 + mask5 + 7;
		unmask5 = mask16 + mask13 + 15;
		unmask16 = mask5 + 15;  //31
		unmask13 = mask16 + 31; 
	}
};

class Procesor {
	thread tastatura;
	static	mutex Mutex;
	static mutex Kraj;
	const string al = "11";
	const string eq = "00";
	const string ne = "01";
	const string gt = "10";

	int entryPoint;
	int * mem;
	int n = 0;
	map <string, ReallocationSimbol * > reallocText;
	map<string, ReallocationSimbol * > reallocData;
	map<string, ReallocationSimbol * > reallocRoData;
	map<string, GlobSimb *> mapaGlobSimbola;
	vector <Fajl *> duzineFajlova;
	map<string, short int> registri;
   short int psw;
	unsigned short int sp;
	Maske * maske;
	map<string, string> instrukcije;
 unsigned short int ulazPeriferije; // na ovoj adresi je znak koji je unet!
 unsigned short int izlazPeriferije; //ovo treba da se stampa!, sa te adrese ispis

	const int timeSlice= 50;
	const int memTime = 5;
	const int regTime = 2;
	int curTimeSlice = 0;

	static char charKeyboard; // ono sto je tastatura unela!
	static int flagKeyboard; //flag


public:
	Procesor();
	~Procesor();
	void fajlZaLinker(ifstream & ulaz, ifstream & mem);
	void linker();
	void ucitajRelZapise(ifstream &);
	void emulacija();
	


	

private:

	int proveriPreklapanje(Fajl * novi);
	void timer(int );
	void nekorektnaIns();
	void startniPrekid();
	void tastaturaPrekid();
	int izvrsiInstrukciju(string ins, string adr1, string reg1, string adr2, string reg2);
	void ucitajMemoriju(ifstream & ulaz, int pocetak, int kraj, int pocetakBss, int krajBss);
	void trebaIspis();
	
	void pomocnaZaRetData(ifstream & ulaz, regex r);

	static void runTast();

};
