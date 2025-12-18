///
/// @file      GfxCelestialBody.hpp
/// @brief     天体可视化类
/// @details   用于可视化太阳系中的天体，包括行星、卫星、恒星等
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

// OSG头文件
#include <osg/Group>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Vec3>
#include <osg/Vec4>

namespace osg {
    class Node;
    class Group;
    class PositionAttitudeTransform;
    class Sphere;
    class ShapeDrawable;
    class Material;
    class Texture2D;
}

AST_NAMESPACE_BEGIN

class AST_GFX_API GfxCelestialBody {
public:
    /// @brief 构造函数
    /// @param name 天体名称
    /// @param radius 天体半径
    /// @param color 天体颜色
    GfxCelestialBody(const std::string& name, double radius, const osg::Vec4& color = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    /// @brief 析构函数
    ~GfxCelestialBody();
    
    /// @brief 初始化天体
    bool initialize();
    
    /// @brief 获取天体名称
    const std::string& getName() const;
    
    /// @brief 设置天体位置
    void setPosition(const osg::Vec3d& position);
    
    /// @brief 获取天体位置
    osg::Vec3d getPosition() const;
    
    /// @brief 设置天体姿态
    void setAttitude(const osg::Quat& attitude);
    
    /// @brief 获取天体姿态
    osg::Quat getAttitude() const;
    
    /// @brief 设置天体半径
    void setRadius(double radius);
    
    /// @brief 获取天体半径
    double getRadius() const;
    
    /// @brief 设置天体颜色
    void setColor(const osg::Vec4& color);
    
    /// @brief 获取天体颜色
    osg::Vec4 getColor() const;
    
    /// @brief 设置天体自转速度
    void setRotationSpeed(double speed);
    
    /// @brief 获取天体自转速度
    double getRotationSpeed() const;
    
    /// @brief 设置天体公转速度
    void setOrbitalSpeed(double speed);
    
    /// @brief 获取天体公转速度
    double getOrbitalSpeed() const;
    
    /// @brief 设置天体轨道半径
    void setOrbitalRadius(double radius);
    
    /// @brief 获取天体轨道半径
    double getOrbitalRadius() const;
    
    /// @brief 设置天体纹理
    /// @param textureFile 纹理文件路径
    bool setTexture(const std::string& textureFile);
    
    /// @brief 更新天体状态
    /// @param deltaTime 时间增量（秒）
    void update(double deltaTime);
    
    /// @brief 获取天体的OSG节点
    osg::Node* getNode();
    
    /// @brief 创建轨道线
    /// @param color 轨道颜色
    /// @param segments 轨道线段数
    osg::Node* createOrbitLine(const osg::Vec4& color = osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f), int segments = 100);
    
private:
    /// @brief 天体名称
    std::string m_name;
    
    /// @brief 天体半径
    double m_radius;
    
    /// @brief 天体颜色
    osg::Vec4 m_color;
    
    /// @brief 天体自转速度
    double m_rotationSpeed;
    
    /// @brief 天体公转速度
    double m_orbitalSpeed;
    
    /// @brief 天体轨道半径
    double m_orbitalRadius;
    
    /// @brief 当前自转角度
    double m_currentRotation;
    
    /// @brief 当前公转角度
    double m_currentOrbit;
    
    /// @brief 天体的位置姿态变换节点
    osg::PositionAttitudeTransform* m_pat;
    
    /// @brief 天体的形状节点
    osg::ShapeDrawable* m_shapeDrawable;
    
    /// @brief 天体的材质
    osg::Material* m_material;
    
    /// @brief 天体的纹理
    osg::Texture2D* m_texture;
    
    /// @brief 创建天体模型
    bool createModel();
};

AST_NAMESPACE_END