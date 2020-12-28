#pragma once

#include "value.h"

namespace lisp
{
	class Enviroment
	{
	public:
		Enviroment();
		Enviroment ( const Value & val );
		
		std::optional<Value> onIndex ( const Value & on ) const;

		Enviroment add ( const Value & val ) const;

		Enviroment shifted () const;

		Value data () const;

	private:
		std::optional<Value> onIndex ( int depth, int index, const Value & curr ) const;
		std::optional<Value> onIndexDeep ( int index, const Value & curr ) const;
		/**
		 * Lists of depths
		 * Each depth is list of actual values
		 */
		Value m_Map;
	};
}