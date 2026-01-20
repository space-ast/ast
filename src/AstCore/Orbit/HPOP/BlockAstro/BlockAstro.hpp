///
/// @file      BlockAstro.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
#include "AstCore/FuncBlock.hpp"

AST_NAMESPACE_BEGIN

constexpr const char* kIdentifierPos = "Pos";
constexpr const char* kIdentifierVel = "Vel";
constexpr const char* kIdentifierAcc = "Acc";
constexpr const char* kIdentifierAccTwoBody = "AccTwoBody";
constexpr const char* kIdentifierAccThirdBody = "AccThirdBody";
constexpr const char* kIdentifierAccGravity = "AccGravity";
constexpr const char* kIdentifierAccGravityPert = "AccGravityPert";
constexpr const char* kIdentifierAccRelativity = "AccRelativity";
constexpr const char* kIdentifierAccDrag = "AccDrag";
constexpr const char* kIdentifierAccSRP = "AccSRP";

/// @brief 航天相关的函数块
class BlockAstro: public FuncBlock
{
public:
    // @todo: 提取航天相关函数块的公共接口
};

AST_NAMESPACE_END
