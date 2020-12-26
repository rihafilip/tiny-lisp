#pragma once

#include "gc.h"

namespace lisp
{
	/**
	 * Abstraction above raw memory data
	 * Represents integer, cons cell, null list, string or sedc Instruction
	 */
	class Value
	{
	public:
		/**
		 * \defgroup Val_cons Value constructors
		 * @{
		 */		
		static Value Integer ( int num );
		static Value Symbol ( const std::string & in);
		static Value Cons ( Value car, Value cdr );
		static Value Instruction ( sedc::Instruction instruct );
		static Value Null ();
		///@}
		/// Removes itself from roots in GC
		~Value();

		/**
		 * \defgroup Val_get Value getters
		 * @{
		 */
		Value car ();
		Value cdr ();
		int num ();
		std::string sym ();
		sedc::Instruction ins ();
		/// @}
		
		/// Asks if this value is null
		bool isNull ();
		/// Asks if this value is cons
		bool isCons ();

		/// If this is cons, it returns this with appended val
		Value append (Value & val);

	private:
		/// Asigns itself as root in GC
		Value( GC::Memory * in );

		/// helper error function
		void error ( GC::MemoryType expected );
		GC::Memory * memory;
	};
}