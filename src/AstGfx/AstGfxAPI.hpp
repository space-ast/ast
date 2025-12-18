///
/// @file      AstGfxAPI.hpp
/// @brief     可视化API
/// @details   该文件包含可视化API的定义，提供太阳系可视化功能
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


AST_NAMESPACE_BEGIN

class GfxMain;
class GfxSolarSystem;

/// @brief 可视化API类
class AST_GFX_API AstGfxAPI {
public:
    /// @brief 获取可视化API实例（单例模式）
    static AstGfxAPI& instance();
    
    /// @brief 初始化可视化系统
    bool initialize();
    
    /// @brief 运行可视化系统
    int run();
    
    /// @brief 更新可视化系统
    /// @param deltaTime 时间增量（秒）
    void update(double deltaTime);
    
    /// @brief 获取主可视化系统
    GfxMain* getGfxMain();
    
    /// @brief 获取太阳系可视化系统
    GfxSolarSystem* getSolarSystem();
    
    /// @brief 设置场景背景颜色
    /// @param r 红色分量（0.0-1.0）
    /// @param g 绿色分量（0.0-1.0）
    /// @param b 蓝色分量（0.0-1.0）
    /// @param a 透明度分量（0.0-1.0）
    void setBackgroundColor(float r, float g, float b, float a);
    
private:
    /// @brief 构造函数（私有，单例模式）
    AstGfxAPI();
    
    /// @brief 析构函数（私有，单例模式）
    ~AstGfxAPI();
    
    /// @brief 主可视化系统
    GfxMain* m_gfxMain;
    
    /// @brief 太阳系可视化系统
    GfxSolarSystem* m_solarSystem;
};

/// @brief 初始化可视化系统
AST_GFX_API bool AstGfxInitialize();

/// @brief 运行可视化系统
AST_GFX_API int AstGfxRun();

/// @brief 更新可视化系统
AST_GFX_API void AstGfxUpdate(double deltaTime);

/// @brief 设置场景背景颜色
AST_GFX_API void AstGfxSetBackgroundColor(float r, float g, float b, float a);

AST_NAMESPACE_END