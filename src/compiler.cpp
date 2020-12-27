#include "compiler.h"
#include "instruction.h"
#include "tokens.h"

using namespace lisp;
using namespace lisp::secd;

// enviroment and functions helpers
namespace
{
	// shifts layer of enviroment by one
	Compiler::EnvMap ShiftEnviroment( const Compiler::EnvMap & env )
	{
		Compiler::EnvMap out;

		std::transform( env.cbegin(), env.cend(), std::inserter( out, out.end() ),
			[]( auto i )
			{
				return std::make_pair(
					i.first,
					Value::Cons(
						Value::Integer( i.second.car().num() + 1), // ShiftEnviroment level by one
						i.second.cdr()
					)
				);
			}
		);

		return out;
	}

	Compiler::EnvMap EnviromentAddValues(Compiler:: EnvMap enviroment, const Value & val, int counter = 0 )
	{
		if ( val .isNull() )
			return enviroment;

		enviroment.insert( std::make_pair ( val.car().sym(), Value::Cons( Value::Integer(0), Value::Integer(counter) ) ) );
		return EnviromentAddValues( enviroment, val.cdr(), ++counter );
	}

	int EnviromentMax ( Compiler::EnvMap env )
	{
		int max = 0;
		if ( ! env.empty() )
		{
			max = std::max_element ( env.begin(), env.end(),
				[] ( auto a, auto b )
					{ return a.second.car().num() < b.second.car().num(); }
			) -> second.cdr().car().num();
		}
		return max;
	}

	std::set<std::string> FunctionsRemoveSymbols ( std::set<std::string> set, const Value & val )
	{
		if ( val.isNull() )
			return set;

		set . erase ( val.car().sym() );
		return FunctionsRemoveSymbols( set, val.cdr() );
	}
} // anonymous namespace

/****************************************************************************/

Value Compiler::Begin ( const Value & in )
{
	if ( in.isNull() )
		return Value::Null();

	Compiler c = Compiler (in.car());
	Value next = Begin( in.cdr() );
	return Value::Cons(
		c . CompileCode() . value_or( Stack() ) . data(),
		next
	);
}

Compiler::Compiler ( const Value & in )
: _input ( in )
, _envMap()
, _functions()
, _subroutine( false )
{}

Compiler::Compiler ( const Value & input, const EnvMap & env, const std::set<std::string> funcs, bool sub )
: _input ( input )
, _envMap ( env )
, _functions( funcs )
, _subroutine( sub )
{}

/****************************************************************************/

std::optional<Stack> Compiler::CompileCode()
{
	if ( _input . empty() )
		return std::nullopt;

	std::optional<Stack> car = Compile();
	if ( ! car )
		return std::nullopt;

	std::optional<Stack> cdr = CompileCode();
	if ( ! cdr )
		return *car;

	return Stack() . load( *car ) . load ( *cdr );
}

std::optional<Stack> Compiler::CompileArgsCode( const Stack & st )
{
	std::optional<Stack> cdr = CompileArgsCode();
	if ( ! cdr )
		return std::nullopt;

	return st . load ( *cdr );
}

// TODO merge with CompileCode
std::optional<Stack> Compiler::CompileArgsCode()
{
	if ( _input . empty() )
		return std::nullopt;

	std::optional<Stack> carOpt = Compile();
	if ( ! carOpt )
		return std::nullopt;

	Stack car = Stack() . push ( Value::Instruction( CONS ) ) . load ( *carOpt );

	std::optional<Stack> cdr = CompileArgsCode();
	if ( ! cdr )
		return car . push( Value::Instruction( NIL ) );

	return Stack() . load( car ) . load ( *cdr );	
}

/****************************************************************************/

std::optional<Stack> Compiler::Compile()
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
			return CompileLambda( val );

		else if ( str == "defun" )
			return CompileDefun( val );
	}

	// sub-list is processed individualy
	return Compiler( val, _envMap, _functions ) . CompileCode();
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
	if ( _envMap.count( val ) )
	{
		Stack output = Stack() . push ( _envMap.at(val) ) . push( Value::Instruction(LD) );
		if ( _functions.count( val ) )
			return CompileArgsCode( Stack() . push( Value::Instruction( AP ) ) . load ( output ) );

		return output;
	}
 
	std::cerr << "Token '" << val << "' is not defined." << std::endl;
	return std::nullopt;
}

/****************************************************************************/

std::optional<Stack> Compiler::CompileIf ()
{
	std::optional<Stack> opt1 = Compile();
	std::optional<Stack> opt2 = Compile();
	std::optional<Stack> opt3 = Compile();

	if ( ! ( opt1 && opt2 && opt3 ) )
	{
		std::cerr << "If expected three arguments, got " 
		<< opt1.has_value() + opt2.has_value() + opt3.has_value()
		<< "." << std::endl;

		return std::nullopt;
	}

	Stack joinStack = Stack() . push ( Value::Instruction( JOIN ) );

	Stack arg1 = *opt1;
	Stack arg2 = joinStack . load ( *opt2 );
	Stack arg3 = joinStack . load ( *opt3 );

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

std::optional<Stack> Compiler::CompileLambda ( const Value & val )
{
	std::optional<Stack> out = CompileFunction( val );
	if ( ! out )
		return std::nullopt;

	return CompileArgsCode(
		Stack()
	 	. push( Value::Instruction( AP ) )
		. push( out -> data() )
		. push( Value::Instruction( LDF ) )
	);
}

// input (defun name (params) (body))
std::optional<Stack> Compiler::CompileDefun ( const Value & val )
{
	if ( _subroutine )
	{
		std::cerr << "Defun can be only used in a global scope." << std::endl;
		return std::nullopt;
	}

	if ( ! ( val.cdr().isCons() && val.cdr().car().isSym() ))
	{
		std::cerr << "Wrong structure of defun." << std::endl;
		return std::nullopt;
	}

	std::string name = val.cdr().car().sym();

	std::optional<Stack> lambda = CompileFunction( val.cdr() );
	if ( ! lambda )
		return std::nullopt;

	// Add function to enviroment
	int max = EnviromentMax( _envMap );
	_envMap.insert( std::make_pair ( name, Value::Cons( Value::Integer(0), Value::Integer( ++max ) ) ) );

	return Stack() . push( lambda -> data() ) . push( Value::Instruction( DEFUN ) );
}

// input (symbol (params) (body))
std::optional<Stack> Compiler::CompileFunction ( const Value & val )
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

	EnvMap env = EnviromentAddValues( ShiftEnviroment(_envMap), params );
	std::set<std::string> funcs = FunctionsRemoveSymbols( _functions, params );

	std::optional<Stack> compiledBodyOpt = Compiler( body, env, funcs, true ) . CompileCode();

	if ( ! compiledBodyOpt )
		return std::nullopt;

	return Stack() . push ( Value::Instruction( RTN ) ) . load( *compiledBodyOpt );
}
