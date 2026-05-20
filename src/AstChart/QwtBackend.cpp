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
#include "QwtBackend.hpp"
#include "QwtPlotVisitor.hpp"

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

#include <QApplication>
#include <QEventLoop>
#include <QColor>
#include <QFont>

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
        static int argc = 0;
        static char* argv[] = {nullptr};
        new QApplication(argc, argv);
    }
}

QwtBackend::~QwtBackend() {
    for (auto& kv : impl_->figures) {
        delete kv.second;
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
    f->draw();
    auto* fig = get_or_create_figure(f);
    fig->setAttribute(Qt::WA_DeleteOnClose);
    fig->setVisible(true);
    fig->raise();

    QEventLoop loop;
    QObject::connect(fig, &QObject::destroyed, &loop, &QEventLoop::quit);
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
    fig->setFaceColor(QColor(240, 240, 240));
    fig->resize(static_cast<int>(impl_->width_), static_cast<int>(impl_->height_));
    fig->move(static_cast<int>(impl_->pos_x_), static_cast<int>(impl_->pos_y_));
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
        auto* plot = new QwtPlot();

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

        auto pos = axes->position();
        double figW = static_cast<double>(impl_->width_);
        double figH = static_cast<double>(impl_->height_);
        float qwtTop = 1.0f - pos[1] - pos[3];
        fig->addAxes(plot,
            pos[0] - leftDeco / figW,
            qwtTop - topDeco / figH,
            pos[2] + (leftDeco + rightDeco) / figW,
            pos[3] + (topDeco + bottomDeco) / figH
        );

        QwtPlotVisitor visitor(plot);
        for (auto& obj : axes->children()) {
            obj->accept(visitor);
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
