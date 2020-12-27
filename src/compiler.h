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
		Stack _input;
		// pair( name, ( layer . index ) )
		std::map<std::string, Value> _envMap;

		Compiler ( const Value & in );

		Compiler ( const Value & input, const std::map<std::string, Value> & env );

		static std::map<std::string, Value> Shift ( const std::map<std::string, Value> & env );
		static std::map<std::string, Value> MakeEnviroment ( std::map<std::string, Value> enviroment, const Value & val, int counter = 0 );

		std::optional<Stack> Compile();
		std::optional<Stack> CompileArgs( const Stack & st );
		std::optional<Stack> CompileArgs();

		std::optional<Stack> Single();

		std::optional<Stack> CompileCons ( const Value & val );
		std::optional<Stack> CompileBuiltIn ( const std::string & val );
		std::optional<Stack> CompileSymbol ( const std::string & val );

		std::optional<Stack> CompileIf ();
		std::optional<Stack> CompileDefun ( const Value & val );
		std::optional<Stack> CompileLambda ( const Value & val );
		std::optional<Stack> CompileQuasiquote ();
	};
}