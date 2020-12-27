#pragma once
#include <iostream>
#include <string>

namespace lisp
{
	namespace secd
	{
		enum Instruction
		{
			// load constants
			LDC,
			NIL,

			// arithmetic
			ADD,
			SUB,
			MUL,
			DIV,

			// comparison
			LESS,
			MORE,
			EQ,

			// list
			CONS,
			CAR,
			CDR,

			// type identify
			CONSP,

			// if
			SEL,
			JOIN,

			// aditional loading
			LD,
			LDF,

			// function
			AP,
			RTN,
			DEFUN,

			//IO
			PRINT,
			READ
		};

		std::string toString ( Instruction ins );
		std::ostream & operator<< ( std::ostream & os, Instruction ins );
	}
}