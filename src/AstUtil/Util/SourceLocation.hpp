///
/// @file      SourceLocation.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-09
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


/// @brief 判断字符是否为路径分隔符（反斜杠或正斜杠）
constexpr bool is_sep(char c) {
#ifdef _WIN32
    return c == '\\' || c == '/';
#else
    return c == '/';
#endif
}

/// @brief 查找最后一个 ".." 后的路径，并返回后续实际路径的起始位置
constexpr const char* extract_after_last_dotdot(const char* str, const char* last_dotdot = nullptr) {
    return *str == '\0'
        ? (last_dotdot ? last_dotdot : str)   // 到达末尾，返回上次找到的 .. 位置（若存在）
        : (str[0] == '.' && str[1] == '.' && is_sep(str[2]))
            ? extract_after_last_dotdot(str + 3, str + 3)  // 找到新的 ..\，更新 last_dotdot 并继续
            : extract_after_last_dotdot(str + 1, last_dotdot); // 未匹配，继续扫描
}

/// @brief 从路径中提取文件名
constexpr const char* extract_filename(const char* str, const char* last_sep_pos=nullptr) {
    return *str == '\0'
        ? (last_sep_pos ? last_sep_pos + 1 : str)  // 返回最后一个分隔符后的位置，若无则返回原串
        : (is_sep(*str)
            ? extract_filename(str + 1, str)   // 更新最后一个分隔符位置
            : extract_filename(str + 1, last_sep_pos));
}


#define A_SOURCE_FILE_PATH  _AST extract_after_last_dotdot(__FILE__)  // 仅包含 .. 后的路径部分
#define A_SOURCE_FILE_NAME  _AST extract_filename(__FILE__)
#define A_SOURCE_LINE       __LINE__
#define A_SOURCE_FUNC       __FUNCTION__

/*! @} */

AST_NAMESPACE_END
