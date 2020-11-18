#include "lexer.h"
#include "tokens.h"

#include <cctype>

using namespace lisp;

Value Lexer::Scan( const std::string & str )
{
	// Vector is used as a stack, so string is loaded to it back to front
	std::vector<char> stringVector ( str.rbegin(), str.rend() );

	Lexer lexer ( stringVector, tokens::makeList() );
	return lexer . begin();
}

/*******************************************************/

Lexer::Lexer( std::vector<char> vec, Value built_in )
: m_Stack (vec)
, m_BuiltIn (built_in)
{}

/*******************************************************/

Value Lexer::begin ()
{
	skipSpace();
	return evaluate ( nextToken(), Value::Null() );
}

Value Lexer::evaluate ( std::optional<Value> optionalValue, Value acc )
{
	if ( ! optionalValue )
	{
		if ( m_Stack . empty() )
			return acc;
		else
		{
			std::cerr << "Wrong input.";
			return Value::Null();
		}
	}

	Value val = optionalValue . value();

	skipSpace();
	return evaluate ( nextToken(), acc . append( val ) );
}

/*******************************************************/

// input 	:= [whitespace] [ token | comment ]*
// token 	:= number | string | symbol
// number 	:= -?[0-9]+
// string	:= [a-Z][a-Z|0-9]*
// symbol 	:= in "built-in" list
// comment	:= ; [^\n]* \n
std::optional<Value> Lexer::nextToken()
{
	if ( m_Stack . empty() )
		return std::nullopt;

	char ch = pop();
	std::string str;
	str += ch;
	
	// word
	if ( isalpha( ch ) )
		return Value::Symbol( restOfToken ( isalnum, str ));

	// number
	else if (
		isdigit( ch )
		|| ( ch == '-' && ! m_Stack . empty() && isdigit( m_Stack . back() ) )
	)
		return Value::Integer( std::stoi ( restOfToken ( isdigit, str )));

	// comment
	// ignores ouptut until \n
	else if ( ch == ';' )
	{
		restOfToken(
			[] ( int in ) -> int
			{ return in != '\n'; },
			str
		);
		return nextToken();
	}
	
	// otherwise built in symbol
	// TODO signalize bad input
	return isBuiltIn( str, m_BuiltIn );
}

std::string Lexer::restOfToken( std::function<int(int)> comp, std::string acc )
{
	// empty stack or not a symbol of this token
	if ( m_Stack . empty()
		|| ! comp( m_Stack . back() )
	)
		return acc;

	return restOfToken( comp, acc += pop() );
}

/*******************************************************/

std::optional<Value> Lexer::isBuiltIn ( std::string str, Value lst )
{
	if ( lst . isNull() )
		return std::nullopt;

	if ( lst . car() -> sym() . value() == str )
		return lst . car() . value();

	return isBuiltIn ( str, lst . cdr() . value() );
}

void Lexer::skipSpace()
{
	restOfToken( isspace, std::string() );
}

char Lexer::pop()
{
	char ch = m_Stack . back();
	m_Stack . pop_back();
	return ch;
}