///
/// @file      Dimension.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-12
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "Dimension.hpp"

AST_NAMESPACE_BEGIN

#if defined AST_TEST_DIMENSION || 1

struct dimension_value
{
    int dim1 : 4;
    int dim2 : 4;
    int dim3 : 4;
    int dim4 : 4;
    int dim5 : 4;
    int dim6 : 4;
    int dim7 : 4;
    int dim8 : 4;
};


AST_UTIL_CAPI int dim_get_exponent_shift(int dimension)
{
    int shift = 3 * 4; //static_cast<int>(idx) * 4;
    int exponent = (dimension >> shift) & 0xF;
    // 将4位有符号数转换为实际有符号整数
    return exponent;
}
AST_UTIL_CAPI int dim_set_exponent_shift(int dimension, int exponent)
{

    int shift = 3 * 4;
    int mask = 0xF << shift;
    int newValue = dimension & ~mask; // 清除原值
    // 将指数转换为4位有符号表示
    int expBits = exponent & 0xF;
    newValue |= (expBits << shift); // 设置新值

    return newValue;
}

AST_UTIL_CAPI
int dim_get_exponent_bitfield(int dimension)
{
    return ((dimension_value*)&dimension)->dim3;
}

AST_UTIL_CAPI
int dim_set_exponent_bitfield(int dimension, int exponent)
{
    ((dimension_value*)&dimension)->dim3 = exponent;
    return dimension;
}

#endif

AST_NAMESPACE_END
