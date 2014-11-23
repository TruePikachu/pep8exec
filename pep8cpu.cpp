#include "pep8cpu.hpp"
#include "pep8mem.hpp"
#include <cstdio>
#include <iostream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <stdint.h>
#include <string>
using namespace std;

Pep8Operand::Pep8Operand(Pep8Memory&memory,uint16_t value,AddressMode type,const Pep8Register&X,uint16_t SP) : memory(memory),value(value),type(type),x(X.getUW()),sp(SP) {}

Pep8Operand::AddressMode Pep8Operand::getAddrMode() const {
	return type;
}

std::string Pep8Operand::getFormal() const {
	string result;
	char buf[32];
	sprintf(buf,"%04X",value);
	result = buf;
	switch(type) {
		case ADRi:	return result+",i";
		case ADRd:	return result+",d";
		case ADRn:	return result+",n";
		case ADRs:	return result+",s";
		case ADRsf:	return result+",sf";
		case ADRx:	return result+",x";
		case ADRsx:	return result+",sx";
		case ADRsxf:	return result+",sxf";
		default:	return "";
	}
}

uint16_t Pep8Operand::getRef() const {
	switch(type) {
		case ADRd:
			return value;
		case ADRn:
			return memory.getUW(value);
		case ADRs:
			return sp+value;
		case ADRsf:
			return memory.getUW(sp+value);
		case ADRx:
			return x+value;
		case ADRsx:
			return x+sp+value;
		case ADRsxf:
			return memory.getUW(sp+value)+x;
		default:
			throw Pep8DataException("Invalid operand type");
	}
}

int16_t Pep8Operand::getSW() const {
	if(type==ADRi)
		return (int16_t)value;
	else
		return memory.getSW(getRef());
}

uint16_t Pep8Operand::getUW() const {
	if(type==ADRi)
		return value;
	else
		return memory.getUW(getRef());
}

int8_t Pep8Operand::getSB() const {
	if(type==ADRi)
		return (int8_t)value&0xFF;
	else
		return memory.getSB(getRef());
}

uint8_t Pep8Operand::getUB() const {
	if(type==ADRi)
		return (uint8_t)value&0xFF;
	else
		return memory.getUB(getRef());
}

Pep8Operand& Pep8Operand::setSW(int16_t v) {
	memory.setSW(getRef(),v);
	return *this;
}

Pep8Operand& Pep8Operand::setUW(uint16_t v) {
	        memory.setUW(getRef(),v);
		        return *this;
}

Pep8Operand& Pep8Operand::setSB(int8_t v) {
	        memory.setSB(getRef(),v);
		        return *this;
}

Pep8Operand& Pep8Operand::setUB(uint8_t v) {
	        memory.setUB(getRef(),v);
		        return *this;
}

//////////

const char* Pep8CPU::getMnemon(uint8_t opByte) {
	const char *mnemonA[40] = {"STOP","RETTR","MOVSPA","MOVFLGA","BR","BR","BRLE","BRLE","BRLT","BRLT","BREQ","BREQ","BRNE","BRNE","BRGE","BRGE","BRGT","BRGT","BRV","BRV","BRC","BRC","CALL","CALL","NOTA","NOTX","NEGA","NEGX","ASLA","ASLX","ASRA","ASRX","ROLA","ROLX","RORA","RORX","NOP0","NOP1","NOP2","NOP3"};
	const char *mnemonB[27] = {"DECI","DECO","STRO","CHARI","CHARO","RETn","ADDSP","SUBSP","ADDA","ADDX","SUBA","SUBX","ANDA","ANDX","ORA","ORX","CPA","CPX","LDA","LDX","LDBYTEA","LDBYTEX","STA","STX","STBYTEA","STBYTEX"};
	const char *rets[8] = {"RET0","RET1","RET2","RET3","RET4","RET5","RET6","RET7"};
	if(opByte<40)
		return mnemonA[opByte];
	if(opByte&0xF8 == 0x58)
		return rets[opByte&7];
	return mnemonB[(opByte-0x30)>>3];
}

bool Pep8CPU::doTrace() const {
	if(!traceEnabled)
		return false;
	if(tracingLoader)
		return traceLoad;
	else
		return inTrap?traceTrap:traceProg;
}

Pep8CPU::Pep8CPU(Pep8Memory&memory) : memory(memory) {
	PC=0;
	SP=0;
	IR.OP=0;
	IR.PAR=0;
	inTrap=false;
	traceFile=false;
	traceTrap=false;
	traceLoad=false;
	traceProg=false;
	traceEnabled=false;
}

const Pep8Memory& Pep8CPU::getMemory() const {
	return memory;
}

Pep8Memory& Pep8CPU::getMemory() {
	return memory;
}

const Pep8Flags& Pep8CPU::getFlags() const {
	return NZVC;
}

Pep8Flags& Pep8CPU::getFlags() {
	return NZVC;
}

const Pep8Register& Pep8CPU::getA() const {
	return A;
}

Pep8Register& Pep8CPU::getA() {
	return A;
}

const Pep8Register& Pep8CPU::getX() const {
	return X;
}

Pep8Register& Pep8CPU::getX() {
	return X;
}

uint16_t Pep8CPU::getPC() const {
	return PC;
}

Pep8CPU& Pep8CPU::setPC(uint16_t newPC) {
	PC=newPC;
	return *this;
}

uint16_t Pep8CPU::getSP() const {
	return SP;
}

Pep8CPU& Pep8CPU::setSP(uint16_t newSP) {
	SP=newSP;
	return *this;
}

bool Pep8CPU::doInstruction(std::istream&is, std::ostream&os) {
	char traceBuffer[128];
	// Fetch
	IR.OP=memory.getUB(PC);
	// Read&Increment
	PC++;
	Instruction inst;
	if(IR.OP<BR)
		inst=(Instruction)(IR.OP&0xFF);
	else if(IR.OP<NOPn)
		inst=(Instruction)(IR.OP&0xFE);
	else if(IR.OP<NOP)
		inst=(Instruction)(IR.OP&0xFC);
	else if(IR.OP<ADDr)
		inst=(Instruction)(IR.OP&0xF8);
	else
		inst=(Instruction)(IR.OP&0xF0);
	if(doTrace()) {
		sprintf(traceBuffer,"%04X  %-8s ",PC-1,getMnemon(IR.OP));
		*traceFile << traceBuffer;
	}
	Pep8Operand::AddressMode addr;
	switch(inst) {
		case BR:
		case BRLE:
		case BRLT:
		case BREQ:
		case BRNE:
		case BRGE:
		case BRGT:
		case BRV:
		case BRC:
		case CALL:
			if(IR.OP&1)
				addr=Pep8Operand::ADRx;
			else
				addr=Pep8Operand::ADRi;
			break;
		case NOP:
		case DECI:
		case DECO:
		case STRO:
		case CHARI:
		case CHARO:
		case ADDSP:
		case SUBSP:
		case ADDr:
		case SUBr:
		case ANDr:
		case ORr:
		case CPr:
		case LDr:
		case LDBYTEr:
		case STr:
		case STBYTEr:
			addr=(Pep8Operand::AddressMode)(IR.OP&07);
			break;
		default:
			addr=Pep8Operand::ADRnone;
	}
	if(addr!=Pep8Operand::ADRnone) {
		IR.PAR=memory.getUW(PC);
		PC+=2;
	}
	// Execute
	Pep8Register* r = NULL;
	if(NOTr<=inst && inst<=RORr)
		if(IR.OP&1)
			r=&X;
		else
			r=&A;
	if(ADDr<=inst)
		if(IR.OP&8)
			r=&X;
		else
			r=&A;
	Pep8Operand operand(memory,IR.PAR,addr,X,SP);
	if(doTrace()) {
		sprintf(traceBuffer,"%-10s%02X%04X   ",operand.getFormal().c_str(),IR.OP,IR.PAR);
		*traceFile << traceBuffer;
	}
	uint16_t tmpUWORD;
	bool willTrap = inTrap;
	switch(inst) {
		case STOP:
			break;
		case RETTR:
			NZVC.setBits(0,3,memory.getUW(SP));
			A.setUW(memory.getUW(SP+1));
			X.setUW(memory.getUW(SP+3));
			PC=memory.getUW(SP+5);
			SP=memory.getUW(SP+7);
			willTrap=false;
			break;
		case MOVSPA:
			A.setUW(SP);
			break;
		case MOVFLGA:
			A.setBits(0,11,0);
			A.setBits(12,15,NZVC.getBits(0,3));
			break;
		case BR:
			PC = operand.getUW();
			break;
		case BRLE:
			if(NZVC.getN() || NZVC.getZ())
				PC = operand.getUW();
			break;
		case BRLT:
			if(NZVC.getN())
				PC = operand.getUW();
			break;
		case BREQ:
			if(NZVC.getZ())
				PC = operand.getUW();
			break;
		case BRNE:
			if(!NZVC.getZ())
				PC = operand.getUW();
			break;
		case BRGE:
			if(!NZVC.getN())
				PC = operand.getUW();
			break;
		case BRGT:
			if(!NZVC.getN() && !NZVC.getZ())
				PC = operand.getUW();
			break;
		case BRV:
			if(NZVC.getV())
				PC = operand.getUW();
			break;
		case BRC:
			if(NZVC.getC())
				PC = operand.getUW();
			break;
		case CALL:
			SP -= 2;
			memory.setUW(SP,PC);
			PC = operand.getUW();
			break;
		case NOTr:
			r->setUW(~r->getUW());
			NZVC.setN(r->getBit(0));
			NZVC.setZ(r->getUW()==0);
			break;
		case NEGr:
			NZVC.setV(!r->getBit(0));
			r->setUW(~r->getUW() + 1);
			NZVC.setN(r->getBit(0));
			NZVC.setZ(r->getUW()==0);
			break;
		case ASLr:
			NZVC.setC(r->getBit(0));
			r->setBits(0,14,r->getBits(1,15));
			r->setBit(15,false);
			NZVC.setN(r->getBit(0));
			NZVC.setZ(r->getUW()==0);
			break;
		case ASRr:
			NZVC.setC(r->getBit(15));
			r->setBits(1,15,r->getBits(0,14));
			NZVC.setN(r->getBit(0));
			NZVC.setZ(r->getUW()==0);
			break;
		case ROLr:
			NZVC.setC(r->getBit(0));
			r->setBits(0,14,r->getBits(1,15));
			r->setBit(15,NZVC.getC());
			break;
		case RORr:
			NZVC.setC(r->getBit(15));
			r->setBits(1,15,r->getBits(0,14));
			r->setBit(0,NZVC.getC());
			break;
		case NOPn:
		case NOP:
		case DECI:
		case DECO:
		case STRO: {	uint16_t T = memory.getUW(0xFFFA);
				memory.setUB(T-1,IR.OP);
				memory.setUW(T-3,SP);
				memory.setUW(T-5,PC);
				memory.setUW(T-7,X.getUW());
				memory.setUW(T-9,A.getUW());
				memory.setUB(T-10,NZVC.getBits(0,3));
				SP = T-10;
				PC = memory.getUW(0xFFFE);
				willTrap=true;
				break;	}
		case CHARI:
			is.peek();
			if(!is.good())
				throw runtime_error("Tried to read from non-good input");
			operand.setUB(is.get());
			break;
		case CHARO:
			os << (char)operand.getUB();
			break;
		case RETn:
			SP += IR.OP & 0x7;
			PC = memory.getUW(SP);
			SP += 2;
			break;
		case ADDSP:
			tmpUWORD = SP;
			SP += operand.getUW();
			NZVC.setN((int16_t)SP < 0);
			NZVC.setZ(SP==0);
			NZVC.setV((((int)tmpUWORD+operand.getSW() < -0x8000)||((int)tmpUWORD+operand.getSW() > 0x7FFF))^(tmpUWORD>>15));
			NZVC.setC(((int)tmpUWORD+operand.getUW() < 0) || ((int)tmpUWORD+operand.getUW() > 0xFFFF));
			break;
		case SUBSP:
			tmpUWORD = SP;
			SP -= operand.getUW();
			NZVC.setN((int16_t)SP < 0);
			NZVC.setZ(SP==0);
			NZVC.setV((((int)tmpUWORD-operand.getSW() < -0x8000)||((int)tmpUWORD-operand.getSW() > 0x7FFF))^(tmpUWORD>>15));
			NZVC.setC(((int)tmpUWORD-operand.getUW() < 0) || ((int)tmpUWORD-operand.getUW() > 0xFFFF));
			break;
		case ADDr:
			tmpUWORD = r->getUW();
			r->setUW(tmpUWORD+operand.getUW());
			NZVC.setN(r->getSW() < 0);
			NZVC.setZ(r->getUW()==0);
			NZVC.setV((((int)tmpUWORD+operand.getSW() < -0x8000)||((int)tmpUWORD+operand.getSW() > 0x7FFF))^(tmpUWORD>>15));
			NZVC.setC(((int)tmpUWORD+operand.getUW() < 0) || ((int)tmpUWORD+operand.getUW() > 0xFFFF));
			break;
		case SUBr:
			tmpUWORD = r->getUW();
			r->setUW(tmpUWORD-operand.getUW());
			NZVC.setN(r->getSW() < 0);
			NZVC.setZ(r->getUW()==0);
			NZVC.setV((((int)tmpUWORD-operand.getSW() < -0x8000)||((int)tmpUWORD-operand.getSW() > 0x7FFF))^(tmpUWORD>>15));
			NZVC.setC(((int)tmpUWORD-operand.getUW() < 0) || ((int)tmpUWORD-operand.getUW() > 0xFFFF));
			break;
		case ANDr:
			r->setUW(r->getUW() & operand.getUW());
			NZVC.setN(r->getBit(15));
			NZVC.setZ(r->getUW()==0);
			break;
		case ORr:
			r->setUW(r->getUW() | operand.getUW());
			NZVC.setN(r->getBit(15));
			NZVC.setZ(r->getUW()==0);
			break;
		case CPr:
			tmpUWORD = r->getUW() - operand.getUW();
			NZVC.setN(tmpUWORD&0x8000);
			NZVC.setZ(tmpUWORD==0);
			NZVC.setV(r->getSW() < operand.getSW());
			NZVC.setC(r->getUW() < operand.getUW());
			break;
		case LDr:
			r->setUW(operand.getUW());
			NZVC.setN(r->getBit(15));
			NZVC.setZ(r->getUW()==0);
			break;
		case LDBYTEr:
			r->setUB(operand.getUB());
			NZVC.setN(r->getBit(15));
			NZVC.setZ(r->getUW()==0);
			break;
		case STr:
			operand.setUW(r->getUW());
			break;
		case STBYTEr:
			operand.setUB(r->getUB());
			break;
		default:
			char errorBuf[64];
			sprintf(errorBuf,"Command 0x%02X unimplemented",IR.OP);
			throw logic_error(errorBuf);
	}

	if(doTrace()) {
		sprintf(traceBuffer,"%04X   %04X    %04X    %i %i %i %i   %04X\n",A.getUW(),X.getUW(),SP,NZVC.getN(),NZVC.getZ(),NZVC.getV(),NZVC.getC(),(operand.getAddrMode()!=operand.ADRnone)?operand.getUW():0);
		*traceFile << traceBuffer;
	}
	inTrap = willTrap;
	// ReturnValue
	return (inst!=STOP);
}

bool Pep8CPU::doInstruction(std::istream&is,std::ostream&os,uint16_t where) {
	PC=where;
	return doInstruction(is,os);
}

Pep8CPU& Pep8CPU::doLoader(std::istream&is,std::ostream&os) {
	SP=memory.getUW(0xFFFA);
	PC=memory.getUW(0xFFFC);
	traceEnabled=true;
	tracingLoader=true;
	bool nullTraceFile = !traceFile;
	if(nullTraceFile)
		traceFile=&clog;
	if(traceLoad)
		*traceFile <<
"-------------------------------------------------------------------------\n"
"               Oprnd     Instr           Index   Stack   Status\n"
"Addr  Mnemon   Spec       Reg     Accum   Reg   Pointer  N Z V C  Operand\n"
"-------------------------------------------------------------------------\n";
	while(doInstruction(is,os));
	if(nullTraceFile)
		traceFile=NULL;
	traceEnabled=false;
	return *this;
}

Pep8CPU& Pep8CPU::doProgram(std::istream&is,std::ostream&os) {
	SP=memory.getUW(0xFFF8);
	PC=0x0000;
	traceEnabled=true;
	tracingLoader=false;
	bool nullTraceFile = !traceFile;
	if(nullTraceFile)
		traceFile=&clog;
	if(traceProg || traceTrap)
		*traceFile <<
"-------------------------------------------------------------------------\n"
"               Oprnd     Instr           Index   Stack   Status\n"
"Addr  Mnemon   Spec       Reg     Accum   Reg   Pointer  N Z V C  Operand\n"
"-------------------------------------------------------------------------\n";
	while(doInstruction(is,os));
	if(nullTraceFile)
		traceFile=NULL;
	traceEnabled=false;
	return *this;
}

Pep8CPU& Pep8CPU::setTrace(bool program,bool trap,bool load,std::ostream* strm) {
	traceProg=program;
	traceTrap=trap;
	traceLoad=load;
	traceFile=strm;
	return *this;
}
