#pragma once
#include "AstCore/AstCoreExport.h"
#include "AstTypesForward.h"
#include <string>


AST_NAMESPACE_BEGIN

class Class;

class AST_CORE_API Object
{
public:
    Object();
protected:
    err_t getAttrString(const std::string& path, std::string& value) const;

};


AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(Object)


