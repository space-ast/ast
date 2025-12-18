///
/// @file      AstGfxAPI.cpp
/// @brief     可视化API
/// @details   该文件包含可视化API的实现，提供太阳系可视化功能
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

#include "AstGfxAPI.hpp"
#include "GfxMain.hpp"
#include "GfxSolarSystem.hpp"
#include <osg/Vec4>

AST_NAMESPACE_BEGIN

// 单例模式实现
AstGfxAPI& AstGfxAPI::instance() {
    static AstGfxAPI instance;
    return instance;
}

AstGfxAPI::AstGfxAPI() : m_gfxMain(nullptr), m_solarSystem(nullptr) {
}

AstGfxAPI::~AstGfxAPI() {
    // 删除主可视化系统
    if (m_gfxMain) {
        delete m_gfxMain;
        m_gfxMain = nullptr;
    }
    
    // 删除太阳系可视化系统
    if (m_solarSystem) {
        delete m_solarSystem;
        m_solarSystem = nullptr;
    }
}

bool AstGfxAPI::initialize() {
    // 创建主可视化系统
    m_gfxMain = new GfxMain();
    if (!m_gfxMain->initialize()) {
        delete m_gfxMain;
        m_gfxMain = nullptr;
        return false;
    }
    
    // 创建太阳系可视化系统
    m_solarSystem = new GfxSolarSystem();
    if (!m_solarSystem->initialize()) {
        delete m_solarSystem;
        m_solarSystem = nullptr;
        return false;
    }
    
    // 将太阳系添加到主场景
    if (m_gfxMain->getRootNode() && m_solarSystem->getNode()) {
        m_gfxMain->getRootNode()->addChild(m_solarSystem->getNode());
    }
    
    return true;
}

int AstGfxAPI::run() {
    if (!m_gfxMain) {
        return -1;
    }
    
    return m_gfxMain->run();
}

void AstGfxAPI::update(double deltaTime) {
    // 更新太阳系状态
    if (m_solarSystem) {
        m_solarSystem->update(deltaTime);
    }
}

GfxMain* AstGfxAPI::getGfxMain() {
    return m_gfxMain;
}

GfxSolarSystem* AstGfxAPI::getSolarSystem() {
    return m_solarSystem;
}

void AstGfxAPI::setBackgroundColor(float r, float g, float b, float a) {
    if (m_gfxMain) {
        m_gfxMain->setBackgroundColor(osg::Vec4(r, g, b, a));
    }
}

// 全局函数实现
bool AstGfxInitialize() {
    return AstGfxAPI::instance().initialize();
}

int AstGfxRun() {
    return AstGfxAPI::instance().run();
}

void AstGfxUpdate(double deltaTime) {
    AstGfxAPI::instance().update(deltaTime);
}

void AstGfxSetBackgroundColor(float r, float g, float b, float a) {
    AstGfxAPI::instance().setBackgroundColor(r, g, b, a);
}

AST_NAMESPACE_END