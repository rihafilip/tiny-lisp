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
		typedef std::set<std::string> FunSet;
		typedef std::pair<std::optional<Stack>, Stack> CompilePair;

		static Value Begin ( const Value & val );
		static Value CompileCode ( const Value & val, const EnvMap & env, const FunSet & funcs );
	private:
		/**
		 * \defgroup Compile_over Overheading compiling functions, making sure all tokens get compiled
		 * @{
		 */	
		static std::optional<Stack> CompileSource ( const Stack & st, const EnvMap & env, const FunSet & funcs, const Stack & acc, bool isArgs = false );
		static CompilePair CompileArgsSource ( const Stack & st, const EnvMap & env, const FunSet & funcs, const Stack & acc );
		/// @}
		
		/// Main compile function
		static CompilePair CompileToken( Stack st, const EnvMap & env, const FunSet & funcs );

		/**
		 * \defgroup Compile_sub Compiler helper functions
		 * @{
		 */	
		static CompilePair	CompileCons		( const Value & val, const Stack & st, const EnvMap & env, const FunSet & funcs );
		static CompilePair	CompileBuiltIn	( const std::string & val, const Stack & st, const EnvMap & env, const FunSet & funcs  );
		static CompilePair	CompileSymbol	( const std::string & val, const Stack & st, const EnvMap & env, const FunSet & funcs  );

		static CompilePair 			CompileIf					( const Stack & st, const EnvMap & env, const FunSet & funcs );
		static CompilePair 			CompileQuote				( const Stack & st, const EnvMap & env, const FunSet & funcs );
		static CompilePair 			CompileQuasiquote			( const Stack & st, const EnvMap & env, const FunSet & funcs );
		static std::optional<Stack> CompileQuasiquoteAssemble	( const Value & val, const EnvMap & env, const FunSet & funcs );

		static CompilePair 							CompileLambdaApplicate 	( const Value & val, const Stack & st, const EnvMap & env, const FunSet & funcs );
		static std::optional<Stack> 				CompileLambda 			( const Value & val, const EnvMap & env, const FunSet & funcs );
		static std::tuple<Value, EnvMap, FunSet> 	CompileDefun 			( const Value & val, EnvMap env, FunSet funcs );
		static std::optional<Stack> 				CompileBody 			( const Value & val, const EnvMap & env, const FunSet & funcs );
		///@}
	};
}