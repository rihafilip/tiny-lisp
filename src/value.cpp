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

void Value::error ( GC::MemoryType expected )
{
	std::string mess = "Expected ";
	mess += toString(expected) ;
	mess += ", got ";
	mess += toString(memory -> type);

	throw std::runtime_error(mess);
}

/*********************************************************/

Value Value::car ()
{
	if ( memory -> type != GC::MemoryType::CONS )
		error (GC::MemoryType::CONS);

	return Value ( memory -> cons . first );
}

Value Value::cdr ()
{
	if ( memory -> type != GC::MemoryType::CONS )
		error (GC::MemoryType::CONS);

	return Value ( memory -> cons . second );
}

int Value::num ()
{
	if ( memory -> type != GC::MemoryType::NUM )
		error (GC::MemoryType::NUM);

	return memory -> number;
}

std::string Value::sym ()
{
	if ( memory -> type != GC::MemoryType::SYM )
		error (GC::MemoryType::SYM);

	return memory -> name;
}

sedc::Instruction Value::ins ()
{
	if ( memory -> type != GC::MemoryType::INST )
		error (GC::MemoryType::INST);

	return memory -> instruct;
}

/*********************************************************/

bool Value::isNull ()
{
	return memory -> type == GC::MemoryType::EMPTY;
}

bool Value::isCons ()
{
	return memory -> type == GC::MemoryType::CONS;
}