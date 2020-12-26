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
	return Value ( GC::GetMemory( car.memory, cdr.memory ) );
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
}

/*********************************************************/

std::optional<Value> Value::car () const
{
	if ( memory -> type != GC::MemoryType::CONS )
	{
		error (GC::MemoryType::CONS);
		return std::nullopt;
	}

	return Value ( memory -> cons . first );
}

std::optional<Value> Value::cdr () const
{
	if ( memory -> type != GC::MemoryType::CONS )
	{
		error (GC::MemoryType::CONS);
		return std::nullopt;
	}

	return Value ( memory -> cons . second );
}

std::optional<int> Value::num () const
{
	if ( memory -> type != GC::MemoryType::NUM )
	{
		error (GC::MemoryType::NUM);
		return std::nullopt;
	}

	return memory -> number;
}

std::optional<std::string> Value::sym () const
{
	if ( memory -> type != GC::MemoryType::SYM )
	{
		error (GC::MemoryType::SYM);
		return std::nullopt;
	}

	return memory -> name;
}

std::optional<secd::Instruction> Value::ins () const
{
	if ( memory -> type != GC::MemoryType::INST )
	{
		error (GC::MemoryType::INST);
		return std::nullopt;
	}

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
	if ( isNull() )
		return val;

	if ( ! isCons() )
		return Value::Cons( *this, Value::Cons( val, Value::Null() ) );

	if ( cdr() . value() . isNull() )
		return Value::Cons( car().value(), Value::Cons( val, Value::Null() ) );

	return Value::Cons( car().value(), cdr() -> append( val ) );
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

void Value::print ( std::ostream & os ) const
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
			os << num() . value();
			break;

		case GC::MemoryType::SYM:
			os << '\'' << sym() . value() << '\'';
			break;

		case GC::MemoryType::INST:
			os << ins() . value();
			break;

		case GC::MemoryType::CONS:
			os << "( ";
			
			car() -> print (os);
		 	os << " ";
		 	cdr() -> print (os);

			os << " )";
			break;
	}
}