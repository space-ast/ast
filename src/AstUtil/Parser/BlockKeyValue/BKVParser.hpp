///
/// @file      BlockKeyValueParser.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-08
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
#include "AstUtil/StringView.hpp"
#include <stdio.h>

AST_NAMESPACE_BEGIN

class BKVSax;

/// @brief 键值对解析器（BlockKeyValueParser）
/// @details 本类实现了键值对解析器，用于解析键值对格式的文本。
class AST_UTIL_API BKVParser 
{
public:
    BKVParser();
    BKVParser(StringView filepath);
    ~BKVParser();
    
    err_t parseFile(const StringView filepath, BKVSax& sax);
    err_t parse(BKVSax& sax);
protected:
    void closeFile();
    void openFile(StringView filepath);
    FILE* getFile() const { return file_; }
protected:
    FILE* file_;
};


AST_NAMESPACE_END
