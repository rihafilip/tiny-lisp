#pragma once

#include "lexer.h"
#include "stack.h"
#include "instruction.h"

#include <map>

namespace lisp
{
	using namespace lisp::secd;

	class Parser
	{
	public:
		/**
		 * Parses incoming tokens into Value lists
		 * @param  tokens incoming tokes
		 * @return        parsed lists
		 */
		static Value Parse ( const Value & tokens );

	private:
		/// Stack of input tokens
		Stack input;
		/// Dictionary with syntactic sugar translations
		static const std::map<std::string, std::string> syntaxSugar;

		Parser ( const Value & vinput );

		/// Processes one chunk of code
		std::optional<Value> Process ();
		/// Parses single item
		std::optional<Value> Item ();
		/// Parses list
		std::optional<Value> List ();

		/// Prints message and returns empty optional
		std::optional<Value> error ( const std::string & message );
	};
}
