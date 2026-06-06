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

#include <matplot/core/axes_type.h>
#include <matplot/core/figure_type.h>
#include <matplot/freestanding/plot.h>

#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QCoreApplication>
#include <QIcon>

#include <qwt_figure.h>
#include <qwt_plot.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_canvas_zoomer.h>
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
    navigationGroup_ = new QActionGroup(this);
    navigationGroup_->setExclusive(true);

    // ---- 保存 ----
    auto* saveAction = toolBar_->addAction(loadIcon("Save"), tr("保存"));
    saveAction->setObjectName(QStringLiteral("action_save"));
    saveAction->setToolTip(tr("保存图片"));
    connect(saveAction, &QAction::triggered, this, &UiFigure::saveFigure);

    toolBar_->addSeparator();

    // ---- 平移 ----
    auto* panAction = toolBar_->addAction(loadIcon("Pan"), tr("平移"));
    panAction->setObjectName(QStringLiteral("action_pan"));
    panAction->setCheckable(true);
    panAction->setToolTip(tr("平移视图"));
    navigationGroup_->addAction(panAction);
    connect(panAction, &QAction::toggled, this, &UiFigure::setPanMode);

    // ---- 放大 ----
    auto* zoomInAction = toolBar_->addAction(loadIcon("ZoomIn"), tr("放大"));
    zoomInAction->setObjectName(QStringLiteral("action_zoomin"));
    zoomInAction->setCheckable(true);
    zoomInAction->setToolTip(tr("框选放大"));
    navigationGroup_->addAction(zoomInAction);
    connect(zoomInAction, &QAction::toggled, this, &UiFigure::setZoomInMode);

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

    toolBar_->addSeparator();

    // ---- 图例 ----
    auto* legendAction = toolBar_->addAction(loadIcon("Legend"), tr("图例"));
    legendAction->setObjectName(QStringLiteral("action_legend"));
    legendAction->setCheckable(true);
    legendAction->setToolTip(tr("显示/隐藏图例"));
    connect(legendAction, &QAction::toggled, this, &UiFigure::toggleLegend);

    // ---- 网格 ----
    auto* gridAction = toolBar_->addAction(loadIcon("Grid"), tr("网格"));
    gridAction->setObjectName(QStringLiteral("action_grid"));
    gridAction->setCheckable(true);
    gridAction->setToolTip(tr("显示/隐藏网格"));
    connect(gridAction, &QAction::toggled, this, &UiFigure::toggleGrid);

    // ---- 色条 ----
    auto* colorbarAction = toolBar_->addAction(loadIcon("Colorbar"), tr("色条"));
    colorbarAction->setObjectName(QStringLiteral("action_colorbar"));
    colorbarAction->setCheckable(true);
    colorbarAction->setToolTip(tr("显示/隐藏色条"));
    connect(colorbarAction, &QAction::toggled, this, &UiFigure::toggleColorbar);
}

QIcon UiFigure::loadIcon(const QString& name) const
{
    QString path = QCoreApplication::applicationDirPath()
                   + "/data/icons/" + name + ".svg";
    return QIcon(path);
}

QList<QwtPlot*> UiFigure::allPlots() const
{
    if (!qwtfigure_)
        return {};
    return qwtfigure_->allAxes();
}

void UiFigure::refreshOriginalLimits()
{
    originalLimits_.clear();
    for (auto* plot : allPlots()) {
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

// ==================== Slots ====================

void UiFigure::saveFigure()
{
    if (!qwtfigure_)
        return;

    QString path = QFileDialog::getSaveFileName(
        this,
        tr("保存图片"),
        QString(),
        tr("PNG (*.png);;PDF (*.pdf);;JPG (*.jpg);;BMP (*.bmp)")
    );

    if (path.isEmpty())
        return;

    qwtfigure_->saveFig(path, 300);
}

void UiFigure::resetView()
{
    if (!qwtfigure_)
        return;

    // 恢复原始轴范围
    for (auto it = originalLimits_.begin(); it != originalLimits_.end(); ++it) {
        auto* plot = it.key();
        const auto& limits = it.value();
        plot->setAxisScale(QwtPlot::xBottom, limits.xMin, limits.xMax);
        plot->setAxisScale(QwtPlot::yLeft, limits.yMin, limits.yMax);
    }

    // 清除导航交互器
    clearPanners();
    clearZoomers();

    // 取消导航动作选中
    if (auto* checked = navigationGroup_->checkedAction())
        checked->setChecked(false);

    qwtfigure_->replotAll();
}

void UiFigure::setPanMode(bool on)
{
    if (!qwtfigure_)
        return;

    if (on) {
        clearZoomers();
        for (auto* plot : allPlots()) {
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
        for (auto* plot : allPlots()) {
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

    for (auto* plot : allPlots()) {
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
    if(auto pltfigure = pltfigure_)
    {
        figure_silencer silencer(pltfigure);
        for(auto ax: pltfigure->children())
        {
            if (ax->legend() != nullptr) {
                ax->legend()->visible(on);
            } else {
                auto children = ax->children();
                std::vector<std::string> names;
                for(int i = 0; i < children.size(); i++)
                {
                    if(!children[i]->display_name().empty())
                        names.push_back(children[i]->display_name());
                    else
                        names.push_back("data" + std::to_string(i+1));
                }
                plt::legend(ax, names)->visible(on);
            }
        }
        pltfigure->draw();
        return;
    }
    else if (auto* qwtfigure = qwtfigure_)
    {
        // 2D 图例
        for (auto* plot : allPlots()) {
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
        return;
    }
}

void UiFigure::toggleGrid(bool on)
{
    if (!qwtfigure_)
        return;

    for (auto* plot : allPlots()) {
        for (auto* item : plot->itemList()) {
            if (auto* grid = dynamic_cast<QwtPlotGrid*>(item)) {
                grid->setVisible(on);
            }
        }
    }

    qwtfigure_->replotAll();
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

AST_NAMESPACE_END
