#include "value.h"
using namespace lisp;

Value Value::Integer ( int num )
{
	return Value ( GC::GetMemory( num ) );
}

Value Value::Symbol ( const std::string & in )
{
	return Value ( GC::GetMemory( in ) );
}

Value Value::Cons ( const Value & car, const Value & cdr )
{
	return Value ( GC::GetMemory( car.memory, cdr.memory, false ) );
}

Value Value::Closure ( const Value & function, const Value & enviroment )
{
	return Value ( GC::GetMemory( function.memory, enviroment.memory, true ) );
}

Value Value::Instruction ( secd::Instruction instruct )
{
	return Value ( GC::GetMemory( instruct ) );
}

Value Value::Null ()
{
	return Value ( GC::GetNull() );
}

/*********************************************************/

Value::Value( GC::Memory * in )
: memory(in)
{
	GC::AddRoot ( memory );
}

Value::~Value()
{
	GC::RemoveRoot ( memory );
}

Value::Value ( const Value & src )
{
	this -> memory = src.memory;
	GC::AddRoot( this -> memory );
}

Value & Value::operator= ( const Value & src )
{
	GC::RemoveRoot ( this -> memory );
	this -> memory = src.memory;
	GC::AddRoot( this -> memory );
	return *this;
}

/*********************************************************/
void Value::error ( GC::MemoryType expected ) const
{
	std::cerr << "Expected "
	<< toString(expected)
	<< ", got "
	<< toString(memory -> type) << std::endl;

	throw std::runtime_error("Bad Value access");
}

/*********************************************************/

Value Value::car () const
{
	if ( memory -> type != GC::MemoryType::CONS
		&& memory -> type != GC::MemoryType::CLOS
	)
		error (GC::MemoryType::CONS);

	return Value ( memory -> cons . first );
}

Value Value::cdr () const
{
	if ( memory -> type != GC::MemoryType::CONS
		&& memory -> type != GC::MemoryType::CLOS
	)
		error (GC::MemoryType::CONS);

	return Value ( memory -> cons . second );
}

int Value::num () const
{
	if ( memory -> type != GC::MemoryType::NUM )
		error (GC::MemoryType::NUM);

	return memory -> number;
}

std::string Value::sym () const
{
	if ( memory -> type != GC::MemoryType::SYM )
		error (GC::MemoryType::SYM);

	return memory -> name;
}

secd::Instruction Value::ins () const
{
	if ( memory -> type != GC::MemoryType::INST )
		error (GC::MemoryType::INST);

	return memory -> instruct;
}

/*********************************************************/

bool Value::isNull () const
{
	return memory -> type == GC::MemoryType::EMPTY;
}

bool Value::isCons () const
{
	return memory -> type == GC::MemoryType::CONS;
}

bool Value::isClos () const
{
	return memory -> type == GC::MemoryType::CLOS;
}

bool Value::isNum () const
{
	return memory -> type == GC::MemoryType::NUM;
}

bool Value::isSym () const
{
	return memory -> type == GC::MemoryType::SYM;
}

bool Value::isIns () const
{
	return memory -> type == GC::MemoryType::INST;
}

/*********************************************************/

Value Value::append ( const Value & val ) const
{
	return append( *this, val );
}

Value Value::append ( const Value & first, const Value & last )
{
	if ( first . isNull() )
		return last;

	return Cons( first.car(), append( first.cdr(), last ) );
}

/*********************************************************/

bool Value::equals ( const Value & other ) const
{
	if ( this -> memory -> type != other . memory -> type )
		return false;

	switch ( this -> memory -> type )
	{
		case GC::MemoryType::UNDEF:
			return true;

		case GC::MemoryType::EMPTY:
			return true;

		case GC::MemoryType::NUM:
			return this -> num() == other.num();

		case GC::MemoryType::SYM:
			return this -> sym() == other.sym();

		case GC::MemoryType::INST:
			return this -> ins() == other.ins();

		case GC::MemoryType::CLOS:
		case GC::MemoryType::CONS:
			return this -> car().equals( other.car() )
				&& this -> cdr().equals( other.cdr() );
	}

	// compilator satisfaction
	return false;
}


/*********************************************************/

namespace lisp
{
	std::ostream & operator<< (std::ostream & os, const Value & val )
	{
		val . print ( os );
		return os;
	}
}

void Value::print ( std::ostream & os, bool list ) const
{
	switch ( memory -> type )
	{
		case GC::MemoryType::UNDEF:
			os << "undef";
			break;

		case GC::MemoryType::EMPTY:
			os << "null";
			break;

		case GC::MemoryType::NUM:
			os << num();
			break;

		case GC::MemoryType::SYM:
			os << '\'' << sym() << '\'';
			break;

		case GC::MemoryType::INST:
			os << ins();
			break;

		case GC::MemoryType::CLOS:
			os << "<procedure";
		#ifdef DBG
			os << ": ";
			car().print(os);
			os << ", env: ";
			cdr().print(os);
			os << " ";
		#endif
			os << ">";
			break;
			
		case GC::MemoryType::CONS:
			if ( ! list )
				os << "( ";
			
			car() . print (os);
			if ( ! cdr() .isNull()  )
			{
				if ( ! cdr() . isCons() )
			 		os << " . ";
			 	else
			 		os << " ";
			 	cdr() . print (os, true);
		 	}

			if ( ! list )
				os << " )";
			break;
	}
}