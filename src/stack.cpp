#include "stack.h"

using namespace lisp;

Stack::Stack ()
: _data ( Value::Null() )
{}

Stack::Stack ( const Value & val )
: _data ( val )
{}

Stack & Stack::operator= ( const Stack & other )
{
	this -> _data = other . _data;
	return *this;
}

Stack Stack::push ( Value in ) const
{
	return Stack ( Value::Cons( in, _data ) );
}

Value Stack::top () const
{
	return _data.car().value();
}

Stack Stack::pop () const
{
	if ( _data . isNull() )
		return *this;

	return Stack ( _data . cdr() . value() );
}

bool Stack::empty() const
{
	return _data . isNull();
}

Stack Stack::load ( const Stack & top ) const
{
	return Stack ( top . _data . append ( this -> _data ) );
}