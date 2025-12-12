///
/// @file      Unit.cpp
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

#pragma once

#include "AstGlobal.h"
#include "Dimension.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 单位
class Unit
{
public:
    /// @brief 从内部值转换为外部值
    /// @param value 内部值
    /// @return 外部值
    double fromInternal(double value) const;
    /// @brief 从外部值转换为内部值
    /// @param value 外部值
    /// @return 内部值
    double toInternal(double value) const;
properties:
    std::string name_;      ///< 单位名称
    double scale_ = 1.0;    ///< 单位缩放因子
    Dimension dimension_;   ///< 单位量纲
};




AST_NAMESPACE_END
