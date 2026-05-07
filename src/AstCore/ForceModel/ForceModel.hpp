///
/// @file      ForceModel.hpp
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup ForceModel
    @{
*/

/// @brief 太阳位置
enum class ESunPosition
{
    eTrue,
    eApparentSunToTrueCB,
    eApparent,
};

/// @brief 引力常数来源
enum class EGMSource
{
    eBodyGravity,           ///< 来自天体重力场
    eJplDE,                 ///< 来自JPL DE文件
    eSpecifiedValue,        ///< 用户指定的引力常数
};


/// @brief 力模型
/// @details 力模型用于表示动力学系统中的摄动力，例如重力、大气阻力、太阳光压、三体摄动等
class AST_CORE_API ForceModel: public ObjectNamed
{
public:
    ForceModel() = default;
    virtual ~ForceModel() = default;

};

/*! @} */

AST_NAMESPACE_END
