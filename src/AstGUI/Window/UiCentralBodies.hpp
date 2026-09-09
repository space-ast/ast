///
/// @file      UiCentralBodies.hpp
/// @brief     中心天体列表共享数据
/// @details   提供"中心天体"显示名/标识对，供多个对话框复用。
///            集中定义以避免各 .cpp 在 unity build 合并时对匿名命名空间中同名的
///            CentralBodyItem/kCentralBodies 产生重复定义。
/// @author    axel
/// @date      2026-09-07
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

/// @brief 中心天体列表：显示中文名，底层保留英文标识（与 ast 内部查询键一致）
struct CentralBodyItem
{
    const char* cn;  ///< 中文显示名
    const char* id;  ///< 英文标识（如 "Earth"）
};

const CentralBodyItem kCentralBodies[] = {
    { "地球", "Earth" }, { "月球", "Moon" }, { "太阳", "Sun" },
    { "水星", "Mercury" }, { "金星", "Venus" }, { "火星", "Mars" },
    { "木星", "Jupiter" }, { "土星", "Saturn" }, { "天王星", "Uranus" },
    { "海王星", "Neptune" }, { "冥王星", "Pluto" }
};

AST_NAMESPACE_END
