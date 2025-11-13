#pragma once
#include "AstMacro.h"

#ifdef AST_BUILD_LIB_CORE
#    define AST_CORE_API A_DECL_EXPORT
#else
#    define AST_CORE_API A_DECL_IMPORT
#endif

