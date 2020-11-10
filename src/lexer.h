#pragma once
#include "value.h"

#include <sstream>
#include <functional>

namespace lisp
{
	class Lexer
	{
	public:
		/// Returns tokenized expression, its tokens and built_in tokens
		/// ( tokens . ( built_in_tokens . tokens ) )
		static Value Scan( const std::string & str );
	private:
		/// Overload of Scan that has the right input
		static Value Scan ( std::stringstream & ss, Value tokenized, Value built_in, Value tokens );
		/**
		 * @param  str    searched for string
		 * @param  tokens source where str is searched for
		 * @return        Value if value of str is found, or null value
		 */
		static Value Find ( const std::string & str, Value tokens );

		/// Returns the next word
		static std::string getNext ( std::stringstream & ss );
		/**
		 * Returns the whole word
		 * @param  ss   stream to pull from
		 * @param  acc  accumulator, string which is returned
		 * @param  comp Comparator that decides if next character belong to the word or not
		 * @return      The whole word
		 */
		static std::string getWord ( std::stringstream & ss, std::string acc, std::function<int(int)> comp );
		/// Specialized getWord that skip spaces and throws away the return string
		static std::stringstream & skipSpace( std::stringstream & ss );
	};
}