///
/// @file      PropertyDouble.cpp
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

#include "PropertyDouble.hpp"
#include "AstUtil/ParseFormat.hpp"

AST_NAMESPACE_BEGIN

err_t PropertyDouble::getValueBool(void* container, bool& value)
{
    double d;
    err_t ret = this->getValue(container, &d);
    if (ret != eNoError)
    {
        return ret;
    }
    value = (d != 0.0);
    return eNoError;
}
err_t PropertyDouble::setValueBool(void* container, bool value) 
{
    double d = value ? 1.0 : 0.0;
    return this->setValue(container, &d);
}
err_t PropertyDouble::getValueInt(void* container, int& value) 
{
    double d;
    err_t ret = this->getValue(container, &d);
    if (ret != eNoError)
    {
        return ret;
    }
    value = (int)d;
    return eNoError;
}

err_t PropertyDouble::setValueInt(void* container, int value) 
{
    double d = (double)value;
    return this->setValue(container, &d);
}

err_t PropertyDouble::getValueString(void* container, std::string& value)
{
    double d;
    err_t ret = this->getValue(container, &d);
    if (ret != eNoError)
    {
        return ret;
    }
    return aFormatDouble(d, value);
}

err_t PropertyDouble::setValueString(void* container, StringView value)
{
    double d;
    err_t ret = aParseDouble(value, d);
    if (ret != eNoError)
    {
        return ret;
    }
    return this->setValue(container, &d);
}

err_t PropertyDouble::getValueDouble(void* container, double& value)
{
    return this->getValue(container, &value);
}

err_t PropertyDouble::setValueDouble(void* container, double value)
{
    return this->setValue(container, &value);
}

AST_NAMESPACE_END