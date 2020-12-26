#pragma once

#include "parser.h"

namespace lisp
{
	using namespace lisp::secd;

	class Compiler
	{
	public:
		static Value Begin ( const Value & in );

	private:
		Stack input;

		Compiler ( const Value & in );

		std::optional<Stack> Compile();
		std::optional<Stack> Single();
		std::optional<Stack> CompileBuiltIn ( const std::string & val );
		std::optional<Stack> CompileSymbol ( const std::string & val );

		std::optional<Stack> CompileIf ();
		std::optional<Stack> CompileDefun ();
		std::optional<Stack> CompileLambda ();
	};
}