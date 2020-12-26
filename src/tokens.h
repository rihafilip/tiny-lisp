#pragma once

#include "value.h"
#include "instruction.h"

#include <vector>
#include <optional>
#include <set>
#include <map>

namespace lisp
{
    namespace tokens
    {
        using namespace lisp::secd;
        
        const std::set<std::string> TOKENS_SET = 
        {
            // syntax
        	"(",
        	")",
        	".",

            // quoting
        	"`",
        	",",
        	"'",
            "quote",
            "quasiquote",
        	"unquote",

        	"if",

            // defining
        	"defun",
        	"lambda",
        };

        const std::map<std::string, Instruction> INSTRUCTIONS_MAP =
        {
            // artihmetics
            { "+", ADD },
            { "-", SUB },
            { "*", MUL },
            { "/", DIV },

            // comparison
            { "eq", EQ },
            { "<",  LESS },
            { ">",  MORE },

            // list
            { "cons",   CONS},
            { "car",    CAR},
            { "cdr",    CDR},
            { "nil",    NIL},

            // type identify
            { "consp", CONSP },

            // input output
            { "print",    PRINT},
            { "read",     READ}
        };

        std::optional<Instruction> translate ( const std::string & in );
        bool isSymbol ( const std::string & in );
    }
}
