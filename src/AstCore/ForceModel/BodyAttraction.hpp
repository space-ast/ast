///
/// @file      BodyAttraction.hpp
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
#include "AstCore/ForceModel.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class GravityForce;
class PointMassForce;
class ThirdBodyForce;

/// @brief 天体引力模型类型
enum class EBodyAttractionType
{
    eGravity,       ///< 中心天体重力场模型
    ePointMass,     ///< 中心天体点质量引力模型
    eUnknown,       ///< 未知引力模型类型
};


/// @brief 天体引力模型
class AST_CORE_API BodyAttraction: public ForceModel
{
public:
    /// @brief 获取引力模型类型
    /// @return 引力模型类型
    virtual EBodyAttractionType getBodyAttractionType() const = 0;

    /// @brief 克隆引力模型
    /// @return 引力模型指针
    virtual BodyAttraction* clone() const = 0;

    /// @brief 检查是否为指定引力模型类型
    /// @param type 引力模型类型
    /// @return 是否为指定类型
    bool isAttractionType(EBodyAttractionType type) const{
        return getBodyAttractionType() == type;
    }
    
    /// @brief 尝试将引力模型转换为重力场模型
    /// @return 重力场模型指针
    const GravityForce* asGravityForce() const;
    GravityForce* asGravityForce();

    /// @brief 尝试将引力模型转换为点质量引力模型
    /// @return 点质量引力模型指针
    const PointMassForce* asPointMassForce() const;
    PointMassForce* asPointMassForce();
};



/*! @} */

AST_NAMESPACE_END
