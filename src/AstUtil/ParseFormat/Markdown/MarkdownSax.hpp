///
/// @file      MarkdownSax.hpp
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 表格列对齐方式
enum class ETableAlign : int
{
    eDefault = 0,  ///< 默认（无显式对齐）
    eLeft    = 1,  ///< 左对齐  :---
    eCenter  = 2,  ///< 居中对齐 :---:
    eRight   = 3,  ///< 右对齐  ---:
};


class AST_UTIL_API MarkdownSax
{
public:
    virtual ~MarkdownSax() {};

    // 文档生命周期
    virtual void startDocument() = 0;
    virtual void endDocument() = 0;

    // ---- 块级元素 ----

    /// @param force 是否强制换行
    /// force为true时，表示强制换行，此时html输出为<br>
    /// force为false时，表示不强制换行，仅告知此时有个换行，各渲染器自行决定怎么处理
    // virtual void newline(bool force=false) {};

    virtual void startHeading(int level) = 0;
    virtual void endHeading(int level) = 0;

    virtual void startParagraph() = 0;
    virtual void endParagraph() = 0;

    virtual void startCodeBlock(StringView language) = 0;
    virtual void codeLine(StringView line) = 0;  // 代码行
    virtual void endCodeBlock() = 0;

    virtual void startList(bool ordered) = 0;   // true: 有序, false: 无序
    virtual void startListItem() = 0;
    virtual void endListItem() = 0;
    virtual void endList() = 0;

    virtual void startBlockquote() = 0;
    virtual void endBlockquote() = 0;

    virtual void horizontalRule() = 0;

    // ---- 表格（GFM 扩展） ----
    /// @brief 表格开始
    virtual void startTable() {}
    /// @brief 表头区域开始
    virtual void startTableHead() {}
    /// @brief 表头区域结束
    virtual void endTableHead() {}
    /// @brief 表体区域开始
    virtual void startTableBody() {}
    /// @brief 表体区域结束
    virtual void endTableBody() {}
    /// @brief 表格行开始
    virtual void startTableRow() {}
    /// @brief 表格行结束
    virtual void endTableRow() {}
    /// @brief 表格单元格开始
    /// @param align 本列对齐方式（由分隔行 :--- 语法确定）
    virtual void startTableCell(ETableAlign align) { (void)align; }
    /// @brief 表格单元格结束
    virtual void endTableCell() {}
    /// @brief 表格结束
    virtual void endTable() {}

    // ---- 行内元素 ----
    virtual void text(StringView txt) = 0;
    virtual void startEmphasis() = 0;     // 斜体
    virtual void endEmphasis() = 0;
    virtual void startStrong() = 0;       // 粗体
    virtual void endStrong() = 0;
    virtual void startDelete() = 0;      // 删除线
    virtual void endDelete() = 0;
    virtual void codeSpan(StringView code) = 0;
    virtual void startLink(StringView url) = 0;
    virtual void endLink() = 0;
    virtual void image(StringView alt, StringView url) = 0;
};

/*! @} */

AST_NAMESPACE_END
