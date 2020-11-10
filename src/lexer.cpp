#include "lexer.h"

#include <vector>
#include <cctype>

using namespace lisp;

Value Lexer::Scan( const std::string & str )
{
	std::vector<const char *> vec = 
	{
		"(",
		")",
		";",
		"`",
		",",
		".",
		"+",
		"-",
		"*",
		"/",
		"eq",
    	"<",
    	">",
    	"print",
    	"read",
    	"if",
    	"lambda",
    	"quote",
    	"apply",
    	"cons",
    	"car",
    	"cdr",
    	"consp",
    	"defun",
    	"let",
    	"t",
    	"'",
    	""
	};

	// transforms vector into list
	std::function<Value()> makeList = 
		[ &vec, &makeList ] () mutable -> Value
		{
			if ( vec . empty() )
				return Value::Null();

			Value nextVal = Value::Symbol( vec . back() );
			vec . pop_back();
			return Value::Cons( nextVal, makeList() );
		};

	std::stringstream ss (str);
	return Scan ( ss, Value::Null(), makeList(), Value::Null() );
}

/*******************************************************/

Value Lexer::Find ( const std::string & str, Value lst )
{
	if ( lst . isNull() )
		return Value::Null();

	if ( str == lst. car(). sym() )
		return lst . car();

	return Find( str, lst.cdr() );
}

/*******************************************************/

Value Lexer::Scan ( std::stringstream & ss, Value expression, Value built_in, Value tokens )
{
	if ( skipSpace(ss) . eof() )
		return Value::Cons( expression, Value::Cons( built_in, tokens ) );

	std::string str = getNext( ss );

	Value found = Value::Null();

	// number
	if ( isdigit ( str . front() ) )
		found = Value::Integer( std::stoi(str) );

	// built-in symbol	
	else if ( ! ( found = Find ( str, built_in )) . isNull() )
		;

	// custom symbol
	else if ( ( found = Find ( str, tokens ) ) . isNull() )
	{
		found = Value::Symbol( str );
		tokens = Value::Cons ( found, tokens );
	}

	return Scan( ss, expression . append(found), built_in, tokens );
}

/*******************************************************/

std::string Lexer::getNext ( std::stringstream & ss )
{

	char ch = skipSpace( ss ) . get();
	std::string str;
	str += ch;
	
	// letter => word
	if ( isalpha( ch ) )
		return getWord( ss, str, isalpha );

	// digit or minus => number
	else if ( isdigit( ch ) )
		return getWord( ss, str, isdigit );
	
	return str;
}

std::string Lexer::getWord ( std::stringstream & ss, std::string acc, std::function<int(int)> comp )
{
	if ( ss.eof() || ! comp( ss.peek() ) )
		return acc;

	char ch = ss.get();
	return getWord( ss, acc += ch, comp );
}

std::stringstream & Lexer::skipSpace( std::stringstream & ss )
{
	getWord( ss, std::string(), isspace );
	return ss;
}