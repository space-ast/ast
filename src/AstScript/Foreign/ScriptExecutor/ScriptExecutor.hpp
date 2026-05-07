///
/// @file      ScriptExecutor.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-29
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
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 脚本类型
enum class EScriptLanguage
{
    ePython,            ///< Python 脚本
    eMATLAB,            ///< MATLAB 脚本
    eJavaScript,        ///< JavaScript 脚本
    eJScript,           ///< JScript 脚本
    eVBScript,          ///< VBScript 脚本
    eJulia,             ///< Julia 脚本
    eLua,               ///< Lua 脚本
    eAstScript,         ///< AstScript 脚本
};


AST_SCRIPT_API std::string toString(EScriptLanguage type);

class ScriptExecutor;
class Variable;

/// @brief 创建脚本执行器实例
/// @param type 脚本类型
/// @return 脚本执行器实例指针
/// @note 脚本执行器实例的生命周期由调用方负责管理，调用方需要在使用完成后调用析构函数释放资源
AST_SCRIPT_CAPI ScriptExecutor* aNewScriptExecutor(EScriptLanguage type);




/// @brief 脚本执行器，用于执行外部脚本
/// @note 该类是一个抽象类，需要通过继承来实现具体的脚本执行器
/// @details 用于执行外部脚本，如Python、MATLAB、JavaScript、JScript、VBScript、Julia、Lua 等
/// 每个脚本执行器实例的生命周期由调用方负责管理，调用方需要在使用完成后调用析构函数释放资源
/// 在脚本执行器实例的生命周期内，调用方可以调用execute函数执行执行外部脚本，
/// 并调用setVariable函数设置脚本执行器的变量值，调用getVariable函数获取脚本执行器的变量值
class AST_SCRIPT_API ScriptExecutor
{
public:
    /// @brief 析构函数
    /// @details 释放所有资源
    virtual ~ScriptExecutor() = default;

    /// @brief 初始化脚本执行器
    /// @details 初始化脚本执行器，准备执行脚本
    /// @return 初始化结果
    virtual errc_t initialize() = 0;

    /// @brief 结束脚本执行器
    /// @details 结束脚本执行器，释放所有资源
    virtual void finalize() = 0;

    /// @brief 执行脚本
    /// @param script 要执行的脚本内容
    /// @param errorOut 错误信息指针，用于存储执行脚本时的错误信息
    /// @return 执行结果
    virtual errc_t execute(StringView script, std::string* errorOut=nullptr) = 0;

    /// @brief 获取最近一次执行脚本时的错误信息
    /// @warning 注意只能在execute函数执行失败后调用
    /// 不要通过调用该函数来判断脚本是否执行成功
    /// @return 最近一次执行脚本时的错误信息
    virtual std::string getLastError() const = 0;

    /// @brief 设置脚本执行器的变量
    /// @param name 变量名称
    /// @param value 变量值
    /// @return 设置结果
    virtual errc_t setVariable(StringView name, StringView value) = 0;
    virtual errc_t setVariable(StringView name, double value) = 0;
    virtual errc_t setVariable(StringView name, int value) = 0;
    virtual errc_t setVariable(StringView name, bool value) = 0;
    errc_t setVariable(Variable* var);

    /// @brief 获取脚本执行器的字符串变量值
    /// @param name 变量名称
    /// @param value 变量值
    /// @return 获取结果
    virtual errc_t getVariable(StringView name, std::string& value) const = 0;
    virtual errc_t getVariable(StringView name, double& value) const = 0;
    virtual errc_t getVariable(StringView name, int& value) const = 0;
    virtual errc_t getVariable(StringView name, bool& value) const = 0;
    errc_t getVariable(Variable* var);
};


/*! @} */

AST_NAMESPACE_END
