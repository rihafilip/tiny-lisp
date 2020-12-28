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
	if ( empty() )
		return Value::Null();

	// Non-nil ending list
	if ( ! _data.isCons() )
		return _data;

	return _data.car();
}

Stack Stack::pop () const
{
	if ( empty() )
		return *this;

	// Non-nil ending list
	if ( ! _data.isCons() )
		return Stack();

	return Stack ( _data . cdr() );
}

bool Stack::empty() const
{
	return _data . isNull();
}

bool Stack::single() const
{
	return ! ( _data.isCons() || _data.isNull() );
}

Stack Stack::load ( const Stack & input ) const
{
	return Stack( input . _data . append( this -> _data ) );
}

Value Stack::data () const
{
	return _data;
}

namespace lisp
{
	std::ostream & operator<< ( std::ostream & os, const Stack & st )
	{
		if ( st.empty() )
			return os << std::endl;

		return os << st . top() << " " << st . pop();
	}
}