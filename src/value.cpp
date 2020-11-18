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

Value Value::Cons ( Value car, Value cdr )
{
	return Value ( GC::GetMemory( car.memory, cdr.memory ) );
}

Value Value::Instruction ( sedc::Instruction instruct )
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

std::optional<sedc::Instruction> Value::ins () const
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

/*********************************************************/

Value Value::append ( Value val )
{
	if ( isNull() )
		return val;

	if ( ! isCons() )
		return Value::Cons( *this, Value::Cons( val, Value::Null() ) );

	if ( cdr() . value() . isNull() )
		return Value::Cons( car().value(), Value::Cons( val, Value::Null() ) );

	return Value::Cons( car().value(), cdr() -> append( val ) );
}

namespace lisp
{
	std::ostream & operator<< (std::ostream & os, const Value & val )
	{
		switch ( val . memory -> type)
		{
			case GC::MemoryType::UNDEF:
				os << "undef";
				break;

			case GC::MemoryType::EMPTY:
				os << "null";
				break;

			case GC::MemoryType::NUM:
				os << val . num() . value();
				break;

			case GC::MemoryType::SYM:
				os << '"' << val . sym() . value() << '"';
				break;

			case GC::MemoryType::INST:
				os << val . ins() . value();
				break;

			case GC::MemoryType::CONS:
				os << "( " << val . car() . value() << " . " << val . cdr() . value() << " )";
				break;
		}

		return os;
	}
}