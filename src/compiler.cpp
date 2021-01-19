#include "compiler.h"
#include "instruction.h"
#include "tokens.h"

#include <functional>

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

	/**
	 * add list of values to enviroment
	 * @param  enviroment input enviroment
	 * @param  val        list of values
	 * @param  counter    index of first variable
	 * @return            new variable
	 */
	Compiler::EnvMap EnviromentAddValues(Compiler:: EnvMap enviroment, const Value & val, int counter = 0 )
	{
		if ( val .isNull() )
			return enviroment;

		enviroment.insert( { val.car().sym(), Value::Cons( Value::Integer(0), Value::Integer(counter)) });
		return EnviromentAddValues( enviroment, val.cdr(), ++counter );
	}

	/// EnviromentNext subroutine
	int EnviromentNextSub ( Compiler::EnvMap::iterator i, Compiler::EnvMap::iterator end, int max )
	{
		if ( i == end )
			return ++max;

		std::cout << "\033[31m I is" << i -> second.cdr().num() << "\033[39m" << std::endl;
		int nextMax = std::max( max, i -> second.cdr().num() );
		return EnviromentNextSub( ++i, end, nextMax );
	}

	/// Returns index of the next value to be added to enviroment
	int EnviromentNext ( Compiler::EnvMap env )
	{
		if ( env.empty() )
			return 0;
		return EnviromentNextSub( env.begin(), env.end(), 0 );
	}

	Value ConsAppend ( const Value & ls1, const Value & ls2, bool flag = true )
	{
		if ( ls1.isNull() )
		{
			return Value::Cons( ls2, Value::Null() );
		}

		return Value::Cons( ls1.car(), ConsAppend( ls1.cdr(), ls2, false) );
	}
} // anonymous namespace

/****************************************************************************/

std::pair<Value, Compiler::EnvMap> Compiler::CompileCode ( const Value & lst, const EnvMap & env, const Value & acc )
{
	if ( lst.isNull() )
		return { acc, env };

	Value next = lst.car();

	if ( next.isCons() )
	{
		// (defun ...)
		if ( next.car().isSym() && next.car().sym() == "defun" )
		{
			auto [out, outEnv] = CompileDefun( next, env );
			return CompileCode( lst.cdr(), outEnv, ConsAppend(acc, out) );
		}

		// cons -> Applicate
		std::optional<Stack> compiled = CompileCall ( Stack (next), env );

		// failed to compile, ignore
		if ( ! compiled )
			return CompileCode( lst.cdr(), env, acc );
		return CompileCode( lst.cdr(), env, ConsAppend(acc, compiled -> data()) );
	}

	// simple compile
	std::optional<Stack> compiled = CompileArguments ( Stack (next), env, Stack() );

	// failed to compile, ignore
	if ( ! compiled )
		return CompileCode( lst.cdr(), env, acc );
	return CompileCode( lst.cdr(), env, ConsAppend(acc, compiled -> data()) );
}

/****************************************************************************/

std::optional<Stack> Compiler::CompileCall ( Stack st, const EnvMap & env )
{
	if ( st . empty() )
	{
		std::cerr << "Incorrect application of empty list." << std::endl;
		return std::nullopt;
	}

	Value val = st.top();
	st = st.pop();

	if ( val.isNum() )
	{
		std::cerr << "Number cannot be applied." << std::endl;
		return std::nullopt;
	}

	// apply cons cell
	if ( val.isCons() )
	{
		std::optional<Stack> output = CompileCall( Stack(val), env );
		if ( ! output )
			return std::nullopt;

		return CompileArgumentsList( st, env, Stack() . push ( Value::Instruction(AP) ) . load ( *output ) );
	}

	// built-ins compile their own arguments
	std::string str = val.sym();
	if ( tokens::isSymbol( str ) )
		return CompileBuiltInCall ( str, st, env );

	std::optional<Stack> output = CompileSymbol( str, st, env );
	if ( !output )
		return std::nullopt;

	return CompileArgumentsList( st, env, Stack() . push ( Value::Instruction(AP) ) . load ( *output ) );
}

/****************************************************************************/

// Arguments, individual tokens 
std::optional<Stack> Compiler::CompileArguments ( const Stack & st, const EnvMap & env, const Stack & acc, bool append )
{
	if ( st.single() )
	{
		if ( append )
			return acc . push ( st.top() );
		
		std::cerr << "Non-null ending list of arguments." << std::endl;
		return std::nullopt;
	}

	if ( st.empty() )
	{
		if ( append )
			return acc . push ( Value::Instruction ( NIL ) );
		return acc;
	}

	if ( st . top(). isNull() )
	{
		std::cerr << "Unexpected nil." << std::endl;
		return std::nullopt;
	}

	std::optional<Stack> compiled = CompileToken( st, env );
	if ( ! compiled )
		return std::nullopt;

	Stack nextAcc;
	if ( append )
		nextAcc = acc . push( Value::Instruction( CONS ) ) . load ( *compiled );
	else
		nextAcc = acc.load ( *compiled );

	return CompileArguments ( st.pop(), env, nextAcc, append );
}

std::optional<Stack> Compiler::CompileArgumentsList ( const Stack & st, const EnvMap & env, const Stack & acc )
{
	return CompileArguments( st, env, acc, true );
}

std::optional<Stack> Compiler::CompileToken ( Stack st, const EnvMap & env )
{
	Value val = st.top();
	st = st.pop();

	// Number is loaded via LDC
	if ( val.isNum() )
		return CompileNumber( val );

	// Cons cell is applicated
	if ( val.isCons() )
		return CompileCall ( Stack( val ), env );

	// is Symbol
	std::string str = val.sym();

	// is built-in symbol
	if ( tokens::isSymbol( str ) )
		return CompileBuiltIn( str, st, env );

	return CompileSymbol( str, st, env );
}

/****************************************************************************/

Stack Compiler::CompileNumber ( const Value & val )
{
	// Number is loaded via LDC
	return Stack() . push ( val ) . push( Value::Instruction( LDC ) );
}

/****************************************************************************/

// Simple translation with checks
std::optional<Stack> Compiler::CompileBuiltIn ( const std::string & val, const Stack & st, const EnvMap & env )
{
	// simple translation
	std::optional<Instruction> opt = tokens::translate( val );
	if ( opt )
		return Stack() . push ( Value::Instruction( *opt ) );

	if ( val == "unquote" )
	{
		std::cerr << "Unquote not in quasiquote." << std::endl;
		return std::nullopt;
	}

	if ( val == "defun" )
	{
		std::cerr << "Defun can be only used in a global scope." << std::endl;
		return std::nullopt;
	}

	// symbol not usable without 
	std::cerr << "Incorrect usage of symbol '" << val << "'." << std::endl;
	return std::nullopt;
}

std::optional<Stack> Compiler::CompileBuiltInCall ( const std::string & val, const Stack & st, const EnvMap & env )
{
	// simple translation
	std::optional<Instruction> opt = tokens::translate( val );

	if ( opt )
		return CompileBuiltInCallSimple( *opt, st, env );

	static const std::map<std::string, std::function< std::optional<Stack>( const Stack&, const Compiler::EnvMap& ) >> 
	builtInMap =
	{
		{ "if", CompileIf },
		{ "quote", CompileQuote },
		{ "quasiquote", CompileQuasiquote },
		{ "lambda", CompileLambda  },
		{ "let", CompileLet },
		{ "unquote",
			[] ( const Stack & st, const EnvMap & env )
			{
				std::cerr << "Unquote not in quasiquote." << std::endl;
				return std::nullopt;
			}
		},
		{ "defun",
			[] ( const Stack & st, const EnvMap & env )
			{
				std::cerr << "Defun can be only used in a global scope." << std::endl;
				return std::nullopt;
			}
		},
		{ "letrec", CompileLetrec }
	};

	if ( builtInMap.count(val) )
		return builtInMap . at(val) ( st, env );

	std::cerr << "Incorrect usage of symbol '" << val << "'." << std::endl;
	return std::nullopt;
}

std::optional<Stack> Compiler::CompileBuiltInCallSimple ( Instruction val, const Stack & st, const EnvMap & env )
{
	static const std::set<Instruction> zeroArg = { NIL, READ };
	static const std::set<Instruction> oneArg = { CAR, CDR, CONSP };
	static const std::set<Instruction> twoArg = { CONS, ADD, SUB, MUL, DIV, EQ, LESS, MORE };

	bool correct;
	if ( val == PRINT ) 				correct = true;
	else if ( zeroArg.count ( val ) ) 	correct = AssertArgsCount( 0, st );
	else if ( oneArg.count ( val ) ) 	correct = AssertArgsCount( 1, st );
	else if ( twoArg.count ( val ) ) 	correct = AssertArgsCount( 2, st );
	else
	{
		std::cerr << "Unhandeled simple translate args count." << std::endl;
		return std::nullopt;
	}

	if ( ! correct )
	{
		std::cerr << val << " has incorrect number of arguments." << std::endl;
		return std::nullopt;	
	}

	Stack output = Stack() . push ( Value::Instruction( val ) );
	if ( val == PRINT )
		return CompileArgumentsList( st, env, output );
	else
		return CompileArguments( st, env, output );
}

bool Compiler::AssertArgsCount( int num, const Stack & val )
{
	if ( ! num )
		return val.empty();
	else if ( val.empty() )
		return false;

	return AssertArgsCount( --num, val.pop() );
}

/****************************************************************************/

std::optional<Stack> Compiler::CompileSymbol ( const std::string & val, const Stack & st, const EnvMap & env )
{
	if ( env.count( val ) )
	{
		Stack output = Stack() . push ( env.at(val) ) . push( Value::Instruction(LD) );
		return output;
	}
 
	std::cerr << "Token '" << val << "' is not defined." << std::endl;
	return std::nullopt;
}

/****************************************************************************/

std::optional<Stack> Compiler::CompileIf ( const Stack & st, const EnvMap & env )
{
	if ( ! AssertArgsCount( 3, st ) )
	{
		std::cerr << "If has incorrect number of arguments." << std::endl;
		return std::nullopt;
	}

	std::optional<Stack> opt1 = CompileToken( st, env );
	std::optional<Stack> opt2 = CompileToken( st.pop(), env );
	std::optional<Stack> opt3 = CompileToken( st.pop().pop(), env );

	if ( ! ( opt1 && opt2 && opt3 ) )
		return std::nullopt;

	Stack joinStack = Stack() . push ( Value::Instruction( JOIN ) );

	Stack arg1 = *opt1;
	Stack arg2 = joinStack . load ( *opt2 );
	Stack arg3 = joinStack . load ( *opt3 );

	return Stack()
		. push ( arg3.data() )
		. push ( arg2.data() )
		. push( Value::Instruction( SEL ) )
		. load ( arg1 );
}

/****************************************************************************/

std::optional<Stack> Compiler::CompileLet ( const Stack & st, const EnvMap & env )
{
	auto [ optCompiledBody, argsBodies, nextEnv ] = CompileLetMain( st, env );

	if ( ! optCompiledBody )
		return std::nullopt;

	Stack outAccumulator = Stack()
		.push( Value::Instruction( AP ) )
		.push( *optCompiledBody )
		.push( Value::Instruction( LDF ) );

	return CompileArgumentsList ( Stack ( argsBodies ), env, outAccumulator );
}

std::optional<Stack> Compiler::CompileLetrec ( const Stack & st, const EnvMap & env )
{
	auto [ optCompiledBody, argsBodies, nextEnv ] = CompileLetMain( st, env );

	if ( ! optCompiledBody )
		return std::nullopt;

	Stack outAccumulator = Stack()
		.push( Value::Instruction( RAP ) )
		.push( *optCompiledBody )
		.push( Value::Instruction( LDF ) );

	std::optional<Stack> out = CompileArgumentsList ( Stack ( argsBodies ), nextEnv, outAccumulator );
	if ( ! out )
		return std::nullopt;

	return out -> push( Value::Instruction( DUM ) );
}

std::tuple<std::optional<Value>, Value, Compiler::EnvMap> Compiler::CompileLetMain ( const Stack & st, const EnvMap & env )
{
	if ( ! AssertArgsCount( 2, st ) )
	{
		std::cerr << "Let has incorrect number of arguments." << std::endl;
		return { std::nullopt, Value::Null(), env };
	}

	// let definitions and let body
	Value args = st.top();
	Value body = st.pop().top();

	// definitions names and bodies
	auto [ optArgsBodies, argsNames ] = CompileLetArgs( Stack(args), Value::Null(), Value::Null() );

	if ( ! optArgsBodies )
		return { std::nullopt, Value::Null(), env };

	// next enviroment
	EnvMap nextEnv = EnviromentAddValues( ShiftEnviroment( env ), argsNames );

	// compile let body
	std::optional<Stack> compiledBody;
	if ( body.isCons() )
		compiledBody = CompileCall( Stack( body ), nextEnv );
	else
		compiledBody = CompileArguments ( Stack() . push( body ), nextEnv, Stack() );

	if ( ! compiledBody )
		return { std::nullopt, Value::Null(), env };

	return { compiledBody -> data(), *optArgsBodies, nextEnv };
}

std::pair<std::optional<Value>, Value> Compiler::CompileLetArgs ( const Stack & st, const Value & bodies, const Value & names )
{
	if ( st.empty() )
		return { bodies, names };

	Value next = st.top();

	if ( ! ( next.isCons()
		&& next.car().isSym()
		&& next.cdr().isCons()
		&& next.cdr().cdr().isNull() )
	)
	{
		std::cerr << "Let argument '" << next << "' is in invalid form." << std::endl;
		return { std::nullopt, names };
	}

	Value nextName = next.car();
	Value nextBody = next.cdr().car();

	return CompileLetArgs( st.pop(), Value::Cons( nextBody, bodies ), Value::Cons( nextName, names ) );
}

/****************************************************************************/


std::optional<Stack> Compiler::CompileQuote ( const Stack & st, const EnvMap & env )
{
	if ( st.empty() )
	{
		std::cerr << "Quote used without arguments." << std::endl;
		return std::nullopt;
	}

	if ( ! st.pop().empty() )
	{
		std::cerr << "Too many agruments in quote" << std::endl;
		return std::nullopt;
	}

	return Stack() . push( st.top() ) . push( Value::Instruction( LDC ) );
}

std::optional<Stack> Compiler::CompileQuasiquote ( const Stack & st, const EnvMap & env )
{
	if ( st.empty() )
	{
		std::cerr << "Quasiquote used without arguments." << std::endl;
		return std::nullopt;
	}

	if ( ! st.pop().empty() )
	{
		std::cerr << "Too many agruments in quasiquote" << std::endl;
		return std::nullopt;
	}

	return CompileQuasiquoteAssemble( st.top(), env );
}

std::optional<Stack> Compiler::CompileQuasiquoteAssemble( const Value & val, const EnvMap & env )
{
	if ( val.isNull() )
		return Stack() . push ( Value::Instruction( NIL ) );

	if ( ! val.isCons() )
		return Stack() . push( val ) . push ( Value::Instruction( LDC ) );

	if ( val.car().isSym() && val.car().sym() == "unquote" )
	{
		Value toUnquote = val.cdr().car();
		
		if ( toUnquote.isCons() )
			return CompileCall( Stack( toUnquote ), env );

		return CompileArguments ( Stack() . push(toUnquote), env, Stack() );
	}

	std::optional<Stack> car = CompileQuasiquoteAssemble( val.car(), env );
	std::optional<Stack> cdr = CompileQuasiquoteAssemble( val.cdr(), env );

	if ( ! ( car && cdr ) )
		return std::nullopt;

	return Stack() . push( Value::Instruction( CONS ) ) . load ( *car ) . load ( *cdr );
}

/****************************************************************************/

std::optional<Stack> Compiler::CompileLambda ( const Stack & st, const EnvMap & env )
{
	std::optional<Stack> opt = CompileBody( st, env );
	if ( ! opt )
		return std::nullopt;

	return Stack()
		. push( opt -> data() )
		. push( Value::Instruction( LDF ) );
}

// input (defun name (params) (body))
std::tuple<Value, Compiler::EnvMap> Compiler::CompileDefun ( const Value & val, const EnvMap & env  )
{
	if ( ! ( val.cdr().isCons() && val.cdr().car().isSym() ))
	{
		std::cerr << "Wrong structure of defun." << std::endl;
		return { Value::Null(), env };
	}

	std::string name = val.cdr().car().sym();

	// Add function to enviroment
	EnvMap nextEnv = env;

	int index = EnviromentNext( nextEnv );
	nextEnv.insert( { name, Value::Cons( Value::Integer(0), Value::Integer( index ) ) } );

	std::optional<Stack> body = CompileBody( Stack( val.cdr().cdr() ), nextEnv );
	if ( ! body )
		return { Value::Null(), env }; // return unchanged enviroment map

	return { Stack() . push( body -> data() ) . push( Value::Instruction( DEFUN ) ) . data(), nextEnv };
}

// input stack = (params), (body)
std::optional<Stack> Compiler::CompileBody ( const Stack & st, const EnvMap & env )
{
	// correct structure
	if ( ! AssertArgsCount( 2, st ) )
	{
		std::cerr << "Wrong structure of function body." << std::endl;
		return std::nullopt;
	}

	Value params = st.top();
	EnvMap nextEnv = EnviromentAddValues( ShiftEnviroment( env ), params );

	Value body = st.pop().top();

	std::optional<Stack> compiledBodyOpt;
	if ( body.isCons() )
		compiledBodyOpt = CompileCall ( Stack( body ), nextEnv );
	else
		compiledBodyOpt = CompileArguments ( Stack().push( body ), nextEnv, Stack() );

	if ( ! compiledBodyOpt )
		return std::nullopt;

	return Stack() . push ( Value::Instruction( RTN ) ) . load( *compiledBodyOpt );
}
