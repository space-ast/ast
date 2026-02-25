///
/// @file      CriticallyInclinedOrbitDesigner.hpp
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

/// @brief 临界轨道设计器
class AST_CORE_API CriticallyInclinedOrbitDesigner final: public BaseOrbitDesigner
{
public:
    CriticallyInclinedOrbitDesigner();
    CriticallyInclinedOrbitDesigner(CelestialBody* body);
    ~CriticallyInclinedOrbitDesigner()=default;
    enum EDirection
    {
        ePosigrade,     ///< 顺行
        eRetrograde,    ///< 逆行
    };

    using BaseOrbitDesigner::getOrbitState;
    err_t getOrbitState(ModOrbElem& orbElem) const override;

    /// @brief 获取轨道方向
    /// @return 轨道方向
    EDirection getDirection() const { return direction_; }

    /// @brief 获取远地点高度
    /// @return 远地点高度
    double getApogeeAltitude() const { return apogeeAltitude_; }

    /// @brief 获取近地点高度
    /// @return 近地点高度
    double getPerigeeAltitude() const { return perigeeAltitude_; }

    /// @brief 获取升交点经度
    /// @return 升交点经度
    // double getLongitudeOfAscendingNode() const { return longitudeOfAscendingNode_; }
    
    /// @brief 获取升交点赤经
    /// @return 升交点赤经
    double getRAAN() const { return raan_; }

    /// @brief 设置轨道方向
    /// @param direction 轨道方向
    void setDirection(EDirection direction) { direction_ = direction; }

    /// @brief 设置远地点高度
    /// @param apogeeAltitude 远地点高度
    void setApogeeAltitude(double apogeeAltitude) { apogeeAltitude_ = apogeeAltitude; }

    /// @brief 设置近地点高度
    /// @param perigeeAltitude 近地点高度
    void setPerigeeAltitude(double perigeeAltitude) { perigeeAltitude_ = perigeeAltitude; }

    /// @brief 设置升交点经度
    /// @param longitudeOfAscendingNode 升交点经度
    // void setLongitudeOfAscendingNode(double longitudeOfAscendingNode) { longitudeOfAscendingNode_ = longitudeOfAscendingNode; }
    
    /// @brief 设置升交点赤经
    void setRAAN(double raan) { raan_ = raan; }

protected:
    EDirection direction_{ePosigrade};           ///< 轨道方向
    double apogeeAltitude_{0};                   ///< 远地点高度
    double perigeeAltitude_{0};                  ///< 近地点高度
    // double longitudeOfAscendingNode_{0};         ///< 升交点经度
    double raan_{0};                            ///< 升交点经度
};


/*! @} */

AST_NAMESPACE_END
