///
/// @file      TwoBody.hpp
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 点质量引力模型/
class PointMassForce: public BodyAttraction
{
public:
    EBodyAttractionType getBodyAttractionType() const override{return EBodyAttractionType::ePointMass;}
    BodyAttraction* clone() const override{return new PointMassForce(*this);}
public:
    EGMSource gmSource_{EGMSource::eBodyGravity};       ///< 引力常数来源
    double specifiedGM_{0.0};                           ///< 指定的引力常数
};


/*! @} */

AST_NAMESPACE_END
