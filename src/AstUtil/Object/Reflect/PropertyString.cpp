///
/// @file      PropertyString.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-09
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

#include "PropertyString.hpp"
#include "AstUtil/ParseFormat.hpp"

AST_NAMESPACE_BEGIN


err_t PropertyString::getValueBool(const void* container, bool& value)
{
    std::string str;
    err_t ret = this->getValue(container, &str);
    if (ret != eNoError)
    {
        return ret;
    }
    return aParseBool(str, value);
}
err_t PropertyString::setValueBool(void* container, bool value) 
{
    std::string str = aFormatBool(value);
    StringView strview = str;
    return this->setValue(container, &strview);
}

err_t PropertyString::getValueInt(const void* container, int& value) 
{
    std::string str;
    err_t ret = this->getValue(container, &str);
    if (ret != eNoError)
    {
        return ret;
    }
    return aParseInt(str, value);
}

err_t PropertyString::setValueInt(void* container, int value) 
{
    std::string str;
    err_t err = aFormatInt(value, str);
    if (err != eNoError)
    {
        return err;
    }
    StringView strview = str;
    return this->setValue(container, &strview);
}

err_t PropertyString::getValueString(const void* container, std::string& value)
{
    return this->getValue(container, &value);
}

err_t PropertyString::setValueString(void* container, StringView value)
{
    return this->setValue(container, &value);
}

err_t PropertyString::getValueDouble(const void* container, double& value)
{
    std::string str;
    err_t ret = this->getValue(container, &str);
    if (ret != eNoError)
    {
        return ret;
    }
    return aParseDouble(str, value);
}

err_t PropertyString::setValueDouble(void* container, double value)
{
    std::string str;
    err_t err = aFormatDouble(value, str);
    if (err != eNoError)
    {
        return err;
    }
    StringView strview = str;
    return this->setValue(container, &strview);
}

AST_NAMESPACE_END