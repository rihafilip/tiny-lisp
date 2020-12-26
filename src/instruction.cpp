#include "instruction.h"
#include <map>
using namespace lisp::secd;

std::string lisp::secd::toString ( Instruction ins )
{
	static const std::map<Instruction, std::string> map =
	{
		{ LDC, "LDC" },
		{ NIL, "NIL" },

		{ ADD, "ADD" },
		{ SUB, "SUB" },
		{ MUL, "MUL" },
		{ DIV, "DIV" },

		{ LESS, "LESS"},
		{ MORE, "MORE"},
		{ EQ, "EQ"},

		{ CONS, "CONS" },
		{ CAR, "CAR" },
		{ CDR, "CDR" },

		{ CONSP, "CONSP" },

		{ SEL, "SEL" },
		{ JOIN, "JOIN" },

		{ LD, "LD" },
		{ LDF, "LDF" },

		{ AP, "AP" },
		{ RTN, "RTN" },

		{ PRINT, "PRINT" },
		{ READ, "READ" }
	};

	return map . at(ins);
}

std::ostream & lisp::secd::operator<< ( std::ostream & os, Instruction ins )
{
	return os << toString(ins);
}