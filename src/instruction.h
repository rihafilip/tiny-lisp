#pragma once
#include <iostream>
#include <string>

namespace lisp
{
	namespace secd
	{
		enum Instruction
		{
			LDC,
			NIL,
			ADD,
			SUB,
			MUL,
			DIV,
			CONS,
			CAR,
			CDR,
			CONSP,
			SEL,
			JOIN,
			LD,
			LDF,
			AP,
			RTN
		};

		std::string toString ( Instruction ins );
		std::ostream & operator<< ( std::ostream & os, Instruction ins );
	}
}