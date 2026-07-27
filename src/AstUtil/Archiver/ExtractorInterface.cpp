#include "ExtractorInterface.hpp"

AST_NAMESPACE_BEGIN

bool ExtractorInterface::canExtract(StringView source) const
{
    (void)source;
    return true;
}

AST_NAMESPACE_END
