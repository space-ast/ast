///
/// @file      PythonExecutor.hpp
/// @brief     Python 脚本执行器
/// @details   基于 PythonAPI 单例，实现 ScriptExecutor 接口，
///            支持 Python 语句执行和表达式求值。
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
#include "ScriptExecutor.hpp"

struct _object;
typedef struct _object PyObject;

AST_NAMESPACE_BEGIN

class PythonAPI;

/// @brief Python 脚本执行器
/// @details 通过 PythonAPI 单例驱动 CPython 解释器，
///          多个 PythonExecutor 实例共享同一解释器状态。
class AST_SCRIPT_API PythonExecutor : public ScriptExecutor
{
public:
    PythonExecutor();
    ~PythonExecutor() override;

    errc_t initialize() override;
    void finalize() override;
    errc_t execute(StringView script, ScriptResult* resultOut = nullptr) override;
    errc_t evaluate(StringView expression, ScriptResult* resultOut = nullptr) override;
    std::string getLastError() const override;

    errc_t setVariable(StringView name, StringView value) override;
    errc_t setVariable(StringView name, double value) override;
    errc_t setVariable(StringView name, int value) override;
    errc_t setVariable(StringView name, bool value) override;

    // 防止 const char* 被隐式转换为 bool
    errc_t setVariable(StringView name, const char* value) {
        return setVariable(name, StringView(value));
    }

    errc_t getVariable(StringView name, std::string& value) const override;
    errc_t getVariable(StringView name, double& value) const override;
    errc_t getVariable(StringView name, int& value) const override;
    errc_t getVariable(StringView name, bool& value) const override;

protected:
    std::string captureError() const;

protected:
    PythonAPI* api_;
    PyObject* globals_{nullptr};  // 私有无名 dict，每个 executor 独立命名空间
    mutable std::string lastError_;
};

AST_NAMESPACE_END
