#pragma once

#include "stack.h"
#include "instruction.h"
#include "enviroment.h"


namespace lisp::secd
{
	using namespace lisp;
	class Runtime
	{
	public:	
		static Enviroment executeCode ( const Stack & code, const Enviroment & env = Enviroment() );

	private:
		struct Registers
		{
			Registers( const Stack & s, const Enviroment & e, const Stack & c, const Stack & d )
			: _s ( s )
			, _e ( e )
			, _c ( c )
			, _d ( d )
			{}

			Stack _s;
			Enviroment _e;
			Stack _c;
			Stack _d;
		};

		static std::optional<Enviroment> executeSource ( const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d );
		static std::optional<Registers> executeInstruction ( Instruction ins, const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d );

		static std::optional<Registers> binaryOperator ( Instruction ins, const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d );
		static std::optional<Registers> equals ( const Stack & _s, const Enviroment & _e, const Stack & _c, const Stack & _d );

		static void print ( const Stack & s );
		static Value read ();
	};
}