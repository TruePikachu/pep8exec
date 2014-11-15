#include "pep8mem.hpp"
#include <cstring>
#include <exception>
#include <ostream>
#include <stdint.h>
#include <string>
using namespace std;

Pep8DataException::Pep8DataException(std::string str) : whatIs(str) {};

Pep8DataException::Pep8DataException(const Pep8DataException&parent,std::string str) {
	whatIs = parent.what() + str;
}

Pep8DataException::~Pep8DataException() throw() {}

const char* Pep8DataException::what() const throw() {
	return whatIs.c_str();
}

//////////

Pep8Memory::Pep8Memory() {
	image = new uint8_t[0x10000];
}

Pep8Memory::Pep8Memory(const Pep8Memory&o) {
	image=NULL;
	*this = o;
}

Pep8Memory::~Pep8Memory() {
	delete[] image;
}

Pep8Memory& Pep8Memory::operator=(Pep8Memory o) {
	delete[] image;
	image = new uint8_t[0x10000];
	memmove(image,o.image,0x10000);
	return *this;
}

const uint8_t* Pep8Memory::getImage() const {
	return image;
}

uint8_t* Pep8Memory::getImage() {
	return image;
}

int16_t Pep8Memory::getSW(off_t i) const {
	return (int16_t)getUW(i);
}

uint16_t Pep8Memory::getUW(off_t i) const {
	if(i==0xFFFF)
		return (image[0xFFFF] << 8) | image[0x0000];
	else
		return (image[i] << 8) | image[i+1];
}

int8_t Pep8Memory::getSB(off_t i) const {
	return (int8_t)image[i];
}

uint8_t Pep8Memory::getUB(off_t i) const {
	return image[i];
}

Pep8Memory& Pep8Memory::setSW(off_t i, int16_t n) {
	return setUW(i,(uint16_t)n);
}

Pep8Memory& Pep8Memory::setUW(off_t i, uint16_t n) {
	image[i] = n >> 8;
	if(i==0xFFFF)
		image[0x0000] = n&0xFF;
	else
		image[i+1] = n&0xFF;
	return *this;
}

Pep8Memory& Pep8Memory::setSB(off_t i, int8_t n) {
	return setUB(i,(uint8_t)n);
}

Pep8Memory& Pep8Memory::setUB(off_t i, uint8_t n) {
	image[i] = n;
	return *this;
}

//////////

Pep8Bits::Pep8Bits(size_t nBits) : nBits(nBits) {
	bits=0;
}

bool Pep8Bits::getBit(int i) const {
	if(i<0 || i>=nBits)
		throw Pep8DataException("Pep8Bits::getBit(): Tried to access an out-of-range bit");
	return bits & (1 << i);
}

Pep8Bits& Pep8Bits::setBit(int i, bool v) {
	if(v)
		bits |= 1<<i;
	else
		bits &= ~(1<<i);
	return *this;
}

unsigned Pep8Bits::getBits(int a,int b) const {
	unsigned result=0;
	int min,max;
	if(a<b) {
		min=a;
		max=b;
	} else {
		min=b;
		max=a;
	}
	for(int i=min;i<=max;i++) {
		result <<= 1;
		result |= getBit(i);
	}
	return result;
}

Pep8Bits& Pep8Bits::setBits(int a,int b,unsigned n) {
	int min,max;
	if(a<b) {
		min=a;
		max=b;
	} else {
		min=b;
		max=a;
	}
	for(int i=max;i>=min;i--) {
		setBit(i,n&1);
		n >>= 1;
	}
	return *this;
}

//////////

Pep8Flags::Pep8Flags() : Pep8Bits(4) {};

bool Pep8Flags::getN() const {
	return getBit(3);
}

bool Pep8Flags::getZ() const {
	return getBit(2);
}

bool Pep8Flags::getV() const {
	return getBit(1);
}

bool Pep8Flags::getC() const {
	return getBit(0);
}

Pep8Flags& Pep8Flags::setN(bool b) {
	setBit(3,b);
	return *this;
}

Pep8Flags& Pep8Flags::setZ(bool b) {
	setBit(2,b);
	return *this;
}

Pep8Flags& Pep8Flags::setV(bool b) {
	setBit(1,b);
	return *this;
}

Pep8Flags& Pep8Flags::setC(bool b) {
	setBit(0,b);
	return *this;
}

//////////

Pep8Register::Pep8Register() : Pep8Bits(16) {};

int16_t Pep8Register::getSW() const {
	return (int16_t)getUW();
}

uint16_t Pep8Register::getUW() const {
	return bits;
}

int8_t Pep8Register::getSB() const {
	return (int8_t)getSB();
}

uint8_t Pep8Register::getUB() const {
	return bits;
}

Pep8Register& Pep8Register::setSW(int16_t n) {
	return setUW((uint16_t)n);
}

Pep8Register& Pep8Register::setUW(uint16_t n) {
	bits=n;
	return *this;
}

Pep8Register& Pep8Register::setSB(int8_t n) {
	return setUB((uint8_t)n);
}

Pep8Register& Pep8Register::setUB(uint8_t n) {
	bits &= 0xFF00;
	bits |= n;
	return *this;
}
