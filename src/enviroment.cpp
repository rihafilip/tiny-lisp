#include "enviroment.h"
using namespace lisp;

Enviroment::Enviroment()
: m_Map ( Value::Cons( Value::Null(), Value::Null() ) )
{}

Enviroment::Enviroment ( const Value & val )
: m_Map ( val )
{}

std::optional<Value> Enviroment::onIndex ( const Value & on ) const
{
	return onIndex( on.car().num(), on.cdr().num(), m_Map );
}

std::optional<Value> Enviroment::onIndex ( int depth, int index, const Value & curr ) const
{
	if ( curr.isNull() )
		return std::nullopt;

	if ( depth == 0 )
		return onIndexDeep ( index, curr.car() );
	
	return onIndex( --depth, index, curr.cdr() );
}

std::optional<Value> Enviroment::onIndexDeep ( int index, const Value & curr ) const
{
	if ( curr.isNull() )
		return std::nullopt;

	if ( index == 0 )
		return curr.car();

	return onIndexDeep( --index, curr.cdr() );
}

/********************************************************************/

Enviroment Enviroment::add ( const Value & val ) const
{
	return Enviroment ( m_Map.car().append ( val ) );
}

Enviroment Enviroment::shifted () const
{
	// prepend empty depth
	return Enviroment( Value::Cons( Value::Null(), m_Map ) );
}

Value Enviroment::data () const
{
	return m_Map;
}