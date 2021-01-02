#pragma once

#include "parser.h"
#include <set>

namespace lisp
{
	using namespace lisp::secd;

	class Compiler
	{
	public:
		typedef std::map<std::string, Value> EnvMap;
	
		static Value Begin ( const Value & val );
		static Value CompileCode ( const Value & lst, const EnvMap & env );

	private:
		/**
		 * \defgroup Compile_over Overheading compiling functions, making sure all tokens get compiled
		 * @{
		 */	
		static std::optional<Stack> CompileCall ( Stack st, const EnvMap & env );

		static std::optional<Stack> CompileArguments ( const Stack & st, const EnvMap & env , const Stack & acc, bool append = false );
		static std::optional<Stack> CompileArgumentsList ( const Stack & st, const EnvMap & env, const Stack & acc );
		static std::optional<Stack> CompileToken ( Stack st, const EnvMap & env );
		///@}
		
		/**
		 * \defgroup Compile_sub Compiler helper functions
		 * @{
		 */	
		static Stack CompileNumber ( const Value & val );
		static std::optional<Stack> CompileBuiltIn ( const std::string & val, const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileBuiltInCall ( const std::string & val, const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileSymbol ( const std::string & val, const Stack & st, const EnvMap & env );

		static std::optional<Stack> CompileIf ( const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileLet ( const Stack & st, const EnvMap & env );
		static std::pair<std::optional<Value>, Value> CompileLetArgs ( const Stack & st, const Value & bodies, const Value & names );

		static std::optional<Stack> CompileQuote ( const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileQuasiquote ( const Stack & st, const EnvMap & env );
		static std::optional<Stack> CompileQuasiquoteAssemble( const Value & val, const EnvMap & env );

		static std::optional<Stack> CompileLambda ( const Stack & st, const EnvMap & env );
		static std::tuple<Value, EnvMap> CompileDefun ( const Value & val, EnvMap env  );
		static std::optional<Stack> CompileBody ( const Stack & st, const EnvMap & env );
		/// @}
		
		static bool AssertArgsCount( int num, const Stack & val );
	};
}