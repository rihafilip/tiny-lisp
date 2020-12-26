#include "parser.h"
#include <cassert>
#include <stdexcept>
using namespace lisp;

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
	GC::Start(2);

	for ( int i = 0; i < 10; ++i )
	{
		Value a = Value::Integer( i );
		assert( *(a.num()) == i );
	}

	Value b = Value::Integer( 5 );
	std::cerr << "Expected error: ";
	assert( ! b.sym() );

	for ( int i = 0; i < 3; ++i )
	{
		Value a = Value::Symbol( std::string("a") );
		assert( *(a.sym()) == "a");		
	}

	assert ( *(b.num()) == 5 );
	GC::Stop();
}

void testLexer ()
{
	GC::Start();
	Value out = Lexer::Scan( "( defun (addtwelve x) (+ x 12))" );
	std::cout << "expression: " << out << std::endl << std::endl;

	out = Lexer::Scan( "( + 1 -2 ;------ 1 + 2 \n )" );
	std::cout << "expression: " << out << std::endl << std::endl;

	out = Lexer::Scan( "(((((((((((((((((((((((((((((((((())))))))))))))))))))))))))))))))))" );
	std::cout << "expression: " << out << std::endl << std::endl;
	GC::Stop();
}

void testParser()
{
	GC::Start();
	std::cout << Parser::Parse( Lexer::Scan( "( + 1 2 )" ) ) << std::endl;
	std::cout << std::endl;
	Value out = Parser::Parse( Lexer::Scan( "(car ( + 1 2 ))" ) );
	std::cout << out << std::endl;
	GC::Stop();
}

int main(int argc, char const *argv[])
{
	testGC();
	testValue();
	testLexer();
	testParser();
	return 0;
}