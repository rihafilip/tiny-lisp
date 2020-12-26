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
		std::map<std::string, std::string> syntaxSugar;

		Parser ( const Value & vinput );

		Value Process ();
		Value Item ();
		Value List ();
	};
}
