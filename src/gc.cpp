#include "gc.h"
#include <stdexcept>
using namespace lisp;

// Initialization of static members
GC::List * GC::m_Roots = nullptr;
GC::List * GC::m_Available = nullptr;
GC::List * GC::m_All = nullptr;

/*******************************************/

void GC::Start ( int todo )
{
	if ( ! todo )
		return;

	Memory * mem = new Memory();
	m_All = new List ( mem, m_All );
	m_Available = new List ( mem, m_Available );
	Start ( --todo );
}

void GC::Stop ()
{
	m_All -> clean();
	delete m_All;
	delete m_Roots;
	delete m_Available;

	m_All = nullptr;
	m_Roots = nullptr;
	m_Available = nullptr;
}

void GC::AddRoot ( Memory * root )
{
	m_Roots = new List( root, m_Roots );
}

void GC::RemoveRoot( Memory * root )
{
	if ( m_Roots != nullptr )
		m_Roots = m_Roots -> rm( root );
}

/*******************************************/

GC::Memory * GC::GetMemory( int num )
{
	Memory * mem = GetNextEmptyMemory();
	mem -> type = NUM;
	mem -> number = num;
	return mem;
}

GC::Memory * GC::GetMemory( const std::string & str )
{
	Memory * mem = GetNextEmptyMemory();
	mem -> type = STR;
	mem -> name = str.c_str();
	return mem;
}

GC::Memory * GC::GetMemory( Memory * car, Memory * cdr )
{
	Memory * mem = GetNextEmptyMemory();
	mem -> type = CONS;
	mem -> cons . first = car;
	mem -> cons . second = cdr;
	return mem;
}

GC::Memory * GC::GetMemory ( sedc::Instruction instruct )
{
	Memory * mem = GetNextEmptyMemory();
	mem -> type = INST;
	mem -> instruct = instruct;
	return mem;	
}

GC::Memory * GC::GetNull ()
{
	Memory * mem = GetNextEmptyMemory();
	mem -> type = EMPTY;
	return mem;
}

/*******************************************/

void GC::Mark( List * lst )
{
	if ( lst == nullptr )
		return;

	lst -> memory -> Mark();
	Mark( lst -> next );
}

void GC::Sweep ( List * lst )
{
	if ( lst == nullptr )
		return;
	// not marked == not used
	if ( ! lst -> memory -> marked )
		// prepend to m_Available;
		m_Available = new List ( lst -> memory, m_Available );

	Sweep ( lst -> next );
}

GC::Memory * GC::GetNextEmptyMemory ()
{
	// no free memory, need to collect garbage
	if ( m_Available == nullptr )
	{
		Mark();
		delete m_Available;
		Sweep();
		/// still no empty memory block, allocate more
		if ( m_Available == nullptr )
			Start();
	}

	Memory * ret = m_Available -> memory;
	List * tmp = m_Available -> next;
	m_Available -> next = nullptr;
	delete m_Available;
	m_Available = tmp;
	return ret;
}

/*******************************************/

GC::Memory::Memory()
{
	marked = false;
	type = UNDEF;
}

void GC::Memory::Mark ()
{
	marked = true;
	// Recursively amrk cons cells
	if ( type == MemoryType::CONS )
	{
		this -> cons . first -> Mark();
		this -> cons . second -> Mark();
	}
}

bool GC::Memory::operator== ( const Memory & other )
{
	if ( this -> type != other . type )
		return false;

	switch ( this -> type )
	{
		case NUM:
			return this -> number == other . number;

		case STR:
			return this -> name == other . name;

		case CONS:
			return ( this -> cons . first == other . cons . first )
				&& ( this -> cons . second == other . cons . second );
		// UNDEF, NULL
		default:
			return true;
	}
}

/********************************/

std::string lisp::toString ( GC::MemoryType type )
{
	switch (type)
	{
		case GC::MemoryType::UNDEF:
			return "undefined";
		case GC::MemoryType::EMPTY:
			return "null";
		case GC::MemoryType::NUM:
			return "number";
		case GC::MemoryType::STR:
			return "string";
		case GC::MemoryType::CONS:
			return "cons";
		case GC::MemoryType::INST:
			return "instruction";
	}

	// compilator satisfaction
	return "";
}