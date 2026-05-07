///
/// @file      GravityForce.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-21
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
#include "BodyAttraction.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


enum class ESolidTideType{
    eFull,
    ePermanentOnly,
    eNone,
};


class AST_CORE_API GravityForce : public BodyAttraction
{
public:
    GravityForce() = default;
    ~GravityForce() = default;
    EBodyAttractionType getBodyAttractionType() const override{return EBodyAttractionType::eGravity;}
    BodyAttraction* clone() const override{return new GravityForce(*this);}
public:
    // 模型与阶次
    std::string model_{};                                  ///< 中心天体重力场模型(模型名称或者引力场文件路径)
    int maxDegree_{2};                                     ///< 中心天体重力场计算阶数
    int maxOrder_{0};                                      ///< 中心天体重力场计算次数
    bool useSecularVariations_{false};                     ///< 是否考虑引力场的长期变化
    // 固体潮汐配置参数
    ESolidTideType solidTideType_{ESolidTideType::eNone};  ///< 潮汐类型
    bool includeTimeDependentSolidTides_{false};            ///< 是否考虑时间依赖的潮汐
    double minAmplitudeSolidTides_{0.0};                   ///< 最小潮汐振幅
    bool truncateSolidTides_{false};                       ///< 是否截断潮汐
    // 海洋潮汐配置参数
    bool useOceanTides_{false};                            ///< 是否使用海洋潮汐
    int maxDegreeOceanTides_{2};                           ///< 海洋潮汐计算阶数
    int maxOrderOceanTides_{0};                            ///< 海洋潮汐计算次数
    double minAmplitudeOceanTides_{0.0};                   ///< 最小海洋潮汐振幅
};


/*! @} */

AST_NAMESPACE_END
