#include "instruction.h"
#include <map>
using namespace lisp::secd;

std::string lisp::secd::toString ( Instruction ins )
{
	std::map<Instruction, std::string> map;
	map . insert( std::make_pair(LDC, "LDC") );
	map . insert( std::make_pair(NIL, "NIL") );
	map . insert( std::make_pair(ADD, "ADD") );
	map . insert( std::make_pair(SUB, "SUB") );
	map . insert( std::make_pair(MUL, "MUL") );
	map . insert( std::make_pair(DIV, "DIV") );
	map . insert( std::make_pair(CONS, "CONS") );
	map . insert( std::make_pair(CAR, "CAR") );
	map . insert( std::make_pair(CDR, "CDR") );
	map . insert( std::make_pair(CONSP, "CONSP") );
	map . insert( std::make_pair(SEL, "SEL") );
	map . insert( std::make_pair(JOIN, "JOIN") );
	map . insert( std::make_pair(LD, " LD") );
	map . insert( std::make_pair(LDF, "LDF") );
	map . insert( std::make_pair(AP, " AP") );
	map . insert( std::make_pair(RTN, "RTN") );

	return map[ins];
}

std::ostream & lisp::secd::operator<< ( std::ostream & os, Instruction ins )
{
	return os << toString(ins);
}