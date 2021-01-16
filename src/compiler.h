#pragma once

#include "parser.h"
#include <set>

namespace lisp
{
	using namespace lisp::secd;
	/**
	 * Compiler from s-expression to SECD instructions
	 */
	class Compiler
	{
	public:
		typedef std::map<std::string, Value> EnvMap;
		
		/// Compiles given code with given enviroment map
		static std::pair<Value, Compiler::EnvMap> CompileCode ( const Value & lst, const EnvMap & env = EnvMap(), const Value & acc = Value::Null() );

	private:
		/**
		 * \defgroup Compile_over Overheading compiling functions, making sure all tokens get compiled
		 * @{
		 */	
		/// Compiles the first element in list as calll and the rest as arguments
		static std::optional<Stack> CompileCall ( Stack st, const EnvMap & env );

		/// Compiles arguments
		static std::optional<Stack> CompileArguments ( const Stack & st, const EnvMap & env , const Stack & acc, bool append = false );
		/// Compiles arguments that compound into list
		static std::optional<Stack> CompileArgumentsList ( const Stack & st, const EnvMap & env, const Stack & acc );
		/// Compiles single argument token
		static std::optional<Stack> CompileToken ( Stack st, const EnvMap & env );
		///@}
		
		/**
		 * \defgroup Compile_sub Compiler helper functions
		 * @{
		 */	
		static Stack CompileNumber ( const Value & val );
		static std::optional<Stack> CompileBuiltIn ( const std::string & val, const Stack & st, const EnvMap & env );
		/// Compiles calling of built-in symbol
		static std::optional<Stack> CompileBuiltInCall ( const std::string & val, const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileBuiltInCallSimple ( Instruction val, const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileSymbol ( const std::string & val, const Stack & st, const EnvMap & env );

		static std::optional<Stack> CompileIf ( const Stack & st, const EnvMap & env );
		
		static std::optional<Stack> CompileLet ( const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileLetrec ( const Stack & st, const EnvMap & env );
		static std::tuple<std::optional<Value>, Value, EnvMap> CompileLetMain ( const Stack & st, const EnvMap & env );
		static std::pair<std::optional<Value>, Value> CompileLetArgs ( const Stack & st, const Value & bodies, const Value & names );

		static std::optional<Stack> CompileQuote ( const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileQuasiquote ( const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileQuasiquoteAssemble( const Value & val, const EnvMap & env );

		static std::optional<Stack> CompileLambda ( const Stack & st, const EnvMap & env );
		static std::tuple<Value, EnvMap> CompileDefun ( const Value & val, const EnvMap & env  );
		/// Compiles body of lambda or function
		static std::optional<Stack> CompileBody ( const Stack & st, const EnvMap & env );
		/// @}
		
		static bool AssertArgsCount( int num, const Stack & val );
	};
}