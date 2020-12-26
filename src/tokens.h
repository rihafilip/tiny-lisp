#pragma once

#include <vector>
#include "value.h"

namespace lisp
{
    namespace tokens
    {
        const std::vector<const char *> TOKENS_VECTOR = 
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

            // artihmetics
        	"+",
        	"-",
        	"*",
        	"/",

            // comparison
        	"eq",
        	"<",
        	">",

            // input output
        	"print",
        	"read",

        	"if",

            // defining
        	"defun",
        	"let",
        	"lambda",

            // list
        	"cons",
        	"car",
        	"cdr",

            // type identify
        	"consp"
        };

        /// Transforms vector into list
        Value makeList();
        Value makeList ( std::vector<const char *>::const_iterator i, std::vector<const char *>::const_iterator end, Value acc );
    }
}
