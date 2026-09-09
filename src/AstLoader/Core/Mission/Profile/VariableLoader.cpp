///
/// @file      VariableLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-01
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

#include "VariableLoader.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/ExprAttribute.hpp"
#include "AstScript/ExprCalculation.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/ObjectLinker.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/ScStateCalculation.hpp"
#include "AstLoader/AttributeResolve.hpp"
#include "AstLoader/ResultLoader.hpp"
#include "AstScript/ExprCalculation.hpp"

AST_NAMESPACE_BEGIN


class ExprCurrentScStateCalculation: public ExprCalculationBase
{
public:
    static ExprCurrentScStateCalculation* New(ScStateCalculation* calculation=nullptr);

    explicit ExprCurrentScStateCalculation(ScStateCalculation* calculation=nullptr);
    Value* eval() const override;
    void accept(ExprVisitor& visitor) override;
    std::string getExpression(Object* scope) const override;
    void setCalculation(ScStateCalculation* calculation);
private:
    WeakPtr<ScStateCalculation> calculation_;
};


ExprCurrentScStateCalculation* ExprCurrentScStateCalculation::New(ScStateCalculation* calculation)
{
    return new ExprCurrentScStateCalculation(calculation);
}


ExprCurrentScStateCalculation::ExprCurrentScStateCalculation(ScStateCalculation* calculation)
    : calculation_(calculation)
{}

void ExprCurrentScStateCalculation::accept(ExprVisitor& visitor)
{
    visitor.visitExpr(*this);
}

Value* ExprCurrentScStateCalculation::eval() const
{
    auto calculation = calculation_.lock();
    if(calculation == nullptr)
    {
        aError(_("计算对象为空"));
        return nullptr;
    }
    Sequence* sequence = aGetAncestorScope<Sequence*>(const_cast<ExprCurrentScStateCalculation*>(this));
    if(sequence == nullptr)
    {
        aError(_("获取祖先序列失败"));
        return nullptr;
    }
    auto inputState = sequence->getInputState();
    if(inputState == nullptr)
    {
        aError(_("获取当前输入状态失败"));
        return nullptr;
    }
    double value;
    errc_t rc = calculation->calculate(*inputState, value);
    if(rc != 0)
    {
        aError(_("求值失败"));
        return nullptr;
    }
    return aNewValueDouble(value);
}


std::string ExprCurrentScStateCalculation::getExpression(Object* scope) const
{
    return "CurrentScStateCalculation()";
}

void ExprCurrentScStateCalculation::setCalculation(ScStateCalculation* calculation)
{
    calculation_ = calculation;
}


errc_t aLoadParameter(const Value& value, Variable& var)
{
    // 检查类型
    {
        std::string type = value["Type"];
        if(type != "AgScriptingToolParameter")
        {
            aError(_("无效的类型：'%s'，应为 'AgScriptingToolParameter'"), type.c_str());
            return -1;
        }
    }
    // 加载参数
    std::string paramType = value["ParamType"];
    // 加载数量参数
    if(paramType == "Quantity")
    {
        const Value& valueQuantity = value["ParamValue"];
        var.setExpr(valueQuantity.toQuantity());
    }
    else if(paramType == "Double")
    {
        var.setExpr(value["ParamValue"].toDouble());
    }
    else if(paramType == "Boolean")
    {
        auto& valueParam = const_cast<Value&>(value["ParamValue"]);
        // 如果是string类型，则需要解析为bool
        if(aValueIsString(&valueParam)){
            var.setExpr(aParseBool(valueParam.toString()));
        }
        else{
            var.setExpr(valueParam.toBool());
        }
    }
    else if(paramType == "Enumeration")
    {
        aWarning(_("枚举类型尚未支持，请改用字符串"));
        var.setExpr(value["ParamValue"].toString());
    }
    else if(paramType == "Integer")
    {
        var.setExpr(value["ParamValue"].toInt());
    }
    else if(paramType == "Date")
    {
        aWarning(_("类型 'Date' 尚未支持，请改用字符串"));
        var.setExpr(value["ParamValue"].toString());
    }
    else if(paramType == "String")
    {
        var.setExpr(value["ParamValue"].toString());
    }
    else
    {
        Value* paramValue = const_cast<Value*>(&value["ParamValue"]);
        var.setExpr(paramValue);
        aWarning(_("不支持的参数类型：'%s'"), paramType.c_str());
    }
    return 0;
}

errc_t aLoadCalcObject(const Value& value, Variable& var, Object* scope)
{
    // 检查类型
    {
        std::string type = value["Type"];
        if(type != "AgAsStateCalc")
        {
            aError(_("无效的类型：'%s'，应为 'AgAsStateCalc'"), type.c_str());
            return -1;
        }
    }
    {
        auto& calcObject = value["CalcObject"];
        ExprCurrentScStateCalculation* expr = aNewObject<ExprCurrentScStateCalculation>(scope);
        var.setBindExpr(static_cast<Expr*>(expr));
        SharedPtr<ObjectCalculation> result;
        errc_t rc = aLoadResult(calcObject, result, expr);
        if(rc == 0 && result)
        {
            expr->setCalculation(aobject_cast<ScStateCalculation*>(result.get()));
            return 0;
        }
        else
        {
            aError(_("加载计算对象失败"));
        }
    }
    return 0;
}


errc_t aLoadAttribute(const Value& value, Variable& var, Object* scope)
{
    // 检查类型
    {
        std::string type = value["Type"];
        if(type != "ScriptingToolAttr")
        {
            aError(_("无效的类型：'%s'，应为 'ScriptingToolAttr'"), type.c_str());
            return -1;
        }
    }
    {
        std::string object = value["Object"];
        std::string attribute = value["Attribute"];
        std::string unit = value["Unit"];
        Variable* variable = &var;
        WeakPtr<Object> scopePtr(scope);
        var.addDelayedLink([variable, object, attribute, unit, scopePtr]() {
            auto sequence = aobject_cast<Sequence*>(scopePtr.get());
            if(!sequence)
            {
                std::string typeName;
                if(auto object = scopePtr.get())
                    typeName = object->getType()->name();
                else
                    typeName = "nullptr";
                aError(_("作用域类型应为 sequence，但实际为 %s"), typeName.c_str());
                return -1;
            }
            MissionCommand* command = nullptr;
            // 查找段
            {
                StringView objectPath(object);
                if(objectPath.starts_with("Segments."))
                {
                    objectPath = objectPath.substr(9);
                    command = sequence->getCommandByPath(objectPath);
                    if(!command)
                        aError(_("未找到命令：%.*s"), objectPath.size(), objectPath.data());
                }
                else
                {
                    aWarning(_("不支持的对象路径：'%s'"), object.c_str());
                }
            }
            // 解析属性值
            if(command)
            {
                Attribute attr = aResolveAttribute(command, attribute);
                if(!attr.isValid())
                {
                    aError(_("解析属性 '%.*s' 失败"), attribute.size(), attribute.data());
                    return -1;
                }
                Expr* expr = new ExprAttribute(attr);
                variable->setBindExpr(expr);
            }
            
            // printf("variable: %s\n object: %s\n attribute: %s\n unit: %s\n", variable->getName().c_str(), object.c_str(), attribute.c_str(), unit.c_str());
            return 0;
        });
    }
    return 0;
}



AST_NAMESPACE_END

