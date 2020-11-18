#include "lexer.h"
#include "tokens.h"

#include <cctype>

using namespace lisp;

Value Lexer::Scan( const std::string & str )
{
	// Vector is used as a stack, so string is loaded to it back to front
	vStack stringVector ( str.rbegin(), str.rend() );

	Lexer lexer ( tokens::makeList() );
	return lexer . begin ( stringVector );
}

/*******************************************************/

Lexer::Lexer( Value builtin )
: m_Tokens ( builtin )
{}

/*******************************************************/

Value Lexer::begin ( vStack stack )
{
	return evaluate ( nextToken( skipSpace(stack) ), Value::Null() );
}

Value Lexer::evaluate ( std::pair<std::optional<Value>, vStack> pair , Value acc )
{
	auto [ optional, stack ] = pair;

	if ( ! optional )
		// reached eof?
		return stack . empty() ? acc : Value::Null();

	Value val = optional . value();

	return evaluate (
		nextToken( skipSpace(stack) ),
		acc . append( val ) );
}

/*******************************************************/

// input 	:= [whitespace] [ token | comment ]*
// token 	:= number | string | symbol
// number 	:= -?[0-9]+
// string	:= [a-Z] [ a-Z | 0-9 ]*
// symbol 	:= in "built-in" list
// comment	:= ; [^\n]* \n
std::pair<std::optional<Value>, vStack> Lexer::nextToken( vStack stack )
{
	if ( stack . empty() )
		return std::make_pair(std::nullopt, stack);

	char ch = stack . back();
	stack . pop_back();

	std::string str;
	str += ch;
		
	// word
	if ( isalpha( ch ) )
		return makeSymbol( restOfToken ( stack, isalnum, str ));

	// number
	else if (
		isdigit( ch )
		||  ( ch == '-' 					// negative number
			&& ! stack . empty() 			// can extract
			&& isdigit( stack . back() ) ) 	// after minus is digit
	)
		return makeNumber( restOfToken ( stack, isdigit, str ));

	// comment
	// ignores output until \n
	else if ( ch == ';' )
	{
		stack = restOfToken(
			stack,
			[] ( int in ) -> int { return in != '\n'; },
			str
		) . second;

		// skipSpace consumes the trailing \n
		return nextToken( skipSpace(stack) );
	}
	
	// otherwise built in symbol
	std::optional<Value> ret = isToken( str );

	// if not built-in symbol, it fails
	if ( ! ret )
	{
		std::cerr << "\"" << str << "\" is not a token." << std::endl;
		return std::make_pair( std::nullopt, vStack {'f'} ); // dummy stack, is not empty
	}

	return std::make_pair( ret, stack );
}

std::pair<std::string, vStack> Lexer::restOfToken( vStack stack, std::function<int(int)> comp, std::string acc )
{
	// empty stack or not a symbol of this token
	if ( stack . empty()
		|| ! comp( stack . back() )
	)
		return std::make_pair(acc, stack);

	char ch = stack . back();
	stack . pop_back();
	return restOfToken( stack, comp, acc += ch );
}

/*******************************************************/

std::pair<std::optional<Value>, vStack> Lexer::makeNumber ( std::pair<std::string, vStack> pair )
{
	auto [str, stack] = pair;
	Value val = Value::Integer( std::stoi ( str ) );
	return std::make_pair(val, stack);
}

std::pair<std::optional<Value>, vStack> Lexer::makeSymbol ( std::pair<std::string, vStack> pair )
{
	auto [str, stack] = pair;
	Value val = Value::Symbol( str );
	return std::make_pair(val, stack);
}

vStack Lexer::skipSpace( vStack stack )
{
	return restOfToken( stack, isspace, std::string() ) . second;
}

/*******************************************************/

std::optional<Value> Lexer::isToken ( std::string str )
{
	return isToken( str, m_Tokens );
}

std::optional<Value> Lexer::isToken ( std::string str, Value lst )
{
	if ( lst . isNull() )
		return std::nullopt;

	if ( lst . car() -> sym() . value() == str )
		return lst . car() . value();

	return isToken ( str, lst . cdr() . value() );
}
