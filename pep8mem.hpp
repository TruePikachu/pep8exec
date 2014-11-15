#ifndef __PEP8MEM_HPP
#define __PEP8MEM_HPP
class Pep8DataException;
class Pep8Memory;
class Pep8Bits;
class Pep8Flags;
class Pep8Register;
#include <exception>
#include <ostream>
#include <stdint.h>
#include <string>

class Pep8DataException : public std::exception {
	private:
		std::string	whatIs;
	public:
			 Pep8DataException	(std::string);
			 Pep8DataException	(const Pep8DataException&,std::string);
			~Pep8DataException	() throw();
		const char* what		() const throw();
};

class Pep8Memory {
	private:
		uint8_t*	image;
	public:
				 Pep8Memory	();
				 Pep8Memory	(const Pep8Memory&);
				~Pep8Memory	();
		Pep8Memory&	operator=	(Pep8Memory);

		const uint8_t*	getImage	() const;
		uint8_t*	getImage	();
		int16_t		getSW		(off_t) const;
		uint16_t	getUW		(off_t) const;
		int8_t		getSB		(off_t) const;
		uint8_t		getUB		(off_t) const;
		Pep8Memory&	setSW		(off_t,int16_t);
		Pep8Memory&	setUW		(off_t,uint16_t);
		Pep8Memory&	setSB		(off_t,int8_t);
		Pep8Memory&	setUB		(off_t,uint8_t);
};

class Pep8Bits {
	friend std::ostream& operator<<(std::ostream&,const Pep8Bits&);
	protected:
		unsigned	bits;
		size_t		nBits;
	public:
				 Pep8Bits	(size_t nBits);
		bool		getBit		(int) const;
		Pep8Bits&	setBit		(int,bool);
		unsigned	getBits		(int,int) const;
		Pep8Bits&	setBits		(int,int,unsigned);
};

class Pep8Flags : public Pep8Bits {
	friend std::ostream& operator<<(std::ostream&,const Pep8Flags&);
	public:
				 Pep8Flags	();
		bool		getN		() const;
		bool		getZ		() const;
		bool		getV		() const;
		bool		getC		() const;
		Pep8Flags&	setN		(bool);
		Pep8Flags&	setZ		(bool);
		Pep8Flags&	setV		(bool);
		Pep8Flags&	setC		(bool);
};

class Pep8Register : public Pep8Bits {
	friend std::ostream& operator<<(std::ostream&,const Pep8Register&);
	public:
				 Pep8Register	();
		int16_t		getSW		() const;
		uint16_t	getUW		() const;
		int8_t		getSB		() const;
		uint8_t		getUB		() const;
		Pep8Register&	setSW		(int16_t);
		Pep8Register&	setUW		(uint16_t);
		Pep8Register&	setSB		(int8_t);
		Pep8Register&	setUB		(uint8_t);
};

#endif
