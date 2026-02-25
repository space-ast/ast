///
/// @file      SunSynchronousOrbitDesigner.hpp
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


/// @brief 太阳同步轨道设计器
class AST_CORE_API SunSynchronousOrbitDesigner final: public BaseOrbitDesigner
{
public:
    enum EGeometryDefinition
    {
        eAltitude = 0,
        eInclination = 1,
    };
    enum ENodeDefinition
    {
        eAscendingNode = 0,
        eDescendingNode = 1,
    };
    SunSynchronousOrbitDesigner();
    SunSynchronousOrbitDesigner(CelestialBody *body);
    ~SunSynchronousOrbitDesigner() = default;
    err_t getOrbitState(ModOrbElem &orbElem) const override;

    /// @brief 获取几何定义方式
    EGeometryDefinition getGeometryDefinition() const { return geometryDefinition_; }
    /// @brief 设置几何定义方式
    void setGeometryDefinition(EGeometryDefinition def) { geometryDefinition_ = def; }
    /// @brief 获取升交点定义方式
    ENodeDefinition getNodeDefinition() const { return nodeDefinition_; }
    /// @brief 设置升交点定义方式
    void setNodeDefinition(ENodeDefinition def) { nodeDefinition_ = def; }

    /// @brief 计算轨道倾角
    double calcInclination(double alt) const;
    /// @brief 计算轨道倾角
    double calcInclination() const;
    /// @brief 计算轨道高度
    double calcAltitude(double inc) const;
    /// @brief 计算轨道高度
    double calcAltitude() const;

    /// @brief 获取轨道倾角
    double getInclination() const;
    
    /// @brief 设置轨道倾角
    void setInclination(double inc);
    
    /// @brief 获取轨道高度
    double getAltitude() const;

    /// @brief 设置轨道高度
    void setAltitude(double alt);

    double getLocalTimeOfAscendingNode() const { return localTimeOfAscendingNode_; }
    void setLocalTimeOfAscendingNode(double t) { localTimeOfAscendingNode_ = t; }
private:
    EGeometryDefinition geometryDefinition_{eAltitude};
    ENodeDefinition     nodeDefinition_{eAscendingNode};
    double              inclination_{0};
    double              altitude_{0};
    double              localTimeOfAscendingNode_{0};
    double              localTimeOfDescendingNode_{0};
};


/*! @} */

AST_NAMESPACE_END
