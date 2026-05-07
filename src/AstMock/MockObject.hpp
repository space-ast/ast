///
/// @file      MockObject.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-29
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstUtil/Object.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_MOCK_API MockObject: public Object
{
public:
    AST_OBJECT(MockObject)
    AST_PROPERT(x)
    AST_PROPERT(duration)
    AST_PROPERT(factor)
    MockObject() = default;
    ~MockObject() = default;
PROPERTIES:
    length_d x() const { return x_; }
    void setX(length_d x) { x_ = x; }
    time_d duration() const { return duration_; }
    void setDuration(time_d duration) { duration_ = duration; }
    double factor() const { return factor_; }
    void setFactor(double factor) { factor_ = factor; }
private:
    length_d x_{0.0};
    time_d duration_{0.0};
    double factor_{1.0};
};

/*! @} */

AST_NAMESPACE_END
