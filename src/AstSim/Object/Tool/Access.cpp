///
/// @file      Access.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-08
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

#include "Access.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/ScopeExit.hpp"
#include "AstSim/ObjectComponent.hpp"
#include "AstSim/ObjectAccessConstraint.hpp"
#include "AstSim/ObjectAccessConstraints.hpp"
#include "AstCore/AccessEvaluator.hpp"
#include "AstCore/AccessConstraint.hpp"
#include "AstCore/FixedStepStepper.hpp"
#include "AstCore/BodyObstructionConstraint.hpp"


AST_NAMESPACE_BEGIN


Body& aPoint_GetBody(Point& object)
{
    Body* body = nullptr;
    auto frame = object.getFrame();
    if(frame != nullptr)
        body = frame->getBody();
    if(body == nullptr)
    {
        body = aGetDefaultBody();
        aWarning(_("点对象'%s'没有关联的天体，将使用默认天体 '%s'"), object.name().c_str(), body->name().c_str());
    }
    return *body;
}

void collectObjectAccessConstraints(
    Point& object,
    Point& otherObject,
    std::vector<AccessConstraint*>& constraints
)
{
    auto& accessConstraints = aObject_EnsureAccessConstraints(object);
    for(auto& accessConstraint: accessConstraints)
    {
        auto type = accessConstraint->type();
        switch(type)
        {
            case EAccessConstraint::eLineOfSight:
            {
                auto constraint = new BodyObstructionConstraint(&object, &otherObject, &aPoint_GetBody(object));
                constraints.push_back(constraint);
                break;
            }
            default:
            {
                aError(_("暂不支持的访问约束类型: %s"), toString(type).c_str());
                break;
            }
        }
    }
}

errc_t Access::compute()
{
    errc_t rc;
    
    // 获取分析对象
    Point* baseObject = aobject_cast<Point*>(this->baseObject());
    Point* targetObject = aobject_cast<Point*>(this->targetObject());
    if (baseObject == nullptr || targetObject == nullptr)
    {
        aWarning(_("主对象或目标对象不是点对象或为空"));
        return eErrorNullPtr;
    }

    // 获取访问时间间隔
    TimeInterval intervalAccess;
    {
        TimeInterval intervalBaseObject, intervalTargetObject;
        rc = baseObject->getInterval(intervalBaseObject);        AST_CHECK_ERRCODE(rc, _("获取主对象时间段失败"));
        rc = targetObject->getInterval(intervalTargetObject);    AST_CHECK_ERRCODE(rc, _("获取目标对象时间段失败"));
        intervalAccess = intervalBaseObject & intervalTargetObject;
        if (intervalAccess.isEmpty())
        {
            aInfo(_("主对象和目标对象的时间段交集为空"));
            this->accessIntervals_.clear();
            return eNoError;
        }
    }

    // 获取访问约束
    std::vector<AccessConstraint*> constraints;
    collectObjectAccessConstraints(*baseObject, *targetObject, constraints);
    collectObjectAccessConstraints(*targetObject, *baseObject, constraints);
    auto guard = aScopeExit([&]{ for (auto& constraint : constraints) delete constraint;});

    // 计算访问时间段
    FixedStepStepper stepper;
    rc = aEvaluateAccess(constraints, &stepper, intervalAccess, this->accessIntervals_);
    AST_CHECK_ERRCODE(rc, _("计算访问时间段失败"));

    return rc;
}

AST_NAMESPACE_END
