#include "pep8cpu.hpp"
#include "pep8mem.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct pSettings {	std::istream*	inputFile;
			std::ostream*	outputFile;
			bool		doTrace;
			std::ostream*	traceFile;
			bool		traceTrap;
			bool		traceLoad;
			bool		traceProg;
	} settings = {	NULL,
			NULL,
			false,
			NULL,
			false,
			false,
			true};
Pep8Memory	mem;
Pep8CPU		cpu(mem);

void doMenu();

int main(int argc, char *argv[]) {
	string osFilename = PKGDIR "/pep8os.pepo";
	string imageName;
	bool autorun=false;
	// Handle command line arguments
	for(int i=1;i<argc;i++) {
		string arg=argv[i];
		if(arg.substr(0,2)=="-i")
			settings.inputFile = new ifstream(arg.substr(2).c_str());
		else if(arg.substr(0,8)=="--input=")
			settings.inputFile = new ifstream(arg.substr(8).c_str());
		else if(arg.substr(0,2)=="-o")
			settings.outputFile = new ofstream(arg.substr(2).c_str());
		else if(arg.substr(0,9)=="--output=")
			settings.outputFile = new ofstream(arg.substr(9).c_str());
		else if(arg.substr(0,2)=="-t") {
			settings.doTrace=true;
			if(arg.substr(2).length())
				settings.traceFile=new ofstream(arg.substr(2).c_str());
		}else if(arg.substr(0,8)=="--trace=") {
			settings.doTrace=true;
			if(arg.substr(8).length())
				settings.traceFile=new ofstream(arg.substr(8).c_str());
		}else if(arg=="--trap-trace")
			settings.traceTrap=true;
		else if(arg=="--loader-trace")
			settings.traceLoad=true;
		else if(arg=="--no-program-trace")
			settings.traceProg=false;
		else if(arg.substr(0,5)=="--os=")
			osFilename=arg.substr(5);
		else if(arg=="-x")
			autorun=true;
		else if(arg=="--execute")
			autorun=true;
		else if(!imageName.length())
			imageName = arg;
		else {
			cerr << "Unknown argument: " << arg << endl;
			exit(1);
		}
	}
	// Load OS
	ifstream os(osFilename.c_str());
	mem.loadOS(os);
	os.close();
	// Handle execute mode
	if(autorun) {
		if(!imageName.length()) {
			cerr << "ERROR: -x/--execute requires a program name!\n";
			return 1;
		}
		ifstream src(imageName.c_str());
		cpu.setSP(mem.getUW(0xFFFA));
		cpu.setPC(mem.getUW(0xFFFC));
		while(cpu.doInstruction(src,clog));
		src.close();
		cpu.setSP(mem.getUW(0xFFF8));
		cpu.setPC(0x0000);
		if(!settings.inputFile)
			settings.inputFile=&cin;
		if(!settings.outputFile)
			settings.outputFile=&cout;
		while(cpu.doInstruction(*settings.inputFile,*settings.outputFile));
		return 0;
	}
	doMenu();
	return 0;
}

void doMenu() {
	char input[512];
	cout << mem.getRamSize() << "B RAM   " << mem.getRomSize() << "B ROM\n";
	while(true) {
		cout << "(l)oad  e(x)ecute  (d)ump  (t)race  (i)nput  (o)utput  (q)uit: ";
		cin.getline(input,sizeof(input));
		if(cin.eof())
			break;
		switch(toupper(input[0])) {
			case 'L': {
					clog << "TODO: LOAD\n";	// TODO
				  } break;
			case 'X': {
					clog << "TODO: EXECUTE\n"; // TODO
				  } break;
			case 'D': {
					clog << "TODO: DUMP\n"; // TODO
				  } break;
			case 'T': {
					clog << "TODO: TRACE\n"; // TODO
				  } break;
			case 'I': {
					clog << "TODO: INPUT\n"; // TODO
				  } break;
			case 'O': {
					clog << "TODO: OUTPUT\n"; // TODO
				  } break;
			case 'Q': return;
			default: cout << "Invalid command.\n";
		}
	}
}
