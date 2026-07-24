///
/// @file      TLELines.hpp
/// @brief     TLE 原始行文本容器
/// @details   存储 NORAD 两行根数（TLE）的原始三行文本：卫星名称、第一行、第二行。
///            TLE 格式规范参见 https://celestrak.org/NORAD/documentation/tle-fmt.php
/// @author    axel
/// @date      2026-07-18
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
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief TLE 原始行文本
/// @details 存储一组 NORAD 两行根数的原始文本，包含卫星名称和两行轨道数据。
///          TLE 格式规范参见 https://celestrak.org/NORAD/documentation/tle-fmt.php
///          可通过 name()/line1()/line2() 访问，empty() 判断是否有效。
class TLELines
{
public:
    /// 检查是否为空（以 line1 是否为空判断）
    bool empty() const { return line1_.empty(); }

    /// 获取卫星名称（第 0 行）
    const std::string& name() const { return name_; }
    std::string&       name() { return name_; }

    /// 获取 TLE 第一行
    const std::string& line1() const { return line1_; }
    std::string&       line1() { return line1_; }

    /// 获取 TLE 第二行
    const std::string& line2() const { return line2_; }
    std::string&       line2() { return line2_; }

private:
    std::string name_{};     ///< 卫星名称（TLE 第 0 行，最长 24 字符）
    std::string line1_{};    ///< TLE 第一行（69 字符）
    std::string line2_{};    ///< TLE 第二行（69 字符）
};


/*! @} */

AST_NAMESPACE_END
