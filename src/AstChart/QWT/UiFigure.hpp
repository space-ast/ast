///
/// @file      UiFigure.hpp
/// @brief     绘图面板，包含工具栏和 QwtFigure 容器
/// @details   提供保存/平移/缩放/重置/图例/网格/色条等交互功能，
///            基于 QWidget，可嵌入任意父控件或作为独立窗口使用
/// @author    axel
/// @date      2026-06-05
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
#include "EditFigureDialog.hpp"
#include <QWidget>
#include <QMap>
#include <QList>
#include <QPointer>
#include <QScopedPointer>

class QwtFigure;
class QwtPlot;
class QwtPlotPanner;
class QwtPlotCanvasZoomer;
class QwtPlotSeriesDataPicker;
class QwtPlotSeriesDataPickerGroup;
class QToolBar;
class QToolButton;
class QActionGroup;
class QMenu;
class QwtFigureWidgetOverlay;

namespace matplot
{
    class figure_type;
}

AST_NAMESPACE_BEGIN

class AST_CHART_API UiFigure : public QWidget {
    Q_OBJECT
public:
    /// @brief 构造并内部创建 QwtFigure
    explicit UiFigure(matplot::figure_type* pltfigure, QWidget* parent = nullptr);

    ~UiFigure() override;

    /// @brief 获取内部的 QwtFigure
    QwtFigure* qwtfigure() const { return qwtfigure_; }

    /// @brief 获取内部的 matplot::figure_type
    matplot::figure_type* pltfigure() const { return pltfigure_; }

    /// @brief 获取工具栏
    QToolBar* toolBar() const { return toolBar_; }

    /// @brief 设置 QwtFigure 的初始尺寸（含工具栏高度自适应）
    void setFigureSize(int w, int h);

    /// @brief 建议尺寸 = figure 尺寸 + toolbar 高度
    QSize sizeHint() const override;

    /// @brief 重新记录当前轴范围作为"原始状态"（新增子图后调用）
    void refreshOriginalLimits();

    /// @brief 从 matplot 数据模型同步工具栏按钮状态（图例/网格/色条）
    void syncToolbarState();

    /// @brief 重新绘制所有子图
    void replotAll();

    /// @brief 重建当前激活的导航交互器（数据拾取/平移/放大），用于 redraw 后恢复
    void restoreNavigationState();

    /// @brief 重建编辑模式（overlay + picker），renderFigure 后调用
    void restoreEditModeIfNeeded();

public slots:
    /// @brief 保存图片（弹出文件对话框）
    void saveFigure();

    /// @brief 重置所有子图到原始轴范围，关闭导航模式
    void resetView();

    /// @brief 启用/禁用平移模式
    void setPanMode(bool on);

    /// @brief 启用/禁用框选放大模式
    void setZoomInMode(bool on);

    /// @brief 缩小所有子图一级
    void zoomOut();

    /// @brief 切换图例可见性
    void toggleLegend(bool on);

    /// @brief 切换网格可见性
    void toggleGrid(bool on);

    /// @brief 切换 3D 色条可见性
    void toggleColorbar(bool on);

    /// @brief 启用/禁用编辑模式
    void setEditMode(bool on);

    /// @brief 打开属性检查器对话框
    void openPropertyInspector();

private slots:
    /// @brief overlay 拖拽边角后应用新位置
    void onOverlayGeometryChanged(QWidget* w, QRectF oldNorm, QRectF newNorm);
    /// @brief 数据拾取 Action 的 toggled 统一处理（Y值/临近点）
    void onPickActionToggled(bool on);

private:
    void setupUi();
    void createActions();
    void clearPanners();
    void clearZoomers();
    void clearDataPickers();
    void createDataPickers();
    void createOverlay();
    void clearOverlay();
    QIcon loadIcon(const QString& name) const;
    QList<QwtPlot*> allAxes() const;

    struct AxisLimits {
        double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    };

    QwtFigure*                           qwtfigure_ = nullptr;
    matplot::figure_type*                pltfigure_ = nullptr;
    QToolBar*                            toolBar_ = nullptr;
    QAction*                             panAction_ = nullptr;
    QAction*                             zoomInAction_ = nullptr;
    QAction*                                legendAction_ = nullptr;
    QAction*                                gridAction_ = nullptr;
    QAction*                                colorbarAction_ = nullptr;
    QAction*                                pickYAction_ = nullptr;
    QAction*                                pickNearestAction_ = nullptr;
    QMenu*                                  dataPickMenu_ = nullptr;
    QToolButton*                            dataPickBtn_ = nullptr;
    QMap<QwtPlot*, AxisLimits>              originalLimits_;
    QList<QPointer<QwtPlotPanner>>          panners_;
    QList<QPointer<QwtPlotCanvasZoomer>>    zoomers_;
    QList<QPointer<QwtPlotSeriesDataPicker>> dataPickers_;
    QScopedPointer<QwtPlotSeriesDataPickerGroup> dataPickerGroup_;

    // 编辑模式
    QAction*                                editModeAction_ = nullptr;
    QScopedPointer<QwtFigureWidgetOverlay>  overlay_;

    // 属性检查器
    QAction*                                propertiesAction_ = nullptr;
    QPointer<EditFigureDialog>              propertiesDialog_;
};

AST_NAMESPACE_END
