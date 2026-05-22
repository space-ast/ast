///
/// @file      QwtBackend.cpp
/// @brief     matplot++ Qwt backend implementation
/// @details   Implements matplot++ backend_interface using QwtFigure for Qt/Qwt rendering
/// @author    axel
/// @date      2026-05-19
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

#include <QPen>
#include <QBrush>
#include "QwtBackend.hpp"
#include "QwtPlotVisitor.hpp"
#include "ColoredSurfacePlot.hpp"

#include <matplot/core/axes_type.h>
#include <matplot/core/figure_type.h>
#include <matplot/backend/backend_registry.h>

#include <qwt_figure.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_renderer.h>
#include <qwt_text.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include <qwt3d_surfaceplot.h>

#include <matplot/axes_objects/surface.h>

#include <QApplication>
#include <QEventLoop>
#include <QColor>
#include <QFont>
#include <QLayout>
#include <QMainWindow>

#include <map>
#include <string>

AST_NAMESPACE_BEGIN

struct QwtBackend::Impl {
    std::map<matplot::figure_type*, QwtFigure*> figures;
    unsigned int width_{QwtBackend::kDefaultWidth};
    unsigned int height_{QwtBackend::kDefaultHeight};
    unsigned int pos_x_{QwtBackend::kDefaultPosX};
    unsigned int pos_y_{QwtBackend::kDefaultPosY};
    std::string output_file_;
    std::string output_format_;
    std::string window_title_;
};

QwtBackend::QwtBackend() : impl_(new Impl()) {
    if (!qApp) {
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // 启用高DPI缩放
        QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);  // 共享OpenGL上下文
        static int argc = 0;
        static char* argv[] = {nullptr};
        new QApplication(argc, argv);
    }
}

QwtBackend::~QwtBackend() {
    for (auto& kv : impl_->figures) {
        delete kv.second;  // 未被 QMainWindow 接管的 figure 手动清理
    }
    impl_->figures.clear();
}

bool QwtBackend::consumes_gnuplot_commands() { return false; }

bool QwtBackend::is_interactive() { return impl_->output_file_.empty(); }

const std::string& QwtBackend::output() { return impl_->output_file_; }

const std::string& QwtBackend::output_format() { return impl_->output_format_; }

bool QwtBackend::output(const std::string& filename) {
    impl_->output_file_ = filename;
    if (!filename.empty()) {
        auto dot = filename.find_last_of('.');
        if (dot != std::string::npos) {
            impl_->output_format_ = filename.substr(dot + 1);
        } else {
            impl_->output_format_ = "png";
        }
    }
    return true;
}

bool QwtBackend::output(const std::string& filename, const std::string& format) {
    impl_->output_file_ = filename;
    impl_->output_format_ = format;
    return true;
}

unsigned int QwtBackend::width() { return impl_->width_; }
unsigned int QwtBackend::height() { return impl_->height_; }
void QwtBackend::width(unsigned int w) { impl_->width_ = w; }
void QwtBackend::height(unsigned int h) { impl_->height_ = h; }

unsigned int QwtBackend::position_x() { return impl_->pos_x_; }
unsigned int QwtBackend::position_y() { return impl_->pos_y_; }
void QwtBackend::position_x(unsigned int x) { impl_->pos_x_ = x; }
void QwtBackend::position_y(unsigned int y) { impl_->pos_y_ = y; }

void QwtBackend::window_title(const std::string& title) {
    impl_->window_title_ = title;
}

std::string QwtBackend::window_title() {
    return impl_->window_title_;
}

bool QwtBackend::new_frame() { return true; }

bool QwtBackend::render_data() {
    for (auto& kv : impl_->figures) {
        kv.second->replotAll();
    }
    return true;
}

bool QwtBackend::should_close() { return false; }

bool QwtBackend::supports_fonts() { return true; }

void QwtBackend::show(matplot::figure_type* f) {
    auto* fig = get_or_create_figure(f);

    // 包在 QMainWindow 中，QOpenGLWidget (surface) 需要 QMainWindow 祖先
    QMainWindow* mw = new QMainWindow();
    fig->setParent(mw);
    mw->setCentralWidget(fig);
    mw->resize(fig->size());
    mw->move(fig->pos());

    render_figure(f, fig);              // 创建所有子部件

    mw->show();
    mw->raise();
    mw->setWindowTitle(fig->windowTitle());
    mw->setAttribute(Qt::WA_DeleteOnClose);

    QEventLoop loop;
    QObject::connect(mw, &QObject::destroyed, &loop, &QEventLoop::quit);
    loop.exec();
    impl_->figures.erase(f);
}

void QwtBackend::draw(matplot::figure_type* f) {
    QwtFigure* fig = get_or_create_figure(f);
    render_figure(f, fig);

    if (!impl_->output_file_.empty()) {
        QString path = QString::fromStdString(impl_->output_file_);
        QString format = QString::fromStdString(impl_->output_format_);
        if (format == "png" || format == "jpg" || format == "bmp") {
            QPixmap pixmap = fig->saveFig();
            pixmap.save(path, format.toUpper().toUtf8().constData());
        } else {
            auto plots = fig->allAxes();
            if (plots.size() == 1) {
                QwtPlotRenderer renderer;
                renderer.renderDocument(plots.first(),
                    QString::fromStdString(impl_->output_file_),
                    QString::fromStdString(impl_->output_format_),
                    QSizeF(impl_->width_ / 96.0, impl_->height_ / 96.0));
            } else if (plots.size() > 1) {
                QPixmap pixmap = fig->saveFig();
                pixmap.save(path, "PNG");
            }
        }
    }
}

QwtFigure* QwtBackend::get_or_create_figure(matplot::figure_type* f) {
    auto it = impl_->figures.find(f);
    if (it != impl_->figures.end()) {
        return it->second;
    }
    auto* fig = new QwtFigure();
    fig->resize(static_cast<int>(impl_->width_), static_cast<int>(impl_->height_));
    if (!impl_->window_title_.empty()) {
        fig->setWindowTitle(QString::fromStdString(impl_->window_title_));
    }
    impl_->figures[f] = fig;
    return fig;
}

void QwtBackend::render_figure(matplot::figure_type* f, QwtFigure* fig) {
    fig->clear();

    fig->setFaceColor(toQColor(f->color()));

    if (!impl_->window_title_.empty()) {
        fig->setWindowTitle(QString::fromStdString(impl_->window_title_));
    }

    // 绘图字体
    QFont figureFont(QString::fromStdString(f->font()), static_cast<int>(f->font_size()));

    for (auto& axes : f->children()) {
        auto pos = axes->position();
        float qwtTop = 1.0f - pos[1] - pos[3];

        // 检测是否包含 surface（3D 绘图）
        bool hasSurface = false;
        for (auto& obj : axes->children()) {
            if (dynamic_cast<matplot::surface*>(obj.get())) {
                hasSurface = true;
                break;
            }
        }

        if (hasSurface) {
            // [重要]: 必须让 QMainWindow 显示，否则 OpenGLWidget 无法创建 GL 上下文
            if (auto mw = qobject_cast<QMainWindow*>(fig->window())) {
                mw->show();
            }
            if(!f->custom_color())
            {
                fig->setFaceColor(Qt::white);
            }
            auto* surface3d = new ColoredSurfacePlot(fig);
            surface3d->setRotation(axes->elevation(), 0, -axes->azimuth());  // 视角
            surface3d->setTitle(QString::fromStdString(axes->title()));
            // surface3d->setBackgroundColor(Qwt3D::RGBA(axes->color()[1], axes->color()[2], axes->color()[3], 1 - axes->color()[0]));
            // QOpenGLWidget 需要非零尺寸才能创建 GL 上下文
            // surface3d->resize(static_cast<int>(impl_->width_ * pos[2]),
            //                   static_cast<int>(impl_->height_ * pos[3]));
            fig->addWidget(surface3d, pos[0], qwtTop, pos[2], pos[3]);
            // fig->addWidget(surface3d, 0., 0., 1., 1.);
            surface3d->show();
            QwtPlotVisitor visitor(surface3d);
            for (auto& obj : axes->children()) {
                obj->accept(visitor);
            }
            // 显示颜色图例(colorbar)
            if(axes->cb_axis().visible())
            {
                surface3d->showColorLegend(true);
            }
        } else {
            auto* plot = new QwtPlot(fig);

            auto xlim = axes->xlim();
            auto ylim = axes->ylim();
            plot->setAxisScale(QwtPlot::xBottom, xlim[0], xlim[1]);
            plot->setAxisScale(QwtPlot::yLeft, ylim[0], ylim[1]);

            // 坐标轴轴背景颜色
            plot->setCanvasBackground(QBrush(toQColor(axes->color())));

            // 坐标轴字体
            auto axesFontFamily = QString::fromStdString(axes->font());
            int axesFontSize = static_cast<int>(axes->font_size());
            if (!axesFontFamily.isEmpty() && axesFontSize > 0) {
                QFont axesFont(axesFontFamily, axesFontSize);
                plot->setAxisFont(QwtPlot::xBottom, axesFont);
                plot->setAxisFont(QwtPlot::yLeft, axesFont);
            } else {
                plot->setAxisFont(QwtPlot::xBottom, figureFont);
                plot->setAxisFont(QwtPlot::yLeft, figureFont);
            }

            // 坐标轴标题
            if (axes->title_visible() && !axes->title().empty()) {
                QwtText titleText(QString::fromStdString(axes->title()));
                titleText.setColor(toQColor(axes->title_color()));
                auto titleFontFamily = QString::fromStdString(axes->font());
                int titleFontSize = static_cast<int>(axes->font_size() * axes->title_font_size_multiplier());
                if (!titleFontFamily.isEmpty() && titleFontSize > 0) {
                    titleText.setFont(QFont(titleFontFamily, titleFontSize));
                }
                plot->setTitle(titleText);
            }

            // 坐标轴标签
            auto& xLabel = axes->x_axis().label();
            if (!xLabel.empty() && axes->x_axis().visible()) {
                QwtText xLabelText(QString::fromStdString(xLabel));
                xLabelText.setColor(toQColor(axes->x_axis().color()));
                plot->setAxisTitle(QwtPlot::xBottom, xLabelText);
            }
            auto& yLabel = axes->y_axis().label();
            if (!yLabel.empty() && axes->y_axis().visible()) {
                QwtText yLabelText(QString::fromStdString(yLabel));
                yLabelText.setColor(toQColor(axes->y_axis().color()));
                plot->setAxisTitle(QwtPlot::yLeft, yLabelText);
            }

            // 坐标轴可见性
            plot->setAxisVisible(QwtPlot::xBottom, axes->x_axis().visible());
            plot->setAxisVisible(QwtPlot::yLeft, axes->y_axis().visible());

            // 网格
            if (axes->grid()) {
                QwtPlotGrid* grid = new QwtPlotGrid();
                grid->enableXMin(true);
                grid->enableYMin(true);
                grid->setPen(QPen(QColor(200, 200, 200), 0.5, Qt::DotLine));
                grid->attach(plot);
            }

            // 计算轴装饰占用的像素，补偿归一化坐标使 canvas 精确定位
            QFont axisFont = plot->axisFont(QwtPlot::xBottom);
            double leftDeco = 0, rightDeco = 0, bottomDeco = 0, topDeco = 0;
            if (axes->y_axis().visible() && plot->axisScaleDraw(QwtPlot::yLeft)) {
                leftDeco = plot->axisScaleDraw(QwtPlot::yLeft)->extent(axisFont);
            }
            if (axes->x_axis().visible() && plot->axisScaleDraw(QwtPlot::xBottom)) {
                bottomDeco = plot->axisScaleDraw(QwtPlot::xBottom)->extent(axisFont);
            }
            double figW = static_cast<double>(impl_->width_);
            double figH = static_cast<double>(impl_->height_);
            fig->addAxes(plot,
                pos[0] - leftDeco / figW,
                qwtTop - topDeco / figH,
                pos[2] + (leftDeco + rightDeco) / figW,
                pos[3] + (topDeco + bottomDeco) / figH
            );
            auto legend = axes->legend();
            bool showLegend = legend && legend->visible();
            // 显示图例
            if(showLegend)
            {
                QwtPlotLegendItem* plotLegend = new QwtPlotLegendItem();
                plotLegend->setMaxColumns(1);
                QPen borderPen(Qt::black);
                borderPen.setWidth(0.5);
                plotLegend->setBorderPen(borderPen);
                plotLegend->setBackgroundBrush(QBrush(Qt::white));
                auto& items = plot->itemList();
                auto& children = axes->children();
                int lastItemSize = items.size();
                auto& strings = legend->strings();
                QwtPlotVisitor visitor(plot);
                for (int i = 0; i < children.size(); i++) {
                    auto& child = children[i];
                    child->accept(visitor);
                    if(items.size() > lastItemSize)
                    {
                        auto item = items.back();
                        if(!child->display_name().empty())
                        {
                            item->setTitle(QString::fromUtf8(child->display_name().c_str()));
                        }
                        else if(i < strings.size() && !strings[i].empty())
                        {
                            item->setTitle(QString::fromUtf8(strings[i].c_str()));
                        }
                        else
                        {
                            // 如果数据项没有显示名称，或者图例字符串为空，表示主动隐藏该数据项的图例
                            item->setItemAttribute(QwtPlotItem::Legend, false);
                        }
                        lastItemSize = items.size();
                    }
                }
                // 注意：图例必须在所有数据项之后添加，否则会导致图例显示错误
                plotLegend->attach(plot);
            }
            else{
                plot->insertLegend(nullptr);
                QwtPlotVisitor visitor(plot);
                for (auto& obj : axes->children()) {
                    obj->accept(visitor);
                }
            }
            
        }
    }
}

void aUseQwtBackend() {
    matplot::register_backend("qwt", []() -> matplot::backend::backend_interface* {
        return new QwtBackend();
    });
    matplot::change_default_backend("qwt");
}

AST_NAMESPACE_END
