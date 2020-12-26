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
	return p.Process();
}

Parser::Parser( const Value & vinput  )
: input( vinput )
{
	syntaxSugar = 
	{
		{ "'", "quote" },
		{ "`", "quasiquote" },
		{ ",", "quote" }
	};
}

Value Parser::Process ()
{
	if ( input.empty() )
		return Value::Null();

	Value next = Item();
	return Value::Cons( next, Process() );
}

Value Parser::Item ()
{
	// unexpected EOF
	if ( input.empty() )
	{
		std::cerr << "Unexpected EOF." << std::endl;
		return Value::Null();
	}

	Value next = input.top();

	if ( next.isSym() )
	{
		std::string str = next.sym().value();
		input = input.pop();

		if ( str == "(" )
			return List();

		if ( syntaxSugar.count( str ) )
		{
			Value car = Value::Symbol( syntaxSugar[str] );
			return Value::Cons( car, Item() );
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
	std::cerr << "Invalid item in parser." << std::endl;
	return Value::Null();
}

Value Parser::List ()
{
	// ERROR, should at least be ')'
	if ( input.empty() )
	{
		std::cerr << "Unexpected EOF." << std::endl;
		return Value::Null();
	}

	Value top = input.top();
	if ( top.isSym() )
	{
		// closing bracket
		if ( top.sym().value() == ")" )
		{
			input = input.pop();
			return Value::Null();
		}

		// ... ". ITEM )"
		else if ( top.sym().value() == "." )
		{
			input = input.pop();
			Value next = Item();

			top = input.top();
			if ( ! top.isSym() || top.sym().value() != ")" )
			{
				std::cerr << "Expected ')' after cons." << std::endl;
				// TODO ERROR
				return Value::Null();
			}
		}
	}

	Value next = Item();
	return Value::Cons( next, List() );
}

