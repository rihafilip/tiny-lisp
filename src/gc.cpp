#include "gc.h"
#include <stdexcept>
#include <cstring>
using namespace lisp;

// Initialization of static members
GC::List * GC::m_Roots = nullptr;
GC::List * GC::m_Available = nullptr;
GC::List * GC::m_All = nullptr;
GC::Memory * GC::m_Null = nullptr;

/*******************************************/
void GC::Start ( int howMuch )
{
	// allocates null memory ptr
	m_Null = new Memory();
	m_Null -> type = EMPTY;

	Allocate ( --howMuch );
}

void GC::Allocate ( int howMuch )
{
	if ( ! howMuch )
		return;

	Memory * mem = new Memory();
	m_All = new List ( mem, m_All );
	m_Available = new List ( mem, m_Available );
	Allocate ( --howMuch );
}

void GC::Stop ()
{
	m_All -> clean();
	delete m_All;
	delete m_Roots;
	delete m_Available;
	delete m_Null;

	m_All = nullptr;
	m_Roots = nullptr;
	m_Available = nullptr;
	m_Null = nullptr;
}

void GC::AddRoot ( Memory * root )
{
	if ( root -> type == EMPTY )
		return;

	m_Roots = new List( root, m_Roots );
}

void GC::RemoveRoot( Memory * root )
{	
	// roots are deleted
	if ( m_Roots == nullptr )
		return;

	if ( root -> type == EMPTY )
		return;
	
	RemoveRoot ( root, m_Roots );
}

void GC::RemoveRoot( Memory * mem, List * current, List * prev )
{
	// not found
	if ( current == nullptr )
		return;

	// found
	if ( current -> memory == mem )
	{
		// no previous => current == m_Roots
		if ( ! prev )
			m_Roots = current -> next;
		else
			prev -> next = current -> next;

		// prevent recursive delete
		current -> next = nullptr;
		delete current;
		return;
	}

	// find in next
	RemoveRoot( mem, current -> next, current );
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
	mem -> type = SYM;
	mem -> name = std::strcpy( new char[ str.size() + 1 ], str.c_str());
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
	return m_Null;
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
	{
		if ( lst -> memory -> type == SYM )
		{
			delete[] lst -> memory -> name;
			lst -> memory -> type = UNDEF;
		}
		// prepend to m_Available;
		m_Available = new List ( lst -> memory, m_Available );
	}

	Sweep ( lst -> next );
}

void GC::Collect ()
{
	Mark();
	delete m_Available;
	m_Available = nullptr;
	Sweep();
	// still no empty memory block, allocate more
	if ( m_Available == nullptr )
		Allocate();
}

GC::Memory * GC::GetNextEmptyMemory ()
{
	// no free memory, need to collect garbage
	if ( m_Available == nullptr )
		Collect();

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

GC::Memory::~Memory()
{
	if ( type == SYM )
		delete[] name;
}

void GC::Memory::Mark ()
{
	marked = true;
	// Recursively mark cons cells
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

		case SYM:
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
		case GC::MemoryType::SYM:
			return "symbol";
		case GC::MemoryType::CONS:
			return "cons";
		case GC::MemoryType::INST:
			return "instruction";
	}

	// compilator satisfaction
	return "";
}