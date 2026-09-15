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

    BodyShape();

    virtual double majorAxis() const = 0;

    /// @brief 将笛卡尔坐标（天体固连系）转换为大地坐标
    /// @param cartesian 笛卡尔坐标（天体固连系）
    /// @param detic 大地坐标
    virtual void transform(const Vector3d& cartesian, GeodeticPoint& detic) const = 0;

    /// @brief 将大地坐标转换为笛卡尔坐标（天体固连系）
    /// @param detic 大地坐标
    /// @param cartesian 笛卡尔坐标（天体固连系）
    virtual void transform(const GeodeticPoint& detic, Vector3d& cartesian) const = 0;

    /// @brief 将天体固连系下的位置与速度转换为大地坐标及其变化率
    /// @param pos 天体固连系位置
    /// @param vel 天体固连系速度
    /// @param detic 输出的大地坐标
    /// @param rate 输出的大地坐标变化率, 依次为经度率[rad/s]、纬度率[rad/s]、高度率[m/s]
    /// @return 错误码，成功返回eNoError
    /// @note rate 复用 @ref LatLonAlt , 但其纬度、经度、高度三个字段依次为纬度率、经度率、高度率;
    ///       变化率描述的是天体固连系下的速度, 不含任何牵连速度;
    ///       速度分解所依赖的度量因子随形状而异, 由各形状自行实现
    ///       (见 @ref SpheroidShape 、 @ref SphereShape );
    ///       形状不支持该转换时返回 eErrorNotImplemented。
    virtual errc_t transform(const Vector3d& pos, const Vector3d& vel,
                             GeodeticPoint& detic, LatLonAlt& rate) const;

    /// @brief 将大地坐标及其变化率转换为天体固连系下的位置与速度
    /// @param detic 大地坐标
    /// @param rate 大地坐标变化率, 依次为经度率[rad/s]、纬度率[rad/s]、高度率[m/s]
    /// @param pos 输出的天体固连系位置
    /// @param vel 输出的天体固连系速度
    /// @return 错误码，成功返回eNoError
    /// @note 与上一重载互为逆变换; 形状不支持该转换时返回 eErrorNotImplemented。
    virtual errc_t transform(const GeodeticPoint& detic, const LatLonAlt& rate,
                             Vector3d& pos, Vector3d& vel) const;

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
