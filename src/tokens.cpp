#include "tokens.h"

using namespace lisp::secd;

namespace lisp::tokens
{
    std::optional<Instruction> translate ( const std::string & in )
    {
        if ( ! INSTRUCTIONS_MAP.count( in ) )
            return std::nullopt;

        return INSTRUCTIONS_MAP. at( in );
    }

    bool isSymbol ( const std::string & in )
    {
        return TOKENS_SET.count(in) || INSTRUCTIONS_MAP.count(in);
    }
}