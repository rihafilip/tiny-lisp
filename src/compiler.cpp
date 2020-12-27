#include "compiler.h"
#include "instruction.h"
#include "tokens.h"

using namespace lisp;
using namespace lisp::secd;

Value Compiler::Begin ( const Value & in )
{
	if ( in.isNull() )
		return Value::Null();

	Compiler c = Compiler (in.car());
	Value next = Begin( in.cdr() );
	return Value::Cons(
		c . Compile() . value_or( Stack() ) . data(),
		next
	);
}

Compiler::Compiler ( const Value & in )
: _input ( in )
, _envMap()
{}

Compiler::Compiler ( const Value & input, const std::map<std::string, Value> & env )
: _input ( input )
, _envMap ( env )
{}

// shifts layer of enviroment by one
std::map<std::string, Value> Compiler::Shift( const std::map<std::string, Value> & env )
{
	std::map<std::string, Value> out;

	std::transform( env.cbegin(), env.cend(), std::inserter( out, out.end() ),
		[]( auto i )
		{
			return std::make_pair(
				i.first,
				Value::Cons(
					Value::Integer( i.second.car().num() + 1), // shift level by one
					i.second.cdr()
				)
			);
		}
	);

	return out;
}

std::map<std::string, Value> Compiler::MakeEnviroment( std::map<std::string, Value> enviroment, const Value & val, int counter )
{
	if ( val .isNull() )
		return enviroment;

	enviroment.insert( std::make_pair ( val.car().sym(), Value::Cons( Value::Integer(0), Value::Integer(counter) ) ) );
	return MakeEnviroment( enviroment, val.cdr(), ++counter );
}

/****************************************************************************/

std::optional<Stack> Compiler::Compile()
{
	if ( _input . empty() )
		return std::nullopt;

	std::optional<Stack> car = Single();
	if ( ! car )
		return std::nullopt;

	std::optional<Stack> cdr = Compile();
	if ( ! cdr )
		return *car;

	return Stack() . load( *car ) . load ( *cdr );
}

std::optional<Stack> Compiler::CompileArgs( const Stack & st )
{
	std::optional<Stack> cdr = CompileArgs();
	if ( ! cdr )
		return std::nullopt;

	return st . load ( *cdr );
}

// TODO merge with Compile
std::optional<Stack> Compiler::CompileArgs()
{
	if ( _input . empty() )
		return std::nullopt;

	std::optional<Stack> carOpt = Single();
	if ( ! carOpt )
		return std::nullopt;

	Stack car = Stack() . push ( Value::Instruction( CONS ) ) . load ( *carOpt );

	std::optional<Stack> cdr = CompileArgs();
	if ( ! cdr )
		return car . push( Value::Instruction( NIL ) );

	return Stack() . load( car ) . load ( *cdr );	
}

/****************************************************************************/

std::optional<Stack> Compiler::Single()
{
	if ( _input . empty() )
	{
		std::cerr << "Unexpected EOF." << std::endl;
		return std::nullopt;
	}

	Value val = _input.top();
	_input = _input.pop();

	// Instruction is just pushed
	if ( val.isIns() )
		Stack() . push( Value::Instruction( val.ins() ) );

	// Number is loaded via LDC
	if ( val.isNum() )
		return Stack() . push ( val ) . push( Value::Instruction( LDC ) ) ;

	// Cons cell which means sub-list, lambda or defun
	if ( val.isCons() )
		return CompileCons( val );

	// is Symbol
	std::string str = val.sym();

	// is built-in symbol
	if ( tokens::isSymbol( str ) )
		return CompileBuiltIn( str );

	return CompileSymbol( str );
}

/****************************************************************************/

std::optional<Stack> Compiler::CompileCons ( const Value & val )
{
	// lambda or defun
	// (lambda (params) (body)
	// car -> 'lambda' | 'defun'
	if ( val.car().isSym() )
	{
		std::string str = val.car().sym();

		if ( str == "lambda" )
			return CompileArgs( CompileLambda( val ) );

		else if ( str == "defun" )
			return CompileDefun( val );
	}

	// sub-list is processed individualy
	return Compiler( val, _envMap ) . Compile();
}

std::optional<Stack> Compiler::CompileBuiltIn ( const std::string & val )
{
	// simple translation
	std::optional<Instruction> opt = tokens::translate( val );
	if ( opt )
		return Stack() . push ( Value::Instruction( *opt ) );

	if ( val == "if" )
		return CompileIf();

	if ( val == "quote" )
	{
		//TODO
		return Stack();
	}

	if ( val == "quasiquote" )
		return CompileQuasiquote();

	if ( val == "unquote" )
	{
		std::cerr << "Unquote not in quasiquote." << std::endl;
		return std::nullopt;
	}

	// shouldn't occur
	std::cerr << "Incorrect usage of symbol '" << val << "'." << std::endl;
	return std::nullopt;
}


std::optional<Stack> Compiler::CompileSymbol ( const std::string & val )
{
	//TODO
	if ( _envMap.count( val ) )
		return Stack() . push ( _envMap.at(val) ) . push( Value::Instruction(LD) );
 
	std::cerr << "Token '" << val << "' is not defined." << std::endl;
	return std::nullopt;
}

/****************************************************************************/

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
		. push ( arg1.data() );
}

std::optional<Stack> Compiler::CompileQuasiquote ()
{
	//TODO
	return Stack();
}

/****************************************************************************/
// input (defun name (params) (body))
std::optional<Stack> Compiler::CompileDefun ( const Value & val )
{
	if ( !( val.cdr().isCons()
		&& val.cdr().car().isSym()
	))
	{
		std::cerr << "Wrong structure of defun." << std::endl;
		return std::nullopt;
	}


	return Stack();
}

// input (symbol (params) (body))
std::optional<Stack> Compiler::CompileLambda ( const Value & val )
{
	// correct structure
	if ( !(	val.cdr().isCons() // ( (params) . ( (body) . nil ) )
		&& 	val.cdr().cdr().isCons() // (body . nil)
		&& 	val.cdr().cdr().cdr().isNull() // nil
	))
	{
		std::cerr << "Wrong structure of lambda." << std::endl;
		return std::nullopt;
	}

	Value params = val.cdr().car();
	Value body = val.cdr().cdr().car();

	std::map<std::string, Value> enviroment = MakeEnviroment( Shift(_envMap), params );

	std::optional<Stack> compiledBodyOpt = Compiler( body, enviroment ) . Compile();

	if ( ! compiledBodyOpt )
		return std::nullopt;

	Stack compiledBody = Stack() . push ( Value::Instruction( RTN ) ) . load( *compiledBodyOpt );
	return Stack()
	 	. push( Value::Instruction( AP ) )
		. push( compiledBody . data() )
		. push( Value::Instruction( LDF ) );
}
