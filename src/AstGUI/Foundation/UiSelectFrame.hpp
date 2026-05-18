///
/// @file      UiSelectFrame.hpp
/// @brief     坐标系选择器 — 天体列表 + Frame 列表，仿 STK VGT
/// @details   左侧列出 SolarSystem 中所有天体，右侧列出选中天体下的 Frame 子对象。
///            Frame 通过 aFindChildren 查询；天体列表支持筛选（筛选项待扩展）。
/// @author    axel
/// @date      2026-05-17
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
#include <QWidget>

class QListWidget;
class QListWidgetItem;

AST_NAMESPACE_BEGIN

class Frame;
class CelestialBody;

/// @brief 坐标系选择器 — 天体 + Frame 双列表选择
///
///      [天体列表]        [坐标系列表]
///      Earth         →  Inertial
///      Moon              ICRF
///      Mars              J2000
///      ...               ...
///
/// 数据源:
///   - 天体: aGetSolarSystem() → aFindChildren(CelestialBody)
///   - Frame: aFindChildren(body, Frame::StaticType())
///           若为空则先调用工厂方法创建并注册到天体下，再重新查询
class AST_GUI_API UiSelectFrame : public QWidget
{
    Q_OBJECT
public:
    explicit UiSelectFrame(QWidget* parent = nullptr);
    ~UiSelectFrame() override;

    Frame* getSelectedFrame();
    void setSelectedFrame(Frame* frame);
    void setBody(CelestialBody* body);
    CelestialBody* getSelectedBody() const;

signals:
    void frameChanged(Frame* frame);

private slots:
    void onBodySelected();
    void onFrameSelected();

private:
    void setupUi();
    void populateBodies();
    void ensureFramesInitialized(CelestialBody* body);
    void populateFrames(CelestialBody* body);
    void resolveAndEmit();

    /// @brief 筛选天体 — 返回 true 表示该天体应在列表中显示（待扩展）
    static bool filterBody(CelestialBody* body);

    QListWidget* bodyList_ = nullptr;
    QListWidget* frameList_ = nullptr;
};

/// @brief 弹出模态对话框选择坐标系
AST_GUI_CAPI Frame* aUiSelectFrame();

AST_NAMESPACE_END
