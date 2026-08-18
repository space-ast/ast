///
/// @file      SpatialCondition.hpp
/// @brief     空间条件基类
/// @author    axel
/// @date      2026-07-29
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
#include "AstUtil/ObjectNamed.hpp"
#include "AstMath/Vector.hpp"
#include "SpatialConditionVisitor.hpp"

AST_NAMESPACE_BEGIN

/// @brief 空间条件基类
/// @details 空间条件是一个依赖于时间和位置的布尔函数
/// @ingroup Spatial
class AST_CORE_API SpatialCondition : public ObjectNamed
{
public:
    SpatialCondition();
    ~SpatialCondition() override = default;

    /// @brief 接受访问者
    virtual void accept(SpatialConditionVisitor& visitor) = 0;

    /// @brief 在指定时间和位置评估条件
    /// @param time 评估时间
    /// @param position 评估位置
    /// @param[out] result 条件是否满足
    /// @return eNoError 成功，其他值表示错误
    virtual errc_t evaluate(const TimePoint& time, const Vector3d& position, bool& result) const = 0;
};

AST_NAMESPACE_END
