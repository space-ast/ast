///
/// @file      TextRenderer.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-15
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
#include "AstUtil/StringView.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 流式文本格式转换器
/// @details 
/// 根据渲染规则将流式文本渲染为字符串，可以用于LLM流式输出的渲染
/// 可以覆盖以下功能：
/// - 将markdown渲染为普通文本、ANSI转义序列、html等；
/// - 将html渲染为普通文本、markdown等
class AST_UTIL_API BaseRenderer
{
public:
    virtual ~BaseRenderer() = default;

    /// @brief 渲染一个字符串块
    /// @param chunk 字符块
    /// @return 渲染结果
    std::string operator()(StringView chunk);

    /// @brief 将渲染结果写入文件
    /// @param chunk 字符块
    /// @param fp 文件指针
    void print(StringView chunk);

    /// @brief 接受一个字符串，渲染为对应的字符串
    /// @param chunk 字符块
    /// @param accumulated 输出字符串，会将渲染结果追加到accumulated中
    virtual void feed(StringView chunk, std::string& accumulated) = 0;

    /// @brief 停止渲染，并返回缓冲区中还未渲染的字符
    virtual void end(std::string& remaining) = 0;

    /// @brief 重置渲染状态机
    virtual void reset() = 0;
};


/*! @} */

AST_NAMESPACE_END
