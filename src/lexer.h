#pragma once
#include "value.h"

#include <functional>
#include <utility>
#include <vector>

namespace lisp
{
	class Lexer
	{
	public:
		/// Returns tokenized expression, its tokens and built_in tokens
		/// ( tokens . ( built_in_tokens . tokens ) )
		static Value Scan( const std::string & str );
	private:
		/// Constructor for lexer class, constructed by Scan
		Lexer( std::vector<char> vec, Value built_in );

		/// Starts tokenizing
		Value begin ();
		/// Evaluates the tokens
		Value evaluate ( std::optional<Value> optionalValue, Value acc );

		/// Returns the next token in stack
		std::optional<Value> nextToken();
		/**
		 * Returns the rest of token that satisfies comp
		 * @param  comp comparator to be satisfied
		 * @param  acc  string accumulator, return value
		 * @return      string of the whole token
		 */
		std::string restOfToken( std::function<int(int)> comp, std::string acc );

		/// Tries to find the input string in built-in tokens
		/// @return Value if fnound, empty optional otherwise
		std::optional<Value> isBuiltIn ( std::string str, Value lst);
		/// Pops all whitespace characters form stack
		void skipSpace();
		/// Helper function that pops the next token in stack and returns it
		char pop ();

		/// Stack where is the rest of expression
		std::vector<char> m_Stack;

		/// built-in tokens (brackets, quotes, 'defun' etc.)
		Value m_BuiltIn;
	};
}