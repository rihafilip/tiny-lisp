#pragma once
#include "value.h"

#include <functional>
#include <utility>
#include <vector>

namespace lisp
{
	typedef std::vector<char> vStack;

	class Lexer
	{
	public:
		/// Returns tokenized expression form string
		static Value Scan( const std::string & str );

	private:
		/// Default contructor
		Lexer();

		Value begin ( vStack stack );

		/**
		 * Evaluates the tokens
		 * @param  pair First is last token, second is rest of stack
		 * @param  acc  Accumulator of return value
		 * @return      Tokenized expression
		 */
		Value evaluate ( std::pair<std::optional<Value>, vStack> pair , Value acc );

		/**
		 * Returns the next token in stack
		 * @param  stack stack to pull form
		 * @return  	    First is token or empty if there is none, second is rest of stack
		 */
		std::pair<std::optional<Value>, vStack> nextToken( vStack stack );

		/**
		 * Returns the rest of token that satisfies comp
		 * @param  stack stack to pull form
		 * @param  comp  comparator to be satisfied
		 * @param  acc   string accumulator, return value
		 * @return       string of the whole token
		 */
		std::pair<std::string, vStack> restOfToken( vStack stack, std::function<int(int)> comp, std::string acc );

		/// Makes Integer Value from string, returns it with untouched stack
		static std::pair<std::optional<Value>, vStack> makeNumber ( std::pair<std::string, vStack> pair );
		/// Makes Symbol Value from string, returns it with untouched stack
		static std::pair<std::optional<Value>, vStack> makeSymbol ( std::pair<std::string, vStack> pair );

		/// Return stack without leading whitespaces
		vStack skipSpace( vStack stack );
	};
}