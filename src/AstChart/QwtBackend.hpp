///
/// @file      QwtBackend.hpp
/// @brief     Qwt 绘图后端兼容入口
/// @details   Qwt 后端实现已迁移到 AstPluginQwt 插件，
///            本文件仅保留旧接口，内部转发到 AstPlot 的插件查找
/// @author    axel
/// @date      2026-11-11
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

/// @brief 选择 Qwt 绘图后端
/// @note 保留的兼容接口，内部转发到 aUsePlotBackend("qwt")；
///       需要相应插件（AstPluginQwt）已构建并可被扫描到
AST_CHART_API void aUseQwtBackend();

AST_NAMESPACE_END
