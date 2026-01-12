///
/// @file      NutationTerm.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-10
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

AST_NAMESPACE_BEGIN


/// @brief IERS 2003/2010 岁差章动模型XYS系数
/// 参考 NutationSeries 类
class NutationTerm
{
public:
    AST_DEF_ACCESS_METHOD(double, sinCoeff)
    AST_DEF_ACCESS_METHOD(double, cosCoeff)
    AST_DEF_ACCESS_METHOD(int, nL)
    AST_DEF_ACCESS_METHOD(int, nLP)
    AST_DEF_ACCESS_METHOD(int, nF)
    AST_DEF_ACCESS_METHOD(int, nD)
    AST_DEF_ACCESS_METHOD(int, nOm)
    AST_DEF_ACCESS_METHOD(int, nMe)
    AST_DEF_ACCESS_METHOD(int, nVe)
    AST_DEF_ACCESS_METHOD(int, nE)
    AST_DEF_ACCESS_METHOD(int, nMa)
    AST_DEF_ACCESS_METHOD(int, nJu)
    AST_DEF_ACCESS_METHOD(int, nSa)
    AST_DEF_ACCESS_METHOD(int, nUr)
    AST_DEF_ACCESS_METHOD(int, nNe)
    AST_DEF_ACCESS_METHOD(int, nPa)
public:
    double sinCoeff_;
    double cosCoeff_;
    // 日月章动系数
    int nL_;
    int nLP_;
    int nF_;
    int nD_;
    int nOm_;
    // 行星章动系数
    int nMe_;
    int nVe_;
    int nE_;
    int nMa_;
    int nJu_;
    int nSa_;
    int nUr_;
    int nNe_;
    int nPa_;
};


AST_NAMESPACE_END
