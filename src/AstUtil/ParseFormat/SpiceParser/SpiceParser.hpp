///
/// @file      SpiceParser.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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
#include "AstUtil/BaseParser.hpp"
#include "AstUtil/BKVItemView.hpp"
#include <cstdio>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup ParseFormat
    @{
*/

class BKVItemView;

/// @brief SPICE内核文件解析器(parser for SPICE kernel file)
class AST_UTIL_API SpiceParser: public BaseParser
{
public:
    SpiceParser();
    SpiceParser(StringView filepath);
    ~SpiceParser() = default;

    /// @brief 获取下一个键值对项
    /// @param item 输出参数，用于存储获取到的键值对项
    /// @return 错误码，-1(EOF) 表示文件结束
    err_t   getNext(BKVItemView& item);  
    
protected:
    std::vector<char>  keyBuffer_;              ///< 键缓冲区
    std::string        valueBuffer_;            ///< 值缓冲区
    bool               inDataBlock_{false};     ///< 是否在数据块中
};

/*! @} */

AST_NAMESPACE_END
