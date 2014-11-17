#ifndef __PEP8CPU_HPP
#define __PEP8CPU_HPP
class Pep8CPU;
#include "pep8mem.hpp"
#include <istream>
#include <ostream>
#include <stdint.h>

class Pep8CPU {
	friend std::ostream& operator<<(std::ostream&,const Pep8CPU&);
	private:
		typedef enum {
			STOP	= 0x00,
			RETTR	= 0x01,
			MOVSPA	= 0x02,
			MOVFLGA	= 0x03,

			// Mask:  0xFE
			BR	= 0x04,
			BRLE	= 0x06,
			BRLT	= 0x08,
			BREQ	= 0x0A,
			BRNE	= 0x0C,
			BRGE	= 0x0E,
			BRGT	= 0x10,
			BRV	= 0x12,
			BRC	= 0x14,
			CALL	= 0x16,

			NOTr	= 0x18,
			NEGr	= 0x1A,
			ASLr	= 0x1C,
			ASRr	= 0x1E,
			ROLr	= 0x20,
			RORr	= 0x22,

			// Mask:  0xFC
			NOPn	= 0x24,
			// Mask:  0xF8
			NOP	= 0x28,

			DECI	= 0x30,
			DECO	= 0x38,
			STRO	= 0x40,
			CHARI	= 0x48,
			CHARO	= 0x50,
			
			RETn	= 0x58,

			ADDSP	= 0x60,
			SUBSP	= 0x68,

			// Mask:  0xF0
			ADDr	= 0x70,
			SUBr	= 0x80,
			ANDr	= 0x90,
			ORr	= 0xA0,
			CPr	= 0xB0,

			LDr	= 0xC0,
			LDBYTEr	= 0xD0,
			STr	= 0xE0,
			STBYTEr	= 0xF0	} Instruction;
		typedef enum {
			ADRnone	= 0xFF,
			ADRi	= 00,
			ADRd	= 01,
			ADRn	= 02,
			ADRs	= 03,
			ADRsf	= 04,
			ADRx	= 05,
			ADRsx	= 06,
			ADRsxf	= 07	} AddressMode;



		Pep8Memory&	memory;
		Pep8Flags	NZVC;
		Pep8Register	A;
		Pep8Register	X;
		uint16_t	PC;
		uint16_t	SP;
		struct {
			uint8_t		OP;
			uint16_t	PAR;
			}	IR;
	public:
					 Pep8CPU	(Pep8Memory&);
		const Pep8Memory&	getMemory	() const;
		Pep8Memory&		getMemory	();
		const Pep8Flags&	getFlags	() const;
		Pep8Flags&		getFlags	();
		const Pep8Register&	getA		() const;
		Pep8Register&		getA		();
		const Pep8Register&	getX		() const;
		Pep8Register&		getX		();
		uint16_t		getPC		() const;
		Pep8CPU&		setPC		(uint16_t);
		uint16_t		getSP		() const;
		Pep8CPU&		setSP		(uint16_t);
		bool			doInstruction	(std::istream&,std::ostream&);	// Returns FALSE on STOP
		bool			doInstruction	(std::istream&,std::ostream&,uint16_t);	// Set PC first
};

#endif
