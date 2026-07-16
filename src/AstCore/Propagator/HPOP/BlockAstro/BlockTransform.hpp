///
/// @file      BlockTransform.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-25
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "BlockDerivative.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 坐标系转换函数块（待实现）
/// @details
/// 计划实现通用的坐标系变换功能，将坐标转换矩阵作为公共信号，避免一些重复计算。
///
/// 当前 run() 返回 -1（未实现）。
///
/// @todo 待实现：定义输出端口
class AST_CORE_API BlockTransform : public BlockDerivative
{
public:
    BlockTransform() = default;
    ~BlockTransform() = default;

    errc_t run(const SimTime& simTime) override;
protected:

};

/*! @} */

AST_NAMESPACE_END
