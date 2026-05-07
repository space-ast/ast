///
/// @file      JsonDomSax.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-24
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
#include "JsonSax.hpp"
#include "JsonValue.hpp"
#include <stack>
#include <string>


AST_NAMESPACE_BEGIN


/*!
    @addtogroup 
    @{
*/


/// @brief 基于 SAX 事件构建 JSON DOM 的类
class AST_UTIL_API JsonDomSax : public JsonSax
{
public:
    JsonDomSax();
    JsonDomSax(JsonValue& result);
    ~JsonDomSax() override = default;

    /// @brief 获取构建完成的 JsonValue
    const JsonValue& getResult() const;

    /// @brief 解析过程中是否发生错误
    bool hasError() const;
 

    // SAX 事件处理
    errc_t startObject() override;
    errc_t endObject() override;
    errc_t startArray() override;
    errc_t endArray() override;
    errc_t key(StringView key) override;
    errc_t index(size_t index) override;
    errc_t nullValue() override;
    errc_t boolValue(bool value) override;
    errc_t intValue(int value) override;
    errc_t doubleValue(double value) override;
    errc_t stringValue(StringView value) override;
    errc_t parseError(StringView message) override;

private:
    // 错误/中止用的错误码
    static const errc_t kStopParsing = -1;

    /// @brief 获取当前值
    JsonValue& currentValue();
private:
    bool                   error_ = false;           ///< 是否发生错误
    std::stack<JsonValue*> stack_;                   ///< 嵌套结构栈
    JsonValue              internalResult_;          ///< 如果没有传入JsonValue，则用于存储最终结果
    JsonValue&             result_;                  ///< 最终结果
};

/*! @} */

AST_NAMESPACE_END