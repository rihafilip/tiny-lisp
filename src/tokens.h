#pragma once

#include <vector>
#include "value.h"

namespace lisp
{
    namespace tokens
    {
        const std::vector<const char *> TOKENS_VECTOR = 
        {
        	"(",
        	")",
        	";",
        	"`",
        	",",
        	".",
        	"+",
        	"-",
        	"*",
        	"/",
        	"eq",
        	"<",
        	">",
        	"print",
        	"read",
        	"if",
        	"lambda",
        	"quote",
        	"apply",
        	"cons",
        	"car",
        	"cdr",
        	"consp",
        	"defun",
        	"let",
        	"t",
        	"'",
        	""
        };

        /// Transforms vector into list
        Value makeList();
        Value makeList ( std::vector<const char *>::const_iterator i, std::vector<const char *>::const_iterator end, Value acc );
    }
}
