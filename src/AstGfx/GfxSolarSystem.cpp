
///
/// @file      GfxSolarSystem.cpp
/// @brief     太阳系可视化类实现
/// @details   实现整个太阳系的可视化管理，包括太阳、行星和它们的轨道
/// @author    axel
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

#include "GfxSolarSystem.hpp"

// OSG头文件
#include <osg/Group>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/LightSource>
#include <osg/Light>
#include <osg/Vec3>
#include <osg/Vec4>
#include <iostream>
#include <algorithm>

AST_NAMESPACE_BEGIN

GfxSolarSystem::GfxSolarSystem() : m_root(nullptr) {
}

GfxSolarSystem::~GfxSolarSystem() {
    // 删除天体
    for (auto body : m_celestialBodies) {
        delete body;
    }
    m_celestialBodies.clear();
    
    // OSG使用引用计数，不需要手动删除m_root
    // 轨道线也由OSG管理
}

bool GfxSolarSystem::initialize() {
    // 创建太阳系根节点
    m_root = new osg::Group();
    
    // 创建完整的太阳系
    if (!createCompleteSolarSystem()) {
        return false;
    }
    
    return true;
}

void GfxSolarSystem::update(double deltaTime) {
    // 更新所有天体的状态
    for (auto body : m_celestialBodies) {
        body->update(deltaTime);
    }
}

osg::Node* GfxSolarSystem::getNode() {
    return m_root;
}

void GfxSolarSystem::addCelestialBody(GfxCelestialBody* body) {
    if (!body) {
        return;
    }
    
    // 添加天体到列表
    m_celestialBodies.push_back(body);
    
    // 添加天体节点到场景
    if (m_root) {
        m_root->addChild(body->getNode());
        
        // 创建并添加轨道线
        osg::Node* orbitLine = body->createOrbitLine();
        if (orbitLine) {
            m_root->addChild(orbitLine);
            m_orbitLines.push_back(orbitLine);
        }
    }
}

void GfxSolarSystem::removeCelestialBody(GfxCelestialBody* body) {
    if (!body) {
        return;
    }
    
    // 从列表中移除天体
    auto it = std::find(m_celestialBodies.begin(), m_celestialBodies.end(), body);
    if (it != m_celestialBodies.end()) {
        m_celestialBodies.erase(it);
    }
    
    // 从场景中移除天体节点
    if (m_root) {
        m_root->removeChild(body->getNode());
    }
    
    // 注意：轨道线没有单独的引用，需要重新创建轨道线列表
}

GfxCelestialBody* GfxSolarSystem::getCelestialBody(const std::string& name) {
    for (auto body : m_celestialBodies) {
        if (body->getName() == name) {
            return body;
        }
    }
    return nullptr;
}

bool GfxSolarSystem::createSun() {
    // 创建太阳
    GfxCelestialBody* sun = new GfxCelestialBody("Sun", 1.0, osg::Vec4(1.0f, 0.9f, 0.0f, 1.0f));
    if (!sun->initialize()) {
        delete sun;
        return false;
    }
    
    // 设置太阳的自转速度
    sun->setRotationSpeed(0.1);
    
    // 添加太阳到太阳系
    addCelestialBody(sun);
    
    return true;
}

bool GfxSolarSystem::createPlanet(const std::string& name, double radius, double orbitalRadius, 
                                 double orbitalSpeed, double rotationSpeed, const osg::Vec4& color) {
    // 创建行星
    GfxCelestialBody* planet = new GfxCelestialBody(name, radius, color);
    if (!planet->initialize()) {
        delete planet;
        return false;
    }
    
    // 设置行星的轨道参数
    planet->setOrbitalRadius(orbitalRadius);
    planet->setOrbitalSpeed(orbitalSpeed);
    planet->setRotationSpeed(rotationSpeed);
    
    // 添加行星到太阳系
    addCelestialBody(planet);
    
    return true;
}

bool GfxSolarSystem::createCompleteSolarSystem() {
    // 创建太阳
    if (!createSun()) {
        return false;
    }
    
    // 创建八大行星
    // 水星
    createPlanet("Mercury", 0.38, 2.0, 0.24, 0.01, osg::Vec4(0.7f, 0.7f, 0.7f, 1.0f));
    
    // 金星
    createPlanet("Venus", 0.95, 3.0, 0.62, 0.001, osg::Vec4(0.9f, 0.8f, 0.6f, 1.0f));
    
    // 地球
    createPlanet("Earth", 1.0, 4.0, 1.0, 0.041, osg::Vec4(0.2f, 0.3f, 0.8f, 1.0f));
    
    // 火星
    createPlanet("Mars", 0.53, 5.0, 1.88, 0.039, osg::Vec4(0.8f, 0.3f, 0.2f, 1.0f));
    
    // 木星
    createPlanet("Jupiter", 11.2, 7.0, 11.86, 0.41, osg::Vec4(0.8f, 0.7f, 0.5f, 1.0f));
    
    // 土星
    createPlanet("Saturn", 9.45, 9.0, 29.46, 0.38, osg::Vec4(0.9f, 0.8f, 0.7f, 1.0f));
    
    // 天王星
    createPlanet("Uranus", 4.0, 11.0, 84.01, 0.16, osg::Vec4(0.5f, 0.7f, 0.8f, 1.0f));
    
    // 海王星
    createPlanet("Neptune", 3.88, 13.0, 164.8, 0.15, osg::Vec4(0.3f, 0.4f, 0.8f, 1.0f));
    
    return true;
}

AST_NAMESPACE_END