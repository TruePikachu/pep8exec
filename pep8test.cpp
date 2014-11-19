#include "pep8cpu.hpp"
#include "pep8mem.hpp"
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <tap++/tap++.h>
#include <vector>
using namespace std;
using namespace TAP;

static const char* tests[] = {"test01","inputs"};

int main() {
	plan(sizeof(tests)/sizeof(char*));
	Pep8Memory mem;
	Pep8CPU cpu(mem);

	// Load the OS
	try {
		ifstream os(PKGDIR "/pep8os.pepo");
		if(!os.good())
			throw runtime_error("Couldn't open OS file");
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
		diag((string)"OS load failed: "+e.what());
		return exit_status();
	}

	// Run each test
	for(int i=0;i<sizeof(tests)/sizeof(char*);i++) {
		char buf[1024];
		
		sprintf(buf,"%s/tests/%s.pepo",PKGDIR,tests[i]);
		ifstream src(buf);
		if(!src.good())
			throw logic_error((string)"Couldn't get test file "+tests[i]+".pepo");
		cpu.setSP(mem.getUW(0xFFFA));
		cpu.setPC(mem.getUW(0xFFFC));
		while(cpu.doInstruction(src,clog));
		src.close();

		sprintf(buf,"%s/tests/%s.in",PKGDIR,tests[i]);
		ifstream in(buf);
		sprintf(buf,"%s.tmp",tests[i]);
		ofstream out(buf);
		cpu.setSP(mem.getUW(0xFFF8));
		cpu.setPC(0x0000);
		while(cpu.doInstruction(in,out));
		in.close();
		out.close();

		sprintf(buf,"diff -q %1$s/tests/%2$s.out %2$s.tmp",PKGDIR,tests[i]);
		bool isOK = !system(buf);
		sprintf(buf,"rm %s/tests/%s.tmp",PKGDIR,tests[i]);
		if(isOK)
			system(buf);
		ok(isOK,tests[i]);
	}
	return exit_status();
}
