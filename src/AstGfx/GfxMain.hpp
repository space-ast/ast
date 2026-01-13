///
/// @file      GfxMain.hpp
/// @brief     可视化系统主类
/// @details   负责OSG可视化系统的初始化、场景创建和渲染循环
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

#pragma once

#include "AstGlobal.h"

// OSG头文件
#include <osgViewer/Viewer>
#include <osg/Group>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>

namespace osg {
    class Node;
    class Group;
    class PositionAttitudeTransform;
}

AST_NAMESPACE_BEGIN

class GfxMain {
public:
    /// @brief 构造函数
    GfxMain();
    
    /// @brief 析构函数
    ~GfxMain();
    
    /// @brief 初始化可视化系统
    bool initialize();
    
    /// @brief 运行可视化系统
    int run();
    
    /// @brief 获取OSG场景根节点
    osg::Group* getRootNode();
    
    /// @brief 添加节点到场景
    void addNode(osg::Node* node);
    
    /// @brief 移除节点从场景
    void removeNode(osg::Node* node);
    
    /// @brief 设置场景背景颜色
    void setBackgroundColor(const osg::Vec4& color);
    
    /// @brief 获取OSG查看器
    osgViewer::Viewer* getViewer();
    
private:
    /// @brief OSG查看器
    osgViewer::Viewer* m_viewer;
    
    /// @brief 场景根节点
    osg::Group* m_root;
    
    /// @brief 初始化场景
    bool initScene();
    
    /// @brief 初始化相机
    bool initCamera();
    
    /// @brief 初始化灯光
    bool initLight();
};

AST_NAMESPACE_END