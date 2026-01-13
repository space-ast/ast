///
/// @file      GfxCelestialBody.cpp
/// @brief     天体可视化类实现
/// @details   实现太阳系中天体的可视化功能
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

#include "GfxCelestialBody.hpp"

// OSG头文件
#include <osgDB/ReadFile>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/Image>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/Geometry>
#include <osg/LineWidth>

AST_NAMESPACE_BEGIN

GfxCelestialBody::GfxCelestialBody(const std::string& name, double radius, const osg::Vec4& color) 
    : m_name(name),
      m_radius(radius),
      m_color(color),
      m_rotationSpeed(0.0),
      m_orbitalSpeed(0.0),
      m_orbitalRadius(0.0),
      m_currentRotation(0.0),
      m_currentOrbit(0.0),
      m_pat(nullptr),
      m_shapeDrawable(nullptr),
      m_material(nullptr),
      m_texture(nullptr) {
}

GfxCelestialBody::~GfxCelestialBody() {
    // OSG使用引用计数，不需要手动删除节点
}

bool GfxCelestialBody::initialize() {
    return createModel();
}

const std::string& GfxCelestialBody::getName() const {
    return m_name;
}

void GfxCelestialBody::setPosition(const osg::Vec3d& position) {
    if (m_pat) {
        m_pat->setPosition(position);
    }
}

osg::Vec3d GfxCelestialBody::getPosition() const {
    if (m_pat) {
        return m_pat->getPosition();
    }
    return osg::Vec3d(0.0, 0.0, 0.0);
}

void GfxCelestialBody::setAttitude(const osg::Quat& attitude) {
    if (m_pat) {
        m_pat->setAttitude(attitude);
    }
}

osg::Quat GfxCelestialBody::getAttitude() const {
    if (m_pat) {
        return m_pat->getAttitude();
    }
    return osg::Quat();
}

void GfxCelestialBody::setRadius(double radius) {
    m_radius = radius;
    
    // 如果已经创建了模型，更新模型大小
    if (m_shapeDrawable) {
        osg::Sphere* sphere = dynamic_cast<osg::Sphere*>(m_shapeDrawable->getShape());
        if (sphere) {
            sphere->setRadius(m_radius);
        }
    }
}

double GfxCelestialBody::getRadius() const {
    return m_radius;
}

void GfxCelestialBody::setColor(const osg::Vec4& color) {
    m_color = color;
    
    // 如果已经创建了材质，更新材质颜色
    if (m_material) {
        m_material->setDiffuse(osg::Material::FRONT_AND_BACK, m_color);
    }
    
    // 如果已经创建了形状绘制，更新颜色
    if (m_shapeDrawable) {
        m_shapeDrawable->setColor(m_color);
    }
}

osg::Vec4 GfxCelestialBody::getColor() const {
    return m_color;
}

void GfxCelestialBody::setRotationSpeed(double speed) {
    m_rotationSpeed = speed;
}

double GfxCelestialBody::getRotationSpeed() const {
    return m_rotationSpeed;
}

void GfxCelestialBody::setOrbitalSpeed(double speed) {
    m_orbitalSpeed = speed;
}

double GfxCelestialBody::getOrbitalSpeed() const {
    return m_orbitalSpeed;
}

void GfxCelestialBody::setOrbitalRadius(double radius) {
    m_orbitalRadius = radius;
}

double GfxCelestialBody::getOrbitalRadius() const {
    return m_orbitalRadius;
}

bool GfxCelestialBody::setTexture(const std::string& textureFile) {
    if (textureFile.empty()) {
        return false;
    }
    
    // 读取纹理图像
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(textureFile);
    if (!image) {
        return false;
    }
    
    // 创建纹理
    m_texture = new osg::Texture2D();
    m_texture->setImage(image);
    m_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    m_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    m_texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    m_texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    
    // 获取状态集并应用纹理
    osg::Geode* geode = dynamic_cast<osg::Geode*>(m_pat->getChild(0));
    if (geode) {
        osg::StateSet* stateSet = geode->getOrCreateStateSet();
        stateSet->setTextureAttributeAndModes(0, m_texture, osg::StateAttribute::ON);
    }
    
    return true;
}

void GfxCelestialBody::update(double deltaTime) {
    if (!m_pat) {
        return;
    }
    
    // 更新自转
    m_currentRotation += m_rotationSpeed * deltaTime;
    osg::Quat rotation = osg::Quat(m_currentRotation, osg::Vec3(0.0, 0.0, 1.0));
    m_pat->setAttitude(rotation);
    
    // 更新公转
    m_currentOrbit += m_orbitalSpeed * deltaTime;
    double x = m_orbitalRadius * cos(m_currentOrbit);
    double y = m_orbitalRadius * sin(m_currentOrbit);
    double z = 0.0; // 假设在XY平面上公转
    m_pat->setPosition(osg::Vec3d(x, y, z));
}

osg::Node* GfxCelestialBody::getNode() {
    return m_pat;
}

osg::Node* GfxCelestialBody::createOrbitLine(const osg::Vec4& color, int segments) {
    // 创建轨道线
    osg::ref_ptr<osg::Geometry> orbitGeometry = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    
    // 生成轨道上的点
    for (int i = 0; i <= segments; ++i) {
        double angle = 2.0 * osg::PI * static_cast<double>(i) / static_cast<double>(segments);
        double x = m_orbitalRadius * cos(angle);
        double y = m_orbitalRadius * sin(angle);
        double z = 0.0;
        vertices->push_back(osg::Vec3d(x, y, z));
    }
    
    orbitGeometry->setVertexArray(vertices);
    orbitGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));
    
    // 设置颜色
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    colors->push_back(color);
    orbitGeometry->setColorArray(colors);
    orbitGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    
    // 设置线宽
    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(1.0f);
    orbitGeometry->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
    
    // 创建Geode并添加轨道线
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable(orbitGeometry);
    
    return geode.release();
}

bool GfxCelestialBody::createModel() {
    // 创建位置姿态变换节点
    m_pat = new osg::PositionAttitudeTransform();
    
    // 创建Geode
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    
    // 创建球体
    osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), m_radius);
    
    // 创建形状绘制
    m_shapeDrawable = new osg::ShapeDrawable(sphere);
    m_shapeDrawable->setColor(m_color);
    
    // 添加形状绘制到Geode
    geode->addDrawable(m_shapeDrawable);
    
    // 创建材质
    m_material = new osg::Material();
    m_material->setDiffuse(osg::Material::FRONT_AND_BACK, m_color);
    m_material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
    m_material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_material->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
    
    // 设置材质
    osg::StateSet* stateSet = geode->getOrCreateStateSet();
    stateSet->setAttributeAndModes(m_material, osg::StateAttribute::ON);
    
    // 添加Geode到位置姿态变换节点
    m_pat->addChild(geode);
    
    return true;
}

AST_NAMESPACE_END