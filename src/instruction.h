#pragma once
#include <iostream>
#include <string>

namespace lisp::secd
{
	/// SECD machine instruction
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

		// recursive function
		DUM,
		RAP,

		//IO
		PRINT,
		READ
	};

	/// Transforms instruction to string
	std::string toString ( Instruction ins );
	/// Prints instruction to ostream
	std::ostream & operator<< ( std::ostream & os, Instruction ins );
}