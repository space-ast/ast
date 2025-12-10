///
/// @file      PropertyInt.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-10
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

#include "PropertyInt.hpp"
#include "AstUtil/ParseFormat.hpp"

AST_NAMESPACE_BEGIN

err_t PropertyInt::getValueBool(void* container, bool& value)
{
    int i;
    err_t ret = this->getValue(container, &i);
    if (ret != eNoError)
    {
        return ret;
    }
    value = (i != 0);
    return eNoError;
}
err_t PropertyInt::setValueBool(void* container, bool value) 
{
    int i = value ? 1 : 0;
    return this->setValue(container, &i);
}
err_t PropertyInt::getValueInt(void* container, int& value) 
{
    return this->getValue(container, &value);
}

err_t PropertyInt::setValueInt(void* container, int value) 
{
    return this->setValue(container, &value);
}

err_t PropertyInt::getValueString(void* container, std::string& value)
{
    int i;
    err_t ret = this->getValue(container, &i);
    if (ret != eNoError)
    {
        return ret;
    }
    return aFormatInt(i, value);
}

err_t PropertyInt::setValueString(void* container, StringView value)
{
    int i;
    err_t ret = aParseInt(value, i);
    if (ret != eNoError)
    {
        return ret;
    }
    return this->setValue(container, &i);
}

err_t PropertyInt::getValueDouble(void* container, double& value)
{
    int i;
    err_t ret = this->getValue(container, &i);
    if (ret != eNoError)
    {
        return ret;
    }
    value = (double)i;
    return eNoError;
}

err_t PropertyInt::setValueDouble(void* container, double value)
{
    int i = (int)value;
    return this->setValue(container, &i);
}

AST_NAMESPACE_END
