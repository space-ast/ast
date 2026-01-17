///
/// @file      ODEFixedStepIntegrator.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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

#include "ODEFixedStepIntegrator.hpp"
#include <limits>

AST_NAMESPACE_BEGIN

ODEFixedStepIntegrator::Workspace::Workspace()
{

}

void ODEFixedStepIntegrator::Workspace::reset(int dimension, int stage)
{
    clear();

    // 重置统计数据
    numSteps_ = 0;
    largestStepSize_ = 0;
    smallestStepSize_ = std::numeric_limits<double>::max();

    // 重置多步法中间结果数组
    KArr_ = new double*[stage];
    for(int i = 0; i < stage; i++)
    {
        KArr_[i] = new double[dimension];
    }
    absErrPerLen_ = new double[dimension];
    ymid_ = new double[dimension];
    y_ = new double[dimension];
    ynew_ = new double[dimension];
    ystep_ = new double[dimension];

}

void ODEFixedStepIntegrator::Workspace::clear()
{
    // 清空中间结果数组
    for(int i = 0; i < stage_; i++)
    {
        delete[] KArr_[i];
    }
    delete[] KArr_;
    delete[] absErrPerLen_;
    delete[] ymid_;
    delete[] y_;
    delete[] ynew_;
}


ODEFixedStepIntegrator::Workspace::~Workspace()
{

}

ODEFixedStepIntegrator::ODEFixedStepIntegrator()
{

}

ODEFixedStepIntegrator::~ODEFixedStepIntegrator()
{
}

AST_NAMESPACE_END


