#include "pep8cpu.hpp"
#include "pep8mem.hpp"
#include <cstdio>
#include <exception>
#include <fstream>
#include <sstream>
#include <tap++/tap++.h>
#include <vector>
using namespace std;
using namespace TAP;

int main() {
	plan(37);
	Pep8Memory mem;
	Pep8CPU cpu(mem);

	// Load the OS
	try {
		ifstream os(PKGDIR "/pep8os.pepo");
		vector< uint8_t > image;
		while(os.good()) {
			char buf[4];
			os.read(buf,3);
			buf[3]='\0';
			uint8_t newByte;
			if(!sscanf(buf,"%02X",&newByte))
				break;
			image.push_back(newByte);
		}
		for(off_t where=0;where<image.size();where++)
			mem.setUB(where+0x10000-image.size(),image[where]);
		os.close();
	} catch (exception& e) {
		fail((string)"OS Load: "+e.what());
		skip(36);
		return exit_status();
	}
	ok(true,"OS Load");

	// Load the testing image
	try {
		ifstream ti(PKGDIR "/test.pepo");
		cpu.setSP(mem.getUW(0xFFFA));
		cpu.setPC(mem.getUW(0xFFFC));
		while(cpu.doInstruction(ti,clog));
		ok(true,"Image Load");
	} catch (exception& e) {
		fail((string)"Image Load: "+e.what());
		diag("Falling back to MANUAL load");
		FILE *ti = fopen(PKGDIR "/test.pepo","r");
		uint8_t* pMem = mem.getImage();
		while(fscanf(ti,"%02X",pMem))
			pMem++;
		diag("Loaded ",pMem-mem.getImage()," bytes");
		fclose(ti);
	}

	// Run the testing image
	stringstream output;
	try {
		cpu.setSP(mem.getUW(0xFFF8));
		cpu.setPC(0x0000);
		while(cpu.doInstruction(output,output));
	} catch (exception& e) {
		fail((string)"Test Image: "+e.what());
		skip(34);
		return exit_status();
	}
	ok(true,"Test Image");

	// Check the testing image output
	const char* keywords[] = {"STOP ","MOVSPA ","MOVFLGA ","BR ","BRLE ","BRLT ","BREQ ","BRNE ","BRGE ","BRGT ","BRV ","BRC ","CALL ","NOTr ","NEGr ","ASLr ","ASRr ","ROLr ","RORr ","DECO ","STRO ","CHARO ","RETn ","ADDSP ","SUBSP ","ADDr ","SUBr ","ANDr ","ORr ","CPr ","LDr ","LDBYTEr ","STr ","STBYTEr "};
	for(int i=0;i<34;i++)
		ok(output.str().find(keywords[i])!=string::npos,keywords[i]);
	return exit_status();
}
