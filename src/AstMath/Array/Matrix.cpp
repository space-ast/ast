///
/// @file      Matrix.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-30
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

#include "Matrix.hpp"

AST_NAMESPACE_BEGIN

template<>
AST_MATH_API
std::string Matrix3d::toString() const
{
    // 9 个 %.15g 最坏各占 22 字节
    char buf[256];
    snprintf(
        buf, sizeof(buf), 
        "\n%.15g, %.15g, %.15g\n%.15g, %.15g, %.15g\n%.15g, %.15g, %.15g\n", 
        data_[0][0], data_[0][1], data_[0][2], 
        data_[1][0], data_[1][1], data_[1][2], 
        data_[2][0], data_[2][1], data_[2][2]
    );
    return std::string(buf);
}

AST_NAMESPACE_END
