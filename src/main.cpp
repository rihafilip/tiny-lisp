#include "lexer.h"
#include <cassert>
#include <stdexcept>
using namespace lisp;

void testLexer ()
{
	GC::Start(1000);
	Value out =  Lexer::Scan( "( defun (addtwelve x) (+ x 12))" );
	std::cout << "expression: " << out.car() << std::endl <<std::endl;
	std::cout << "Built-in: " << out.cdr().car() << std::endl<<std::endl;
	std::cout << "Tokens: " << out.cdr().cdr() << std::endl<<std::endl;
	GC::Stop();
}

void testGC ()
{
	GC::Start(20);
	for (int i = 0; i < 10; ++i)
	{
		GC::Memory * mem = GC::GetMemory(i);
		GC::AddRoot(mem);
		GC::RemoveRoot(mem);
	}

	for (int i = 0; i < 20; ++i)
	{
		GC::Memory * mem = GC::GetMemory("hh");
		GC::AddRoot(mem);
		GC::RemoveRoot(mem);
	}

	GC::Stop();
	GC::Start(20);
	GC::Stop();
}

void testValue ()
{
	GC::Start();

	for ( int i = 0; i < 10; ++i )
	{
		Value a = Value::Integer( i );
		std::cout << a.num() << std::endl;
	}

	bool ex = false;
	Value b = Value::Integer( 5 );
	try
	{
		std::cout << b.sym() << std::endl;	
	}
	catch ( std::runtime_error & e )
	{
		ex = true;
	}
	assert(ex);

	for ( int i = 0; i < 3; ++i )
	{
		Value a = Value::Symbol( std::string("a") );
		std::cout << a.sym() << std::endl;
	}

	GC::Stop();
}

int main(int argc, char const *argv[])
{
	testGC();
	testValue();
	testLexer();
	return 0;
}