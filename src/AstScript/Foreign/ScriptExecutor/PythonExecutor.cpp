///
/// @file      PythonExecutor.cpp
/// @brief     Python 脚本执行器实现
/// @details   基于 PythonAPI 单例，每个 executor 持有一个私有无名 dict 作为命名空间。
///            多个 executor 之间变量完全隔离。
/// @author    axel
/// @date      2026-05-16
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

#include "PythonExecutor.hpp"
#include "AstScript/PythonAPI.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN


PythonExecutor::PythonExecutor()
    : api_(PythonAPI::Instance())
{
}


PythonExecutor::~PythonExecutor()
{
    finalize();
}


errc_t PythonExecutor::initialize()
{
    if (!api_->isLoaded())
    {
        lastError_ = "Python shared library not loaded";
        return eErrorInvalidFile;
    }
    api_->Py_Initialize();

    if (!globals_)
    {
        globals_ = api_->PyDict_New();
        if (!globals_)
        {
            lastError_ = captureError();
            return eError;
        }
    }

    lastError_.clear();
    return eNoError;
}


void PythonExecutor::finalize()
{
    if (globals_)
    {
        api_->Py_DecRef(globals_);
        globals_ = nullptr;
    }
}


errc_t PythonExecutor::execute(StringView script, ScriptResult* resultOut)
{
    auto* pyResult = api_->PyRun_String(
        std::string(script).c_str(),
        PythonAPI::Py_file_input,
        globals_, globals_);

    if (!pyResult)
    {
        lastError_ = captureError();
        if (resultOut)
            resultOut->error_ = lastError_;
        return eError;
    }

    api_->Py_DecRef(pyResult);
    if (resultOut)
        resultOut->error_.clear();
    return eNoError;
}


errc_t PythonExecutor::evaluate(StringView expression, ScriptResult* resultOut)
{
    auto* result = api_->PyRun_String(
        std::string(expression).c_str(),
        PythonAPI::Py_eval_input,
        globals_, globals_);

    if (!result)
    {
        lastError_ = captureError();
        if (resultOut) resultOut->error_ = lastError_;
        return eError;
    }

    if (resultOut)
    {
        resultOut->error_.clear();
        auto* boolType   = api_->PyBool_Type();
        auto* longType   = api_->PyLong_Type();
        auto* floatType  = api_->PyFloat_Type();
        auto* unicodeType = api_->PyUnicode_Type();

        if (boolType && api_->PyObject_IsInstance(result, boolType))
        {
            resultOut->value_ = aNewValueBool(api_->PyLong_AsLong(result) != 0);
        }
        else if (longType && api_->PyObject_IsInstance(result, longType))
        {
            resultOut->value_ = aNewValueInt(static_cast<int>(api_->PyLong_AsLong(result)));
        }
        else if (floatType && api_->PyObject_IsInstance(result, floatType))
        {
            resultOut->value_ = aNewValueDouble(api_->PyFloat_AsDouble(result));
        }
        else if (unicodeType && api_->PyObject_IsInstance(result, unicodeType))
        {
            const char* s = api_->PyUnicode_AsUTF8(result);
            resultOut->value_ = aNewValueString(s ? s : "");
        }
        else
        {
            auto* strObj = api_->PyObject_Str(result);
            if (strObj)
            {
                const char* s = api_->PyUnicode_AsUTF8(strObj);
                resultOut->value_ = aNewValueString(s ? s : "");
                api_->Py_DecRef(strObj);
            }
        }
    }

    api_->Py_DecRef(result);
    return eNoError;
}


std::string PythonExecutor::getLastError() const
{
    return lastError_;
}


errc_t PythonExecutor::setVariable(StringView name, StringView value)
{
    auto* pyVal = api_->PyUnicode_FromString(std::string(value).c_str());
    if (!pyVal) { captureError(); return eError; }

    int rc = api_->PyDict_SetItemString(globals_, std::string(name).c_str(), pyVal);
    api_->Py_DecRef(pyVal);
    if (rc != 0) { captureError(); return eError; }
    return eNoError;
}


errc_t PythonExecutor::setVariable(StringView name, double value)
{
    auto* pyVal = api_->PyFloat_FromDouble(value);
    if (!pyVal) { captureError(); return eError; }

    int rc = api_->PyDict_SetItemString(globals_, std::string(name).c_str(), pyVal);
    api_->Py_DecRef(pyVal);
    if (rc != 0) { captureError(); return eError; }
    return eNoError;
}


errc_t PythonExecutor::setVariable(StringView name, int value)
{
    auto* pyVal = api_->PyLong_FromLong(value);
    if (!pyVal) { captureError(); return eError; }

    int rc = api_->PyDict_SetItemString(globals_, std::string(name).c_str(), pyVal);
    api_->Py_DecRef(pyVal);
    if (rc != 0) { captureError(); return eError; }
    return eNoError;
}


errc_t PythonExecutor::setVariable(StringView name, bool value)
{
    auto* pyVal = api_->PyBool_FromLong(value ? 1 : 0);
    if (!pyVal) { captureError(); return eError; }

    int rc = api_->PyDict_SetItemString(globals_, std::string(name).c_str(), pyVal);
    api_->Py_DecRef(pyVal);
    if (rc != 0) { captureError(); return eError; }
    return eNoError;
}


errc_t PythonExecutor::getVariable(StringView name, std::string& value) const
{
    auto* pyVal = api_->PyDict_GetItemString(globals_, std::string(name).c_str());
    if (!pyVal) return eError;

    auto* strObj = api_->PyObject_Str(pyVal);
    if (!strObj) { captureError(); return eError; }

    const char* s = api_->PyUnicode_AsUTF8(strObj);
    if (s) value = s;
    api_->Py_DecRef(strObj);
    return s ? eNoError : eError;
}


errc_t PythonExecutor::getVariable(StringView name, double& value) const
{
    auto* pyVal = api_->PyDict_GetItemString(globals_, std::string(name).c_str());
    if (!pyVal) return eError;

    double tmp = api_->PyFloat_AsDouble(pyVal);
    if (api_->PyErr_Occurred()) { captureError(); return eError; }
    value = tmp;
    return eNoError;
}


errc_t PythonExecutor::getVariable(StringView name, int& value) const
{
    auto* pyVal = api_->PyDict_GetItemString(globals_, std::string(name).c_str());
    if (!pyVal) return eError;

    // 统一用 PyFloat_AsDouble 做数值转换，对 int/float/bool 均有效
    double tmp = api_->PyFloat_AsDouble(pyVal);
    if (api_->PyErr_Occurred()) { captureError(); return eError; }
    value = static_cast<int>(tmp);
    return eNoError;
}


errc_t PythonExecutor::getVariable(StringView name, bool& value) const
{
    auto* pyVal = api_->PyDict_GetItemString(globals_, std::string(name).c_str());
    if (!pyVal) return eError;

    double tmp = api_->PyFloat_AsDouble(pyVal);
    if (api_->PyErr_Occurred()) { captureError(); return eError; }
    value = (tmp != 0.0);
    return eNoError;
}


std::string PythonExecutor::captureError() const
{
    PyObject *type = nullptr, *value = nullptr, *traceback = nullptr;
    api_->PyErr_Fetch(&type, &value, &traceback);

    std::string msg;
    if (value)
    {
        auto* strObj = api_->PyObject_Str(value);
        if (strObj)
        {
            const char* s = api_->PyUnicode_AsUTF8(strObj);
            if (s) msg = s;
            api_->Py_DecRef(strObj);
        }
        api_->Py_DecRef(value);
    }
    if (type)      api_->Py_DecRef(type);
    if (traceback) api_->Py_DecRef(traceback);

    return msg;
}


AST_NAMESPACE_END
