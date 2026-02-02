///
/// @file      NoPlot.hpp
/// @brief     空实现plot函数
/// @details   空实现plot函数，用于在没有matplot++的情况下编译通过
/// @author    axel
/// @date      2026-01-30
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
#include "AstUtil/StringView.hpp"
#include <vector>


AST_NAMESPACE_BEGIN

// 当没有matplot++时，切换到空实现函数，
// 避免在没有matplot++的情况下编译错误


/// @brief 空实现函数，用于在没有matplot++的情况下编译通过
#define _AST_EMPTY_FUNCTION(NAME) \
    template<typename ...Args>\
    void NAME(Args&&... args){}


/// @brief 空实现plot函数，用于在没有matplot++的情况下编译通过
#define _AST_EMPTY_PLOT_FUNCTION(NAME) \
    _AST_EMPTY_FUNCTION(NAME)\
    template<typename T, typename U, typename ...Args>\
    void NAME(const std::initializer_list<T>&, const std::initializer_list<U>&, Args&&... args){}\
    template<typename T, typename ...Args>\
    void NAME(const std::initializer_list<T>&, Args&&... args){}\


namespace noplot
{
    template<typename ...Args>
    void xlim(Args&&... args){}

    template<typename ...Args>
    void ylim(Args&&... args){}

    template<typename ...Args>
    void figure(Args&&... args){}

    template<typename ...Args>
    void title(Args&&... args){}

    template<typename ...Args>
    void xlabel(Args&&... args){}   

    template<typename ...Args>
    void ylabel(Args&&... args){}       

    template<typename ...Args>
    void legend(Args&&... args){}

    template<typename ...Args>
    void grid(Args&&... args){} 

    template<typename ...Args>
    void show(Args&&... args){} 

    template<typename ...Args>
    void savefig(Args&&... args){}

    template<typename ...Args>
    void subplot(Args&&... args){}

    _AST_EMPTY_PLOT_FUNCTION(plot)
    _AST_EMPTY_PLOT_FUNCTION(scatter)
}

AST_NAMESPACE_END
