#include "tokens.h"

namespace lisp::tokens
{
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