///
/// @file      UiFigure.cpp
/// @brief     绘图面板实现
/// @details   工具栏交互逻辑：保存/平移/缩放/重置/图例/网格/色条
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

#include "UiFigure.hpp"
#include "ColoredSurfacePlot.hpp"
#include "EditFigureOverlay.hpp"
#include "EditFigureDialog.hpp"

#include <matplot/core/axes_type.h>
#include <matplot/core/figure_type.h>
#include <matplot/freestanding/plot.h>

#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QFileDialog>
#include <QCoreApplication>
#include <QIcon>

#include <qwt_figure.h>
#include <qwt_figure_widget_overlay.h>
#include <qwt_plot.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_canvas_zoomer.h>
#include <qwt_plot_series_data_picker.h>
#include <qwt_plot_series_data_picker_group.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_item.h>
#include <qwt_scale_div.h>

AST_NAMESPACE_BEGIN

namespace plt = matplot;

class figure_silencer
{
public:
    using figure_type = matplot::figure_type;
    explicit figure_silencer(figure_type *figure)
        : figure_(figure)
        , previous_quiet_mode_(figure->quiet_mode()) 
    {
        figure_->quiet_mode(true);
    }

    ~figure_silencer() {
        figure_->quiet_mode(previous_quiet_mode_);
    }
private:
    figure_type *figure_;
    bool previous_quiet_mode_;
};
UiFigure::UiFigure(matplot::figure_type* pltfigure, QWidget* parent)
    : QWidget(parent)
    , qwtfigure_(new QwtFigure(this))
    , pltfigure_(pltfigure)
{
    setupUi();
    createActions();
}

UiFigure::~UiFigure()
{
    clearPanners();
    clearZoomers();
    clearDataPickers();
    clearOverlay();
}

void UiFigure::setFigureSize(int w, int h)
{
    qwtfigure_->resize(w, h);
    resize(w, h + toolBar_->sizeHint().height());
}

QSize UiFigure::sizeHint() const
{
    return QSize(qwtfigure_->width(),
                 qwtfigure_->height() + toolBar_->sizeHint().height());
}

void UiFigure::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    toolBar_ = new QToolBar(this);
    toolBar_->setIconSize(QSize(20, 20));
    layout->addWidget(toolBar_);

    layout->addWidget(qwtfigure_, 1);
}

void UiFigure::createActions()
{
    // 注：不再使用 QActionGroup::setExclusive(true)，改为手动互斥管理。
    //     原因：exclusive group 不允许点击已选中项来取消选中（无法关闭）。

    // ---- 保存 ----
    auto* saveAction = toolBar_->addAction(loadIcon("Save"), tr("保存"));
    saveAction->setObjectName(QStringLiteral("action_save"));
    saveAction->setToolTip(tr("保存图片"));
    connect(saveAction, &QAction::triggered, this, &UiFigure::saveFigure);

    toolBar_->addSeparator();

    // 收集三个导航按钮，用于手动互斥
    QList<QAction*> navActions;

    // ---- 平移 ----
    panAction_ = toolBar_->addAction(loadIcon("Pan"), tr("平移"));
    panAction_->setObjectName(QStringLiteral("action_pan"));
    panAction_->setCheckable(true);
    panAction_->setToolTip(tr("平移视图"));
    navActions.append(panAction_);
    connect(panAction_, &QAction::toggled, this, &UiFigure::setPanMode);

    // ---- 放大 ----
    zoomInAction_ = toolBar_->addAction(loadIcon("ZoomIn"), tr("放大"));
    zoomInAction_->setObjectName(QStringLiteral("action_zoomin"));
    zoomInAction_->setCheckable(true);
    zoomInAction_->setToolTip(tr("框选放大"));
    navActions.append(zoomInAction_);
    connect(zoomInAction_, &QAction::toggled, this, &UiFigure::setZoomInMode);

    // ---- 缩小 ----
    auto* zoomOutAction = toolBar_->addAction(loadIcon("ZoomOut"), tr("缩小"));
    zoomOutAction->setObjectName(QStringLiteral("action_zoomout"));
    zoomOutAction->setToolTip(tr("缩小视图"));
    connect(zoomOutAction, &QAction::triggered, this, &UiFigure::zoomOut);

    // ---- 重置 ----
    auto* resetAction = toolBar_->addAction(loadIcon("Reset"), tr("重置"));
    resetAction->setObjectName(QStringLiteral("action_reset"));
    resetAction->setToolTip(tr("恢复原始视图"));
    connect(resetAction, &QAction::triggered, this, &UiFigure::resetView);

    // ---- 数据拾取（两个 Action + QToolButton::setDefaultAction 切换） ----
    pickYAction_ = new QAction(loadIcon("DataPick"), tr("Y值拾取"), this);
    pickYAction_->setObjectName(QStringLiteral("action_pick_y"));
    pickYAction_->setCheckable(true);
    pickYAction_->setToolTip(tr("Y值拾取：显示当前X位置所有曲线的Y值"));
    navActions.append(pickYAction_);
    connect(pickYAction_, &QAction::toggled, this, &UiFigure::onPickActionToggled);

    pickNearestAction_ = new QAction(loadIcon("DataPickNearest"), tr("临近拾取"), this);
    pickNearestAction_->setObjectName(QStringLiteral("action_pick_nearest"));
    pickNearestAction_->setCheckable(true);
    pickNearestAction_->setToolTip(tr("临近点拾取：显示距离鼠标最近的数据点"));
    navActions.append(pickNearestAction_);
    connect(pickNearestAction_, &QAction::toggled, this, &UiFigure::onPickActionToggled);

    // 下拉菜单直接复用这两个 Action（点击菜单项 = 触发 Action）
    dataPickMenu_ = new QMenu(tr("拾取模式"), this);
    dataPickMenu_->addAction(pickYAction_);
    dataPickMenu_->addAction(pickNearestAction_);

    // QToolButton：setDefaultAction 切换图标/文本/提示，无需手动换
    dataPickBtn_ = new QToolButton(this);
    dataPickBtn_->setDefaultAction(pickYAction_);
    dataPickBtn_->setPopupMode(QToolButton::MenuButtonPopup);
    dataPickBtn_->setMenu(dataPickMenu_);
    toolBar_->addWidget(dataPickBtn_);

    // 手动互斥：任一导航按钮选中时，取消其他两个
    for (auto* self : navActions) {
        connect(self, &QAction::toggled, this, [this, navActions](bool on) {
            if (!on) return;
            auto* sender = qobject_cast<QAction*>(QObject::sender());
            if (!sender) return;
            for (auto* other : navActions) {
                if (other != sender) {
                    other->blockSignals(true);
                    other->setChecked(false);
                    other->blockSignals(false);
                }
            }
        });
    }

    // ---- 编辑模式 ----
    editModeAction_ = toolBar_->addAction(loadIcon("Edit"), tr("编辑"));
    editModeAction_->setObjectName(QStringLiteral("action_edit"));
    editModeAction_->setCheckable(true);
    editModeAction_->setToolTip(tr("编辑模式：选中子图调整大小、编辑绘图元素属性"));
    navActions.append(editModeAction_);
    connect(editModeAction_, &QAction::toggled, this, &UiFigure::setEditMode);

    // ---- 属性检查器 ----
    propertiesAction_ = toolBar_->addAction(loadIcon("Properties"), tr("属性"));
    propertiesAction_->setObjectName(QStringLiteral("action_properties"));
    propertiesAction_->setToolTip(tr("打开属性检查器：查看和编辑坐标轴及绘图元素属性"));
    connect(propertiesAction_, &QAction::triggered, this, &UiFigure::openPropertyInspector);

    toolBar_->addSeparator();

    // ---- 图例 ----
    legendAction_ = toolBar_->addAction(loadIcon("Legend"), tr("图例"));
    legendAction_->setObjectName(QStringLiteral("action_legend"));
    legendAction_->setCheckable(true);
    legendAction_->setToolTip(tr("显示/隐藏图例"));
    connect(legendAction_, &QAction::toggled, this, &UiFigure::toggleLegend);

    // ---- 网格 ----
    gridAction_ = toolBar_->addAction(loadIcon("Grid"), tr("网格"));
    gridAction_->setObjectName(QStringLiteral("action_grid"));
    gridAction_->setCheckable(true);
    gridAction_->setToolTip(tr("显示/隐藏网格"));
    connect(gridAction_, &QAction::toggled, this, &UiFigure::toggleGrid);

    // ---- 色条 ----
    colorbarAction_ = toolBar_->addAction(loadIcon("Colorbar"), tr("色条"));
    colorbarAction_->setObjectName(QStringLiteral("action_colorbar"));
    colorbarAction_->setCheckable(true);
    colorbarAction_->setToolTip(tr("显示/隐藏色条"));
    connect(colorbarAction_, &QAction::toggled, this, &UiFigure::toggleColorbar);
}

QIcon UiFigure::loadIcon(const QString& name) const
{
    QString path = QCoreApplication::applicationDirPath()
                   + "/data/icons/" + name + ".svg";
    return QIcon(path);
}

QList<QwtPlot*> UiFigure::allAxes() const
{
    if (!qwtfigure_)
        return {};
    return qwtfigure_->allAxes();
}

void UiFigure::refreshOriginalLimits()
{
    originalLimits_.clear();
    for (auto* plot : allAxes()) {
        const auto& xDiv = plot->axisScaleDiv(QwtPlot::xBottom);
        const auto& yDiv = plot->axisScaleDiv(QwtPlot::yLeft);
        AxisLimits limits;
        limits.xMin = xDiv.lowerBound();
        limits.xMax = xDiv.upperBound();
        limits.yMin = yDiv.lowerBound();
        limits.yMax = yDiv.upperBound();
        originalLimits_[plot] = limits;
    }
}

void UiFigure::replotAll()
{
    qwtfigure_->replotAll();
}

void UiFigure::syncToolbarState()
{
    if (!pltfigure_)
        return;

    bool hasLegend = false;
    bool hasGrid   = false;
    bool hasColorbar = false;

    for (auto& axes : pltfigure_->children()) {
        // 图例
        if (axes->legend() && axes->legend()->visible())
            hasLegend = true;
        // 网格
        if (axes->grid())
            hasGrid = true;
        // 色条 (3D surface)
        if (axes->cb_axis().visible())
            hasColorbar = true;
    }

    if (legendAction_) {
        legendAction_->blockSignals(true);
        legendAction_->setChecked(hasLegend);
        legendAction_->blockSignals(false);
    }
    if (gridAction_) {
        gridAction_->blockSignals(true);
        gridAction_->setChecked(hasGrid);
        gridAction_->blockSignals(false);
    }
    if (colorbarAction_) {
        colorbarAction_->blockSignals(true);
        colorbarAction_->setChecked(hasColorbar);
        colorbarAction_->blockSignals(false);
    }
}

// ==================== Slots ====================

void UiFigure::saveFigure()
{
    if (!qwtfigure_)
        return;

    QString path = QFileDialog::getSaveFileName(
        this,
        tr("保存图片"),
        QString(),
        tr("PNG (*.png);;JPG (*.jpg);;BMP (*.bmp);;SVG (*.svg);;PDF (*.pdf)")
    );

    if (path.isEmpty())
        return;
        
    if(!pltfigure_ || !pltfigure_->save(path.toStdString()))
    {
        qwtfigure_->saveFig(path, 300);
    }
}

void UiFigure::resetView()
{
    if (!qwtfigure_)
        return;

    for(auto* plot : allAxes())
    {
        auto it = originalLimits_.find(plot);
        if(it == originalLimits_.end())
            continue;
        const auto& limits = it.value();
        plot->setAxisScale(QwtPlot::xBottom, limits.xMin, limits.xMax);
        plot->setAxisScale(QwtPlot::yLeft, limits.yMin, limits.yMax);
    }

    // 清除导航交互器
    clearPanners();
    clearZoomers();
    clearDataPickers();

    // 取消导航动作选中（blockSignals 避免冗余 toggled 信号）
    if (panAction_)         { panAction_->blockSignals(true);
                              panAction_->setChecked(false);
                              panAction_->blockSignals(false); }
    if (zoomInAction_)      { zoomInAction_->blockSignals(true);
                              zoomInAction_->setChecked(false);
                              zoomInAction_->blockSignals(false); }
    if (pickYAction_)       { pickYAction_->blockSignals(true);
                              pickYAction_->setChecked(false);
                              pickYAction_->blockSignals(false); }
    if (pickNearestAction_) { pickNearestAction_->blockSignals(true);
                              pickNearestAction_->setChecked(false);
                              pickNearestAction_->blockSignals(false); }
    if (editModeAction_)    { editModeAction_->blockSignals(true);
                              editModeAction_->setChecked(false);
                              editModeAction_->blockSignals(false); }

    qwtfigure_->replotAll();
}

void UiFigure::setPanMode(bool on)
{
    if (!qwtfigure_)
        return;

    if (on) {
        clearZoomers();
        clearDataPickers();
        for (auto* plot : allAxes()) {
            auto* panner = new QwtPlotPanner(plot->canvas());
            panners_.append(panner);
        }
    } else {
        clearPanners();
    }
}

void UiFigure::setZoomInMode(bool on)
{
    if (!qwtfigure_)
        return;

    if (on) {
        clearPanners();
        clearDataPickers();
        for (auto* plot : allAxes()) {
            auto* zoomer = new QwtPlotCanvasZoomer(plot->canvas());
            zoomer->setZoomBase();
            zoomers_.append(zoomer);
        }
    } else {
        clearZoomers();
    }
}

void UiFigure::zoomOut()
{
    if (!qwtfigure_)
        return;

    for (auto* plot : allAxes()) {
        const auto& xDiv = plot->axisScaleDiv(QwtPlot::xBottom);
        const auto& yDiv = plot->axisScaleDiv(QwtPlot::yLeft);

        double xCenter = (xDiv.lowerBound() + xDiv.upperBound()) / 2.0;
        double yCenter = (yDiv.lowerBound() + yDiv.upperBound()) / 2.0;
        double xRange = (xDiv.upperBound() - xDiv.lowerBound()) * 1.25;
        double yRange = (yDiv.upperBound() - yDiv.lowerBound()) * 1.25;

        plot->setAxisScale(QwtPlot::xBottom,
                           xCenter - xRange / 2.0,
                           xCenter + xRange / 2.0);
        plot->setAxisScale(QwtPlot::yLeft,
                           yCenter - yRange / 2.0,
                           yCenter + yRange / 2.0);
    }
    qwtfigure_->replotAll();
}

void UiFigure::toggleLegend(bool on)
{
    // 静默更新 matplot 数据模型（保持模型一致性）
    if (auto pltfigure = pltfigure_) {
        figure_silencer silencer(pltfigure);
        for (auto ax : pltfigure->children()) {
            if (ax->legend() != nullptr) {
                ax->legend()->visible(on);
            } else {
                auto children = ax->children();
                std::vector<std::string> names;
                for (int i = 0; i < children.size(); i++) {
                    if (!children[i]->display_name().empty())
                        names.push_back(children[i]->display_name());
                    else
                        names.push_back("data" + std::to_string(i + 1));
                }
                plt::legend(ax, names)->visible(on);
            }
        }
        pltfigure->draw();
    }
    else if (auto* qwtfigure = qwtfigure_)
    {
        // 2D 图例
        for (auto* plot : allAxes()) {
            for (auto* item : plot->itemList()) {
                if (auto* legendItem = dynamic_cast<QwtPlotLegendItem*>(item)) {
                    legendItem->setVisible(on);
                }
            }
        }

        // 3D 色条图例（用 dynamic_cast 避免 Qwt3D::SurfacePlot::staticMetaObject 链接问题）
        for (auto* child : qwtfigure_->children()) {
            if (auto* s = dynamic_cast<ColoredSurfacePlot*>(child)) {
                s->showColorLegend(on);
            }
        }

        qwtfigure_->replotAll();
    }
}

void UiFigure::toggleGrid(bool on)
{
    // 静默更新 matplot 数据模型
    if (auto pltfigure = pltfigure_) {
        figure_silencer silencer(pltfigure);
        for (auto ax : pltfigure->children()) {
            ax->grid(on);
        }
        pltfigure->draw();
    }
    else if(auto qwtfigure = qwtfigure_)
    {
        for (auto* plot : allAxes()) {
            for (auto* item : plot->itemList()) {
                if (auto* grid = dynamic_cast<QwtPlotGrid*>(item)) {
                    grid->setVisible(on);
                }
            }
        }
        qwtfigure_->replotAll();
    }

}

void UiFigure::toggleColorbar(bool on)
{
    if (!qwtfigure_)
        return;

    for (auto* child : qwtfigure_->children()) {
        if (auto* s = dynamic_cast<ColoredSurfacePlot*>(child)) {
            s->showColorLegend(on);
        }
    }
}

// ==================== Private Helpers ====================

void UiFigure::clearPanners()
{
    for (auto& ptr : panners_) {
        if (ptr)
            delete ptr.data();
    }
    panners_.clear();
}

void UiFigure::clearZoomers()
{
    for (auto& ptr : zoomers_) {
        if (ptr)
            delete ptr.data();
    }
    zoomers_.clear();
}

// ---- Data Picker ----

void UiFigure::onPickActionToggled(bool on)
{
    if (on) {
        auto* sender = qobject_cast<QAction*>(QObject::sender());

        // 切换到当前模式前，先静默取消另一个拾取 Action
        auto* other = (sender == pickNearestAction_)
            ? static_cast<QAction*>(pickYAction_)
            : static_cast<QAction*>(pickNearestAction_);
        if (other) {
            other->blockSignals(true);
            other->setChecked(false);
            other->blockSignals(false);
        }

        if (dataPickBtn_ && sender)
            dataPickBtn_->setDefaultAction(sender);

        clearPanners();
        clearZoomers();
        createDataPickers();
    } else {
        // 仅当两个拾取 Action 都未选中时才清除（模式切换时不误清）
        bool anyChecked = (pickYAction_ && pickYAction_->isChecked())
                       || (pickNearestAction_ && pickNearestAction_->isChecked());
        if (!anyChecked)
            clearDataPickers();
    }
}

void UiFigure::createDataPickers()
{
    clearDataPickers();  // 确保干净状态

    auto mode = (pickNearestAction_ && pickNearestAction_->isChecked())
        ? QwtPlotSeriesDataPicker::PickNearestPoint
        : QwtPlotSeriesDataPicker::PickYValue;

    dataPickerGroup_.reset(new QwtPlotSeriesDataPickerGroup(this));

    for (auto* plot : allAxes()) {
        auto* picker = new QwtPlotSeriesDataPicker(plot->canvas());
        picker->setPickMode(mode);
        picker->setInterpolationMode(QwtPlotSeriesDataPicker::LinearInterpolation);
        picker->setTextArea(QwtPlotSeriesDataPicker::TextPlaceAuto);
        picker->setEnableDrawFeaturePoint(true);
        picker->setEnableShowXValue(true);
        dataPickers_.append(picker);
        dataPickerGroup_->addPicker(picker);
    }
}
void UiFigure::clearDataPickers()
{
    dataPickerGroup_.reset();  // 先销毁 group（断开信号连接）
    for (auto& ptr : dataPickers_) {
        if (ptr)
            delete ptr.data();
    }
    dataPickers_.clear();
}

void UiFigure::restoreNavigationState()
{
    // 重建所有因 renderFigure clear+重建而失效的导航交互器
    if ((pickYAction_ && pickYAction_->isChecked()) ||
        (pickNearestAction_ && pickNearestAction_->isChecked())) {
        createDataPickers();
    }
    if (panAction_ && panAction_->isChecked()) {
        for (auto* plot : allAxes()) {
            auto* panner = new QwtPlotPanner(plot->canvas());
            panners_.append(panner);
        }
    }
    if (zoomInAction_ && zoomInAction_->isChecked()) {
        for (auto* plot : allAxes()) {
            auto* zoomer = new QwtPlotCanvasZoomer(plot->canvas());
            zoomer->setZoomBase();
            zoomers_.append(zoomer);
        }
    }
}

// ---- Edit Mode ----

void UiFigure::onOverlayGeometryChanged(QWidget* w, QRectF /*oldNorm*/, QRectF newNorm)
{
    if (qwtfigure_ && w)
        qwtfigure_->setWidgetNormPos(w, newNorm);
}

void UiFigure::setEditMode(bool on)
{
    if (on) {
        clearPanners();
        clearZoomers();
        clearDataPickers();
        createOverlay();
    } else {
        clearOverlay();
    }
}

void UiFigure::createOverlay()
{
    if (!qwtfigure_) return;
    clearOverlay();

    overlay_.reset(new EditFigureOverlay(qwtfigure_));
    overlay_->setBuiltInFunctionsEnable(QwtFigureWidgetOverlay::FunSelectCurrentPlot, true);
    overlay_->setBuiltInFunctionsEnable(QwtFigureWidgetOverlay::FunResizePlot, true);
    overlay_->setBorderPen(QPen(QColor(0, 120, 215), 2));
    overlay_->setControlPointBrush(QBrush(QColor(0, 120, 215)));
    overlay_->setControlPointSize(QSize(10, 10));
    overlay_->show();

    // 拖拽边角后实际应用新位置/大小
    connect(overlay_.get(), &QwtFigureWidgetOverlay::widgetNormGeometryChanged,
            this, &UiFigure::onOverlayGeometryChanged);
}

void UiFigure::clearOverlay()
{
    if (overlay_) {
        overlay_->hide();
        overlay_.reset();
    }
}

void UiFigure::restoreEditModeIfNeeded()
{
    if (editModeAction_ && editModeAction_->isChecked()) {
        createOverlay();
    }
}

void UiFigure::openPropertyInspector()
{
    if (!pltfigure_)
        return;

    // 复用已有对话框（如果还存在），否则新建
    if (!propertiesDialog_) {
        propertiesDialog_ = new EditFigureDialog(pltfigure_, this);
        propertiesDialog_->setAttribute(Qt::WA_DeleteOnClose);
    }
    propertiesDialog_->show();
    propertiesDialog_->raise();
    propertiesDialog_->activateWindow();
}

AST_NAMESPACE_END
