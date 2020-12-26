#pragma once

#include "gc.h"
#include <optional>

namespace lisp
{
	/**
	 * Abstraction above raw memory data
	 * Represents integer, cons cell, null list, string or secd Instruction
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
		static Value Cons ( const Value & car, const Value & cdr );
		static Value Instruction ( secd::Instruction instruct );
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
		std::optional<secd::Instruction> 	ins () const; 
		/// @}
		
		/**
		 * \defgroup Val_bool Value to boolean converters
		 * @{
		 */
		bool isNull () const;
		bool isCons () const;
		bool isNum () const;
		bool isSym () const;
		bool isIns () const;
		/// @}

		/// If this is cons, it returns this with appended val
		Value append ( const Value & val ) const;

		friend std::ostream & operator<< (std::ostream & os, const Value & val );

	private:
		/// Asigns itself as root in GC
		Value( GC::Memory * in );

		/// Helper error function, prints to stderr
		void error ( GC::MemoryType expected ) const;

		void print ( std::ostream & os ) const;

		/// Raw memory block
		GC::Memory * memory;
	};
}