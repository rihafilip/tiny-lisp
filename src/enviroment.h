#pragma once

#include "value.h"

namespace lisp
{
	/**
	 * Immutable structure that organizes enviroment of SECD machine
	 */
	class Enviroment
	{
	public:
		/// Default constructor
		Enviroment();
		/// Constructor with val lying underneath
		Enviroment ( const Value & val );
			
		/**
		 * Returns value on given index
		 * @param Value cons cell in format ( depth . index ) 
		 * @return Value on given index or empty optional input is invalid
		 */
		std::optional<Value> onIndex ( const Value & on ) const;

		/// Return new enviroment with added input
		Enviroment add ( const Value & val ) const;

		/// Return new enviroment, where zero depth is input
		Enviroment setZeroDepth ( const Value & val ) const;
		/// Returns new enviroment, where new zero depth is pushed
		Enviroment shifted () const;

		/// Returns raw underlying Value
		Value data () const;

		friend std::ostream & operator<< ( std::ostream & os, const Enviroment & env );

	private:
		/// onIndex subproccess
		std::optional<Value> onIndex ( int depth, int index, const Value & curr ) const;
		/// onIndex subproccess
		std::optional<Value> onIndexDeep ( int index, const Value & curr ) const;

		static void print ( std::ostream & os, int depth, const Value & val );
		/**
		 * Lists of depths
		 * Each depth is list of actual values
		 */
		Value m_Map;
	};
}