#pragma once

#include "stack.h"
#include "instruction.h"
#include "environment.h"


namespace lisp::secd
{
	using namespace lisp;
	/**
	 * Virtual SECD machine that executes SECD instructions
	 */
	class Runtime
	{
	public:
		/// Execute given code with given environment
		static Environment executeCode ( const Stack & code, const Environment & env = Environment() );

	private:
		/// Helper structure, containing all registers of SECD machine
		struct Registers
		{
			Registers( const Stack & s, const Environment & e, const Stack & c, const Stack & d )
			: _s ( s )
			, _e ( e )
			, _c ( c )
			, _d ( d )
			{}

			Stack _s;
			Environment _e;
			Stack _c;
			Stack _d;
		};

		/// Executes single chunk of code
		static std::optional<Environment> executeSource ( const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );
		/// Executes single instruction
		static std::optional<Registers> executeInstruction ( Instruction ins, const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );

		/**
		 * \defgroup Runtime_help Runtime helper subroutines
		 * @{
		 */
		static std::optional<Registers> binaryOperator ( Instruction ins, const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );
		static std::optional<Registers> equals ( const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );

		static std::optional<Registers> consAccess ( Instruction ins, const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );

		static std::optional<Registers> defun ( const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );

		static std::optional<Registers> select ( const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );
		static std::optional<Registers> apply ( const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );
		static std::optional<Registers> recursiveApply ( const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );
		static std::optional<Registers> returns ( const Stack & _s, const Environment & _e, const Stack & _c, const Stack & _d );

		static void print ( const Stack & s, bool first = true );
		static Value read ();
		/// @}
	};
}