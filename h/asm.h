#pragma once


#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <map> //mozda unorderd map ali to je u osu2 nesto zajebavalo ljude???  ovo je kao stablo pretrazivanja, slozenost O(logn)

using namespace std;

class Mapa;
typedef unsigned int LG;
const LG l = 0;
const LG g = 1;

typedef unsigned tipAdr;
const tipAdr regdir = 0;
const tipAdr regindpom = 1;
const tipAdr memdir = 2;
const tipAdr immed = 3;
const tipAdr regind = 4;


typedef string RelAbs;
const RelAbs relative = "R_386_32_PC32";
const RelAbs absolute = "R_386_32";


struct Ret1 { // dobio si ovakvu liniju :    r1 ,  r2[100]
	tipAdr adr; //u adr ide regdir
	string s;  //   r1  
	string linija; //,   r2[100]
	Ret1(tipAdr adr, string s,string linija) {
		this->adr = adr; this->s = s;
		this->linija = linija;
	}
};

struct Simbol {
	string name;
	string sekcija; // odluci se za jedan od ova dva
	int sekcijaRB;  //odluci se za jedan od ova dva
	int vrednost;
	LG lg; // local or global;
	int rb;

	int duzina; // samo ako je sekcija
	int pocetak;//samo ako je sekcija
	int kraj;//samo ako je sekcija

	Simbol() {

	}
	Simbol(string name, string sekcija, int vrednost, LG lg, int rb) {
		this->name = name;
		this->sekcija = sekcija;
		this->vrednost = vrednost;
		this->lg = lg;
		this->rb = rb;
	}

	~Simbol() {

	}

	


};

struct Instrukcija {
	string name;
	string opcode; //ovo je 4bita samo, da li koristiti ono iz osa 2, sto samo 4 bita upisuje?
	int length; //u bajtovima, ali ovo je verovatno visak

	Instrukcija() {

	}
	Instrukcija(string name, string opcode, int length):name(name),opcode(opcode),length(length) {

	}
	Instrukcija(string name, string opcode) {
		this->name = name;
		this->opcode = opcode;
	}

	~Instrukcija() {

	}
};


struct ReallocationSimbol {
	string adresa;
	RelAbs tip;

	int rb;
	string imeRB;

	ReallocationSimbol() {

	}

	~ReallocationSimbol() {

	}

	ReallocationSimbol(string adresa, RelAbs tip, int rb, string imeRB) {
		this->adresa = adresa;
		this->tip = tip;
		this->rb = rb;
		this->imeRB = imeRB;
	}



};




class Asembler {


public:

	Asembler();
	~Asembler();
	void prviProlaz(ifstream & ulaz,int pocetak);
	void drugiProlaz(ifstream & ulaz,int pocetak);
	void writeSimbolTable(ofstream & izlaz);
	void writeRealocatioTables(ofstream & izlaz);
	void writeInitContent(ofstream & izlaz);
	void writeMemory(ofstream & izlaz);

private:


	ReallocationSimbol * noviRelSimb(string ime, int adresa, string sekcija, RelAbs x);

	Ret1 odrediTipAdresiranja(string linija);

	string opCodeAdr(tipAdr a);

	string registar(int d);

	string getConditionBits(string s);

	int pocetakTrenutnogFajla = 0;
	int krajTrenutnogFajla = 0;
	int duzinaTrenutnogFajla = 0;

	int pocetakBssSekcije = -1;
	int krajBssSekcije = -1;


private:
	Mapa * mapIns;
	map<string, Instrukcija *> mapaIns;
	//ifstream & ulaz;
	int rbCnt = 5; //preskoci od 1-4 za sekcija
	int locationCnt;
	map <string, Simbol *> tabelaSimbola;

	map <string, ReallocationSimbol * > reallocText;
	map<string, ReallocationSimbol * > reallocData;
	map<string, ReallocationSimbol * > reallocRoData;


	vector <string> codeText; //jedan string ce biti jedan bajt
	vector <string> codeData; //jedan string ce biti jedan bajt
	vector <string> codeRodata;


    vector <string> Memorija; //jedan string jedan bajt, posle svakog bajta ce biti razmak zbog lakseg citanja posle






};


