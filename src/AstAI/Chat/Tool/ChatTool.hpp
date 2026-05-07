///
/// @file      AITool.hpp
/// @brief     AI工具定义
/// @author    axel
/// @date      2026-04-23
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/JsonValue.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @defgroup Chat
    @brief AI大语言模型相关接口
    @{
*/

class JsonValue;

/// @brief AI工具基类
/// @details 所有AI工具的基类，定义了工具的基本属性和方法
class ChatTool
{
public:
    ChatTool() = default;
    
    virtual ~ChatTool() = default;
    /// @brief 调用工具
    /// @param arguments 函数参数
    /// @return 执行结果
    virtual std::string call(const JsonValue& arguments) = 0;

    /// @brief 获取工具名称
    /// @return 工具名称
    const std::string& name() const { return name_; }

    /// @brief 设置工具名称
    /// @param name 工具名称
    void setName(StringView name){name_ = std::string(name);}
    
    /// @brief 获取工具描述
    /// @return 工具描述
    const std::string& description() const { return description_; }

    /// @brief 设置工具描述
    /// @param description 工具描述
    void setDescription(StringView description){description_ = std::string(description);}

    /// @brief 获取工具参数schema
    /// @return 工具参数schema
    const JsonValue& parameters() const { return parameters_; }

    /// @brief 设置工具参数schema
    /// @param parameters 工具参数schema
    void setParameters(const JsonValue& parameters){parameters_ = parameters;}

    /// @brief 转换为JSON值
    /// @return JSON值
    JsonValue toJson() const;
private:
    std::string name_;              ///< 工具名称
    std::string description_;       ///< 工具描述
    JsonValue parameters_;          ///< 工具参数schema
};

/// @brief 通用模板封装
template<typename Func>
class ChatToolGeneric: public ChatTool
{
public:
    ChatToolGeneric(Func func) : func_(std::move(func)) {}
    std::string call(const JsonValue& arguments) override { return func_(arguments); }
private:
    Func func_;
};


/*! @} */


AST_NAMESPACE_END