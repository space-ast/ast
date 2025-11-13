#pragma once
#include "AstCore/AstCoreExport.h"
#include "AstTypesForward.h"
#include <string>

class AClass;

class AST_CORE_API AObject
{
public:
    AObject();
protected:
    err_t getAttrString(const std::string& path, std::string& value) const;

};
