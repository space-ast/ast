///
/// @file      RepeatingSunSyncOrbitDesigner.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "BaseOrbitDesigner.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup Orbit
    @{
*/

/// @brief 太阳同步回归轨道设计器
class AST_CORE_API RepeatingSunSyncOrbitDesigner final: public BaseOrbitDesigner
{
public:
    enum EPositionType
    {
        eAltitude,
        eRevsPerDay
    };
    RepeatingSunSyncOrbitDesigner();
    RepeatingSunSyncOrbitDesigner(CelestialBody *body);
    ~RepeatingSunSyncOrbitDesigner() = default;
    
    using BaseOrbitDesigner::getOrbitState;
    err_t getOrbitState(ModOrbElem &orbElem) const override;

    EPositionType getPositionType() const { return positionType_; }
    void setPositionType(EPositionType type) { positionType_ = type; }

    double getApproxAltitude() const { return approxAltitude_; }
    err_t setApproxAltitude(double alt);

    double getApproxRevsPerDay() const { return approxRevsPerDay_; }
    err_t setApproxRevsPerDay(double revs);

    int getNumberOfRevsRepeat() const { return numberOfRevsRepeat_; }
    void setNumberOfRevsRepeat(int revs) { numberOfRevsRepeat_ = revs; }
    
    double getLongitudeOfAscendingNode() const { return longitudeOfAscendingNode_; }
    void setLongitudeOfAscendingNode(double node) { longitudeOfAscendingNode_ = node; }
protected:
    EPositionType    positionType_{eAltitude};
    double           approxAltitude_{0};
    double           approxRevsPerDay_{0};
    int              numberOfRevsRepeat_{0};
    double           longitudeOfAscendingNode_{0};
};

/*! @} */

AST_NAMESPACE_END
