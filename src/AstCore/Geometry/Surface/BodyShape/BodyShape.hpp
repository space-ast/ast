///
/// @file      BodyShape.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-11
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "AstCore/GeodeticPoint.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/// @brief 天体形状基类
class AST_CORE_API BodyShape : public ObjectNamed
{
public:
    AST_OBJECT(BodyShape)

    /// @brief 将笛卡尔坐标（天体固连系）转换为大地坐标
    /// @param cartesian 笛卡尔坐标（天体固连系）
    /// @param detic 大地坐标
    virtual void transform(const Vector3d& cartesian, GeodeticPoint& detic) const = 0;

    /// @brief 将大地坐标转换为笛卡尔坐标（天体固连系）
    /// @param detic 大地坐标
    /// @param cartesian 笛卡尔坐标（天体固连系）
    virtual void transform(const GeodeticPoint& detic, Vector3d& cartesian) const = 0;

    /// @brief 将笛卡尔坐标（天体固连系）转换为大地坐标
    /// @param cartesian 笛卡尔坐标（天体固连系）
    /// @return 大地坐标
    A_ALWAYS_INLINE GeodeticPoint transform(const Vector3d& cartesian) const
    {
        GeodeticPoint detic;
        this->transform(cartesian, detic);
        return detic;
    }

    /// @brief 将大地坐标转换为笛卡尔坐标（天体固连系）
    /// @param detic 大地坐标
    /// @return 笛卡尔坐标（天体固连系）
    A_ALWAYS_INLINE Vector3d transform(const GeodeticPoint& point) const
    {
        Vector3d cartesian;
        this->transform(point, cartesian);
        return cartesian;
    }
};


/// @brief 获取WGS84椭球体形状
AST_CORE_CAPI BodyShape* aWGS84Spheroid();
 
/// @brief 获取CGCS2000椭球体形状
AST_CORE_CAPI BodyShape* aCGCS2000Spheroid();



AST_NAMESPACE_END
