#include "compiler.h"
#include "instruction.h"
#include "tokens.h"

using namespace lisp;
using namespace lisp::secd;

Value Compiler::Begin ( const Value & in )
{
	Compiler c = Compiler (in);
	return c . Compile() . value_or( Stack() ) . data();//TODO
}

Compiler::Compiler ( const Value & in )
: input ( in )
{}

std::optional<Stack> Compiler::Compile()
{
	if ( input . empty() )
		return std::nullopt;

	std::optional<Stack> car = Single();
	if ( ! car )
		return std::nullopt;

	std::optional<Stack> cdr = Compile();
	if ( ! cdr )
		return *car;

	return Stack() . load( *car ) . load ( *cdr );
}

std::optional<Stack> Compiler::Single()
{
	if ( input . empty() )
		return std::nullopt;

	Value val = input.top();
	input = input.pop();

	// Instruction is just pushed
	if ( val.isIns() )
		Stack() . push( Value::Instruction( val.ins().value() ) );

	// Number is loaded via LDC
	if ( val.isNum() )
		return Stack() . push ( val ) . push( Value::Instruction( LDC ) ) ;

	// Cons cell which means sub-list, is processed individualy
	if ( val.isCons() )
		return Compiler( val ) . Compile(); // TODO processes wrong

	// is Symbol
	std::string str = val.sym().value();
	if ( tokens::isSymbol( str ) )
		return CompileBuiltIn( str );

	return CompileSymbol( str );
}

std::optional<Stack> Compiler::CompileBuiltIn ( const std::string & val )
{
	// simple translation
	std::optional<Instruction> opt = tokens::translate( val );
	if ( opt )
		return Stack() . push ( Value::Instruction( *opt ) );

	// quote, unquote, quasiquote, if, defun, lambda
	
	if ( val == "if" )
		return CompileIf();

	//TODO
	return Stack();
}


std::optional<Stack> Compiler::CompileSymbol ( const std::string & val )
{
	//TODO
	return Stack();
}

std::optional<Stack> Compiler::CompileIf ()
{
	std::optional<Stack> arg1opt = Single();
	std::optional<Stack> arg2opt = Single();
	std::optional<Stack> arg3opt = Single();

	if ( ! ( arg1opt && arg2opt && arg3opt ) )
	{
		std::cerr << "If expected three arguments, got " 
		<< arg1opt.has_value()
			+ arg2opt.has_value()
			+ arg3opt.has_value()
		<< std::endl;

		return std::nullopt;
	}

	Stack joinStack = Stack() . push ( Value::Instruction( JOIN ) );

	Stack arg1 = *arg1opt;
	Stack arg2 = joinStack . load ( *arg2opt );
	Stack arg3 = joinStack . load ( *arg3opt );

	return Stack()
		. push ( arg3.data() )
		. push ( arg2.data() )
		. push( Value::Instruction( SEL ) )
		. load ( arg1.data() );
}

std::optional<Stack> Compiler::CompileDefun ()
{
	//TODO
	return Stack();
}

std::optional<Stack> Compiler::CompileLambda ()
{
	//TODO
	return Stack();
}


