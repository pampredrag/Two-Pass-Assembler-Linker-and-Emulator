
#include "cpu.h"
#include "Converter.h"
#include <regex>
#include <assert.h>
#include <iomanip>
#include <conio.h>

char Procesor::charKeyboard;
int Procesor::flagKeyboard = 0;
mutex Procesor::Mutex;
mutex Procesor::Kraj;
Procesor::Procesor() : tastatura(runTast) {
	mem = new int[memory_size];
	for (int i = 0; i < memory_size; i++) mem[i] = 0;
	maske = new Maske();
	registri["000"] = 0;
	registri["001"] = 0;
	registri["010"] = 0;
	registri["011"] = 0;
	registri["100"] = 0;
	registri["101"] = 0;
	short int x= Converter::HexaToDecimal("FF00");
	registri["110"] = x;
	sp = x;
	registri["111"] = 0;
	psw = 0;
//	psw |= maske->mask1; //setovan zero bit jer je nula u akumulatoru

	//popuni mapu instrukcija
	instrukcije["0000"] = "add";
	instrukcije["0001"] = "sub";
	instrukcije["0010"] = "mov";
	instrukcije["0011"] = "div";
	instrukcije["0100"] = "cmp";
	instrukcije["0101"] = "and";
	instrukcije["0110"] = "or";
	instrukcije["0111"] = "not";
	instrukcije["1000"] = "test";
	instrukcije["1001"] = "push";
	instrukcije["1010"] = "pop";
	instrukcije["1011"] = "call";
	instrukcije["1100"] = "iret";
	instrukcije["1101"] = "mov";
	instrukcije["1110"] = "shl";
	instrukcije["1111"] = "shr";
	////

	ulazPeriferije = Converter::HexaToDecimal("FFFC"); // na ovoj adresi je znak koji je unet!
	izlazPeriferije = Converter::HexaToDecimal("FFFE");

	string * nizPom=Converter::ugradi(Converter::DecimalToHexa(100),2);
	mem[2] = Converter::HexaToDecimal(nizPom[0]);
	mem[3] = Converter::HexaToDecimal(nizPom[1]);

    nizPom = Converter::ugradi(Converter::DecimalToHexa(200), 2);
	mem[4] = Converter::HexaToDecimal(nizPom[0]);
	mem[5] = Converter::HexaToDecimal(nizPom[1]);

	nizPom = Converter::ugradi(Converter::DecimalToHexa(302), 2);
	mem[0] = Converter::HexaToDecimal(nizPom[0]);
	mem[1] = Converter::HexaToDecimal(nizPom[1]);

	nizPom = Converter::ugradi(Converter::DecimalToHexa(170), 2); // zajedno u fajlu sa tajmerom!
	mem[6] = Converter::HexaToDecimal(nizPom[0]);
	mem[7] = Converter::HexaToDecimal(nizPom[1]);


	psw |= maske->mask13;
	psw |= maske->mask16; // dozvoli tajmer

	

}


Procesor::~Procesor() {
	delete[] mem;
	delete maske;
}

void Procesor::ucitajMemoriju(ifstream & ulaz,int pocetak,int kraj,int pocetakBss, int krajBss) {
	string a;
	int i = pocetak;
	while (ulaz >> a) {
		if (pocetak == pocetakBss) {
			pocetak = krajBss;
	   }
		int d = Converter::HexaToDecimal(a);
		mem[pocetak++] = d;

	}
	if (pocetak != kraj) {
		assert(0);
	}
//cout << Converter::DecimalToHexa(mem[0]) << endl;
	//cout << Converter::DecimalToHexa(mem[6]) << endl;
	//konacno imam celu memoriju kako treba! yeeeeeeee

}


void Procesor::ucitajRelZapise(ifstream & ulaz) {
	regex text("\.ret\.text");
	string linija;
std::getline(ulaz, linija);
	while (regex_search(linija, text) == false) {
	std::getline(ulaz, linija);
	}
std::getline(ulaz, linija);
std::getline(ulaz, linija);
std::getline(ulaz, linija);
regex alfanum("[a-z|A-Z|0-9]+");
	string a;
	ReallocationSimbol * novi = 0;
	regex nijeRazmak("[a-z|A-Z|0-9|\_|\.]+");
	while (regex_search(linija, alfanum)) {
		novi = new ReallocationSimbol();
		smatch sm;
		regex_search(linija, sm, nijeRazmak);
		string tmp = sm[0];
		linija = sm.suffix().str();
		novi->adresa = tmp;
		regex_search(linija, sm, nijeRazmak);
		tmp = sm[0];
		linija = sm.suffix().str();
		novi->tip = tmp;
		regex_search(linija, sm, nijeRazmak);
	     tmp = sm[0];
		linija = sm.suffix().str();
		novi->rb = stoi(tmp);

		regex_search(linija, sm, nijeRazmak);
		tmp = sm[0];
		linija = sm.suffix().str();
		novi->imeRB = tmp;

	//	reallocText[novi->adresa] = novi;
		int adresa = Converter::HexaToDecimal(novi->adresa);
		if (novi->imeRB != ".text" && novi->imeRB != ".data" && novi->imeRB != ".rodata" && novi->imeRB != ".bss") {
			//znaci zavisi od globalnog simbola
			if (novi->tip == absolute) {
				//apsolutna adresa
				GlobSimb * gs = mapaGlobSimbola[novi->imeRB];
				if (gs == 0 || gs->definisan == 0) {
					cout << "zavisi od nedefiisanog glob simbola nesto" << endl;
					assert(0);
				}
				//znaci samo ugradi sada vrednost simbola umesto onoga sto stoji u mem
				regex prvaDva("[0-9|a-f|A-F]{2}");
				string * niz;
				niz = Converter::ugradi(gs->vrednost,2);
				mem[adresa] = Converter::HexaToDecimal(niz[0]);
				mem[adresa+1] = Converter::HexaToDecimal(niz[1]);
			}
			else {
				//znaci realtivno je
				GlobSimb * gs = mapaGlobSimbola[novi->imeRB];
				if (gs == 0 || gs->definisan == 0) {
		
					cout << "zavisi od nedefiisanog glob simbola nesto 2" << endl;
					assert(0);
				}
				//znaci samo ugradi sada vrednost simbola umesto onoga sto stoji u mem
				regex prvaDva("[0-9|a-f|A-F]{2}");
				string * niz;
				int dec1 = Converter::HexaToDecimal(gs->vrednost);
				int dec2 = Converter::HexaToDecimal(novi->adresa);

				//znaci kako ces da sklopis ovo sto vec ims u memoriji?
				//moras da prvo napravis vrednost
				string lowB =Converter::DecimalToHexa( mem[adresa]);
				string highB = Converter::DecimalToHexa(mem[adresa + 1]);

				string vredsnot = highB + lowB;
				int dec3 = Converter::HexaToDecimal(vredsnot);
				short int dec3zamena = dec3;
				int noviZaU = dec1 - dec2 + dec3zamena;
				string x = Converter::DecimalToHexa(noviZaU);
				niz = Converter::ugradi(x, 2);
				mem[adresa] = Converter::HexaToDecimal(niz[0]);
				mem[adresa + 1] = Converter::HexaToDecimal(niz[1]);
			}
		}
		std::getline(ulaz, linija);
	}

	//ucitao si ret.text
	//sada sve ovo ponovi za ret.data 
	//-------------------------------------------------------------------------------------------------------------------------------
	regex data("\.ret\.data");
	pomocnaZaRetData(ulaz,data);

	regex rodata("\.ret\.rodata");
	pomocnaZaRetData(ulaz, rodata);
	

	//-------------------------------------------------------------------------------------------------------------------------------


}

void Procesor::fajlZaLinker(ifstream & ulaz, ifstream & mem) {
	string linija;

	std::getline(ulaz, linija);
	regex broj("[0-9|\-]+");
	smatch brojsm;
	int pocetak, duzina;
	int pocetakBss, krajBss;
	if (regex_search(linija, brojsm, broj)) {
		pocetak = stoi(brojsm[0]);
		linija = brojsm.suffix().str();
	}
	else {
		cout << "nema podataka o duzini fajla" << endl; assert(0);
	}
	if (regex_search(linija, brojsm, broj)) {
		duzina = stoi(brojsm[0]);
		linija = brojsm.suffix().str();
	}
	else {
		cout << "nema podataka o duzini fajla" << endl; assert(0);
	}

	Fajl * novi = new Fajl(pocetak, duzina);////////////// nije ovo duzina nego kraj ipak
	proveriPreklapanje(novi);

	if (regex_search(linija, brojsm, broj)) {
		pocetakBss = stoi(brojsm[0]);
		linija = brojsm.suffix().str();
	}
	else {
		cout << "nema podataka o duzini bss" << endl; assert(0);
	}
	if (regex_search(linija, brojsm, broj)) {
		krajBss = stoi(brojsm[0]);
		linija = brojsm.suffix().str();
	}
	else {
		cout << "nema podataka o duzini bss" << endl; assert(0);
	}

	ucitajMemoriju(mem, novi->pocetak, novi->kraj, pocetakBss, krajBss);

	std::getline(ulaz, linija);
	std::getline(ulaz, linija);
	std::getline(ulaz, linija);
	std::getline(ulaz, linija);

	regex alfanum("[a-z|A-Z|0-9]");
	while (regex_search(linija, alfanum)) {
		//citaj simbol po simbol;
		regex nijeRazmak("[a-z|A-Z|0-9|\.]+");
		smatch sm;
		regex_search(linija, sm, nijeRazmak);
		string ime = sm[0];
		linija = sm.suffix().str();

		regex_search(linija, sm, nijeRazmak);
		string sekcija = sm[0];
		linija = sm.suffix().str();

		regex_search(linija, sm, nijeRazmak);
		string vrednost = sm[0];
		linija = sm.suffix().str();

		regex_search(linija, sm, nijeRazmak);
		string locGlob = sm[0];
		linija = sm.suffix().str();

		if (locGlob == "glo") {
			GlobSimb * novi = new GlobSimb();
			novi->ime = ime;
			if (sekcija == "UND") novi->definisan = 0;
			else {
				novi->definisan = 1;
				novi->vrednost = vrednost;

			}
			mapaGlobSimbola[ime] = novi;

		}


		std::getline(ulaz, linija);
	}
}

void Procesor::linker() {
	string linija;
	//napravljena je tabela simobla, obidji je sada da vidis da li ima ndef simbola i da li postoji start
	int flagEntryPoint = 0;

	map<string, GlobSimb*>::iterator iter;
	for (iter = mapaGlobSimbola.begin(); iter != mapaGlobSimbola.end(); iter++) {
		GlobSimb * tmp = iter->second;
		if (tmp->ime == "START") flagEntryPoint++;
		if (tmp->definisan == 0) {
			cout << "Nisu svi globalni simboli definisani" << endl;
			assert(0);
		}
	}

	if (flagEntryPoint == 0) {

		cout << "nije definisan simbol START" << endl;
		assert(0);
	}

	
}



int Procesor::proveriPreklapanje(Fajl * novi) {

	vector<Fajl *>::iterator i;
	for (i = duzineFajlova.begin(); i != duzineFajlova.end(); i++) {
		Fajl * tmp = (*i);
		if (novi->pocetak > tmp->pocetak && novi->pocetak < tmp->kraj) {
			cout << "Preklapanje fajlova1!!" << endl;
			assert(0);
		}
		if (novi->kraj > tmp->pocetak && novi->kraj < tmp->kraj) {
			cout << "Preklapanje fajlova2!!" << endl;
			assert(0);
		}
	}
	duzineFajlova.push_back(novi);
	return 0;
}


void Procesor::emulacija() {

	GlobSimb * start = mapaGlobSimbola["START"];
	registri["111"] = Converter::HexaToDecimal(start->vrednost);

	//sada skoci na onu startnu prekidnu rutinu
	startniPrekid();
	while (1) { //kako da znam do kada cu ovo da izvrsavam????????
    
		string bajt1 =Converter::DecimalToBinary( mem[registri["111"]]);
		string bajt2 = Converter::DecimalToBinary(mem[registri["111"] + 1]);
		int  xyPC = mem[registri["111"]];
		string uslov;
		string ins;
		string adr1;
		string reg1;
		string adr2;
		string reg2;

		smatch sm1;
		regex cifra("[0-1][0-1]");

		if (regex_search(bajt1, sm1, cifra)) {
			uslov = sm1[0];
			bajt1 = sm1.suffix().str();
		}
		else {
			assert(0);
		}
		int flagRadiDalje = 0;
		if (uslov == al) flagRadiDalje = 1;
		if (uslov == eq) {
			if (psw & maske->mask1) flagRadiDalje = 1;
		}
		if (uslov == ne) {
			if ((psw & maske->mask1) ==0 ) flagRadiDalje = 1;
		}
		if (uslov == gt) {
			if ((psw & maske->mask1) == 0)
				if ((psw & maske->mask4) == 0) flagRadiDalje++;
		}

		regex rins("[0-1]{4}");
		if (regex_search(bajt1, sm1, rins)) {
			ins = sm1[0];
			bajt1 = sm1.suffix().str();
		}
		else {
			assert(0);
		}

		if (regex_search(bajt1, sm1, cifra)) {
			adr1 = sm1[0];
			bajt1 = sm1.suffix().str();
		}
		else {
			assert(0);
		}

		regex triCif("[0-1]{3}");
		if (regex_search(bajt2, sm1, triCif)) {
			reg1 = sm1[0];
			bajt2 = sm1.suffix().str();
		}
		else {
			assert(0);
		}
		if (regex_search(bajt2, sm1, cifra)) {
			adr2 = sm1[0];
			bajt2 = sm1.suffix().str();
		}
		else {
			assert(0);
		}
		if (regex_search(bajt2, sm1, triCif)) {
			reg2= sm1[0];
			bajt2 = sm1.suffix().str();
		}
		else {
			assert(0);
		}
		string imeIns = instrukcije[ins];
		int duration = 0;
		if ( (adr1 == "01" && adr2 == "01" )|| (imeIns == "iret" )|| ((imeIns== "push" || imeIns=="pop" || imeIns == "call" ) && adr1 =="01" )){ // pazi ovde, ne menja ti se lepo PC uvek !!! ipak je bolje promeniti pc kada udjes u izvrsavanje ins!
			registri["111"] += 2;
			duration = regTime;
		}
		else {
			registri["111"] += 4;
			duration = memTime;
		}
		if (flagRadiDalje == 0) continue;
		
		if (imeIns != "pop" && imeIns != "push" && imeIns != "call" && imeIns != "jmp" && imeIns != "iret") {
			if (adr1 != "01" && adr2 != "01") {
				nekorektnaIns();
				continue;
			}
		}
	    
		//znaci sada proveravam koja mi je koja instrukcija

		int flagic=izvrsiInstrukciju(ins, adr1, reg1, adr2, reg2);
		trebaIspis();
		timer(duration);
		tastaturaPrekid();

		if (flagic == -1) {
			break;
			//kraj je;
		}
	}
	Mutex.lock();
	flagKeyboard = -1;
	Mutex.unlock();

	
	tastatura.join();
	//ispisi na konzolu sadrzaj registara samo!

	cout << endl << endl;
	cout << "r0=  " << registri["000"] << endl;
	cout << "r1=  " << registri["001"] << endl;
	cout << "r2=  " << registri["010"] << endl;
	cout << "r3=  " << registri["011"] << endl;
	cout << "r4=  " << registri["100"] << endl;
	cout << "r5=  " << registri["101"] << endl;
	cout << "r6=  " << (unsigned short int)registri["110"] << endl;
	cout << "r7=  " << registri["111"] << endl;
	cout << "psw=  " << psw <<  "\t \t" << "najnizih 8 bita "    ;
	cout << "psw=   " << Converter::DecimalToBinary(psw) << endl;

	
}

int Procesor::izvrsiInstrukciju(string ins1, string adr1, string reg1, string adr2, string reg2) {
	string ins = instrukcije[ins1];
	short int operand1;
	short int operand2;
	unsigned short int adresa1 = -1;
   unsigned short int adresa2 = -1;

	if (ins == "iret") {
		//onda samo pop psw, pop pc sa steka
		sp = registri["110"];
		string prviB = Converter::DecimalToHexa(mem[sp]);
		string drugiB = Converter::DecimalToHexa(mem[sp + 1]);
		sp += 2;
		
		operand1 = Converter::HexaToDecimal(drugiB + prviB);

		psw = operand1;

	    prviB = Converter::DecimalToHexa(mem[sp]);
		drugiB = Converter::DecimalToHexa(mem[sp + 1]);
		sp += 2;
		operand1 = Converter::HexaToDecimal(drugiB + prviB);
		registri["110"] = sp;
		registri["111"] = operand1;

		return 0;
	}

	

	if (adr1 == "01") {
		operand1 = registri[reg1];
	}
	else {
		if (adr1 == "11") {
			//regindpom
			adresa1 = registri[reg1];
			int pomeraj;
			string prviB = Converter::DecimalToHexa(mem[registri["111"] - 2]);
			string drugiB = Converter::DecimalToHexa(mem[registri["111"] - 1]);
			pomeraj = Converter::HexaToDecimal(drugiB + prviB);

			adresa1 += pomeraj;
			prviB = Converter::DecimalToHexa(mem[adresa1]);
			drugiB = Converter::DecimalToHexa(mem[adresa1 + 1]);

			operand1 = Converter::HexaToDecimal(drugiB + prviB);
		}
		else {
			if (adr1 == "10") {
				//memdir
				int pomeraj;
				string prviB = Converter::DecimalToHexa(mem[registri["111"] - 2]);
				string drugiB = Converter::DecimalToHexa(mem[registri["111"] - 1]);
				pomeraj = Converter::HexaToDecimal(drugiB + prviB);
				adresa1 = pomeraj;
				prviB = Converter::DecimalToHexa(mem[adresa1]);
				drugiB = Converter::DecimalToHexa(mem[adresa1 + 1]);

				operand1 = Converter::HexaToDecimal(drugiB + prviB);
			}
			else {
				//immed 
				if (ins != "call" && ins != "jmp" && ins != "push") {
					//assert(0); nemoj assert nego pozovi onaj prekid za nekorektnu instrukciju !
					nekorektnaIns();
					return 0;
				}

				int pomeraj;
				string prviB = Converter::DecimalToHexa(mem[registri["111"] - 2]);
				string drugiB = Converter::DecimalToHexa(mem[registri["111"] - 1]);
				pomeraj = Converter::HexaToDecimal(drugiB + prviB);
				operand1 = pomeraj;


			}
		}


	}


	if (ins == "push" || ins == "pop" || ins == "call") {

		if (ins == "push") {
			sp = registri["110"];
			sp -= 2;
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[sp] = Converter::HexaToDecimal(niz[0]);
			mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
			registri["110"] = sp;
			return 0;

		}

		if (ins == "pop") {
			sp = registri["110"];
			string prviB = Converter::DecimalToHexa(mem[sp]);
			string drugiB = Converter::DecimalToHexa(mem[sp + 1]);
			sp += 2;
			registri["110"] = sp;
			operand1 = Converter::HexaToDecimal(drugiB + prviB);
			if (adresa1 == 65535) {
				registri[reg1] = operand1;
			}
			else {
				string * niz1 = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
				mem[adresa1] = Converter::HexaToDecimal(niz1[0]);
				mem[adresa1 + 1] = Converter::HexaToDecimal(niz1[1]);
			}
			return 0;
		}
		if (ins == "call") {
			//znaci prvo nam treba push pc pa tek onda skok, pc je vec uvecan tako da je to super
			sp = registri["110"];
			sp -= 2;
			short int xyIP = registri["111"];
			string * niz = Converter::ugradi(Converter::DecimalToHexa(registri["111"]), 2);
			
			mem[sp] = Converter::HexaToDecimal(niz[0]);
			mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
			string x = niz[0];
			string y = niz[1];
			//sada skok, samo upisujem prvi operand u pc
			registri["110"] = sp;
			registri["111"] = operand1;
			return 0;

		}
	    
	}


	if (adr2 == "01") {
		operand2 = registri[reg2];
	}
	else {
		if (adr2 == "11") {
			//regindpom
			adresa2 = registri[reg2];
			int pomeraj;
			string prviB = Converter::DecimalToHexa(mem[registri["111"] - 2]);
			string drugiB = Converter::DecimalToHexa(mem[registri["111"] - 1]);
			pomeraj = Converter::HexaToDecimal(drugiB + prviB);

			adresa2 += pomeraj;
			prviB = Converter::DecimalToHexa(mem[adresa2]);
			drugiB = Converter::DecimalToHexa(mem[adresa2 + 1]);

			operand2 = Converter::HexaToDecimal(drugiB + prviB);
		}
		else {
			if (adr2 == "10") {
				//memdir
				int pomeraj;
				string prviB = Converter::DecimalToHexa(mem[registri["111"] - 2]);
				string drugiB = Converter::DecimalToHexa(mem[registri["111"] - 1]);
				pomeraj = Converter::HexaToDecimal(drugiB + prviB);
				adresa2 = pomeraj;
				prviB = Converter::DecimalToHexa(mem[adresa2]);
				drugiB = Converter::DecimalToHexa(mem[adresa2 + 1]);

				operand2 = Converter::HexaToDecimal(drugiB + prviB);
			}
			else {
				//immed 
				int pomeraj;
				string prviB = Converter::DecimalToHexa(mem[registri["111"] - 2]);
				string drugiB = Converter::DecimalToHexa(mem[registri["111"] - 1]);
				pomeraj = Converter::HexaToDecimal(drugiB + prviB);
				operand2 = pomeraj;
			}
		}


	}
	//znaci imam oba operanda sada, samo proveri sta treba da uradis!!

	if (ins == "add") {
		int flagKraj = 0;
		int rez = operand1 + operand2;
		if ((rez >= 1 << 16) || (rez < (0 - 1<<16)) ) psw |= maske->mask2; //ovo je za overflow
		else psw &= maske->unmask2;

		//hm sta cu da radim za carry bit???
		//ccccccccccccccccccccc
		

		operand1 = operand1 + operand2;
        
		if (adresa1 == 65535) {
			registri[reg1] = operand1;
			if (reg1 == "111" && operand1 == 0) flagKraj = 1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		//sta cemo za flegove
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;

		

		//hm sta cu za overflow ovde?////
		if (flagKraj == 1) return -1;
		return 0;
	}
	if (ins == "sub") {
		int rez = operand1 - operand2;
		if ((rez >= 1 << 16) || (rez < (0 - 1 << 16))) psw |= maske->mask2; //ovo je za overflow
		else psw &= maske->unmask2;

		operand1 = operand1 - operand2;
		if (adresa1 == 65535) {
			registri[reg1] = operand1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;
	}

	if (ins == "mul") {
		operand1 = operand1 * operand2;
		if (adresa1 == 65535) {
			registri[reg1] = operand1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;
	}
	if (ins == "div") {
		operand1 = operand1 * operand2;
		if (adresa1 == 65535) {
			registri[reg1] = operand1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;
	}
	if (ins == "cmp") {
		int rez = operand1 - operand2;
		if ((rez >= 1 << 16) || (rez < (0 - 1 << 16))) psw |= maske->mask2; //ovo je za overflow
		else psw &= maske->unmask2;
		operand1 = operand1 - operand2;

		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;

		return 0;
	}
	if (ins == "and") {
		operand1 = operand1 & operand2;
		if (adresa1 == 65535) {
			registri[reg1] = operand1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;
	}

	if (ins == "or") {
		operand1 = operand1 | operand2;
		if (adresa1 == 65535) {
			registri[reg1] = operand1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;
	}

	if (ins == "not") {
		operand1 = ~operand2;
		if (adresa1 == 65535) {
			registri[reg1] = operand1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;
	}

	if (ins == "test") {

		operand1 = operand1 & operand2;
		
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;
	}

	if (ins == "mov") {
		int flagKraj = 0;
		operand1 = operand2;
		if (adresa1 == 65535) {
			registri[reg1] = operand1;
			if (reg1 == "111" && operand1 == 0) flagKraj++;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		if (flagKraj == 1) return -1;
		return 0;
	}

	if (ins == "shl") {
		if (operand2 == 0) return 0;
		int cary = 0;
		if (operand2 > 16) {
			
		}
		else {
			int maska = 1 << (16 - operand2);
			if (operand1 & maska) cary++;
		}
		if (cary == 1) psw |= maske->mask3;
		else psw &= maske->unmask3;

		operand1 = operand1 << operand2;
	

		if (adresa1 == 65535) {
			registri[reg1] = operand1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;
	}

	if (ins == "shr") {
		//ostavi ovo
		if (operand2 == 0) return 0;
		int cary = 0;
		if (operand2 > 16) {

		}
		else {
			int maska = 1 << (operand2);
			if (operand1 & maska) cary++;
		}
		if (cary == 1) psw |= maske->mask3;
		else psw &= maske->unmask3;

		operand1 = operand1 >> operand2;


		if (adresa1 == 65535) {
			registri[reg1] = operand1;
		}
		else {
			//mem[adresa1] = operand1; // ne moze ovako, moras da ugradis u memoriju lepo opet u hexa obliku
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[adresa1] = Converter::HexaToDecimal(niz[0]);
			mem[adresa1 + 1] = Converter::HexaToDecimal(niz[1]);

		}
		if (operand1 == 0) psw |= maske->mask1;
		else psw &= maske->unmask1;
		if (operand1 < 0) psw |= maske->mask4;
		else psw &= maske->unmask4;
		return 0;

	}
	
}

void Procesor::timer(int duration) {
	curTimeSlice += duration;
	if (curTimeSlice >= timeSlice) {
		//znaci tajmer treba da opali
		curTimeSlice = 0;
		if (psw & maske->mask13) {
			//znaci sada izvrsi ovo za tajmer, prvo push pc, pa push psw
			sp = registri["110"];
			sp -= 2;
			short int operand1 = registri["111"];
			string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[sp] = Converter::HexaToDecimal(niz[0]);
			mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
			registri["110"] = sp;

			sp = registri["110"];
			sp -= 2;
			operand1 = psw;
			niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
			mem[sp] = Converter::HexaToDecimal(niz[0]);
			mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
			registri["110"] = sp;
			//hm unmask16 ili unmask13? da li sme tastatura da prekine timer?
			psw &= maske->unmask16; // dok se tajmer izvrsava, ne sme da se izvrsi neki prekid, cim skines pc i psw sa steka, vratice se dozvoljen timer

			//samo jos u pc upisi adresu prekidne rutine za tajmer! tajmer je na adresi 2

			string prviB = Converter::DecimalToHexa(mem[2]);
			string drugiB = Converter::DecimalToHexa(mem[2 + 1]);
			operand1 = Converter::HexaToDecimal(drugiB + prviB);
			registri["111"] = operand1;
			//i to je to, sada ce da krene da se izvrsava tajmer, kada se naidje na iret, vratice pc koji treba i to je to i vratice i psw

		}
	}
}

void Procesor::trebaIspis() {
	int flag = mem[30];
	if (flag != 0) {
		mem[30] = 0;
		string prviB = Converter::DecimalToHexa(mem[65534]);
		string drugiB = Converter::DecimalToHexa(mem[65534 + 1]);
		int operand1 = Converter::HexaToDecimal(drugiB + prviB);
		char c = (char)operand1;
		cout << c;
	}
}


void Procesor::nekorektnaIns() {
	//push pc, to je vec pc sledece Ins!
	sp = registri["110"];
	sp -= 2;
	short int operand1 = registri["111"];
	string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
	mem[sp] = Converter::HexaToDecimal(niz[0]);
	mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
	registri["110"] = sp;
	//push psw
	sp = registri["110"];
	sp -= 2;
	operand1 = psw;
	niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
	mem[sp] = Converter::HexaToDecimal(niz[0]);
	mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
	registri["110"] = sp;
	//maskiraj prekdide
	psw &= maske->unmask16;

	//skokci na prekidnu rutinu
	string prviB = Converter::DecimalToHexa(mem[4]);
	string drugiB = Converter::DecimalToHexa(mem[4 + 1]);
	operand1 = Converter::HexaToDecimal(drugiB + prviB);
	registri["111"] = operand1;

}

void Procesor::startniPrekid() {

	sp = registri["110"];
	sp -= 2;
	short int operand1 = registri["111"];
	string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
	mem[sp] = Converter::HexaToDecimal(niz[0]);
	mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
	registri["110"] = sp;
	//push psw
	sp = registri["110"];
	sp -= 2;
	operand1 = psw;
	niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
	mem[sp] = Converter::HexaToDecimal(niz[0]);
	mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
	registri["110"] = sp;
	//maskiraj prekdide
	psw &= maske->unmask16;

	//skoci na prekidnu rutinu
	string prviB = Converter::DecimalToHexa(mem[0]);
	string drugiB = Converter::DecimalToHexa(mem[0 + 1]);
	operand1 = Converter::HexaToDecimal(drugiB + prviB);
	registri["111"] = operand1;

}


void Procesor::pomocnaZaRetData(ifstream & ulaz, regex text) {
	
	string linija;
	std::getline(ulaz, linija);
	while (regex_search(linija, text) == false) {
		std::getline(ulaz, linija);
	}
	std::getline(ulaz, linija);
	std::getline(ulaz, linija);
	std::getline(ulaz, linija);
	regex alfanum("[a-z|A-Z|0-9]+");
	string a;
	ReallocationSimbol * novi = 0;
	regex nijeRazmak("[a-z|A-Z|0-9|\_|\.]+");
	while (regex_search(linija, alfanum)) {
		novi = new ReallocationSimbol();
		smatch sm;
		regex_search(linija, sm, nijeRazmak);
		string tmp = sm[0];
		linija = sm.suffix().str();
		novi->adresa = tmp;
		regex_search(linija, sm, nijeRazmak);
		tmp = sm[0];
		linija = sm.suffix().str();
		novi->tip = tmp;
		regex_search(linija, sm, nijeRazmak);
		tmp = sm[0];
		linija = sm.suffix().str();
		novi->rb = stoi(tmp);

		regex_search(linija, sm, nijeRazmak);
		tmp = sm[0];
		linija = sm.suffix().str();
		novi->imeRB = tmp;

		//	reallocText[novi->adresa] = novi;
		int adresa = Converter::HexaToDecimal(novi->adresa);
		if (novi->imeRB != ".text" && novi->imeRB != ".data" && novi->imeRB != ".rodata" && novi->imeRB != ".bss") {
			//znaci zavisi od globalnog simbola
			if (novi->tip == absolute) {
				//apsolutna adresa
				GlobSimb * gs = mapaGlobSimbola[novi->imeRB];
				if (gs == 0 || gs->definisan == 0) {
					cout << "zavisi od nedefiisanog glob simbola nesto" << endl;
					assert(0);
				}
				//znaci samo ugradi sada vrednost simbola umesto onoga sto stoji u mem
				regex prvaDva("[0-9|a-f|A-F]{2}");
				string * niz;
				niz = Converter::ugradi(gs->vrednost, 2);
				mem[adresa] = Converter::HexaToDecimal(niz[0]);
				mem[adresa + 1] = Converter::HexaToDecimal(niz[1]);
			}
			else {
				//znaci realtivno je
				GlobSimb * gs = mapaGlobSimbola[novi->imeRB];
				if (gs == 0 || gs->definisan == 0) {

					cout << "zavisi od nedefiisanog glob simbola nesto 2" << endl;
					assert(0);
				}
				//znaci samo ugradi sada vrednost simbola umesto onoga sto stoji u mem
				regex prvaDva("[0-9|a-f|A-F]{2}");
				string * niz;
				int dec1 = Converter::HexaToDecimal(gs->vrednost);
				int dec2 = Converter::HexaToDecimal(novi->adresa);

				//znaci kako ces da sklopis ovo sto vec ims u memoriji?
				//moras da prvo napravis vrednost
				string lowB = Converter::DecimalToHexa(mem[adresa]);
				string highB = Converter::DecimalToHexa(mem[adresa + 1]);

				string vredsnot = highB + lowB;
				int dec3 = Converter::HexaToDecimal(vredsnot);

				int noviZaU = dec1 - dec2 + dec3;
				string x = Converter::DecimalToHexa(noviZaU);
				niz = Converter::ugradi(x, 2);
				mem[adresa] = Converter::HexaToDecimal(niz[1]);
				mem[adresa + 1] = Converter::HexaToDecimal(niz[0]);
			}
		}
		std::getline(ulaz, linija);
	}
}


void Procesor::tastaturaPrekid() {

	if (!(psw & maske->mask16)) return;

	Mutex.lock();

	if (flagKeyboard == 0) {
		Mutex.unlock();
		return;
	}
	//char c; //recimo unet znak
	flagKeyboard = 0;

	int x = charKeyboard;
	mem[ulazPeriferije] = x;

	sp = registri["110"];
	sp -= 2;
	short int operand1 = registri["111"];
	string * niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
	mem[sp] = Converter::HexaToDecimal(niz[0]);
	mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
	registri["110"] = sp;
	//push psw
	sp = registri["110"];
	sp -= 2;
	operand1 = psw;
	niz = Converter::ugradi(Converter::DecimalToHexa(operand1), 2);
	mem[sp] = Converter::HexaToDecimal(niz[0]);
	mem[sp + 1] = Converter::HexaToDecimal(niz[1]);
	registri["110"] = sp;
	//maskiraj prekdide
	psw &= maske->unmask16;

	//skoci na prekidnu rutinu
	string prviB = Converter::DecimalToHexa(mem[6]);
	string drugiB = Converter::DecimalToHexa(mem[6 + 1]);
	operand1 = Converter::HexaToDecimal(drugiB + prviB);
	registri["111"] = operand1;

	Mutex.unlock();
}


void Procesor::runTast() {
	Kraj.lock();
	Mutex.lock();
	int i = 0;
	while (flagKeyboard != - 1) {
		Mutex.unlock();
		i++;
		if (!_kbhit()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			Mutex.lock();
			continue;
		}

		char c = _getch();

		//printf("%c", c);

	    Mutex.lock();
		flagKeyboard = 1;
		charKeyboard = c;
		
		Mutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		Mutex.lock();
		

	}
	Mutex.unlock();
	Kraj.unlock();

}