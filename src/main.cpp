#include "parser.h"
#include "compiler.h"
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
		assert( a.num() == i );
	}

	Value b = Value::Integer( 5 );
	bool catching = false;
	try
	{
		std::cerr << "Expected error: ";
		b.sym();	
	}
	catch ( std::runtime_error & e )
	{
		catching = true;
	}
	assert( catching );

	for ( int i = 0; i < 3; ++i )
	{
		Value a = Value::Symbol( std::string("a") );
		assert( a.sym() == "a");		
	}

	assert ( b.num() == 5 );
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
	std::cout << Parser::Parse( Lexer::Scan( "( + 1 2 )" ) ) << std::endl << std::endl;
	std::cout << Parser::Parse( Lexer::Scan( "(car ( + 1 2 ))" ) ) << std::endl << std::endl;
	std::cout << Parser::Parse( Lexer::Scan( "(car ( + 1 2 )) (cdr ( 2 3 )) (cdr ( 2 3 ) )" ) ) << std::endl << std::endl;

	std::cout << "Expected error: ";
	Parser::Parse( Lexer::Scan( "( car ' (1 2 3 )" ) );
	std::cout << std::endl;

	GC::Stop();
}

void compile ( std::string str )
{
	std::cout << "input " << str << std::endl;
	std::cout << Compiler::Begin( Parser::Parse( Lexer::Scan( str ) ) ) << std::endl << std::endl;	
}

void testCompiler()
{
	GC::Start();

	compile( "( + 1 2 )" );
	compile ( "(car ( + 1 2 )) (cdr ( 2 3 ))" );
	compile( "( if 0 ( + 1 2 ) ( - 2 3) )" );
	compile( "( if ( if ( + 2 3 ) 1 0 ) ( consp 1 ) ( consp 0 ) )" );
	compile( "( + 1 ((lambda (x y) (+ x y)) 10 20 ) )" );
	compile( "( (lambda (z) ((lambda (x y) (+ y z) ) 10 20 ) ) 5 )" );
	compile( "(defun foo (x) (+ x 1))" );
	compile( "(defun foo (x) (+ x 1)) (foo 0)" );
	// compile ( "(lambda (x) (x) )" );
	// compile( "(defun foo (x) (+ x 1)) (defun bar (y) (- y 2))" );

	GC::Stop();
}

int main(int argc, char const *argv[])
{
	testGC();
	testValue();
	testLexer();
	testParser();
	testCompiler();
	return 0;
}
