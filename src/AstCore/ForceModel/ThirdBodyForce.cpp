///
/// @file      ThirdBodyForce.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-21
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


#include "AstGlobal.h"
#include "ThirdBodyForce.hpp"

AST_NAMESPACE_BEGIN


GravityForce& ThirdBodyForce::gravity()
{
    return gravity_;
}

PointMassForce& ThirdBodyForce::pointMass()
{
    return pointMass_;
}

BodyAttraction& ThirdBodyForce::bodyAttraction()
{
    if(attractionType_ == EBodyAttractionType::eGravity)
        return gravity_;
    else if(attractionType_ == EBodyAttractionType::ePointMass)
        return pointMass_;
    else
        return pointMass_; // 默认返回点质量引力模型
}

const BodyAttraction &ThirdBodyForce::bodyAttraction() const
{
    return const_cast<ThirdBodyForce&>(*this).bodyAttraction();
}

EBodyAttractionType ThirdBodyForce::bodyAttractionType() const
{
    return attractionType_;
}

void ThirdBodyForce::setAttractionType(EBodyAttractionType type)
{
    attractionType_ = type;
}

CelestialBody* ThirdBodyForce::body() const
{
    return body_.get();
}

void ThirdBodyForce::setBody(CelestialBody* body)
{
    if(body)
        body_ = body;
}


AST_NAMESPACE_END
