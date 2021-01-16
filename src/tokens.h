#pragma once

#include "value.h"
#include "instruction.h"

#include <vector>
#include <optional>
#include <set>
#include <map>

namespace lisp::tokens
{
    using namespace lisp;
    using namespace lisp::secd;
    
    /// Set of correct, non-trivialy translatable tokens
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
        "let",
        "letrec",

        // defining
    	"defun",
    	"lambda",
    };

    /// Map of trivialy translatable tokens
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

    /// Returns translation of trivialy-translatable tokens, empty otherwise
    std::optional<Instruction> translate ( const std::string & in );
    /// Returns true if given string is translatable token, false otherwise
    bool isSymbol ( const std::string & in );
}