#include "parser.h"
#include "compiler.h"
#include "runtime.h"
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
	Value out = Lexer::Scan( "( defun (addtwelve x) (+ x 12))" );
	std::cout << "expression: " << out << std::endl << std::endl;

	out = Lexer::Scan( "( + 1 -2 ;------ 1 + 2 \n )" );
	std::cout << "expression: " << out << std::endl << std::endl;

	out = Lexer::Scan( "(((((((((((((((((((((((((((((((((())))))))))))))))))))))))))))))))))" );
	std::cout << "expression: " << out << std::endl << std::endl;
}

void testParser()
{
	std::cout << Parser::Parse( Lexer::Scan( "( + 1 2 )" ) ) << std::endl << std::endl;
	std::cout << Parser::Parse( Lexer::Scan( "(car ( + 1 2 ))" ) ) << std::endl << std::endl;
	std::cout << Parser::Parse( Lexer::Scan( "(car ( + 1 2 )) (cdr ( 2 3 )) (cdr ( 2 3 ) )" ) ) << std::endl << std::endl;
	std::cout << Parser::Parse( Lexer::Scan( "' ( + 10 (- 1 2 ) ) " ) ) << std::endl << std::endl;
	std::cout << Parser::Parse( Lexer::Scan( "` ( + 10 ,(- 1 2 ) ) " ) ) << std::endl << std::endl;
	std::cout << Parser::Parse( Lexer::Scan( "` +  '(- 1 2 ) " ) ) << std::endl << std::endl;

	std::cout << Parser::Parse( Lexer::Scan( " (1 . 2 ) " ) ) << std::endl << std::endl;


	std::cout << "Expected error: ";
	Parser::Parse( Lexer::Scan( "( car ' (1 2 3 )" ) );
	std::cout << std::endl;
}

void compile ( std::string str )
{
	std::cout << "input " << str << std::endl;
	std::cout << Compiler::CompileCode( Parser::Parse( Lexer::Scan( str ) ) ).first << std::endl << std::endl;	
}

void testCompiler()
{
	compile( "( + 1 2 )" );
	compile( "( + ( / 10 5) 2 )" );
	compile ( "(car ( + 1 2 )) (cdr '( 2 3 ))" );
	compile( "( if 0 ( + 1 2 ) ( - 2 3) )" );
	compile( "( + 1 (if 0 1 2) )" );
	compile( "( if ( if ( + 2 3 ) 1 0 ) ( consp 1 ) ( consp 0 ) )" );

	std::cout << "Expected error:" << std::endl;
	compile( "(if 0 1 2 3)" );

	std::cout << "Expected error:" << std::endl;
	compile ( "( + 1 2 3 ) ");

	std::cout << "FUNCS:" << std::endl<< std::endl;
	
	compile( "( + 1 ((lambda (x y) (+ x y)) 10 20 ) )" );
	compile( "( (lambda (z) ((lambda (x y) (+ y z) ) 10 20 ) ) 5 )" );
	compile( "(defun foo (x) (+ x 1))" );
	compile( "(defun foo (x) (+ x 1)) (foo 0)" );

	compile ( "(lambda (x) (x))" );

	std::cout << "Expected error:" << std::endl;
	compile("((+ lambda (x) ) 10)");

	compile( "(defun foo (x) (+ x 1)) (defun bar (y) (foo y))" );
	compile( "(defun foo (x) (+ x 1)) (defun bar (y) (foo y)) (car '(10 20))"  );

	std::cout << "QUOTES:" << std::endl<< std::endl;

	compile( "`(1 (2 ,(- 4 1)))" );
	compile( "`(1 . ,(+ 1 2 ))");
	compile( "'()");
	compile( "(defun foo (x) (x)) `(+ 3 ,foo 15) " );
	compile( "(defun foo (x) (x)) `(+ 3 ,(foo 15)) " );
	compile( "(print 'hello 'world)" );

	std::cout << "LET:" << std::endl<< std::endl;
	compile( "(let ((x 10)) (+ x 10))" );
	compile( "(let ((x 10) (y 20) (z 30)) ( `(,x ,y ,z) ))" );

	compile( "(let ((x 10)) (+ x (let ((x 20)) x) ))" );

	std::cout << "LETREC:" << std::endl<< std::endl;
	compile( "(letrec ((x 10)) (+ x 10))" );
	compile( "(letrec ((foo (lambda (x) (if (eq x 0) 0 (foo (- x 1))) ) )) (foo 5))" );
	compile( "(letrec ((foo (lambda (x) (if (eq x 0) (print 'foo) (bar (- x 1))))) (bar (lambda (x) (if (eq x 0) (print 'bar) (foo (- x 1)))))) (foo 5 ) )");
}

void exec ( const std::string & str )
{
	std::cout << "input " << str << std::endl;
	std::cout << "compiled " << Compiler::CompileCode( Parser::Parse( Lexer::Scan( str ) ) ).first << std::endl;
	lisp::secd::Runtime::executeCode( Stack ( Compiler::CompileCode( Parser::Parse( Lexer::Scan( str ) ) ).first ) );

	std::cout << std::endl << std::endl;
}

void testRuntime ()
{
	exec ( "( + 1 2 )" );
	exec ( "( + 1 2 ) ( - 1 2 )" );
	exec( "( if 0 ( + 1 2 ) ( - 2 3) )" );
	exec( "( if ( if ( + 2 3 ) 0 1 ) ( consp 1 ) ( consp '( 0 1 ) ) )" );

	exec( "( + ((lambda (x y) (+ x y)) 10 20 ) 1 )" );
	exec( "( (lambda (z) ((lambda (x y) (+ y z) ) 10 20 ) ) 5 )" );
	exec ( "(lambda (x) (x))" );
	exec ( "((lambda (x) x) 10)" );

	std::cout << "Expected error:" << std::endl;
	exec ( "((lambda (x) (x)) 10)" );	

	exec( "(defun foo (x) (+ x 1))" );
	exec( "(defun foo (x) (+ x 1)) (foo 0)" );
	exec( "(defun foo (x) (+ x 1)) (defun bar (y) (- 10 (foo y)) ) (bar 5)" );
	exec( "(defun foo (x) (+ x 1)) (- 4 (foo 2))" );
	exec( "(defun foo (x) (+ x 1)) ((lambda (x y) (x (+ y 10)) ) foo 10)" );	

	exec( "(print 'hello 'world) ");

	exec ( "(let ((foo (+ 1 3))) foo)" );
	exec( "(let ((foo (lambda () (print 'foo))) (bar (lambda () (print 'bar)))) (foo))" );
	
	exec ( "(letrec ((foo (lambda (x) (if (eq x 0) 0 (foo (- x 1))) ) )) (foo 2))" );
}

void test ()
{
	testGC();
	testValue();
	GC::Start();
	testLexer();
	testParser();
	testCompiler();
	testRuntime();

	GC::Stop();
}