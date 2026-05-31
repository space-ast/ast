///
/// @file      PythonAPI.hpp
/// @brief     Python C API 动态加载封装
/// @details   参考 SpiceAPI 模式，动态加载 Python 共享库并提供核心 C API 的包装。
///            公开方法名和签名与 Python C API 保持严格一致。
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

#pragma once

#include "AstGlobal.h"
#include <mutex>
#include <vector>
#include <string>
#include <array>

#ifndef SWIG  // PythonAPI 为内部实现类，不暴露给 SWIG

// Python 类型前置声明（不依赖 Python.h，保证动态加载场景下的编译独立性）
struct _object;
typedef struct _object PyObject;


AST_NAMESPACE_BEGIN


/*!
    @addtogroup Script
    @{
*/

/// @brief Python API — 与 Python C API 接口一致的动态加载包装
/// @details 通过动态加载 Python 共享库的方式提供 Python C API 的核心函数。
/// 公开方法名和签名与 Python C API 严格一致，但底层为运行时动态解析。
class AST_SCRIPT_API PythonAPI
{
public:
    // 函数索引
    enum{
        iPy_Initialize = 0,
        iPy_FinalizeEx,
        iPy_IsInitialized,
        iPyRun_SimpleString,
        iPyImport_ImportModule,
        iPyObject_GetAttrString,
        iPyObject_CallObject,
        iPy_DecRef,
        iPy_IncRef,
        iPyErr_Print,
        iPyErr_Occurred,
        iPyUnicode_AsUTF8,
        iPyRun_String,
        iPyLong_AsLong,
        iPyFloat_AsDouble,
        iPyLong_FromLong,
        iPyFloat_FromDouble,
        iPyUnicode_FromString,
        iPyObject_SetAttrString,
        iPyObject_IsInstance,
        iPyErr_Fetch,
        iPyObject_Str,
        iPyFloat_Type,
        iPyLong_Type,
        iPyBool_Type,
        iPyUnicode_Type,
        iPyBool_FromLong,
        iPyDict_New,
        iPyDict_SetItemString,
        iPyDict_GetItemString,
        iPyGILState_Ensure,
        iPyGILState_Release,
        numfunctions,
    };

    // eval 模式常量（匿名 enum，SWIG 兼容）
    enum{
        Py_eval_input = 258,
        Py_file_input = 257,
        Py_single_input = 256,
    };
    using funcarray = std::array<void*, numfunctions>;

    /// @brief 获取单例实例
    static PythonAPI* Instance();

    PythonAPI() = default;

    explicit PythonAPI(bool shouldLoadDynamicLib);

    ~PythonAPI();

    /// @brief 加载指定路径的Python动态库
    errc_t load(StringView libpath);

    /// @brief 尝试加载库，按顺序尝试直到成功
    errc_t tryload(const std::vector<std::string>& libpaths);

    /// @brief 卸载库
    errc_t unload();

    /// @brief 检查是否已加载库
    bool isLoaded() const { return library_ != nullptr; }

public: // -- 以下接口名称和签名与 Python C API 完全一致 --

    void Py_Initialize();

    int Py_FinalizeEx();

    int Py_IsInitialized();

    int PyRun_SimpleString(const char* command);

    PyObject* PyImport_ImportModule(const char* name);

    PyObject* PyObject_GetAttrString(PyObject* o, const char* attr_name);

    PyObject* PyObject_CallObject(PyObject* callable, PyObject* args);

    void Py_DecRef(PyObject* o);

    void Py_IncRef(PyObject* o);

    void PyErr_Print();

    PyObject* PyErr_Occurred();

    const char* PyUnicode_AsUTF8(PyObject* unicode);

    // -- 以下为 PythonExecutor 所需的新增接口 --

    PyObject* PyRun_String(const char* str, int start, PyObject* globals, PyObject* locals);

    long PyLong_AsLong(PyObject* o);
    double PyFloat_AsDouble(PyObject* o);

    PyObject* PyLong_FromLong(long v);
    PyObject* PyFloat_FromDouble(double v);
    PyObject* PyUnicode_FromString(const char* u);
    PyObject* PyBool_FromLong(long v);

    PyObject* PyDict_New();
    int PyDict_SetItemString(PyObject* dict, const char* key, PyObject* val);
    PyObject* PyDict_GetItemString(PyObject* dict, const char* key);

    int PyObject_SetAttrString(PyObject* o, const char* attr_name, PyObject* v);
    int PyObject_IsInstance(PyObject* obj, PyObject* cls);

    void PyErr_Fetch(PyObject** ptype, PyObject** pvalue, PyObject** ptraceback);
    PyObject* PyObject_Str(PyObject* o);

    // GIL 管理（多线程场景）
    int PyGILState_Ensure();
    void PyGILState_Release(int gstate);

    // 类型对象访问器（从 DLL 加载的数据符号）
    PyObject* PyFloat_Type()   const { return static_cast<PyObject*>(functions_[iPyFloat_Type]); }
    PyObject* PyLong_Type()    const { return static_cast<PyObject*>(functions_[iPyLong_Type]); }
    PyObject* PyBool_Type()    const { return static_cast<PyObject*>(functions_[iPyBool_Type]); }
    PyObject* PyUnicode_Type() const { return static_cast<PyObject*>(functions_[iPyUnicode_Type]); }

protected:
    void checkerror();
    A_DISABLE_COPY(PythonAPI);

protected:
    void*  library_{nullptr};
    funcarray functions_{};
    std::mutex mutex_;
};

/*! @} */

AST_NAMESPACE_END

#endif // SWIG
