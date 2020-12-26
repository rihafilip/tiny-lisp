#pragma once

#include "value.h"

namespace lisp
{
	/**
	 * Imutable abstraction above Value list
	 * FIFO structure
	 */
	class Stack
	{
	public:
		/// Empty stack constructor
		Stack ();
		/// Creates non-empty stack
		Stack ( const Value & val );
		Stack & operator= ( const Stack & other );

		/// Returns stack with added value
		Stack push ( Value in ) const;
		/// Returns top value in stack
		Value top () const;
		/// Returns stack without top value
		Stack pop () const;

		/// Returns true if stack is empty
		bool empty () const;

		/// Returns stack with input on top of *this
		Stack load ( const Stack & input ) const;

		/// Returns underlying cons cells
		Value data () const;

		friend std::ostream & operator<< ( std::ostream & os, const Stack & st );

	private:
		/// Raw list data
		Value _data;
	};
}