///
/// @file      ValVector.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-27
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "ValArray.hpp"
#include "AstUtil/SharedPtr.hpp"
#include <string>

AST_NAMESPACE_BEGIN

template<typename T>
class ValVector: public ValArray<T, 1>
{
public:
    AST_OBJECT(ValVector)
    static extern Class typeDescr;     /// 类型描述符
protected:
    
};

typedef ValVector<bool> ValVectorBool;
typedef ValVector<int> ValVectorInt;
typedef ValVector<double> ValVectorDouble;
typedef ValVector<std::string> ValVectorString;
typedef ValVector<SharedPtr<Value>> ValVectorAny;

AST_NAMESPACE_END
