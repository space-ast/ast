///
/// @file      VisVisitor.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-06
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class VisGroup;
class VisTrajectory;
class VisCelestialBody;
class VisGroundTrack;

/// @brief 可视化对象访问者
/// @details 用于对场景图中的各可视化对象做双分派。渲染后端各自实现本接口，
///          通过 visit() 重载决定每个对象如何被渲染，对象本身只提供数据
///          （如轨迹采样、天体半径），不依赖任何具体后端。
class AST_VISUALIZATION_API VisVisitor
{
public:
    virtual ~VisVisitor() = default;

    /// @brief 访问对象分组
    virtual void visit(VisGroup& group) = 0;

    /// @brief 访问轨迹对象
    virtual void visit(VisTrajectory& trajectory) = 0;

    /// @brief 访问天体对象
    virtual void visit(VisCelestialBody& celestialBody) = 0;

    /// @brief 访问地面轨迹对象
    virtual void visit(VisGroundTrack& groundTrack) = 0;
};

/*! @} */

AST_NAMESPACE_END
