///
/// @file      MockBuffer.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-23
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

#include "MockBuffer.hpp"
#include "AstMath/LagrangeInterpolator.hpp"
#include <stdlib.h> // for malloc and free

AST_NAMESPACE_BEGIN

const double x[6] = {1,2,3,4,5,6};
const double y[6][3] = {{1,2,3},{4,5,6},{7,8,9},{10,11,12},{13,14,15},{16,17,18}};

/// @brief     测试 malloc 分配内存
/// @details   ~
void aMockBufferMalloc(unsigned int size)
{
    double y_interp[3] = {0.0};
    double* workArray = (double*)malloc(size * sizeof(double));
    aLagrangeInterpolate(x, (double*)y, size, 3, 3.5, (double*)y_interp, workArray);
    free(workArray);
}

/// @brief     测试 new 分配内存
/// @details   ~
void aMockBufferNew(unsigned int size)
{
    double y_interp[3] = {0.0};
    double* workArray = new double[size];
    aLagrangeInterpolate(x, (double*)y, size, 3, 3.5, (double*)y_interp, workArray);
    delete[] workArray;
}

/// @brief     测试栈分配内存
/// @details   ~
void aMockBufferStack(unsigned int size)
{
    double y_interp[3] = {0.0};
    double workArray[20];
    aLagrangeInterpolate(x, (double*)y, size, 3, 3.5, (double*)y_interp, workArray);
}

/// @brief     测试 alloca 分配内存
/// @details   ~
void aMockBufferAlloca(unsigned int size)
{
    double y_interp[3] = {0.0};
    double* workArray = (double*)alloca(size * sizeof(double));
    aLagrangeInterpolate(x, (double*)y, size, 3, 3.5, (double*)y_interp, workArray);
}


AST_NAMESPACE_END

