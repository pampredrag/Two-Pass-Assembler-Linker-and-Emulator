
#include "asm.h"
#include <regex>
#include <assert.h>
#include "Converter.h"

#include <math.h>





Asembler::Asembler() {
   // trebalo bi popuniti mapu instrukcija sa svim instrukcijama
	mapaIns["add"] = new Instrukcija("add","0000");
	mapaIns["sub"] = new Instrukcija("sub", "0001");
	mapaIns["mul"] = new Instrukcija("mul", "0010");
	mapaIns["div"] = new Instrukcija("div", "0011");
	mapaIns["cmp"] = new Instrukcija("cmp", "0100");
	mapaIns["and"] = new Instrukcija("and", "0101");
	mapaIns["or"] = new Instrukcija("or", "0110");
	mapaIns["not"] = new Instrukcija("not", "0111");

	mapaIns["test"] = new Instrukcija("test", "1000");
	mapaIns["push"] = new Instrukcija("push", "1001");
	mapaIns["pop"] = new Instrukcija("pop", "1010");
	mapaIns["call"] = new Instrukcija("call", "1011");
	mapaIns["iret"] = new Instrukcija("iret", "1100");
	mapaIns["mov"] = new Instrukcija("mov", "1101");
	mapaIns["shl"] = new Instrukcija("shl", "1110");
	mapaIns["shr"] = new Instrukcija("shr", "1111");

	mapaIns["jmp"] = new Instrukcija("jmp", "1101");
	mapaIns["ret"] = new Instrukcija("ret", "");
	
    

   //

	locationCnt = 0;
	rbCnt = 5;

}

Asembler::~Asembler() {

	

}


void Asembler::prviProlaz(ifstream & ulaz,int pocetak) {
	locationCnt = pocetak;
	pocetakTrenutnogFajla = pocetak;
	duzinaTrenutnogFajla = 0;
	string linija;
	std::getline(ulaz, linija);
	string  curSection = "";  //trenutna sekcija u kojoj se nalazim

	Simbol * und = new Simbol("UND","UND",0,l,0);
	tabelaSimbola["UND"] = und;
 regex rEND(".end");
	
	while (!regex_search(linija,rEND)) {

		regex slovox("[a-z|0-9|A-Z]");

		if (regex_search(linija,slovox)) {
			/*
                        regex rLinux("[^\\r]+");
smatch pomx;
regex_search(linija,pomx,rLinux);
linija=pomx[0];
*/
			smatch rez;
			regex recx  ("[a-z|A-Z|\.|0-9|\:]+");
			if (regex_search(linija, rez, recx)) {
				string prvaRec = rez[0];
				linija = rez.suffix().str(); // samo da izbaci prvu rec iz linije -> ako je bilo .global a,b ostace samo a,b

				//***************************************888


				//potrazi dvotacku da vidis da li je labela

				regex dvot("\:");

				smatch sm;

				if (regex_search(prvaRec, dvot)) {
					//labela znaci

					//znaci samo da iz a: uzmem a
					regex ime("[^\:]+");
					string noviS;
					regex_search(prvaRec, sm, ime);
					noviS = sm[0];

					int vrednost = locationCnt;
					Simbol * x = tabelaSimbola[noviS];
					if (x != 0) {
						if (x->lg == l) {
							cout << "ne mozete imati dva simbola sa istim imenom";
							assert(0);
						}
						//mozda je globalni ipak
						x->vrednost = vrednost;
						x->sekcija = curSection;

					}
					else {
						//simbol tek uvodimo prvi put
						x = new Simbol(noviS, curSection, vrednost, l, rbCnt++);
						tabelaSimbola[noviS] = x;

					}



					//sada jos vidi da li ima nesto iza labele, ako nema nista vise, onda samo nastavi u while petlji
					string drugaRec;
					smatch sm2;
					if (regex_search(linija, sm2, recx)) {
						drugaRec = sm2[0];
						
						prvaRec = drugaRec;
						linija = sm2.suffix().str();
					}
					else {
						//smao nastavi while petlju onda jer nema nista iza labele
						std::getline(ulaz, linija);
						continue;

					}

				}
				//************************************************************************


				if (prvaRec == ".rodata") {
					if (curSection != "") {


						int duzina = locationCnt;
						Simbol * staraSe = tabelaSimbola[curSection];
						if (staraSe == 0) {
							cout << "nema sekcije u tabeli a stigao si do njenog kraja" << endl;
							assert(0);
						}
						staraSe->duzina = duzina;
						staraSe->kraj = duzina;
					}
					//locationCnt = 0; ipak nema resetovanja locationCountera !!!!!!!!
					curSection = prvaRec;
					Simbol * simb = tabelaSimbola[curSection];
					if (simb == 0) {
						// samo od .rodata napravis rodata
						regex x("[a-z]+");
						smatch sm2;

						regex_search(prvaRec, sm2, x);
						string recBezT = sm2[0];
						Simbol * novi = new Simbol(prvaRec, recBezT, 0, l, 4);
						novi->pocetak = locationCnt;
						tabelaSimbola[curSection] = novi;
					}
				}
				else {
					if (prvaRec == ".text") {
						if (curSection != "") {


							int duzina = locationCnt;
							Simbol * staraSe = tabelaSimbola[curSection];
							if (staraSe == 0) {
								assert(0);
							}
							staraSe->duzina = duzina;
							staraSe->kraj = duzina;
						}
				//		locationCnt = 0;
						curSection = prvaRec;
						Simbol * simb = tabelaSimbola[curSection];
						if (simb == 0) {
							regex x("[a-z]+");
							smatch sm2;

							regex_search(prvaRec, sm2, x);
							string recBezT = sm2[0];
							Simbol * novi = new Simbol(prvaRec, recBezT, 0, l, 1);
							novi->pocetak = locationCnt;

							tabelaSimbola[curSection] = novi;
						}

					}
					else {
						if (prvaRec == ".data") {
							if (curSection != "") {


								int duzina = locationCnt;
								Simbol * staraSe = tabelaSimbola[curSection];
								if (staraSe == 0) {
									assert(0);
								}
								staraSe->duzina = duzina;
								staraSe->kraj = duzina;
							}
					//		locationCnt = 0;
							curSection = prvaRec;
							Simbol * simb = tabelaSimbola[curSection];
							if (simb == 0) {
								regex x("[a-z]+");
								smatch sm2;

								regex_search(prvaRec, sm2, x);
								string recBezT = sm2[0];
								Simbol * novi = new Simbol(prvaRec, recBezT, 0, l, 2);
								novi->pocetak = locationCnt;
								tabelaSimbola[curSection] = novi;
							}
						}
						else {
							if (prvaRec == ".bss") {
								if (curSection != "") {


									int duzina = locationCnt;
									Simbol * staraSe = tabelaSimbola[curSection];
									if (staraSe == 0) {
										assert(0);
									}
									staraSe->duzina = duzina;
									staraSe->kraj = duzina;
								}
						//		locationCnt = 0;
								curSection = prvaRec;
								Simbol * simb = tabelaSimbola[curSection];
								if (simb == 0) {
									regex x("[a-z]+");
									smatch sm2;
									
									regex_search(prvaRec, sm2, x);
									string recBezT = sm2[0];
									Simbol * novi = new Simbol(prvaRec, recBezT, 0,l, 3);
									novi->pocetak = locationCnt;
									tabelaSimbola[curSection] = novi;
								}
						
							}
							else {
								if (prvaRec == ".global") {
									regex rec1("[a-z|A-Z]+"); //rec, samo ce da kupi jedno po jedno ime ili zarez, + znaci da se pojavljuje najmanje jednom
									smatch sm1;
									while (regex_search(linija, sm1, rec1)) {
										string ime = sm1[0];
										if (ime == ",") {
											//nista, samo dalje idi

										}
										else {
											//ovo je novo globalno ime, treba ga ubaciti u tabelu simbola, ako vec slucajno ne postoji!
											Simbol * simb = tabelaSimbola[ime];

											if (simb == 0) {
												string omega = "UND";
											//	if (curSection != "") omega = curSection;
												Simbol * novi = new Simbol(ime, omega, 0, g, rbCnt++);
												tabelaSimbola[ime] = novi;
											}
											else {
												if (simb->lg == l) {
													simb->lg = g;
												}
											}
											//ovde se valjda nista drugo ne radi
										}
										linija = sm1.suffix().str();
									}
									//trebalo bi da sam gotov i za global
									//.extern ne treba valjda

									
								}
								else {
									//trebalo bi da su samo instrukcije ostale 
									//da l ima jos neka dirketiva????? 
									// ima jos zajebancija, one .word. byte i neka cudna, .align
									if (prvaRec == ".align") {
										//boga pitaj sta ovde treba **********************************************************************************************
										regex broj("[0-9]+");
										smatch sm;
										if (regex_search(linija, sm, broj)) {
											string brojce = sm[0];
											int stepen = stoi(brojce);
											int delilac = pow(2, stepen);

											int ostatak = locationCnt % delilac;
											if (ostatak != 0) {
												int uvecanje = delilac - ostatak;
												locationCnt += uvecanje;
											}

										}
										else {
											cout << "mora broj u align " << endl;
											assert(0);
										}
									}
									else {

										if (prvaRec == ".char") {
											//1b vrv zauzima
											regex broj("[0-9|a-z|A-Z]+");
											smatch sm;
											while (regex_search(linija, sm, broj)) {
												locationCnt += 1;
												linija = sm.suffix().str(); //mora ovo inace ce beskonacna petlaj da bude
											}
										}
										else {
											if (prvaRec == ".word") {
												regex broj("[0-9|a-z|A-Z]+");
												smatch sm;
												while (regex_search(linija, sm, broj)) {
													locationCnt += 2;
													linija = sm.suffix().str(); //mora ovo inace ce beskonacna petlaj da bude
												}
											}
											else {
												if (prvaRec == ".long") {
												//	locationCnt += 4; //kontam da nista vise ne treba ovde u prvom prpolazu...
													regex broj("[0-9|a-z|A-Z]+");
													smatch sm;
													while (regex_search(linija, sm, broj)) {
														locationCnt += 4;
														linija = sm.suffix().str(); //mora ovo inace ce beskonacna petlaj da bude
													}
												}
												else {
													if (prvaRec == ".skip") { //skip 8 -> samo ovako moze valjda
														                     // ne znam da l moze skip 4,5,6 ->pa se onda saberu ove 4 + 5 + 6
														regex broj("[0-9]+");
														smatch sm;
														if (regex_search(linija, sm, broj)) {
															string x = sm[0];
															int xx =std::stoi(x); // kao atoi, samo stoi prima string umesto char*
															locationCnt += xx;
														}
														else {
															cout << "posle skipa nema broja " << endl;
															assert(0);

															//moze podrazumevano nula da bude
														}
                                                         
													}
													else {
														//sada bi trebalo da mora da bude instrukcija

														string uslovniDeo;
														regex dvaSlova("[a-z|A-Z]{2}");
														smatch ds;
														if (regex_search(prvaRec, ds, dvaSlova)) {
															uslovniDeo = ds[0];
															prvaRec = ds.suffix().str();

														}
														else {
															cout << "uneli ste neku glupost, ne moze ovo da se asemblira";
															assert(0);
														}

														string uslovniBiti = getConditionBits(uslovniDeo);

														Instrukcija * xyIns = mapaIns[prvaRec];
														if (xyIns == 0) {
															cout << "uneli ste neku glupost, ne moze ovo da se asemblira";
															assert(0);
														}

														if (curSection != ".text") {
															cout << "instrukcije mogu da se pisu samo u text sekciji" << endl;
															assert(0);
														}

														locationCnt += 2;
														//sada samo proveri da li location cnt treba mozda za jos 2, tj za 4 ukupno
														//to ce biti prilicno nezgodno sada

														//znaci samo ako je regdir u oba operanda (i u dst i u src) onda ide 2B, u svim ostalim
														//slucajevima treba 4B
														//pokusaj da proveris da li je za2B, pa ako nije to, onda dodaj jos 2

														string drugaRec; //ovde spakuj dst
														string trecaRec; //ovde spakuj src
														
														regex r1("[^\,]+");														
														smatch s1;

														regex r2("\ *r[0-7]\ *\r*");
														smatch s2;
														int flag1 = 0;
														int flag2 = 0;

														if (regex_search(linija, s1, r1)) {
															drugaRec = s1[0];
															if (regex_match(drugaRec, r2)) {
																flag1++;
															}
															linija = s1.suffix().str();
														}
														else {
															if (xyIns->name != "ret" && xyIns->name != "iret") {
																cout << "fale dst operand posle instrukcije ";
																assert(0);
															}
														}


														if (regex_search(linija, s1, r1)) {
															trecaRec = s1[0];
															if (regex_match(trecaRec, r2)) {
																flag2++;
															}
														}
														else {
															if (xyIns->name == "push" || xyIns->name == "pop" || xyIns->name == "call" || xyIns->name == "ret" || xyIns->name == "iret" || xyIns->name == "jmp") {
																flag2++;
															}
															else {
																cout << "fale src operand posle instrukcije ";
																assert(0);
															}
														}
														
														if ((flag1 == 1 && flag2 == 1) || xyIns->name == "ret" || xyIns->name == "iret" ) {
															//ins je 2B, znaci ne radimo vise nista
														}
														else {
															//instrukcija je 4B, pa dodaj jos 2
															locationCnt += 2;
														}


													}
												}
											}
										}
                                     

									}

								}
							}
						}
					}
				}


			}
		}
		
		std::getline(ulaz, linija);


	}

	// naisao si na end, moras da odradis i ovo za kraj poslednje sekcije;
	int duzina = locationCnt;
	locationCnt = 0;
	string sekcija = curSection;
	curSection = "";
	Simbol * s = tabelaSimbola[sekcija];
	if (s == 0) {
		cout << "neka greska prilikokm .sekcija poslednje sekcije pre .end jer smo" <<"dosli do kraja sekcije a nema sekcije u tabeli simb" <<endl;
		assert(0);
	}
	s->duzina = duzina;
	s->kraj = duzina;

	duzinaTrenutnogFajla = duzina;

	Simbol * simb = tabelaSimbola[".bss"];
	if (simb == 0) {

	}
	else {
		pocetakBssSekcije = simb->pocetak;
		krajBssSekcije = simb->kraj;
	}
	

	/*
	valjda ne treba nista vise u prvom prolazu
	imam celu tabelu simbola
	to je to kontam

	*/

}


void Asembler::drugiProlaz(ifstream & ulaz,int pocetak) {

	locationCnt = pocetak;
	string curSection = "";

	string linija;
	////*********************************************
    // da li ovde treba ponovo ucitati fajl nekako? jer je u prvom prolazu dosslo vec do kraja fajla prilikom citanjA??
	std::getline(ulaz, linija);
        regex rEND(".end");
	while (!regex_search(linija,rEND)) {

	 
		regex slovox("[a-z|0-9|A-Z]");

		if (regex_search(linija, slovox)) {
			/*
                        regex rLinux("\\r");
smatch pomx;
regex_search(linija,pomx,rLinux);
linija=pomx.prefix().str();
*/
			smatch rez;
			regex recx("[a-z|\.|0-9|\:]+");
			if (regex_search(linija, rez, recx)) {
				string prvaRec = rez[0];
				linija = rez.suffix().str(); 

				regex dvot("\:");

				smatch sm;

				if (regex_search(prvaRec, dvot)) {
					//znaci labela je
					// u prvom prolazu sam valjda zavrsio sve sa labelama, tako da sada njih samo preskacem
					//pogledaj ima li nesto iza labele
					string drugaRec;
					smatch sm2;
					if (regex_search(linija, sm2, recx)) {
						drugaRec = sm2[0];

						prvaRec = drugaRec;
						linija = sm2.suffix().str();
					}
					else {
						//smao nastavi while petlju onda jer nema nista iza labele
						std::getline(ulaz, linija);
						continue;

					}



				} //kraj labele

				if (prvaRec == ".rodata" || prvaRec == ".global") {
					if (prvaRec == ".rodata") {
						vector <string>::iterator ij;
						if (curSection == ".text") {
				//			cout << codeText.size();
				//			Memorija.push_back("" + codeText.size());
							for (ij = codeText.begin(); ij != codeText.end(); ij++) {
								Memorija.push_back((*ij));

							}
						}
						else {
							if (curSection == ".data") {
								for (ij = codeData.begin(); ij != codeData.end(); ij++) {
									Memorija.push_back((*ij));

								}
							}
							else {
								if (curSection == ".rodata") {
									for (ij = codeRodata.begin(); ij != codeRodata.end(); ij++) {
										Memorija.push_back((*ij));

									}
								}
							}
						}
					curSection = prvaRec;
				//	locationCnt = 0; nema resetovanja ovoga
					}
					else {
						//global
						//samo idi dalje
					}
				}
				else {
					if (prvaRec == ".text") {
						vector <string>::iterator ij;
						if (curSection == ".text") {
							//Memorija.push_back("" + codeText.size());
							for (ij = codeText.begin(); ij != codeText.end(); ij++) {
								Memorija.push_back((*ij));

							}
						}
						else {
							if (curSection == ".data") {
								for (ij = codeData.begin(); ij != codeData.end(); ij++) {
									Memorija.push_back((*ij));

								}
							}
							else {
								if (curSection == ".rodata") {
									for (ij = codeRodata.begin(); ij != codeRodata.end(); ij++) {
										Memorija.push_back((*ij));

									}
								}
							}
						}
						curSection = prvaRec;
						//locationCnt = 0;

					}
					else {
						if (prvaRec == ".data") {
							vector <string>::iterator ij;
							if (curSection == ".text") {
								
								for (ij = codeText.begin(); ij != codeText.end(); ij++) {
									Memorija.push_back((*ij));

								}
							}
							else {
								if (curSection == ".data") {
									for (ij = codeData.begin(); ij != codeData.end(); ij++) {
										Memorija.push_back((*ij));

									}
								}
								else {
									if (curSection == ".rodata") {
										for (ij = codeRodata.begin(); ij != codeRodata.end(); ij++) {
											Memorija.push_back((*ij));

										}
									}
								}
							}
							curSection = prvaRec;
						//	locationCnt = 0;
						}
						else {
							if (prvaRec == ".bss" || prvaRec == ".align") {
								if (prvaRec == ".bss") {
									vector <string>::iterator ij;
									if (curSection == ".text") {
										Memorija.push_back("" + codeText.size());
										for (ij = codeText.begin(); ij != codeText.end(); ij++) {
											Memorija.push_back((*ij));

										}
									}
									else {
										if (curSection == ".data") {
											for (ij = codeData.begin(); ij != codeData.end(); ij++) {
												Memorija.push_back((*ij));

											}
										}
										else {
											if (curSection == ".rodata") {
												for (ij = codeRodata.begin(); ij != codeRodata.end(); ij++) {
													Memorija.push_back((*ij));

												}
											}
										}
									}
									curSection = prvaRec;
							//		locationCnt = 0;
								}
								else {
									//znaci align je ovde sada, ovao sam ubacio jer ne znam da li bih se snasao sa zagradama inace
									regex broj("[0-9]+");
									smatch sm;
									if (regex_search(linija, sm, broj)) {
										string brojce = sm[0];
										int stepen = stoi(brojce);
										int delilac = pow(2, stepen);

										int ostatak = locationCnt % delilac;
										if (ostatak != 0) {
											int uvecanje = delilac - ostatak;
											locationCnt += uvecanje;
											for (int i = 0; i < uvecanje; i++) {
												if (curSection == ".text") {
													codeText.push_back("00");
												}
												else {
													if (curSection == ".data") {
														codeData.push_back("00");
													}
													else {
														if (curSection == ".rodata") {
															codeRodata.push_back("00");
														}
													}
												}
											}
										
										}

									}
									else {
										cout << "mora broj u align " << endl;
										assert(0);
									}
								}
							}
							else {
								//gotovo sa sekcijama
							    //uradimo direktive sada
								if (prvaRec == ".char") {

									string imeS;
									regex reg("[A-Z|a-z|0-9]+");
									smatch sm;

									while (regex_search(linija, sm, reg)) {
										//hocu sada da napravim novi realokacioni zapis
										imeS = sm[0];
										regex tester("[A-Z|a-z]");
										int pomeraj;
										if (curSection != ".bss") {

										if (regex_search(imeS, tester)) { //da li je uz .char samo broj ili neka promenljiva
											ReallocationSimbol * novi = noviRelSimb(imeS, locationCnt, curSection, absolute);
											Simbol * simb = tabelaSimbola[imeS];
											if (simb == 0) assert(0);
											pomeraj = simb->vrednost;
										}
										else {
											pomeraj = stoi(imeS);

										}

										//znaci treba pomeraj da ugradim u kod
										//ali pomeraj treba da bude ugradjen kao jedan bajt
										string hexa = Converter::DecimalToHexa(pomeraj);
										int len = hexa.length();
										if (len > 2) {
											cout << "preveliki pomeraj, ne moze da stane u jedan bajt" << endl;
											assert(0);
										}
										if (hexa.length() == 0) hexa = "00";
										if (hexa.length() == 1) hexa = "0" + hexa;
										if (curSection == ".data") {
											codeData.push_back(hexa);
										}
										else {
											if (curSection == ".text") {
												codeText.push_back(hexa);
											}
											else {
												if (curSection == ".rodata") {
													codeRodata.push_back(hexa);
												}
												else {
													assert(0);
												}
											}
										}


									}
										locationCnt += 1;
										linija = sm.suffix().str();
									}

									//znaci samo smo dodali ovaj realokacioni simbol
									//bilo bi zgodno da smo uradili zajedno sa ovim i generisanje koda zbog onog izracunavanja
									//znaci ostalo nam je da generisemo kod ovde ***********************************************************
									//pre svega, moram da sredim izraz nekako, da vidim da li je validan itd... ako imam ono .long a-b+e;

								}
								else {
									if (prvaRec == ".word") {
										string imeS;
										regex reg("[A-Z|a-z|0-9]+");
										smatch sm;

										while (regex_search(linija, sm, reg)) {
											//hocu sada da napravim novi realokacioni zapis
											imeS = sm[0];
											regex tester("[A-Z|a-z]");

											if (curSection != ".bss") {
												int pomeraj;
												if (regex_search(imeS, tester)) {
													ReallocationSimbol * novi = noviRelSimb(imeS, locationCnt, curSection, absolute);
													Simbol * simb = tabelaSimbola[imeS];
													if (simb == 0) assert(0);
													pomeraj = simb->vrednost;
												}
												else {
													pomeraj = stoi(imeS);

												}
												string hexa = Converter::DecimalToHexa(pomeraj);
												if (hexa.length() > 4) assert(0);

												//////////////////////////////////////////////////////////////////////////////////////
												string prviB;
												string drugiB;
												int len = hexa.length();
												if (len == 2 || len == 4) {
													regex r("[a-f|A-F|0-9]{2}");
													smatch sm1;
													regex_search(hexa, sm1, r);
													string prvaDva = sm1[0];
													hexa = sm1.suffix().str();
													if (len == 2) {
														prviB = prvaDva;
														drugiB = "00";
													}
													else {
														drugiB = prvaDva;
														smatch s2;
														regex_search(hexa, s2, r);
														string drugaDva = s2[0];
														prviB = drugaDva;
													}
												}
												else {
													if (len == 0) {
														prviB = "00";
														drugiB = "00";
													}
													else {
														regex r("[a-f|A-F|0-9]{1}");
														smatch sm1;
														regex_search(hexa, sm1, r);
														string prvi = sm1[0];
														hexa = sm1.suffix().str();
														if (len == 1) {
															drugiB = "00";
															prviB = "0" + prvi;
														}
														else {
															drugiB = "0" + prvi;
															smatch s2;
															regex r1("[a-f|A-F|0-9]{2}");
															regex_search(hexa, s2, r1);
															string drugaDva = s2[0];
															prviB = drugaDva;
														}
													}
												}
												if (curSection == ".text") {
													codeText.push_back(prviB);
													codeText.push_back(drugiB);
												}
												else {
													if (curSection == ".data") {
														codeData.push_back(prviB);
														codeData.push_back(drugiB);
													}
													else {
														if (curSection == ".rodata") {
															codeRodata.push_back(prviB);
															codeRodata.push_back(drugiB);
														}
														else {
															assert(0);
														}

													}
												}

												/////////////////////////////////////////////////////////////////////////////////////////


									     	}
											locationCnt += 2;
											linija = sm.suffix().str();
										}


									}
									else {
										if (prvaRec == ".long") {
											string imeS;
											regex reg("[A-Z|a-z|0-9]+");
											smatch sm;

											while (regex_search(linija, sm, reg)) {
												//hocu sada da napravim novi realokacioni zapis
												imeS = sm[0];
												regex tester("[A-Z|a-z]");
												if (curSection != ".bss") {
													int pomeraj;
													if (regex_search(imeS, tester)) {
														ReallocationSimbol * novi = noviRelSimb(imeS, locationCnt, curSection, absolute);
														Simbol * simb = tabelaSimbola[imeS];
														if (simb == 0) assert(0);
														pomeraj = simb->vrednost;
													}
													else {
														pomeraj = stoi(imeS);
													}
													string hexa = Converter::DecimalToHexa(pomeraj);
													if (hexa.length() > 8) assert(0);
													int len = hexa.length();
													string niz[8] = { "0", "0", "0", "0","0", "0", "0", "0" };
												
														regex rx("[0-9|a-f|A-F]");
														smatch sm;
														int i = 0;
														while (regex_search(hexa, sm, rx)) {
															string tmp = sm[0];
															niz[i] = tmp;
															hexa = sm.suffix().str();
															i++;
														}
														i--;
														string nizB[4] = {"00", "00", "00" , "00"};
														int n = 0;
														for (; i > 0; i-=2) {
															nizB[n++] = niz[i - 1] + niz[i];
														}

														if (i == 0) {
															nizB[n] = "0" + niz[0];
														}
														if (curSection == ".text") {
															for (int i = 0; i < 4; i++) {
																codeText.push_back(nizB[i]);
															}
														}
														else {
															if (curSection == ".data") {
																for (int i = 0; i < 4; i++) {
																	codeData.push_back(nizB[i]);
																}
															}
															else {
																if (curSection == ".rodata") {
																	for (int i = 0; i < 4; i++) {
																		codeRodata.push_back(nizB[i]);
																	}
																}
																else {
																	assert(0);
																}
															}
														}
													
												
												}

												

												locationCnt += 4;
												linija = sm.suffix().str();
											}


										}
										else {
											if (prvaRec == ".skip") {
												regex broj("[0-9]+");
												smatch sm;
												if (regex_search(linija, sm, broj)) {
													string x = sm[0];
													int xx = std::stoi(x); // kao atoi, samo stoi prima string umesto char*
													for (int j = 0; j < xx; j++) {
														if (curSection == ".text") {
															codeText.push_back("00");
														}
														else {
															if (curSection == ".data") {
																codeData.push_back("00");
															}
															else {
																if (curSection == ".rodata") {
																	codeRodata.push_back("00");
																}
															}
														}
													}
													locationCnt += xx;
												}
												else {
													cout << "posle skipa nema broja " << endl;
													assert(0);

													//moze podrazumevano nula da bude
												}


											}
											else {
												//ostale instrukcije
												//ovde treba obratiti dosta paznje ne skokove
												string uslovniDeo;
												regex dvaSlova("[a-z|A-Z]{2}");
												smatch ds;
												if (regex_search(prvaRec, ds, dvaSlova)) {
													uslovniDeo = ds[0];
													prvaRec = ds.suffix().str();

												}
												else {
													cout << "uneli ste neku glupost, ne moze ovo da se asemblira";
													assert(0);
												}
												
												string uslovniBiti = getConditionBits(uslovniDeo);
												

												Instrukcija * xyIns = mapaIns[prvaRec];
												if (xyIns == 0) {
													cout << "uneli ste neku glupost, ne moze ovo da se asemblira";
													assert(0);
												}

												
											    
												if (xyIns->name == "nista") {
													// moze jmp eax , jmp [eax]pomeraj, jmp labela, jmp $eax, 
	//******************************************	//jos valjda samo ovo i gotova !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

													
												}
												else {
													if (xyIns->name == "ret") {
														string opcode = "1010";
														string bajt1 = uslovniBiti + opcode + "01";
														string bajt2 = "11100000";


														string hexalo = Converter::BinaryToHexa(bajt1);
														codeText.push_back(hexalo);
														hexalo = Converter::BinaryToHexa(bajt2);
														codeText.push_back(hexalo);

														locationCnt += 2;


													}
													else {

														
														//znaci samo ako je memdir, mi ima sanse da dodajemo onaj realokacioni zapis
														//mozda treba i ako je regindpom?????//????????????????????????

														if (xyIns->name== "iret") {
															string opcode = mapaIns["iret"]->opcode;
															string opcode1 = uslovniBiti + opcode + "00"; //prvi bajt;
															string hexa1 = Converter::BinaryToHexa(opcode1);
															codeText.push_back(hexa1);
															string opcode2 ="00000000"; //drugi bajt
															string hexa = Converter::BinaryToHexa(opcode2);
															codeText.push_back(hexa);
															locationCnt += 2;


														}
														else {
															string ostatak = linija;
															Ret1 r = odrediTipAdresiranja(linija);
															linija = r.linija;
														    
															string opcode = xyIns->opcode;
															string adr = opCodeAdr(r.adr);
															string bajt =uslovniBiti + opcode + adr;
															if (xyIns->name == "jmp") {
																bajt = uslovniBiti + opcode + "01";
																regex dolarino("\\$");
																if (regex_search(ostatak, dolarino)) {
																	//samo ako je jmp sa pc relativnim, jmp se prevodi kao add
																	bajt = uslovniBiti + "0000" + "01";
																}
															}
															string hexa1 = Converter::BinaryToHexa(bajt);
															codeText.push_back(hexa1);
															Ret1 r1(7, "","");
											
															
															if (xyIns->name == "push" || xyIns->name == "pop" || xyIns->name == "call" || xyIns->name == "jmp") {
																//string reg;
																if (xyIns->name == "push" || xyIns->name == "pop") {
																	if (r.adr == regdir) {
																		//izvuci registar
																		regex rdir("[0-7]");
																		smatch sdir;
																		regex_search(ostatak,sdir,rdir);
																		string broj = sdir[0];
																		int br = stoi(broj);
																	    string  reg = registar(br);
																		//tri bita, jos pet za drugi operadn, ali ovo nema drugi, pa onda smo nule
																		bajt = reg + "00000";
																		string hexalo = Converter::BinaryToHexa(bajt);
																		codeText.push_back(hexalo);
																		
																	}
																	else {
																		//da li sme ista drugo za push i pop???????
																		//trebalo bi ovde da unesem deo ako su push i pop bez regdir-a
																		string reg2;
																		if (r.adr == regindpom) {
																			regex dolar1("\\$");
																			if (regex_search(ostatak, dolar1)) {
																				reg2 = "111";
																			}
																			else {
																				regex registar1("r[0-7]");
																				smatch sm11;
																				if (regex_search(ostatak, sm11, registar1)) {
																					string tmp = sm11[0];
																					regex cifra11("[0-7]");
																					smatch sm12;
																					regex_search(tmp, sm12, cifra11);
																					int regBroj12 = stoi(sm12[0]);
																					reg2 = registar(regBroj12);
																				}
																				else {
																					assert(0);
																				}
																			}
																		}
																		else {
																			reg2 = "000";
																		}

																		string drugiBajtZaU = reg2 + "00" + "000";
																		string hexaZaU = Converter::BinaryToHexa(drugiBajtZaU);
																		codeText.push_back(hexaZaU);
																		
																	}

																}
																else {
																	//ako je call //treba ovde upisati drugi bajt
																	if (xyIns->name == "call") {
																	//	string nule = "00";
																	//	codeText.push_back(nule);
																		string reg2 = "000";
																		if (r.adr == regdir || r.adr == regindpom) {
																			//moras da promenis registar
																			regex dolar1("\\$");
																			if (regex_search(ostatak, dolar1)) {
																				reg2 = "111";
																			}
																			else {
																				regex registar1("r[0-7]");
																				smatch sm11;
																				if (regex_search(ostatak, sm11, registar1)) {
																					string tmp = sm11[0];
																					regex cifra11("[0-7]");
																					smatch sm12;
																					regex_search(tmp, sm12, cifra11);
																					int regBroj12 = stoi(sm12[0]);
																					reg2 = registar(regBroj12);
																				}
																				else {
																					assert(0);
																				}
																			}

																		}
																		string drugiB = reg2 + "0" + "0000"; //i dodaj jos 5 bitova
																		string hexa = Converter::BinaryToHexa(drugiB);
																		codeText.push_back(hexa);
																	}
																	else {
																		//znaci jmp je 
																		string adrxy = opCodeAdr(r.adr);
																		string reg2 = "000";
																		if (r.adr == regdir || r.adr == regindpom) {
																			//moras da promenis registar
																			regex dolar1("\\$");
																			if (regex_search(ostatak, dolar1)) {
																				reg2 = "111";
																				adrxy = "00";
																				reg2 = "000";
																				//izracunaj pomeraj koji treba da ugradis ako je jmp!
																				//aladd pc,labela posle dolar - sledecaIns== pc, labela - (lcoationCnt + 4)
																			}
																			else {
																				regex registar1("r[0-7]");
																				smatch sm11;
																			if (regex_search(ostatak, sm11, registar1)) {
																				string tmp = sm11[0];
																				regex cifra11("[0-7]");
																				smatch sm12;
																				regex_search(tmp, sm12, cifra11);
																				int regBroj12 = stoi(sm12[0]);
																				reg2 = registar(regBroj12);
																			}
																			else {
																				assert(0);
																			}
																			}

																		}
																		string drugiB = "111" + adrxy + reg2; //i dodaj jos 5 bitova
																		string hexa = Converter::BinaryToHexa(drugiB);
																		codeText.push_back(hexa);
																	}

																}


															}
															else {
																r1 = odrediTipAdresiranja(linija);
																linija = r1.linija; // ??????????????????????????/
															}

															if (xyIns->name != "pop" && xyIns->name != "push" && xyIns->name != "call" && xyIns->name !="jmp")
															if (r.adr != regdir  && r1.adr != regdir ) {
																//cout << "ne moze u dva operanda da bude pomeraj/labela, nisu instrukcije dovoljno velike";
																//assert(0);
																//znaci nista jos nije upisano! dodaj 4 bajta onda
																string bajtPrvi1 = "1100 0000";
																string bajtDrugi2 = "00";
																codeText.pop_back();
																codeText.push_back("C0");
																codeText.push_back("00");
																codeText.push_back("00");
																codeText.push_back("00");

																std::getline(ulaz, linija);
																continue;
																
													
															}

															if (r.adr == immed && xyIns->name != "call" && xyIns->name != "jmp" && xyIns->name != "push") {
																//cout << "ne moze immed u src ";
																//assert(0);
															}
															
															if (r.adr == regdir && xyIns->name != "pop" && xyIns->name != "push" && xyIns->name != "call" && xyIns->name != "jmp") {

																regex rx("[0-7]");
																smatch sx;
																string st = r.s;
																regex_search(st, sx, rx);
																string brojS = sx[0];
																int broj = stoi(brojS);
																string reg1 = registar(broj);
																string operand2;
																string reg2;
																if (r1.adr == regdir) {
																	regex rxy("[0-7]");
																	smatch sxy;
																	string sty = r1.s;
																	regex_search(sty, sxy, rxy);
																	string brojSY = sxy[0];
																	int brojy = stoi(brojSY);
																    reg2 = registar(brojy);
																	operand2 = "01" + reg2;
																	string bajtDrugi = reg1 + operand2;
																	string hexaDrugi = Converter::BinaryToHexa(bajtDrugi);
																	codeText.push_back(hexaDrugi);
																}
																else {
																	if (r1.adr == regindpom) {
																		regex dolar("\\$");
																		if (regex_search(r1.s, dolar)) {
																			reg2 = registar(7);
																		}																	
																		else {
																			regex rxy("[0-7]");
																			smatch sxy;
																			string sty = r1.s;
																			regex_search(sty, sxy, rxy);
																			string brojSY = sxy[0];
																			int brojy = stoi(brojSY);    //** ovde puce
																			reg2 = registar(brojy);
																		}
																		operand2 = "11" + reg2;
																		string bajtDrugi = reg1 + operand2;
																		string hexalo2 = Converter::BinaryToHexa(bajtDrugi);
																		codeText.push_back(hexalo2);
																		//da li ovde da kopiram ovaj kod za regindom i za hvatanje pomeraja
																		//i njegovo upisivanje
																		//verovatno nece morati

																	}
																	else {
																		reg2 = "000";
																		string tip = opCodeAdr(r1.adr);
																		operand2 = tip + reg2;
																		string bajtDrugi = reg1 + operand2;
																		string hexalo2 = Converter::BinaryToHexa(bajtDrugi);
																		codeText.push_back(hexalo2);

																	}

																}
															    


															}

															////////////////////////////////////////////////////////////////



															if ((r.adr == regindpom) && xyIns->name != "pop" && xyIns->name != "push" && xyIns->name != "call" && xyIns->name != "jmp") {
																string reg1;
																regex americkiDolar("\\$");
																smatch ad;
																if (regex_search(r.s,ad,americkiDolar)) {
																	reg1 = "111";

																}
																else {
																	regex rx("[0-7]");
																	smatch sx;
																	string st = r.s;
																	regex_search(st, sx, rx);
																	string brojS = sx[0];
																	int broj = stoi(brojS);
																	reg1 = registar(broj);
																}
																string operand2;
																string reg2;
																if (r1.adr == regdir) {
																	regex rxy("[0-7]");
																	smatch sxy;
																	string sty = r1.s;
																	regex_search(sty, sxy, rxy);
																	string brojSY = sxy[0];
																	int brojy = stoi(brojSY);
																	reg2 = registar(brojy);
																	operand2 = "01" + reg2;
																	string bajtDrugi = reg1 + operand2;
																	string hexalo = Converter::BinaryToHexa(bajtDrugi);
																	codeText.push_back(hexalo);
																}
																else {
																	cout << "vise od dva operanda imaju pomeraj";
																	assert(0);

																}



															}

															//******************************************************************************************
															if ((r.adr == memdir || r.adr == immed) && xyIns->name != "pop" && xyIns->name != "push" && xyIns->name != "call" && xyIns->name != "jmp") {

																
																string reg1 = "000";
																string operand2;
																string reg2;
																if (r1.adr == regdir) {
																	regex rxy("[0-7]");
																	smatch sxy;
																	string sty = r1.s;
																	regex_search(sty, sxy, rxy);
																	string brojSY = sxy[0];
																	int brojy = stoi(brojSY);
																	reg2 = registar(brojy);
																	string tip = opCodeAdr(r1.adr);
																	operand2 = tip + reg2;

																	string bajtDrugi = reg1 + operand2;
																	string heksalo = Converter::BinaryToHexa(bajtDrugi);
																	codeText.push_back(heksalo);
																}
																else {
																	cout << "vise od dva operanda imaju pomeraj";
																	/////
																	assert(0);

																}



															}


															////////////////////////////////////////////////////////////////////

															if (r.adr == memdir) {
																//treba da dodamo mozda realokacioni zapis
																string opcode = xyIns->opcode;
																string prviB;
																string imeS;
																smatch sm;
																regex reg("[a-z|0-9]+");
																int flagZvezdica = 0;
																regex rspec("\\*");
																smatch smzvezda;
																if (regex_search(r.s, smzvezda, rspec)) {
																	//string s = sm[0];
																	r.s = smzvezda.suffix().str();
																	flagZvezdica++;
																}

																if (regex_search(r.s, sm, reg)) {
																	imeS = sm[0];
																	int dec = 0;
																	string prviB;
																	string drugiB;
																	if (flagZvezdica == 0) {
																		noviRelSimb(imeS, locationCnt + 2, curSection, absolute);

																		Simbol * simbol = tabelaSimbola[imeS];
																		
																		dec = simbol->vrednost;
																	}
																	else {
																		//znaci ako sam pronasao zvezdicu
																		//nema dodavanja novog rel simbola, nego samo ugradjujem ovo u kod odmah
																		dec = stoi(imeS); //ime simbola je isto ono sve bez zvezdice
																	}
																	string hexa = Converter::DecimalToHexa(dec);
																	int len = hexa.length();
																	if (len == 2 || len == 4) {
																		regex r("[a-f|A-F|0-9]{2}");
																		smatch sm1;
																		regex_search(hexa, sm1, r);
																		string prvaDva = sm1[0];
																		hexa = sm1.suffix().str();
																		if (len == 2) {
																			prviB = prvaDva;
																			drugiB = "00";
																		}
																		else {
																			drugiB = prvaDva;
																			smatch s2;
																			regex_search(hexa, s2, r);
																			string drugaDva = s2[0];
																			prviB = drugaDva;
																		}
																	}
																	else {
																		regex r("[a-f|A-F|0-9]{1}");
																		smatch sm1;
																		regex_search(hexa, sm1, r);
																		string prvi = sm1[0];
																		hexa = sm1.suffix().str();
																		if (len == 1) {
																			drugiB = "00";
																			prviB = "0" + prvi;
																		}
																		else {
																			drugiB = "0" + prvi;
																			smatch s2;
																			regex r1("[a-f|A-F|0-9]{2}");
																			regex_search(hexa, s2, r1);
																			string drugaDva = s2[0];
																			prviB = drugaDva;
																		}
																	}

																	codeText.push_back(prviB);
																	codeText.push_back(drugiB);

																}
																else {
																	assert(0);
																}
																//i ovde ako treba

															}
															if (r.adr == regindpom) {
																//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
																//znaci ako je regindpom, treba svakako da uvrstim pomeraj u kod
																//ako je pomeraj celobrojna velicina, onda ga samo ubaci u kod
																//hajde da proverimo da li je ovo simbol

																regex alfanum("\\[.+\\]");
																smatch reza;
																string imeSaZag;
																string zaPretragu = r.s;
																string ime;
																int flagDolar = 0;
																if (regex_search(zaPretragu, reza, alfanum)) {
																	imeSaZag = reza[0];

																	regex alfanum1("[a-z|A-Z|0-9]+");
																	//string ime;
																	smatch rez1;
																	if (regex_search(imeSaZag, rez1, alfanum1)) {
																		ime = rez1[0];


																	}
																	else {
																		assert(0);
																	}
																}
																else {
																	// pokusaj da nadjes onaj $
																	regex radr("\\$[a-z|A-Z|0-9]+");
																	smatch smadr;
																	if (regex_search(zaPretragu, smadr, radr)) {
							
																		flagDolar++;
																		string tmp = smadr[0];
																		regex alfa("[a-z|A-Z|0-9]+");
																		smatch kon;
																		regex_search(tmp, kon, alfa);
																		ime = kon[0]; //samo si uzeo da ime bude sve posle$ umesto zagrada

																		
																		

																	}
																	else {
																		assert(0);
																	}

																}
																Simbol * simb = tabelaSimbola[ime];
																int vrednost;
																if (simb == 0) {
																	//broj je obican, tj samo je numericki pomeraj
																	//izvuci taj brojcic 
																	regex cifra("[0-9]+");
																	if (regex_match(ime, cifra)) {
																		vrednost = stoi(ime); // mozda je trebalo prebaciti ovo u hexa pa uzeti vrednost!!!!!!!!!
																	}
																	else {
																		cout << "nesto sa pomerajem, nije ni simbol iz tabele simbola a ni cist broj";
																		assert(0);
																	}
																	
																}
																else {
																	//iz tabele simbola je!
																	RelAbs xyz = absolute;
															//		if (flagDolar == 1) xyz = relative;
																	//relativno je samo ako je jmp $labela !!!!!!!!!!!
																	if (xyIns->name == "jmp" && flagDolar) {
																		xyz = relative;
																	}
																	noviRelSimb(ime, locationCnt + 2, curSection, xyz);
																	vrednost = simb->vrednost;
																}
																if (flagDolar && (xyIns->name == "jmp")) {
																	//vrednost je odrediste, oduzmi od toga locationCnt + 4
																	vrednost = vrednost - (locationCnt + 4);
																	//ali ne ugradjujes uvek ovo! ako global ugradis samo minus -2
																	// pa ces posle u emulatoru srediti to sve lepo 
																	if (simb&& simb->lg == g) {
																		vrednost = -2;
																	}
																}
																//sada ga samo ugradi u kod, to je isto koa i za ono sranje dole
																//e nije isto, ako je relativni
																
																string prviB;
																string drugiB;
																
																int pomeraj = vrednost;
																/*
																if (flagDolar == 1 && (simb && simb->lg == g)) {
																	pomeraj = -2;
																}
																else {
																	if (flagDolar && simb) {
																		pomeraj -= 2;
																	}
																}
																*/
																string hexPom = Converter::DecimalToHexa(pomeraj);
																int len = hexPom.length();
																if (len == 2 || len == 4) {
																	regex r("[a-f|A-F|0-9]{2}");
																	smatch sm1;
																	regex_search(hexPom, sm1, r);
																	string prvaDva = sm1[0];
																	hexPom = sm1.suffix().str();
																	if (len == 2) {
																		prviB = prvaDva;
																		drugiB = "00";
																	}
																	else {
																		drugiB = prvaDva;
																		smatch s2;
																		regex_search(hexPom, s2, r);
																		string drugaDva = s2[0];
																		prviB = drugaDva;
																	}
																}
																else {
																	regex r("[a-f|A-F|0-9]{1}");
																	smatch sm1;
																	regex_search(hexPom, sm1, r);
																	string prvi = sm1[0];
																	hexPom = sm1.suffix().str();
																	if (len == 1) {
																		drugiB = "00";
																		prviB = "0" + prvi;
																	}
																	else {
																		drugiB = "0" + prvi;
																		smatch s2;
																		regex r1("[a-f|A-F|0-9]{2}");
																		regex_search(hexPom, s2, r1);
																		string drugaDva = s2[0];
																		prviB = drugaDva;
																	}
																}

																codeText.push_back(prviB);
																codeText.push_back(drugiB);
																//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

															}

															if (r.adr == immed) {

																//##############################################################################################################################
																regex rbroj("[0-9]+");
																string zaPretragu = r.s;
																smatch sbroj;
																string prviB;
																string drugiB;
																int pomeraj;
																
																regex rege("\\&");

																if (!regex_search(zaPretragu,rege)) {
															    regex_search(zaPretragu, sbroj, rbroj);
																string pomS = sbroj[0];
																 pomeraj = stoi(pomS);
																}
																else {
																	//pokusaj da uzmes sada onaj && 
																	regex radr("\\&[a-z|A-Z][a-z|A-Z|0-9]*");
																	smatch smadr;
																	if (regex_search(zaPretragu, smadr, radr)) {
																		string tmp = smadr[0];
																		regex alfa("[a-z|A-Z|0-9]+");
																		smatch kon;
																		regex_search(tmp, kon, alfa);
																	string	ime = kon[0]; //samo si uzeo da ime bude sve posle& umesto zagrada
																		Simbol * xxy = tabelaSimbola[ime];
																		if (xxy == 0) {
																			cout << "ne moze & od necega sto nije simbol" << endl;

																			assert(0);
																		}
																		else {
																			noviRelSimb(ime,locationCnt + 2,curSection,absolute);
																			pomeraj = xxy->vrednost;

																		}

																	}
																	else {
																		assert(0);
																	}
																	////////////////////
																}
																string hexPom = Converter::DecimalToHexa(pomeraj);
																int len = hexPom.length();
																if (len == 2 || len == 4) {
																	regex r("[a-f|A-F|0-9]{2}");
																	smatch sm1;
																	regex_search(hexPom, sm1, r);
																	string prvaDva = sm1[0];
																	hexPom = sm1.suffix().str();
																	if (len == 2) {
																		prviB = prvaDva;
																		drugiB = "00";
																	}
																	else {
																		drugiB = prvaDva;
																		smatch s2;
																		regex_search(hexPom, s2, r);
																		string drugaDva = s2[0];
																		prviB = drugaDva;
																	}
																}
																else {
																	regex r("[a-f|A-F|0-9]{1}");
																	smatch sm1;
																	regex_search(hexPom, sm1, r);
																	string prvi = sm1[0];
																	hexPom = sm1.suffix().str();
																	if (len == 1) {
																		drugiB = "00";
																		prviB = "0" + prvi;
																	}
																	else {
																		drugiB = "0" + prvi;
																		smatch s2;
																		regex r1("[a-f|A-F|0-9]{2}");
																		regex_search(hexPom, s2, r1);
																		string drugaDva = s2[0];
																		prviB = drugaDva;
																	}
																}

																codeText.push_back(prviB);
																codeText.push_back(drugiB);

																//##############################################################################################################################

															}

															if (xyIns->name == "push" || xyIns->name == "pop" || xyIns->name == "call" || xyIns->name == "jmp" ) {
																if (r.adr ==regdir) locationCnt += 2;
																else locationCnt += 4; //call moze valjda samo sa memdir da ide??????////
																std::getline(ulaz, linija);
																continue;

															}

															//znaci onda ovde nam ostaju sammo ins koje imaju dva operanda!
															//Ret1 r1 = odrediTipAdresiranja(linija);
															
															//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
															if (r1.adr == memdir) {
																//treba da dodamo mozda realokacioni zapis
																string opcode = xyIns->opcode;
																string prviB;
																string imeS;
																smatch sm;
																regex reg("[a-z|0-9]+");
																int flagZvezdica = 0;
																regex rspec("\\*");
																smatch smzvezda;
																if (regex_search(r1.s, smzvezda, rspec)) {
																	//string s = sm[0];
																	r1.s = smzvezda.suffix().str();
																	flagZvezdica++;
																}


																if (regex_search(r1.s, sm, reg)) {
																	imeS = sm[0];
																	string prviB;
																	string drugiB;
																	int dec;
																	if (flagZvezdica == 0) {
																		noviRelSimb(imeS, locationCnt + 2, curSection, absolute);

																		Simbol * simbol = tabelaSimbola[imeS];

																		 dec = simbol->vrednost;
																	}
																	else {
																		dec = stoi(imeS);
																	}
																	string hexa = Converter::DecimalToHexa(dec);
																	int len = hexa.length();
																	if (len == 2 || len == 4) {
																		regex r("[a-f|A-F|0-9]{2}");
																		smatch sm1;
																		regex_search(hexa, sm1, r);
																		string prvaDva = sm1[0];
																		hexa = sm1.suffix().str();
																		if (len == 2) {
																			prviB = prvaDva;
																			drugiB = "00";
																		}
																		else {
																			drugiB = prvaDva;
																			smatch s2;
																			regex_search(hexa, s2, r);
																			string drugaDva = s2[0];
																			prviB = drugaDva;
																		}
																	}
																	else {
																		regex r("[a-f|A-F|0-9]{1}");
																		smatch sm1;
																		regex_search(hexa, sm1, r);
																		string prvi = sm1[0];
																		hexa = sm1.suffix().str();
																		if (len == 1) {
																			drugiB = "00";
																			prviB = "0" + prvi;
																		}
																		else {
																			drugiB = "0" + prvi;
																			smatch s2;
																			regex r1("[a-f|A-F|0-9]{2}");
																			regex_search(hexa, s2, r1);
																			string drugaDva = s2[0];
																			prviB = drugaDva;
																		}
																	}

																	codeText.push_back(prviB);
																	codeText.push_back(drugiB);

																}
																else {
																	assert(0);
																}
																//i ovde ako treba

															}

															//regindpom !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
															if (r1.adr == regindpom) {
																//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
																//znaci ako je regindpom, treba svakako da uvrstim pomeraj u kod
																//ako je pomeraj celobrojna velicina, onda ga samo ubaci u kod
																//hajde da proverimo da li je ovo simbol

																regex alfanum("\\[.+\\]");
																int flagDolar = 0;
																smatch reza;
																string imeSaZag;
																string zaPretragu = r1.s;
																
																string ime;
																
																if (regex_search(zaPretragu, reza, alfanum)) {
																	imeSaZag = reza[0];

																	regex alfanum1("[a-z|A-Z|0-9]+");
																	ime;
																	smatch rez1;
																	if (regex_search(imeSaZag, rez1, alfanum1)) {
																		ime = rez1[0];


																	}
																	else {
																		assert(0);
																	}
																}
																else {
																	//ovde kopiraj
																	// pokusaj da nadjes onaj &
																	regex radr("\\$[a-z|A-Z|0-9]+");
																	smatch smadr;
																	if (regex_search(zaPretragu, smadr, radr)) {
																		string tmp = smadr[0];
																		regex alfa("[a-z|A-Z|0-9]+");
																		smatch kon;
																		regex_search(tmp, kon, alfa);
																		ime = kon[0]; //samo si uzeo da ime bude sve posle& umesto zagrada
																	
																		flagDolar += 1;
																	}
																	else {
																		assert(0);
																	}

																}
																Simbol * simb = tabelaSimbola[ime];
																int vrednost;
																if (simb == 0) {
																	//broj je obican, tj samo je numericki pomeraj
																	//izvuci taj brojcic 
																	regex cifra("[0-9]+");
																	if (regex_match(ime, cifra)) {
																		vrednost = stoi(ime); // mozda je trebalo prebaciti ovo u hexa pa uzeti vrednost!!!!!!!!!
																	}
																	else {
																		cout << "nesto sa pomerajem, nije ni simbol iz tabele simbola a ni cist broj";
																		assert(0);
																	}

																}
																else {
																	//iz tabele simbola je!
																	RelAbs xyz = absolute;
																//	if (flagDolar == 0) xyz = absolute;
																	
																	noviRelSimb(ime, locationCnt + 2, curSection, xyz);
																	vrednost = simb->vrednost;
																}

																//sada ga samo ugradi u kod, to je isto koa i za ono sranje dole

																string prviB;
																string drugiB;

																int pomeraj = vrednost;
																/*
																if (flagDolar == 1 && (simb && simb->lg == g)) {
																	pomeraj = -2;
																}
																else {
																	if (flagDolar && simb) {
																		pomeraj -= 2;
																	}
																}
																*/
																string hexPom = Converter::DecimalToHexa(pomeraj);
																int len = hexPom.length();
																if (len == 2 || len == 4) {
																	regex r("[a-f|A-F|0-9]{2}");
																	smatch sm1;
																	regex_search(hexPom, sm1, r);
																	string prvaDva = sm1[0];
																	hexPom = sm1.suffix().str();
																	if (len == 2) {
																		prviB = prvaDva;
																		drugiB = "00";
																	}
																	else {
																		drugiB = prvaDva;
																		smatch s2;
																		regex_search(hexPom, s2, r);
																		string drugaDva = s2[0];
																		prviB = drugaDva;
																	}
																}
																else {
																	regex r("[a-f|A-F|0-9]{1}");
																	smatch sm1;
																	regex_search(hexPom, sm1, r);
																	string prvi = sm1[0];
																	hexPom = sm1.suffix().str();
																	if (len == 1) {
																		drugiB = "00";
																		prviB = "0" + prvi;
																	}
																	else {
																		drugiB = "0" + prvi;
																		smatch s2;
																		regex r1("[a-f|A-F|0-9]{2}");
																		regex_search(hexPom, s2, r1);
																		string drugaDva = s2[0];
																		prviB = drugaDva;
																	}
																}

																codeText.push_back(prviB);
																codeText.push_back(drugiB);
																//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

															}

															//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
															if (r1.adr == immed) {

																//##############################################################################################################################
																regex rbroj("[0-9]+");
																string zaPretragu = r1.s;
																smatch sbroj;
																string prviB;
																string drugiB;
																regex rege("\\&");
																int pomeraj;
																if (!regex_search(zaPretragu, rege)) {
																	regex_search(zaPretragu, sbroj, rbroj);
																	string pomS = sbroj[0];
																	 pomeraj = stoi(pomS);
																}
																else {
																	//**********************************************************
																	//pokusaj da uzmes sada onaj && 
																	regex radr("\\&[a-z|A-Z][a-z|A-Z|0-9]*");
																	smatch smadr;
																	if (regex_search(zaPretragu, smadr, radr)) {
																		string tmp = smadr[0];
																		regex alfa("[a-z|A-Z|0-9]+");
																		smatch kon;
																		regex_search(tmp, kon, alfa);
																		string	ime = kon[0]; //samo si uzeo da ime bude sve posle& umesto zagrada
																		Simbol * xxy = tabelaSimbola[ime];
																		if (xxy == 0) {
																			cout << "ne moze & od necega sto nije simbol" << endl;

																			assert(0);
																		}
																		else {
																			noviRelSimb(ime, locationCnt + 2, curSection, absolute);
																			pomeraj = xxy->vrednost;

																		}

																	}
																	else {
																		assert(0);
																	}
																	////////////////////

																	//************************************************************8

																}
																string hexPom = Converter::DecimalToHexa(pomeraj);
																int len = hexPom.length();
																if (len == 2 || len == 4) {
																	regex r("[a-f|A-F|0-9]{2}");
																	smatch sm1;
																	regex_search(hexPom, sm1, r);
																	string prvaDva = sm1[0];
																	hexPom = sm1.suffix().str();
																	if (len == 2) {
																		prviB = prvaDva;
																		drugiB = "00";
																	}
																	else {
																		drugiB = prvaDva;
																		smatch s2;
																		regex_search(hexPom, s2, r);
																		string drugaDva = s2[0];
																		prviB = drugaDva;
																	}
																}
																else {
																	regex r("[a-f|A-F|0-9]{1}");
																	smatch sm1;
																	regex_search(hexPom, sm1, r);
																	string prvi = sm1[0];
																	hexPom = sm1.suffix().str();
																	if (len == 1) {
																		drugiB = "00";
																		prviB = "0" + prvi;
																	}
																	else {
																		drugiB = "0" + prvi;
																		smatch s2;
																		regex r1("[a-f|A-F|0-9]{2}");
																		regex_search(hexPom, s2, r1);
																		string drugaDva = s2[0];
																		prviB = drugaDva;
																	}
																}

																codeText.push_back(prviB);
																codeText.push_back(drugiB);

																//##############################################################################################################################

															}

															 //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
															 if (r.adr == regdir && r1.adr == regdir) locationCnt += 2;
															 else locationCnt += 4;
															 
														


														}
														

													}

												}

											}
										}
									}
								}


							}
						}
					}
				}








			}

		}


		std::getline(ulaz, linija);
	}

	vector <string>::iterator ij;
	if (curSection == ".text") {
		for (ij = codeText.begin(); ij != codeText.end(); ij++) {
			Memorija.push_back((*ij));

		}
	}
	else {
		if (curSection == ".data") {
			for (ij = codeData.begin(); ij != codeData.end(); ij++) {
				Memorija.push_back((*ij));

			}
		}
		else {
			if (curSection == ".rodata") {
				for (ij = codeRodata.begin(); ij != codeRodata.end(); ij++) {
					Memorija.push_back((*ij));

				}
			}
		}
	}


}


ReallocationSimbol * Asembler::noviRelSimb(string ime, int adresa, string curSekcija, RelAbs x) {
	ReallocationSimbol * novi = 0;
	Simbol * simb = tabelaSimbola[ime];
	if (simb == 0) {
		assert(0);
	}
	string adr;
	if (curSekcija == simb->sekcija) {
		if (x == relative && simb->lg == l)
			return 0;
		if (simb->lg == l) {
		Simbol * sekcija = tabelaSimbola[simb->sekcija];

		int rb = sekcija->rb;
		adr = Converter::DecimalToHexa(adresa);
		novi = new ReallocationSimbol(adr, x, rb, sekcija->name);
	   }
	}
	//string adr;
	if (simb->lg == l) {
		Simbol * sekcija = tabelaSimbola[simb->sekcija];
		
		int rb = sekcija->rb;
		 adr = Converter::DecimalToHexa(adresa);
		novi = new ReallocationSimbol(adr,x,rb,sekcija->name);


	}
	else {
		int rb = simb->rb;
	    adr = Converter::DecimalToHexa(adresa);
		novi = new ReallocationSimbol(adr, x, rb,simb->name);
		

	}

	if (curSekcija == ".data") {
		reallocData[novi->adresa] = novi;

	}
	else {
		if (curSekcija == ".text") {
			reallocText[novi->adresa] = novi;
		}
		else {
			if (curSekcija == ".rodata") {
				reallocRoData[novi->adresa] = novi;
			}
			else {
				cout << " ne sme da bude nista u bss sekciji" << endl;
				assert(0);
			}
		}
	}

	return novi;
}


 Ret1 Asembler::odrediTipAdresiranja(string linija) {
	 //pazi i na spec znake ovdde
	 // $ je regindpom
	 // & trebalo bi da je immed
	 // * trebalo bi da je memdir

	string drugaRec; //ovde spakuj dst
	

	regex r1("[^\,]+");
	smatch s1;

	regex r2("\ *r[0-7]\ *");
	smatch s2;
	
	if (regex_search(linija, s1, r1)) {
		drugaRec = s1[0]; // druga rec mi je ustvari operand sa zagradama ili kako vec
		linija = s1.suffix().str();
		if (regex_match(drugaRec, r2)) {
			//return regdir;
			Ret1 r(regdir, drugaRec,linija);
			return r;
		}
		//linija = s1.suffix().str();   // ********************************************************************************* PAZI

		regex r3("\ *r[0-7]\\[.+\\] *");
		if (regex_search(drugaRec, r3)) {
			
			Ret1 r(regindpom, drugaRec,linija);
			return r;
		}

		//deo za dolar $$$$$$$$$$$$$$$$$$$
		regex rdolar("\\$[a-z|A-Z|0-9]+");
		if (regex_search(drugaRec, rdolar)) {
			Ret1 r(regindpom, drugaRec, linija);
			return r;
		}
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


		// evo ga deo za zvezdicu
		// opet morao onaj dupli escape karakter ******************
		regex rspec("\\*[0-9]+");
		smatch smspec;
		if (regex_search(drugaRec,rspec)) {
			Ret1 r(memdir, drugaRec, linija);
			return r;
		}
	 //// ***********************************
		regex r4("\ *[a-z][a-z|0-9]*\ *");
		if (regex_match(drugaRec, r4)) {
			Ret1 r(memdir, drugaRec,linija);
			return r;
		}

		// deo za &&&&&&&&&&&&&&&&&&&&&

		regex radr("\\&[a-z|A-Z][a-z|A-Z|0-9]*");
		if (regex_search(drugaRec, radr)) {
			Ret1 r(immed, drugaRec, linija);
			return r;
		}

		// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&u

		regex r5("\ *[0-9]+\ *");
		if (regex_match(drugaRec, r5)) {
			Ret1 r(immed, drugaRec,linija);
			return r;
		}
		//verovatno mora assert, jer je uneto nesto debilno
		cout << "los nacin adresiranja";
		assert(0);

	}
	else {
		cout << "fale dst operand posle instrukcije ";
		assert(0);
	}
	

}


 void Asembler::writeSimbolTable(ofstream & izlaz) {
	 izlaz << pocetakTrenutnogFajla << " " << duzinaTrenutnogFajla << " " << pocetakBssSekcije << " " << krajBssSekcije << endl;
	 izlaz << "\t\t #.Tabela simbola " << endl;

	 map<string, Simbol *>::iterator i;
	 izlaz << "ime\t\tsekcija\t\tvred\t\tlg\t\trb\t\tpoc" << "\t\t" << "kraj" <<  endl << endl;
	 int x = 0;
	
		 for (i = tabelaSimbola.begin(); i != tabelaSimbola.end(); i++) {

		 Simbol * s = i->second;
		 if (s == 0) {
			 continue;
			 cout <<endl<< i->first << endl;
			 assert(0);
		 }
		 /*
		 if (s->name == ".data" || s->name == ".text" || s->name == ".rodata" || s->name == ".bss" || s->name == "UND") {
			 continue;
		 }
		 */
		 string lg;
		 if (s->lg == l) lg = "loc";
		 else lg = "glo";
		 string ispis = Converter::DecimalToHexa(s->vrednost);
		 if (ispis == "" || ispis.length() == 0) ispis = "0";
		 izlaz << s->name << "\t\t" << s->sekcija << "\t\t" << ispis << "\t\t" << lg << "\t\t" << s->rb;
		 if (s->name == ".data" || s->name == ".text" || s->name == ".rodata" || s->name == ".bss") {
			 izlaz << "\t\t" << s->pocetak << "\t\t" << s->kraj;
		 }
		

		 izlaz << endl;

	 

	 }


 }

 string Asembler::opCodeAdr(tipAdr a) {
	 if (a == regdir) return "01";
	 if (a == regindpom)return "11";
	 if (a == immed) return "00";
	 if (a == memdir) return "10";
	 assert(0);
 }


 string Asembler::registar(int d) {

	 if (d == 0) return "000";
	 if (d == 1) return "001";
	 if (d == 2) return "010";
	 if (d == 3) return "011";
	 if (d == 4) return "100";
	 if (d == 5) return "101";
	 if (d == 6) return "110";
	 if (d == 7)return "111";
	 assert(0);
 }

 void Asembler::writeRealocatioTables(ofstream & izlaz) {
	 map<string, ReallocationSimbol * >::iterator i;
	 izlaz << endl << endl;
	 izlaz << "\t " << "#.ret.text" << endl;
	 izlaz << "offset" << "\t\t" << "tip_adr    " << "\t\t" << "rb"  << "\t\t" <<"imeRB"<< endl << endl;
	 for (i = reallocText.begin(); i != reallocText.end(); i++) {

		 ReallocationSimbol * simbol = i->second;

		 izlaz << simbol->adresa << "\t\t" << simbol->tip << "\t\t" << simbol->rb <<"\t\t" << simbol->imeRB << endl;


	 }

	 izlaz << endl << endl;
	 izlaz << "\t " << "#.ret.data" << endl;
	 izlaz << "offset" << "\t\t" << "tip_adr    " << "\t\t" << "rb" << "\t\t" << "imeRB" << endl << endl;
	 for (i = reallocData.begin(); i != reallocData.end(); i++) {

		 ReallocationSimbol * simbol = i->second;

		 izlaz << simbol->adresa << "\t\t" << simbol->tip << "\t\t" << simbol->rb << "\t\t" << simbol->imeRB << endl;


	 }

	 izlaz << endl << endl;
	 izlaz << "\t " << "#.ret.rodata" << endl;
	 izlaz << "offset" << "\t\t" << "tip_adr    " << "\t\t" << "rb" << "\t\t" << "imeRB" << endl << endl;
	 for (i = reallocRoData.begin(); i != reallocRoData.end(); i++) {

		 ReallocationSimbol * simbol = i->second;

		 izlaz << simbol->adresa << "\t\t" << simbol->tip << "\t\t" << simbol->rb << "\t\t" << simbol->imeRB << endl;


	 }




	 //ovo isto i za realloc data i za realloc rodata
 }

 void Asembler::writeInitContent(ofstream & izlaz) {

	 vector<string>::iterator i;
	 izlaz << endl << endl;
	 izlaz << "\t " << "#.text" << endl << endl;
	 for (i = codeText.begin(); i != codeText.end(); i++) {

		 string bajt = (*i);
		 izlaz << bajt << "  ";

	 }
	 izlaz << endl << endl;
	 izlaz << "\t " << "#.data" << endl << endl;
	 for (i = codeData.begin(); i != codeData.end(); i++) {

		 string bajt = (*i);
		 izlaz << bajt << "  ";

	 }

	 izlaz << endl << endl;
	 izlaz << "\t " << "#.rodata" << endl << endl;
	 for (i = codeRodata.begin(); i != codeRodata.end(); i++) {

		 string bajt = (*i);
		 izlaz << bajt << "  ";

	 }



 }

 string Asembler::getConditionBits(string s) {
	 if (s == "AL" || s == "aL" || s == "Al" || s == "al") return "11";
	 if (s == "GT" || s == "gT" || s == "Gt" || s == "gt") return "10";
	 if (s == "NE" || s == "nE" || s == "Ne" || s == "ne") return "01";
	 if (s == "EQ" || s == "eQ" || s == "Eq" || s == "eq") return "00";
	 
	assert(0);
 }


 void Asembler::writeMemory(ofstream & izlaz) {
	 vector<string>::iterator i = Memorija.begin();
	 for (; i != Memorija.end(); i++) {
		 izlaz << (*i) << " ";
	 }
 }

 /*
 20 \96 neposredna vrednost 20       
&x \96 vrednost simbola x            mislim da je ovo memorijsko direktno, ali ar kaze da je neposredno!!!???? moze biti,jer nema pristupa mem
x \96 memorijsko direktno adresiranje
*20 \96 lokacija sa adrese 20        ako vidis * to je onda sigurno memdir
r3 \96 direktno registarsko
r4[32] \96 registarsko indirektno sa pomerajem
r5[x] \96 registarsko indirektno sa pomerajem
$x \96 PC relativno adresiranje promenljive x          sta cu sa ovim sunce ti??????????  jmp $x, to znaci na pc dodaj x i skoci tu? da li onda jmp moze da ide jedino sa ovim nacinom adresiranja? ili sme jmp i na neku apsolutnu adresu?

 */
