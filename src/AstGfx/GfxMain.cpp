///
/// @file      GfxMain.cpp
/// @brief     可视化系统主类实现
/// @details   实现OSG可视化系统的初始化、场景创建和渲染循环
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

#include "GfxMain.hpp"

// OSG头文件
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/LineWidth>
#include <osg/Point>

AST_NAMESPACE_BEGIN

GfxMain::GfxMain() : m_viewer(nullptr), m_root(nullptr) {
}

GfxMain::~GfxMain() {
    if (m_viewer) {
        delete m_viewer;
        m_viewer = nullptr;
    }
    
    // 注意：OSG使用引用计数，不需要手动删除m_root
}

bool GfxMain::initialize() {
    // 创建OSG查看器
    m_viewer = new osgViewer::Viewer();
    
    // 设置窗口大小
    m_viewer->setUpViewInWindow(100, 100, 800, 600);
    
    // 初始化场景
    if (!initScene()) {
        return false;
    }
    
    // 初始化相机
    if (!initCamera()) {
        return false;
    }
    
    // 初始化灯光
    if (!initLight()) {
        return false;
    }
    
    return true;
}

int GfxMain::run() {
    if (!m_viewer) {
        return -1;
    }
    
    return m_viewer->run();
}

osg::Group* GfxMain::getRootNode() {
    return m_root;
}

void GfxMain::addNode(osg::Node* node) {
    if (m_root && node) {
        m_root->addChild(node);
    }
}

void GfxMain::removeNode(osg::Node* node) {
    if (m_root && node) {
        m_root->removeChild(node);
    }
}

void GfxMain::setBackgroundColor(const osg::Vec4& color) {
    if (m_viewer) {
        m_viewer->getCamera()->setClearColor(color);
    }
}

osgViewer::Viewer* GfxMain::getViewer() {
    return m_viewer;
}

bool GfxMain::initScene() {
    // 创建场景根节点
    m_root = new osg::Group();
    
    // 设置场景根节点
    m_viewer->setSceneData(m_root);
    
    // 设置默认背景颜色为黑色
    setBackgroundColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
    return true;
}

bool GfxMain::initCamera() {
    // 设置相机操纵器（Trackball）
    m_viewer->setCameraManipulator(new osgGA::TrackballManipulator());
    
    // 获取相机
    osg::Camera* camera = m_viewer->getCamera();
    
    // 设置相机位置和方向
    osg::Vec3d eye(0, 0, 10);
    osg::Vec3d center(0, 0, 0);
    osg::Vec3d up(0, 1, 0);
    camera->setViewMatrixAsLookAt(eye, center, up);
    
    return true;
}

bool GfxMain::initLight() {
    // 创建光源
    osg::ref_ptr<osg::Light> light = new osg::Light();
    light->setLightNum(0);
    light->setPosition(osg::Vec4(10.0f, 10.0f, 10.0f, 1.0f));
    light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    light->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // 创建光源节点
    osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
    lightSource->setLight(light);
    lightSource->setReferenceFrame(osg::LightSource::ABSOLUTE_RF);
    
    // 添加光源到场景
    m_root->addChild(lightSource);
    
    return true;
}

AST_NAMESPACE_END