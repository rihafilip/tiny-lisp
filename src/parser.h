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
		static Value Parse ( const Value & tokens );

	private:
		Stack input;
		static const std::map<std::string, std::string> syntaxSugar;

		Parser ( const Value & vinput );

		std::optional<Value> Process ();
		std::optional<Value> Item ();
		std::optional<Value> List ();

		std::optional<Value> error ( const std::string & message );
	};
}
