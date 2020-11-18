#pragma once

#include "gc.h"
#include <optional>

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
		 * \defgroup Val_ass Value assignement operators
		 * @{
		 */		
		Value (const Value & src); 
		Value & operator= (const Value & src); 
		/// @}
		
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
		std::optional<Value> 				car () const; 
		std::optional<Value> 				cdr () const; 
		std::optional<int> 					num () const; 
		std::optional<std::string> 			sym () const; 
		std::optional<sedc::Instruction> 	ins () const; 
		/// @}
		
		/// Asks if this value is null
		bool isNull () const; 
		/// Asks if this value is cons
		bool isCons () const; 

		/// If this is cons, it returns this with appended val
		Value append ( Value val );

		friend std::ostream & operator<< (std::ostream & os, const Value & val );

	private:
		/// Asigns itself as root in GC
		Value( GC::Memory * in );

		/// Helper error function, prints to stderr
		void error ( GC::MemoryType expected ) const;

		void print ( std::ostream & os, bool inList = false ) const;

		/// Raw memory block
		GC::Memory * memory;
	};
}