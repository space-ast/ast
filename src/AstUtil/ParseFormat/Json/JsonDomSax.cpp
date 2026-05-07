///
/// @file      JsonDomSax.cpp
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

#include "JsonDomSax.hpp"
#include "AstUtil/Logger.hpp"
#include <cassert>
#include <utility>


AST_NAMESPACE_BEGIN

JsonDomSax::JsonDomSax(JsonValue& result)
    : error_(false)
    , internalResult_()
    , result_(result)
{
    stack_.push(&result_);
}



JsonDomSax::JsonDomSax()
    : JsonDomSax(internalResult_)
{
}


const JsonValue& JsonDomSax::getResult() const
{
    return result_;
}

bool JsonDomSax::hasError() const
{
    return error_;
}

errc_t JsonDomSax::startObject()
{
    if (error_)
        return kStopParsing;
    currentValue().setObject();
    return 0;
}

errc_t JsonDomSax::endObject()
{
    if (error_)
        return kStopParsing;

    stack_.pop();
    return 0;
}

errc_t JsonDomSax::startArray()
{
    if (error_)
        return kStopParsing;
    currentValue().setArray();
    return 0;
}

errc_t JsonDomSax::endArray()
{
    if (error_)
        return kStopParsing;

    stack_.pop();
    return 0;
}

errc_t JsonDomSax::key(StringView key)
{
    if (error_)
        return kStopParsing;
    if (currentValue().type() != EJsonValueType::eObject)
    {
        error_ = true;
        aError("Key outside of object context.");
        return kStopParsing;
    }
    JsonValue& childValue = currentValue()[std::string(key)];
    stack_.push(&childValue);
    return 0;
}


errc_t JsonDomSax::index(size_t index)
{
    if (error_)
        return kStopParsing;
    if (currentValue().type() != EJsonValueType::eArray)
    {
        error_ = true;
        aError("Index outside of array context.");
        return kStopParsing;
    }
    JsonValue& childValue = currentValue()[index];
    stack_.push(&childValue);
    return 0;
}


errc_t JsonDomSax::nullValue()
{
    currentValue().setNull();
    stack_.pop();
    return 0;
}

errc_t JsonDomSax::boolValue(bool value)
{
    currentValue().setBool(value);
    stack_.pop();
    return 0;
}

errc_t JsonDomSax::intValue(int value)
{
    currentValue().setInt(value);
    stack_.pop();
    return 0;
}

errc_t JsonDomSax::doubleValue(double value)
{
    currentValue().setDouble(value);
    stack_.pop();
    return 0;
}

errc_t JsonDomSax::stringValue(StringView value)
{
    currentValue().setString(value);
    stack_.pop();
    return 0;
}

errc_t JsonDomSax::parseError(StringView message)
{
    // 格式化错误信息
    aError("%.*s", message.size(), message.data());
    error_ = true;
    return kStopParsing;  // 停止解析
}

JsonValue &JsonDomSax::currentValue()
{
    if(stack_.empty())
    {
        assert(!stack_.empty());
        stack_.push(&result_);
    }
    return *stack_.top();
}

AST_NAMESPACE_END

