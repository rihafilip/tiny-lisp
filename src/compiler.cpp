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

		enviroment.insert( { val.car().sym(), Value::Cons( Value::Integer(0), Value::Integer(counter)) });
		return EnviromentAddValues( enviroment, val.cdr(), ++counter );
	}

	int EnviromentNext ( Compiler::EnvMap env )
	{
		int max = 0;
		if ( ! env.empty() )
		{
			max = std::max_element ( env.begin(), env.end(),
				[] ( auto a, auto b )
					{ return a.second.car().num() < b.second.car().num(); }
			) -> second.cdr().car().num();

			max++;
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

	Value ReverseList ( const Value & val )
	{
		if ( val.isNull() )
			return Value::Null();

		if ( ! val.isCons() )
			return val;

		if ( val.cdr().isNull() )
			return val;

		return ReverseList( val.cdr() ) . append( val.car() );
	}

} // anonymous namespace

/****************************************************************************/

Value Compiler::Begin ( const Value & val )
{
	return CompileCode( val, EnvMap(), FunSet() );
}

/// whole input code
Value Compiler::CompileCode ( const Value & val, const EnvMap & env, const FunSet & funcs )
{
	if ( val.isNull() )
		return Value::Null();

	Value next = val.car();

	if ( next.isCons() && next.car().isSym() )
	{
		std::string str = next.car().sym();
		// (defun ...)
		if ( str == "defun" )
		{
			auto [out, outEnv, outFuncs] = CompileDefun( next, env, funcs );
			return Value::Cons( out, CompileCode( val.cdr(), outEnv, outFuncs ) );
		}

		else if ( str == "lambda" )
		{
			std::cerr << "Lambda definition without application." << std::endl;
			return CompileCode( val.cdr(), env, funcs );
		}

	}
	
	std::optional<Stack> compiled = CompileSource ( Stack (next), env, funcs, Stack() );

	// failed to compile, ignore
	if ( ! compiled )
		return CompileCode( val.cdr(), env, funcs );
	return Value::Cons( compiled -> data(), CompileCode( val.cdr(), env, funcs ) );
}

/****************************************************************************/

/// one single code fragment
std::optional<Stack> Compiler::CompileSource ( const Stack & st, const EnvMap & env, const FunSet & funcs, const Stack & acc, bool isArgs )
{
	if ( st.empty() )
	{
		if ( isArgs  )
			return acc . push ( Value::Instruction ( NIL ) );
		return acc;
	}

	auto [compiled, outStack] = CompileToken( st, env, funcs );
	if ( ! compiled )
		return std::nullopt;

	Stack nextAcc;
	if ( isArgs )
		nextAcc = acc . push( Value::Instruction( CONS ) ) . load ( * compiled );
	else
		nextAcc = acc.load ( *compiled );

	return CompileSource ( outStack, env, funcs, nextAcc, isArgs );
}

Compiler::CompilePair Compiler::CompileArgsSource ( const Stack & st, const EnvMap & env, const FunSet & funcs, const Stack & acc )
{
	return { CompileSource( st, env, funcs, acc, true ), Stack() };
}

Compiler::CompilePair Compiler::CompileToken( Stack st, const EnvMap & env, const FunSet & funcs )
{
	if ( st . empty() )
	{
		std::cerr << "Unexpected EOF." << std::endl;
		return { std::nullopt, st };
	}

	Value val = st.top();
	st = st.pop();

	if ( val.isIns() )
		return { Stack() . push( val ), st };

	// Number is loaded via LDC
	if ( val.isNum() )
		return { Stack() . push ( val ) . push( Value::Instruction( LDC ) ), st };

	// Cons cell which means sub-list or lambda 
	if ( val.isCons() )
		return CompileCons( val, st, env, funcs );

	// is Symbol
	std::string str = val.sym();

	// is built-in symbol
	if ( tokens::isSymbol( str ) )
		return CompileBuiltIn( str, st, env, funcs );

	return CompileSymbol( str, st, env, funcs );
}

/****************************************************************************/

Compiler::CompilePair Compiler::CompileCons ( const Value & val, const Stack & st, const EnvMap & env, const FunSet & funcs )
{
	if ( val.car().isSym() && val.car().sym() == "lambda" )
		return CompileLambda( val, st, env, funcs );

	return { CompileSource ( Stack( val ), env, funcs, Stack() ), st };
}

Compiler::CompilePair Compiler::CompileBuiltIn ( const std::string & val, const Stack & st, const EnvMap & env, const FunSet & funcs  )
{
	// simple translation
	std::optional<Instruction> opt = tokens::translate( val );
	if ( opt )
		return { Stack() . push ( Value::Instruction( *opt ) ), st };

	if ( val == "if" )
		return CompileIf( st, env, funcs );

	//TODO
	// if ( val == "quote" )
	// 	return {std::nullopt, st};

	// if ( val == "quasiquote" )
	// 	return {std::nullopt, st};

	if ( val == "unquote" )
	{
		std::cerr << "Unquote not in quasiquote." << std::endl;
		return {std::nullopt, st};
	}

	if ( val == "defun" )
	{
		std::cerr << "Defun can be only used in a global scope." << std::endl;
		return {std::nullopt, st};
	}

	// shouldn't occur
	std::cerr << "Incorrect usage of symbol '" << val << "'." << std::endl;
	return { std::nullopt, st };
}


Compiler::CompilePair Compiler::CompileSymbol ( const std::string & val, const Stack & st, const EnvMap & env, const FunSet & funcs )
{
	if ( env.count( val ) )
	{
		Stack output = Stack() . push ( env.at(val) ) . push( Value::Instruction(LD) );
		if ( funcs.count( val ) )
		{
			output = Stack() . push( Value::Instruction( AP ) ) . load ( output );
			return CompileArgsSource( st, env, funcs, output );
		}

		return { output, st };
	}
 
	std::cerr << "Token '" << val << "' is not defined." << std::endl;
	return { std::nullopt, st };
}

/****************************************************************************/

Compiler::CompilePair Compiler::CompileIf ( const Stack & st, const EnvMap & env, const FunSet & funcs )
{
	auto [opt1, st1] = CompileToken( st, env, funcs );
	auto [opt2, st2] = CompileToken( st1, env, funcs );
	auto [opt3, st3] = CompileToken( st2, env, funcs );

	if ( ! ( opt1 && opt2 && opt3 ) )
	{
		std::cerr << "If expected three arguments, got " 
		<< opt1.has_value() + opt2.has_value() + opt3.has_value()
		<< "." << std::endl;

		return { std::nullopt, st };
	}

	Stack joinStack = Stack() . push ( Value::Instruction( JOIN ) );

	Stack arg1 = *opt1;
	Stack arg2 = joinStack . load ( *opt2 );
	Stack arg3 = joinStack . load ( *opt3 );

	Stack output = Stack()
		. push ( arg3.data() )
		. push ( arg2.data() )
		. push( Value::Instruction( SEL ) )
		. load ( arg1 );

	return { output, st3 };
}

/****************************************************************************/

Compiler::CompilePair Compiler::CompileLambda ( const Value & val, const Stack & st, const EnvMap & env, const FunSet & funcs )
{
	std::optional<Stack> opt = CompileBody( val, env, funcs );
	if ( ! opt )
		return { std::nullopt, st };

	Stack out = Stack()
	 	. push( Value::Instruction( AP ) )
		. push( opt -> data() )
		. push( Value::Instruction( LDF ) );

	return CompileArgsSource( st, env, funcs, out );
}

// input (defun name (params) (body))
std::tuple<Value, Compiler::EnvMap, Compiler::FunSet> Compiler::CompileDefun ( const Value & val, EnvMap env, FunSet funcs )
{
	if ( ! ( val.cdr().isCons() && val.cdr().car().isSym() ))
	{
		std::cerr << "Wrong structure of defun." << std::endl;
		return { Value::Null(), env, funcs };
	}

	std::string name = val.cdr().car().sym();

	std::optional<Stack> body = CompileBody( val.cdr(), env, funcs );
	if ( ! body )
		return { Value::Null(), env, funcs };

	// Add function to enviroment
	int index = EnviromentNext( env );
	env.insert( { name, Value::Cons( Value::Integer(0), Value::Integer( index ) ) } );
	funcs.insert ( name );

	return { Stack() . push( body -> data() ) . push( Value::Instruction( DEFUN ) ) . data(), env, funcs };
}

// input (symbol (params) (body))
std::optional<Stack> Compiler::CompileBody ( const Value & val, const EnvMap & env, const FunSet & funcs )
{
	// correct structure
	if ( !(	val.cdr().isCons() // ( (params) . ( (body) . nil ) )
		&& 	val.cdr().cdr().isCons() // (body . nil)
		&& 	val.cdr().cdr().cdr().isNull() // nil
	))
	{
		std::cerr << "Wrong structure of function body." << std::endl;
		return std::nullopt;
	}

	Value params = val.cdr().car();
	EnvMap nextEnv = EnviromentAddValues( ShiftEnviroment( env ), params );
	std::set<std::string> nextFuncs = FunctionsRemoveSymbols( funcs, params );

	Value body = val.cdr().cdr().car();

	std::optional<Stack> compiledBodyOpt = CompileSource ( Stack( body ), nextEnv, nextFuncs, Stack() );

	if ( ! compiledBodyOpt )
		return std::nullopt;

	return Stack() . push ( Value::Instruction( RTN ) ) . load( *compiledBodyOpt );
}
