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

        Value makeList ( std::vector<const char *>::const_iterator i, std::vector<const char *>::const_iterator end, Value acc )
        {
            if ( i == end )
                return acc;

            Value nextVal = Value::Symbol( *i );
            return makeList( ++i, end, acc . append ( nextVal) );
        }

        /// Transforms vector into list
        Value makeList()
        {
            return makeList ( TOKENS_VECTOR . cbegin(), TOKENS_VECTOR . cend(), Value::Null() );
        }
    }
}
