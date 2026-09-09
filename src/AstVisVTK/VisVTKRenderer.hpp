///
/// @file      VisVTKRenderer.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-06
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstVisualization/VisRenderer.hpp"
#include "AstUtil/StringView.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief VTK 渲染后端
/// @details 实现 VisRenderer::render()，把场景图翻译为 VTK 场景（轨迹 polyline、
///          天体球体、地面轨迹）。场景坐标统一采用国际单位（米）。
///          作为 AstVisualization 的可插拔后端，由 __AstVisualization__ 在运行时
///          LoadLibrary 本模块并调用 aVisVtkRegister() 完成注册。
class AST_VISVTK_API VisVTKRenderer: public VisRenderer
{
public:
    VisVTKRenderer() = default;
    ~VisVTKRenderer() override = default;

    /// @brief 渲染一个视图
    /// @param view 可视化视图
    /// @param epoch 渲染时刻（用于定位随时间变化的天体等场景元素）
    /// @return 错误码
    errc_t render(const VisView& view, const TimePoint& epoch) override;

public:
    /// @brief 设置是否进入交互窗口（默认 true）
    /// @param interactive 是否交互
    void setInteractive(bool interactive) { interactive_ = interactive; }

    /// @brief 设置是否导出 PNG 截图（默认 false）
    /// @param savePng 是否导出
    void setSavePng(bool savePng) { savePng_ = savePng; }

    /// @brief 设置 PNG 截图保存路径（默认 visualization.png）
    /// @param path 路径
    void setPngPath(StringView path) { pngPath_ = std::string(path); }

    /// @brief 设置窗口尺寸（默认 1024x768）
    /// @param width 宽
    /// @param height 高
    void setWindowSize(int width, int height) { width_ = width; height_ = height; }

    /// @brief 设置背景色（RGB，0~1，默认深空色）
    /// @param r 红
    /// @param g 绿
    /// @param b 蓝
    void setBackground(double r, double g, double b) { bgR_ = r; bgG_ = g; bgB_ = b; }

    /// @brief 设置相机角度（度）
    /// @param azimuthDeg 方位角
    /// @param elevationDeg 俯仰角
    void setCameraAngles(double azimuthDeg, double elevationDeg)
    {
        azimuth_ = azimuthDeg;
        elevation_ = elevationDeg;
    }

    /// @brief 设置坐标缩放因子（默认 1.0，纯米制；如需 km 可视化可设 0.001）
    /// @param scale 缩放因子
    void setUnitScale(double scale) { scale_ = scale; }

    /// @brief 设置鼠标滚轮缩放步进因子（默认 5.0；值越大单次滚轮缩放越剧烈）
    /// @param factor 步进因子
    void setMouseWheelMotionFactor(double factor) { mouseWheelMotionFactor_ = factor; }

private:
    bool    interactive_{true};     ///< 是否交互
    bool    savePng_{false};        ///< 是否导出 PNG
    std::string pngPath_{"visualization.png"}; ///< PNG 路径
    int     width_{1024};           ///< 窗口宽
    int     height_{768};           ///< 窗口高
    double  bgR_{0.0};              ///< 背景红
    double  bgG_{0.0};              ///< 背景绿
    double  bgB_{0.12};             ///< 背景蓝
    double  azimuth_{35.0};         ///< 相机方位角（度）
    double  elevation_{22.0};       ///< 相机俯仰角（度）
    double  scale_{1.0};            ///< 坐标缩放因子
    double  mouseWheelMotionFactor_{5.0}; ///< 鼠标滚轮缩放步进因子
};

/*! @} */

AST_NAMESPACE_END


/// @brief 注册本 VTK 渲染后端为默认渲染器（C 链接符号，供运行时 LoadLibrary 调用）
extern "C" AST_VISVTK_API void aVisVtkRegister();
