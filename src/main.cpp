#include "asm.h"
#include "cpu.h"
#include <assert.h>



int main() {

	ifstream inFile;
	ofstream outFile;



	inFile.open("ulaz1.txt");
	outFile.open("izlaz1.txt");

	if (!outFile) assert(0);

	Asembler * a = new Asembler();

	if (inFile) {

		a->prviProlaz(inFile,1000);
		
		cout << endl << "gotov prvi prolaz";
		
	    a->writeSimbolTable(outFile);
		cout << endl << "gotov upis u fajl" ;
		inFile.close();
	}

	inFile.open("ulaz1.txt");
	if (inFile) {
		a->drugiProlaz(inFile,1000);
		cout << endl << "gotov drugi prolaz" << endl;
		a->writeRealocatioTables(outFile);
		a->writeInitContent(outFile);
		cout << "gotov upis u fajl" << endl;
	}
	outFile.close();
	ofstream memoryOut;
	memoryOut.open("mem1.txt");
	a->writeMemory(memoryOut);

	Asembler a1;
	ifstream ulaz2;
	ulaz2.open("ulaz.txt");
	a1.prviProlaz(ulaz2, 1100);
	ulaz2.close();
	ulaz2.open("ulaz.txt");
	a1.drugiProlaz(ulaz2,1100 );
	ulaz2.close();
	ofstream izlaz2, mem2Upis;
		izlaz2.open("izlaz2.txt");
		mem2Upis.open("mem2.txt");
		a1.writeSimbolTable(izlaz2);
		a1.writeRealocatioTables(izlaz2);
		a1.writeInitContent(izlaz2);
		a1.writeMemory(mem2Upis);
		izlaz2.close();
		mem2Upis.close();

	//znaci moraju prvo svi fajlovi da se upisu u memoriju, pa tek onda da se pozove deo koji simulira linker!!!!!
		ifstream mem2;
		mem2.open("mem2.txt");
		ifstream izlaz2ZaCitanje;
		izlaz2ZaCitanje.open("izlaz2.txt");

	memoryOut.close();
	inFile.close();

	inFile.open("izlaz1.txt");
	
	ifstream memoryIn;
	memoryIn.open("mem1.txt");
    //deo za prekidne rutine----------------------------------------------------------------------------------------------------
	Asembler aTimer;
	ifstream ulazT; ulazT.open("ulazT.txt");
	ofstream izlazT; izlazT.open("izlazT.txt");
	aTimer.prviProlaz(ulazT,100);
	aTimer.writeSimbolTable(izlazT);
	ulazT.close();
	ulazT.open("ulazT.txt");
	aTimer.drugiProlaz(ulazT, 100);
	aTimer.writeRealocatioTables(izlazT);
	aTimer.writeInitContent(izlazT);
	ofstream memTizlaz; memTizlaz.open("memT.txt");
	aTimer.writeMemory(memTizlaz);
	memTizlaz.close();
	ifstream memT; memT.open("memT.txt");
	izlazT.close();
	ifstream izlazTCit; izlazTCit.open("izlazT.txt");
	//tijemar;
	//-------------------------------------------------------------------------------------------------------------------------
	Asembler aFault;
	ifstream ulazFault; ulazFault.open("ulazFault.txt");
	ofstream izlazFault; izlazFault.open("izlazFault.txt");
	aFault.prviProlaz(ulazFault, 200);
	aFault.writeSimbolTable(izlazFault);
	ulazFault.close();
	ulazFault.open("ulazFault.txt");
	aFault.drugiProlaz(ulazFault, 200);
	aFault.writeRealocatioTables(izlazFault);
	aFault.writeInitContent(izlazFault);
	ofstream memFaultizlaz; memFaultizlaz.open("memFault.txt");
	aFault.writeMemory(memFaultizlaz);
	memFaultizlaz.close();
	ifstream memFault; memFault.open("memFault.txt");
	izlazFault.close();
	ifstream izlazFaultCit; izlazFaultCit.open("izlazFault.txt");

	//////////////////////////////////////////----------------------------------------------------------------------------------


	Procesor p;

	//deo koji radi linker
	p.fajlZaLinker(izlazTCit, memT);
	p.fajlZaLinker(izlazFaultCit, memFault);
	p.fajlZaLinker(inFile,memoryIn);
	p.fajlZaLinker(izlaz2ZaCitanje, mem2);
	p.linker();
	p.ucitajRelZapise(inFile);
	p.ucitajRelZapise(izlaz2ZaCitanje);
	//gotov
	cout << "gotov linker" << endl;

	//emulator
	p.emulacija();
	//gotov
	cout << "gotov emu" << endl;
	int aa;
	cin >> aa;

	delete a;






	return 0;
}