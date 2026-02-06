///
/// @file      BKVSax.hpp
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
#include "AstUtil/GenericValue.hpp"
#include "AstUtil/Color.hpp"

AST_NAMESPACE_BEGIN

class ValueView;

/// @brief 键值对解析器SAX接口（BlockKeyValueSax）
/// @details 本接口定义了键值对解析器的SAX接口，用于解析键值对格式的文本。
class AST_UTIL_API BKVSax
{
public:
    /// @brief 析构函数
    virtual ~BKVSax() = default;

    /// @brief 开始解析一个块
    /// @param name 块名
    virtual void begin(StringView name) = 0;
    
    /// @brief 结束解析一个块
    /// @param name 块名
    virtual void end(StringView name) = 0;
    
    /// @brief 迭代解析一个键值对
    /// @param key 键
    /// @param value 值
    virtual void keyValue(StringView key, const GenericValue& value) = 0;

    void begin() { begin(StringView{});}
    void end() { end(StringView{});}
    void keyValue(StringView key, const char* value){ keyValue(key, GenericValue(value)); }
    void keyValue(StringView key, int value){ keyValue(key, GenericValue(value)); }
    void keyValue(StringView key, bool value){ keyValue(key, GenericValue(value)); }
    void keyValue(StringView key, double value){ keyValue(key, GenericValue(value)); }
    void keyValue(StringView key, Color value){ keyValue(key, GenericValue(value)); }
    void keyValue(StringView key, StringView value){ keyValue(key, GenericValue(value)); }
};


AST_NAMESPACE_END
