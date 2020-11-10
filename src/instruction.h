#pragma once
#include <iostream>

namespace lisp
{
	namespace sedc
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

		std::ostream & operator<< ( std::ostream & os, Instruction ins );
	}
}