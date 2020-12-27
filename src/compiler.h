#pragma once

#include "parser.h"
#include <set>

namespace lisp
{
	using namespace lisp::secd;

	class Compiler
	{
	public:
		static Value Begin ( const Value & in );
		typedef std::map<std::string, Value> EnvMap;

	private:
		/// Input code to compile
		Stack _input;
		/**Map mapping symbols to actula index in enviroment
		 * Has structure 'pair( name, ( layer . index ) )''
		 */
		EnvMap _envMap;
		/**Set of names of functions
		 * Is signaling when something in _envMap is function and not just constant
		 */ 
		std::set<std::string> _functions;
		/// Flag signaling if this compiler is compiling subroutine
		bool _subroutine;

		Compiler ( const Value & in );
		Compiler ( const Value & input, const EnvMap & env, const std::set<std::string> funcs, bool sub = false);

		/**
		 * \defgroup Compile_over Overheading compiling functions, making sure all tokens get compiled
		 * @{
		 */	
		std::optional<Stack> CompileCode();
		std::optional<Stack> CompileArgsCode( const Stack & st );
		std::optional<Stack> CompileArgsCode();
		/// @}

		/// Main compile function
		std::optional<Stack> Compile();

		/**
		 * \defgroup Compile_sub COmpiler helper functions
		 * @{
		 */	
		std::optional<Stack> CompileCons ( const Value & val );
		std::optional<Stack> CompileBuiltIn ( const std::string & val );
		std::optional<Stack> CompileSymbol ( const std::string & val );

		std::optional<Stack> CompileIf ();
		std::optional<Stack> CompileQuasiquote ();

		std::optional<Stack> CompileLambda ( const Value & val );
		std::optional<Stack> CompileDefun ( const Value & val );
		std::optional<Stack> CompileFunction ( const Value & val );
		/// @}
	};
}