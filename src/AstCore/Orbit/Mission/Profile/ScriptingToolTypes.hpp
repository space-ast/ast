///
/// @file      ScriptingToolTypes.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

class ScriptingToolParameterEnumChoice;

/// @brief 脚本工具属性
class AST_CORE_API ScriptingToolAttr : public ObjectNamed
{
public:

    ScriptingToolAttr() = default;
    ~ScriptingToolAttr() override = default;

    std::string attribute_;
    std::string category_;
    bool clonable_ = true;
    std::string description_;
    std::string lastRunFinalValue_;
    std::string lastRunInitialValue_;
    std::string objectPath_;
    bool readOnlyProperty_ = false;
    bool readOnly_ = false;
    int stkVersion_ = 1160;
    std::string type_;
    std::string unit_;
    std::string userComment_;
    std::string version_;
};

/// @brief 脚本工具参数
class AST_CORE_API ScriptingToolParameter : public ObjectNamed
{
public:

    ScriptingToolParameter() = default;
    ~ScriptingToolParameter() override = default;

    std::string category_;
    bool clonable_ = true;
    std::string description_;
    bool inheritValue_ = false;
    std::string lastRunValue_;
    double maxValue_ = 0.0;
    double minValue_ = 0.0;
    std::string paramType_;
    double paramValue_ = 0.0;
    bool readOnly_ = false;
    int stkVersion_ = 1160;
    std::string unitDimension_;
    std::string unit_;
    bool useMaxValue_ = false;
    bool useMinValue_ = false;
    std::string userComment_;
    std::string version_;

    std::vector<SharedPtr<ScriptingToolParameterEnumChoice>> enumChoices_;
};

/// @brief 脚本工具参数枚举选项
class AST_CORE_API ScriptingToolParameterEnumChoice : public ObjectNamed
{
public:

    ScriptingToolParameterEnumChoice() = default;
    ~ScriptingToolParameterEnumChoice() override = default;

    std::string category_;
    bool clonable_ = true;
    std::string description_;
    int enumValue_ = 0;
    bool readOnly_ = false;
    int stkVersion_ = 1160;
    std::string type_;
    std::string userComment_;
    std::string version_;
};

/// @brief 脚本工具计算对象包装器
class AST_CORE_API AgScriptingToolCalcObjWrap : public ObjectNamed
{
public:

    AgScriptingToolCalcObjWrap() = default;
    ~AgScriptingToolCalcObjWrap() override = default;

    std::string category_;
    bool clonable_ = true;
    std::string description_;
    std::string lastRunValue_;
    bool readOnly_ = false;
    int stkVersion_ = 1160;
    std::string type_;
    std::string unit_;
    std::string userComment_;
    std::string version_;
};

AST_NAMESPACE_END