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
	std::string mess = "Expected ";
	mess += toString(expected) ;
	mess += ", got ";
	mess += toString(memory -> type);

	throw std::runtime_error(mess);
}

/*********************************************************/

Value Value::car () const
{
	if ( memory -> type != GC::MemoryType::CONS )
		error (GC::MemoryType::CONS);

	return Value ( memory -> cons . first );
}

Value Value::cdr () const
{
	if ( memory -> type != GC::MemoryType::CONS )
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

sedc::Instruction Value::ins () const
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

/*********************************************************/

Value Value::append ( Value & val )
{
	if ( isNull() )
		return val;

	if ( ! isCons() )
		return Value::Cons( *this, val );


	return Value::Cons( car(), cdr() . append( val ) );
}

namespace lisp
{
	std::ostream & operator<< (std::ostream & os, const Value & val )
	{
		if ( val . isNull() )
			os << "null";
		else if ( val . isCons() )
			os << "( " << val.car() << " . " << val.cdr() << " )";
		else
		{
			switch ( val . memory -> type)
			{
				case GC::MemoryType::UNDEF:
					os << "undef";
					break;
				case GC::MemoryType::NUM:
					os << val . num();
					break;
				case GC::MemoryType::SYM:
					os << "\"" << val . sym() << "\"";
					break;
				case GC::MemoryType::INST:
					os << val . ins();
					break;
				default:
					break;
			}
		}

		return os;
	}
}