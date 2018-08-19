# ssp

Opis problema

Problem je napraviti dvoprolazni asembler i emulator. Ulaz asemblera je tekstualni fajl (asemblerski kod) i pocetna adresa masinskog koda u memoriji. Izlaz asemblera je tekstualni fajl sa tabelom sekcija, simbola, realokacija i masinskim kodom za svaku sekciju. Taj tekstualni falj, kao i drugi u istom formatu, su ulaz emulatora ciji je posao da procita ceo fajl i izvrsi dati masinski kod.

Opis resenja

Projekat se sastoji iz dve staticke biblioteke i dva projekta koja koriste te biblioteke. 
Prva biblioteka, tables, sadrzi pomocne strukture za cuvanje sekcija, simbola, realokacija i njihovih tabela. Takodje sadrzi pomocne funkcije koje se koriste za pretvaranje brojeva u razlicite sisteme i formate koje sluze za ispis i citanje. 
Druga biblioteka, input, sluzi za ucitavanje karaktera sa standardnog ulaza. Da bi to uspela, biblioteka prilikom pokretanja menja nacin rada konzole na Linuxu.

Projekat assembler koristi biblioteku tables i sluzi za citanje asemblerskog koda iz tekstualnog fajla i kreiranje objektnog fajla. Sastoji se iz svih struktura podataka koje nudi biblioteka tables. Prilikom prvog prolaza asemblerskog koda, citaju se svi simboli koji su definisani u kodu i upisuju se u tabelu simbola, citaju se sve definisane sekcije i cuvaju se u tabeli sekcija i inkrementira se location counter za svaku procitanu instrukciju. Na pocetku drugog prolaza restartuje se location counter i filestream za ulazni fajl. Tokom drugog prolaza, azuriraju se globalni simboli u tabeli i desifruju se sve instrukcije i direktive.

Projekat emulator koristi obe opisane biblioteke i sluzi za citanje objektnog fajla koji generise asembler i izvrsavanje istog. Ulaz ovog projekta su izlazni fajlovi asemblera. U jednom od tih fajlova mora biti definisan simbol .start, ne smeju se preklapati adrese u memoriji i ne sme postojati visestruka definicija globalnih simbola. Prvi zadatak emulatora je da procita sve objekne fajlove i da konstruise svoje strukture podataka za sekcije i simbole. Drugi zadatak je da razresi sve realokacije koje su nastale tokom asembliranja i da izmeni masinski kod zbog nastalih realokacija. Treci zadatak emulatora je da desifruje masinski kod i da izvrsi asemblerski program pocev od simbola .start. Izlaz emulatora je tekstualni fajl koji ispisuje sve strukture podataka, deo memorije i sve registre.

Uputstvo za prevodjenje

Za prevodjenje je koriscen g++-6 paket. Za instalaciju paketa potrebne su sledece instrukcije:

sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-6
sudo apt-get install g++-6

Sledeci korak je da se naprave biblioteke koje ce se koristiti u oba projekta. Prvo se svaki od .cpp fajlova prevede u objektni fajl, a potom se objektni fajlovi zapakuju u biblioteku komandama:

g++-6 -c naziv.cpp -o ../../bin/static/tables/naziv.out
g++-6 -c input.cpp -o ../../bin/static/input/input.out
ar rcs libtables.a *.out
ar rcs libinput.a *.out

Poslednji korak je da se prevedu glavni projekti.

Asembler:
g++-6 *.h *.cpp -L../../bin/static/tables -ltables -o ../../bin/assembler.out

Emulator:
g++-6 *.h *.cpp -L../../bin/static/tables -ltables -L../../bin/static/input       -linput -o ../../bin/emulator.out -pthread

Za pokretanje:

./assembler.out ../tests/test1/test.txt 1100
./emulator.out assembler_test.txt

 
Test1
Test
.text

.start

#set periodic
alor PSW, 4096

almov r0, 5
alcall &function
r:

almov r1, 10
aljmp &end

function:
almov r2, 15
alret

end:

.data

a: .char 5
b: .word 26
c: .long 3840

.end
 
Izlaz asemblera
#/home/etf/Desktop/test/test.txt
#Section table
#name		start		end		length
.text		1100		1125		26
.data		1126		1132		7

#Simbol table
#name		section		value		visibility	index
UND		UND		0		local		0
.text		.text		0		local		1
.start		.text		1100		global		2
r		.text		1112		local		3
function	.text		1120		local		4
end		.text		1126		local		5
.data		.data		0		local		6
a		.data		1126		local		7
b		.data		1127		local		8
c		.data		1129		local		9

#Reallocation table for .data
#address		type		index

#Reallocation table for .text
#address		type		index

#.data code
05 1A 00 00 0F 00 00
#.text code
D8 E0 00 10 F5 00 05 00 EC 00 60 04 F5 20 0A 00 F5 E0 66 04 F5 40 0F 00 E9 E0
 
Izlaz emulatora
#/home/etf/Desktop/test/assembler.txt
#Section table
#name		start		end		length
.text		1100		1125		26
.data		1126		1132		7

#Simbol table
#name		section		value		visibility	index
.start		.text		1100		global		1

#Reallocation table
#address		type		index

1100: D8 E0 0 10 F5 0 5 0 EC 0 60 4 F5 20 A 0 F5 E0 66 4 F5 40 F 0 E9 E0 
r[0] = 5 r[1] = A r[2] = F r[3] = B747 r[4] = 9910 r[5] = B76A r[6] = FF80 r[7] = 466
 
Test2
Test1
.global function

.text

.start

alcall &function

.end
Test2
.global function

.text

function:
almov r1, a
alret

.rodata

a: .char 5

.end
 
Izlaz asemblera za test1
#../test2/test1.txt
#Section table
#name		start		end		length
.text		1100		1103		4

#Simbol table
#name		section		value		visibility	index
UND		UND		0		local		0
.text		.text		0		local		1
.start		.text		1100		global		2
function	UND		0		global		3

#Reallocation table for .text
#address		type		index
44E		R_386_32		3

#.text code
EC 00 00 00
 
Izlaz asemblera za test2
#../test2/test2.txt
#Section table
#name		start		end		length
.text		1200		1205		6
.rodata		1206		1206		1

#Simbol table
#name		section		value		visibility	index
UND		UND		0		local		0
.text		.text		0		local		1
function	.text		1200		global		2
.rodata		.rodata		0		local		3
a		.rodata		1206		local		4

#Reallocation table for .rodata
#address		type		index

#Reallocation table for .text
#address		type		index

#.rodata code
05
#.text code
F5 30 B6 04 E9 E0
 
Izlaz emulatora
#../test2/assembler1.txt
#../test2/assembler2.txt
#Section table
#name		start		end		length
.text		1100		1103		4

#Section table
#name		start		end		length
.text		1200		1205		6
.rodata		1206		1206		1

#Simbol table
#name		section		value		visibility	index
.start		.text		1100		global		1
function	.text		1200		global		2

#Reallocation table
#address		type		index
44E		R_386_32		2

1100: EC 0 B0 4 
r[0] = E264 r[1] = 5 r[2] = 8225 r[3] = B745 r[4] = 2910 r[5] = B768 r[6] = FF80 r[7] = 450
 
Test3
Test1
.global function, index

.text

.start

#r0 = brojac
almov r0, index
#r1 = rezultat
almov r1, 0

loop:

alpush offset
alcall &function

alsub r0, 1
nejmp &loop

.rodata

offset: .char 5

.end
 
Test2
.global function, index

.text

function:
alpop r2
alpop r3
aladd r1, r3
alpush r2
alret

.rodata

index: .char 5

.end
 
Izlaz asemblera za test1
#test1.txt
#Section table
#name		start		end		length
.text		1100		1123		24
.rodata		1124		1124		1

#Simbol table
#name		section		value		visibility	index
UND		UND		0		local		0
.text		.text		0		local		1
.start		.text		1100		global		2
loop		.text		1108		local		3
.rodata		.rodata		0		local		4
offset		.rodata		1124		local		5
function	UND		0		global		6
index		UND		0		global		7

#Reallocation table for .rodata
#address		type		index

#Reallocation table for .text
#address		type		index
44E		R_386_32		7
45A		R_386_32		6

#.rodata code
05
#.text code
F5 10 00 00 F5 20 00 00 E6 00 64 04 EC 00 00 00 C5 00 01 00 75 E0 54 04
 
Izlaz asemblera za test2
#test2.txt
#Section table
#name		start		end		length
.text		1200		1209		10
.rodata		1210		1210		1

#Simbol table
#name		section		value		visibility	index
UND		UND		0		local		0
.text		.text		0		local		1
function	.text		1200		global		2
.rodata		.rodata		0		local		3
index		.rodata		1210		global		4

#Reallocation table for .rodata
#address		type		index

#Reallocation table for .text
#address		type		index

#.rodata code
05
#.text code
E9 40 E9 60 C1 2B E5 40 E9 E0
 
Izlaz emulatora
#../test3/assembler_test1.txt
#../test3/assembler_test2.txt
#Section table
#name		start		end		length
.text		1100		1123		24
.rodata		1124		1124		1

#Section table
#name		start		end		length
.text		1200		1209		10
.rodata		1210		1210		1

#Simbol table
#name		section		value		visibility	index
.start		.text		1100		global		1
function	.text		1200		global		2
index		.rodata		1210		global		3

#Reallocation table
#address		type		index
44E		R_386_32		3
45A		R_386_32		2

1100: F5 10 BA 4 F5 20 0 0 E6 0 64 4 EC 0 B0 4 C5 0 1 0 75 E0 54 4 
r[0] = 0 r[1] = 19 r[2] = 45C r[3] = 5 r[4] = 6910 r[5] = B76D r[6] = FF80 r[7] = 464
