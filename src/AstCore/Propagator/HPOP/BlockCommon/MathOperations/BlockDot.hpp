///
/// @file      BlockDot.hpp
/// @brief     ~
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

/// @brief 点积块
class AST_CORE_API BlockDot : public FuncBlock
{
public:
    BlockDot();

    err_t evaluate(const SimTime &simTime) override;
protected:
    double* vector1_{nullptr};  // 第一个向量
    double* vector2_{nullptr};  // 第二个向量
    double* output_{&outputBuffer_};
    double outputBuffer_{0.0};
    size_t vectorSize_{3};  // 向量大小，默认为3（用于三维空间）
};

AST_NAMESPACE_END