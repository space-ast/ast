///
/// @file      ComInit.hpp
/// @brief     COM 初始化工具
/// @details   提供 RAII 风格的 COM 初始化和释放守卫，以及线程局部 COM 初始化函数。
///           从 ActiveScriptExecutor.cpp 中抽取，供 Shell COM 等模块复用。
/// @author    axel
/// @date      2026-07-25
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

#if defined(_WIN32) && !defined(SWIG)
#include <Windows.h>
#endif

AST_NAMESPACE_BEGIN

#if defined(_WIN32) && !defined(SWIG)

/// COM 初始化和释放守卫
/// @details RAII 风格，构造时调用 CoInitializeEx，析构时调用 CoUninitialize
class CoInitializeGuard
{
public:
    explicit CoInitializeGuard(COINIT initFlags) : hr_(CoInitializeEx(nullptr, initFlags)) {}
    ~CoInitializeGuard() { if (SUCCEEDED(hr_)) CoUninitialize(); }
    CoInitializeGuard(const CoInitializeGuard&) = delete;
    CoInitializeGuard& operator=(const CoInitializeGuard&) = delete;
    HRESULT hr_{};
};

/// 确保当前线程 COM 已初始化
/// @details 使用 thread_local 静态守卫，每个线程仅初始化一次（STA 模式）
/// @return S_OK 表示成功，否则返回失败 HRESULT
AST_UTIL_API HRESULT aEnsureCoInitialized();

#else

// 非 Windows 平台：空实现
class CoInitializeGuard
{
public:
    explicit CoInitializeGuard(int) {}
    int hr_ = 0;
};

A_ALWAYS_INLINE int aEnsureCoInitialized() { return 0; }

#endif

AST_NAMESPACE_END
