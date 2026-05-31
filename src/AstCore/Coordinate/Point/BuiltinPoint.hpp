///
/// @file      BuiltinPoint.hpp
/// @brief     内置坐标点
/// @details   
/// @author    axel
/// @date      2026-03-11
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
#include "AstCore/PointSSBarycenter.hpp"
#include "AstCore/PointRoot.hpp"
#include "AstCore/PointMacro.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Coordinate
    @{
*/

/*
- 这些函数并不是工厂函数，他们所返回的对象示例都是不变的单个实例，
  不会因为DataContext的更改而返回不同实例。
  但是重新加载或者修改线程DataContext的数据后，会影响这些坐标点的计算结果
  也就是说他们依赖于当前线程DataContext的相关数据

- 虽然天体本身就是坐标点，天体本身与这里的函数所返回的坐标点的区别在于：
  天体是可以独立创建的，而且可以通过修改天体配置使其与DataContext解耦

- 如果这里的函数返回的坐标点就是天体本身，考虑到DataContext是可以切换的
  那么无法保持每个函数返回的坐标点都是不变的实例，而这与`aAxesXXX`系列函数的行为不同
  这进一步会导致依赖这些函数所返回的坐标点实例的`aFrameXXX`系列函数的计算结果无法保持一致
*/

#ifndef SWIG
_AST_DECL_POINT(Mercury)
_AST_DECL_POINT(Venus)
_AST_DECL_POINT(Earth)
_AST_DECL_POINT(Mars)
_AST_DECL_POINT(Jupiter)
_AST_DECL_POINT(Saturn)
_AST_DECL_POINT(Uranus)
_AST_DECL_POINT(Neptune)
_AST_DECL_POINT(Pluto)
_AST_DECL_POINT(Moon)
_AST_DECL_POINT(Sun)
_AST_DECL_POINT(EMBarycenter)

#endif

/*! @} */

AST_NAMESPACE_END
