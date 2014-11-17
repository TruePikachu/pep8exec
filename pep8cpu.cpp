#include "pep8cpu.hpp"
#include <cstdio>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <stdint.h>
using namespace std;

Pep8CPU::Pep8CPU(Pep8Memory&memory) : memory(memory) {
	PC=0;
	SP=0;
	IR.OP=0;
	IR.PAR=0;
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
	AddressMode addr;
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
				addr=ADRx;
			else
				addr=ADRi;
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
			addr=(AddressMode)(IR.OP&07);
			break;
		default:
			addr=ADRnone;
	}
	if(addr!=ADRnone) {
		IR.PAR=memory.getUW(PC);
		PC+=2;
	}
	// Execute
	Pep8Register* r = NULL;
	if(BR<=inst && inst<=CALL)
		if(IR.OP&1)
			r=&X;
		else
			r=&A;
	if(ADDr<=inst)
		if(IR.OP&8)
			r=&X;
		else
			r=&A;
	uint16_t parameter;
	switch(addr) {
		case ADRnone:
			break;
		case ADRi:
			parameter=IR.PAR;
			break;
		case ADRd:
			parameter=memory.getUW(IR.PAR);
			break;
		case ADRn:
			parameter=memory.getUW(memory.getUW(IR.PAR));
			break;
		case ADRs:
			parameter=memory.getUW(SP+IR.PAR);
			break;
		case ADRsf:
			parameter=memory.getUW(memory.getUW(SP+IR.PAR));
			break;
		case ADRx:
			parameter=memory.getUW(X.getUW()+IR.PAR);
			break;
		case ADRsx:
			parameter=memory.getUW(SP+X.getUW()+IR.PAR);
			break;
		case ADRsxf:
			parameter=memory.getUW(memory.getUW(SP+IR.PAR)+X.getUW());
			break;
	}
	switch(inst) {
		case STOP:
			break;
		case RETTR:
			NZVC.setBits(0,3,memory.getUW(SP));
			A.setUW(memory.getUW(SP+1));
			X.setUW(memory.getUW(SP+3));
			PC=memory.getUW(SP+5);
			SP=memory.getUW(SP+7);
			break;
		case MOVSPA:
			A.setUW(SP);
			break;
		case MOVFLGA:
			A.setBits(0,11,0);
			A.setBits(12,15,NZVC.getBits(0,3));
			break;
		case BR:
			PC = parameter;
			break;
		case BRLE:
			if(NZVC.getN() || NZVC.getZ())
				PC = parameter;
			break;
		case BRLT:
			if(NZVC.getN())
				PC = parameter;
			break;
		case BREQ:
			if(NZVC.getZ())
				PC = parameter;
			break;
		case BRNE:
			if(!NZVC.getZ())
				PC = parameter;
			break;
		case BRGE:
			if(!NZVC.getN())
				PC = parameter;
			break;
		case BRGT:
			if(!NZVC.getN() && !NZVC.getZ())
				PC = parameter;
			break;
		case BRV:
			if(NZVC.getV())
				PC = parameter;
			break;
		case BRC:
			if(NZVC.getC())
				PC = parameter;
			break;
		case CALL:
			SP -= 2;
			memory.setUW(SP,PC);
			PC = parameter;
			break;
		case NOTr:
			r->setUW(~r->getUW());
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
		case CHARI:
			is.peek();
			if(!is.good())
				throw runtime_error("Tried to read from non-good input");
			switch(addr) {
				case ADRd:
					memory.setSB(IR.PAR,is.get());
					break;
				case ADRn:
					memory.setSB(memory.getUW(IR.PAR),is.get());
					break;
				case ADRs:
					memory.setSB(SP+IR.PAR,is.get());
					break;
				case ADRsf:
					memory.setSB(memory.getUW(SP+IR.PAR),is.get());
					break;
				case ADRx:
					memory.setSB(X.getUW()+IR.PAR,is.get());
					break;
				case ADRsx:
					memory.setSB(SP+X.getUW()+IR.PAR,is.get());
					break;
				case ADRsxf:
					memory.setSB(memory.getUW(SP+IR.PAR)+X.getUW(),is.get());
					break;
			}
			break;
		case CHARO:
			os << (char)(parameter&0xFF);
			break;
		case RETn:
			SP += parameter;
			PC = memory.getUW(SP);
			SP += 2;
			break;
		case ANDr:
			r->setUW(r->getUW() & parameter);
			NZVC.setN(r->getBit(15));
			NZVC.setZ(r->getUW()==0);
			break;
		case ORr:
			r->setUW(r->getUW() | parameter);
			NZVC.setN(r->getBit(15));
			NZVC.setZ(r->getUW()==0);
			break;
		case LDr:
			r->setUW(parameter);
			NZVC.setN(r->getBit(15));
			NZVC.setZ(r->getUW()==0);
			break;
		case LDBYTEr:
			r->setUB(parameter);
			NZVC.setN(r->getBit(15));
			NZVC.setZ(r->getUW()==0);
			break;
		case STr:
			switch(addr) {
				case ADRd:
					memory.setUW(IR.PAR,r->getUW());
					break;
				case ADRn:
					memory.setUW(memory.getUW(IR.PAR),r->getUW());
					break;
				case ADRs:
					memory.setUW(SP+IR.PAR,r->getUW());
					break;
				case ADRsf:
					memory.setUW(memory.getUW(SP+IR.PAR),r->getUW());
					break;
				case ADRx:
					memory.setUW(X.getUW()+IR.PAR,r->getUW());
					break;
				case ADRsx:
					memory.setUW(SP+X.getUW()+IR.PAR,r->getUW());
					break;
				case ADRsxf:
					memory.setUW(memory.getUW(SP+IR.PAR)+X.getUW(),r->getUW());
					break;
			}
			break;
		case STBYTEr:
			switch(addr) {
				case ADRd:
					memory.setUB(IR.PAR,r->getUB());
					break;
				case ADRn:
					memory.setUB(memory.getUW(IR.PAR),r->getUB());
					break;
				case ADRs:
					memory.setUB(SP+IR.PAR,r->getUB());
					break;
				case ADRsf:
					memory.setUB(memory.getUW(SP+IR.PAR),r->getUB());
					break;
				case ADRx:
					memory.setUB(X.getUW()+IR.PAR,r->getUB());
					break;
				case ADRsx:
					memory.setUB(SP+X.getUW()+IR.PAR,r->getUB());
					break;
				case ADRsxf:
					memory.setUB(memory.getUW(SP+IR.PAR)+X.getUW(),r->getUB());
					break;
			}
			break;
		default:
			char errorBuf[64];
			sprintf(errorBuf,"Command 0x%02X unimplemented",IR.OP);
			throw logic_error(errorBuf);
	}

	// ReturnValue
	return inst!=STOP;
}

bool Pep8CPU::doInstruction(std::istream&is,std::ostream&os,uint16_t where) {
	PC=where;
	return doInstruction(is,os);
}
