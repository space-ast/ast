///
/// @file      GfxSolarSystem.hpp
/// @brief     太阳系可视化类
/// @details   负责管理整个太阳系的可视化，包括太阳、行星和它们的轨道
/// @author    jinke18
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
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
#include "GfxCelestialBody.hpp"

// OSG头文件
#include <osg/Group>
#include <osg/Node>
#include <vector>
#include <string>

namespace osg {
    class Node;
    class Group;
}

AST_NAMESPACE_BEGIN

class AST_GFX_API GfxSolarSystem {
public:
    /// @brief 构造函数
    GfxSolarSystem();
    
    /// @brief 析构函数
    ~GfxSolarSystem();
    
    /// @brief 初始化太阳系
    bool initialize();
    
    /// @brief 更新太阳系状态
    /// @param deltaTime 时间增量（秒）
    void update(double deltaTime);
    
    /// @brief 获取太阳系的OSG节点
    osg::Node* getNode();
    
    /// @brief 添加天体到太阳系
    void addCelestialBody(GfxCelestialBody* body);
    
    /// @brief 移除天体从太阳系
    void removeCelestialBody(GfxCelestialBody* body);
    
    /// @brief 获取天体
    /// @param name 天体名称
    GfxCelestialBody* getCelestialBody(const std::string& name);
    
    /// @brief 创建太阳
    bool createSun();
    
    /// @brief 创建行星
    /// @param name 行星名称
    /// @param radius 行星半径
    /// @param orbitalRadius 轨道半径
    /// @param orbitalSpeed 公转速度
    /// @param rotationSpeed 自转速度
    /// @param color 行星颜色
    bool createPlanet(const std::string& name, double radius, double orbitalRadius, 
                     double orbitalSpeed, double rotationSpeed, const osg::Vec4& color);
    
    /// @brief 创建完整的太阳系（包括太阳和八大行星）
    bool createCompleteSolarSystem();
    
private:
    /// @brief 太阳系根节点
    osg::Group* m_root;
    
    /// @brief 天体列表
    std::vector<GfxCelestialBody*> m_celestialBodies;
    
    /// @brief 轨道线列表
    std::vector<osg::Node*> m_orbitLines;
};

AST_NAMESPACE_END