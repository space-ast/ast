///
/// @file      Atmosphere.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-08
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


double aKpToAp(double kp);

double aApToKp(double ap);


/// @brief 大气模型基类
/// @details 大气模型基类，定义了大气模型的基本接口，参考了orekit的Atmosphere接口
/// 关于 Atmosphere 接口的抽象设计：
/// 1. 核心抽象接口只包含所有大气模型接口的交集。
///    例如，有些大气模型需要太阳位置、F10.7 指数等参数，而有些则不需要。
///    这类“非必需”参数不出现在核心抽象接口中，以避免接口臃肿。
///    但它们可以作为子类模型的配置参数，或通过依赖注入（如注入抽象的 Provider、Frame、Shape 等接口）来提供。
/// 
/// 2. 考虑到接口的可重入性（即同一实例可在多线程中安全并发调用，且不依赖内部可变状态），
///    应将仿真中随时间变化的动态参数（如时间点、位置）直接作为方法的输入参数。
///    其他动态数据（如太阳活动指数）可通过依赖注入的方式由实现类自行获取；
///    纬度、经度、高度等几何量可从输入位置直接计算得到。
class AST_CORE_API Atmosphere
{
public:
    virtual ~Atmosphere() = default;

    /// @brief 获取大气密度
    /// @param tp 当前时间
    /// @param posInBodyFixed 当前位置(相对于getFrame()返回的坐标系)
    /// @return 大气密度（单位：kg/m³）
    virtual double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const = 0;

    /// @brief 获取大气模型的参考坐标系
    virtual Frame* getFrame() const = 0;

};

/*! @} */

AST_NAMESPACE_END
