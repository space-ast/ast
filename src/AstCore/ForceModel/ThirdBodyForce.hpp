///
/// @file      ThirdBodyForce.hpp
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
#include "AstCore/CelestialBody.hpp"
#include "AstCore/PointMassForce.hpp"
#include "AstCore/GravityForce.hpp"
#include "AstUtil/ClonePtr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 星历来源
enum class EEphemerisSource
{
    eBodyEphemeris,      ///< 天体星历
    eJplDE,              ///< JPL DE 星历
    eJplSpice,           ///< JPL SPICE 星历
};


/// @brief 三体引力模型
class AST_CORE_API ThirdBodyForce: public ForceModel
{
public:
    ThirdBodyForce() = default;

    ThirdBodyForce(CelestialBody* body)
        : body_(body)
    {}

    /// @brief 获取引力场模型
    /// @warning 调用该接口后，三体引力模型将被设置为引力场模型
    /// @return 引力场模型
    GravityForce& gravity();

    /// @brief 获取点质量引力模型
    /// @warning 调用该接口后，三体引力模型将被设置为点质量引力模型
    /// @return 点质量引力模型
    PointMassForce& pointMass();
    
    /// @brief 获取三体引力模型
    /// @return 三体引力模型
    BodyAttraction& bodyAttraction();

    /// @brief 获取三体引力模型
    /// @return 三体引力模型
    const BodyAttraction& bodyAttraction() const;

    /// @brief 获取三体引力类型
    /// @return 三体引力类型
    EBodyAttractionType bodyAttractionType() const;
    
    /// @brief 设置三体引力类型
    /// @param type 三体引力类型
    void setAttractionType(EBodyAttractionType type);

    /// @brief 获取三体
    CelestialBody* body() const;

    /// @brief 设置三体
    void setBody(CelestialBody* body);

    /// @brief 获取星历来源
    EEphemerisSource ephemerisSource() const{return ephemerisSource_;}

    /// @brief 设置星历来源
    void setEphemerisSource(EEphemerisSource source){ephemerisSource_ = source;}
private:
    HCelestialBody body_;                                                       ///< 天体
    EEphemerisSource ephemerisSource_{EEphemerisSource::eBodyEphemeris};        ///< 星历来源
    EBodyAttractionType attractionType_{EBodyAttractionType::ePointMass};       ///< 引力类型
    PointMassForce pointMass_{};                                                ///< 点质量引力模型
    GravityForce gravity_{};                                                    ///< 引力场模型
};


/*! @} */

AST_NAMESPACE_END
