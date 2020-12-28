#include "parser.h"

using namespace lisp;

/*
INPUT	:= { ITEM }

ITEM 	:= '(' LIST 
		:= { '`' | ''' | ','} ITEM
		:= ( Symbol | Number )

LIST 	:= ITEM LIST
		:= ')'
		:= '.' ITEM ')'
*/

Value Parser::Parse ( const Value & tokens )
{
	Parser p ( tokens );
	return p.Process().value_or( Value::Null() );
}

Parser::Parser( const Value & vinput  )
: input( vinput )
{
	syntaxSugar = 
	{
		{ "'", "quote" },
		{ "`", "quasiquote" },
		{ ",", "unquote" }
	};
}

std::optional<Value> Parser::Process ()
{
	if ( input.empty() )
		return Value::Null();

	std::optional<Value> car = Item();
	std::optional<Value> cdr = Process();

	if ( car && cdr )
		return Value::Cons( *car, *cdr );
	else
		return std::nullopt;
}

std::optional<Value> Parser::Item ()
{
	// unexpected EOF
	if ( input.empty() )
		return error("Unexpected EOF.");

	Value next = input.top();

	if ( next.isSym() )
	{
		std::string str = next.sym();
		input = input.pop();

		if ( str == "(" )
			return List();

		if ( syntaxSugar.count( str ) )
		{
			Value car = Value::Symbol( syntaxSugar[str] );
			std::optional<Value> cdr = Item();

			if ( cdr )
				return Value::Cons( car, Value::Cons ( *cdr, Value::Null() ) );
			else
				return std::nullopt;
		}

		// plain symbol
		return next;
	}

	// plain number
	else if ( next.isNum() )
	{
		input = input.pop();
		return next;
	}

	// Invalid type of input, shouldn't occur
	return error( "Invalid token in parser." );
}

std::optional<Value> Parser::List ()
{
	// ERROR, should at least be ')'
	if ( input.empty() )
		return error ( "Unexpected EOF." );

	Value top = input.top();
	if ( top.isSym() )
	{
		// closing bracket
		if ( top.sym() == ")" )
		{
			input = input.pop();
			return Value::Null();
		}

		// ... ". ITEM )"
		else if ( top.sym() == "." )
		{
			input = input.pop();
			std::optional<Value> next = Item();
			if ( ! next )
				return std::nullopt;

			top = input.top();
			if ( ! top.isSym() || top.sym() != ")" )
				return error( "Expected ')' after cons." );

			input = input . pop();

			return *next;
		}
	}

	std::optional<Value> car = Item();
	std::optional<Value> cdr = List();
	if ( car && cdr )
		return Value::Cons( *car, *cdr );
	else
		return std::nullopt;

}

std::optional<Value> Parser::error ( const std::string & message )
{
	std::cerr << message << std::endl;
	input = Stack( Value::Null() );
	return std::nullopt;
}
