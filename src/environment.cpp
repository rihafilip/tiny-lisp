#include "environment.h"
using namespace lisp;

Environment::Environment()
: m_Map ( Value::Cons( Value::Null(), Value::Null() ) )
{}

Environment::Environment ( const Value & val )
: m_Map ( val )
{}

std::optional<Value> Environment::onIndex ( const Value & on ) const
{
	return onIndex( on.car().num(), on.cdr().num(), m_Map );
}

std::optional<Value> Environment::onIndex ( int depth, int index, const Value & curr ) const
{
	if ( curr.isNull() )
		return std::nullopt;

	if ( depth == 0 )
		return onIndexDeep ( index, curr.car() );
	
	return onIndex( --depth, index, curr.cdr() );
}

std::optional<Value> Environment::onIndexDeep ( int index, const Value & curr ) const
{
	if ( curr.isNull() )
		return std::nullopt;

	if ( index == 0 )
		return curr.car();

	return onIndexDeep( --index, curr.cdr() );
}

/********************************************************************/

Environment Environment::add ( const Value & val ) const
{
	// if ( m_Map.car().isNull() && ! val.isNull() )
	// 	return Environment( Value::Cons( Value::Cons( val, Value::Null() ), m_Map.cdr() ) );

	return Environment ( Value::Cons ( m_Map.car().append ( Value::Cons( val, Value::Null() ) ), m_Map.cdr() ) );
}

Environment Environment::setZeroDepth ( const Value & val ) const
{
	return Environment( Value::Cons( val, m_Map.cdr() ) );
}

/********************************************************************/

Environment Environment::shifted () const
{
	// prepend empty depth
	return Environment( Value::Cons( Value::Null(), m_Map ) );
}

Value Environment::data () const
{
	return m_Map;
}

namespace lisp
{
	std::ostream & operator<< ( std::ostream & os, const Environment & env )
	{
		Environment::print( os, 0, env.m_Map );
		return os;
	}
}

void Environment::print ( std::ostream & os, int depth, const Value & val )
{
	if ( val.isNull() )
		return;

	os << depth << ": " << val.car();
	os << std::endl;
	return print( os, ++depth, val.cdr() );
}