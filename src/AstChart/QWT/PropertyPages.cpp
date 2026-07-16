///
/// @file      PropertyPages.cpp
/// @brief     Figure 元素属性编辑表单页实现
/// @author    axel
/// @date      2026-06-06
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#include "PropertyPages.hpp"
#include <matplot/core/axes_type.h>
#include <matplot/core/line_spec.h>
#include <matplot/axes_objects/line.h>
#include <matplot/axes_objects/surface.h>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QColorDialog>
#include <QFont>
#include <QLabel>
#include <QCheckBox>
#include <QDebug>
#include <type_traits>

AST_NAMESPACE_BEGIN

// ============ ColorButton ============

ColorButton::ColorButton(QWidget* parent)
    : QPushButton(parent), color_(Qt::black)
{
    setFixedSize(24, 24);
    setStyleSheet(QString("background-color: %1; border: 1px solid #888;").arg(color_.name()));
    connect(this, &QPushButton::clicked, this, &ColorButton::onClick);
}

void ColorButton::setColor(const QColor& c)
{
    color_ = c;
    setStyleSheet(QString("background-color: %1; border: 1px solid #888;").arg(c.name()));
}

void ColorButton::onClick()
{
    QColor c = QColorDialog::getColor(color_, window());
    if (c.isValid()) {
        setColor(c);
        emit colorChanged(c);
    }
}

// ============ AxesPropertyPage ============

AxesPropertyPage::AxesPropertyPage(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void AxesPropertyPage::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);

    // 位置
    auto* posGroup = new QGroupBox(tr("位置 (归一化)"), this);
    auto* posLayout = new QFormLayout(posGroup);
    posLeft_ = new QDoubleSpinBox(this); posLeft_->setRange(0, 1); posLeft_->setDecimals(3); posLeft_->setSingleStep(0.01);
    posBottom_  = new QDoubleSpinBox(this); posBottom_ ->setRange(0, 1); posBottom_ ->setDecimals(3); posBottom_ ->setSingleStep(0.01);
    posW_    = new QDoubleSpinBox(this); posW_   ->setRange(0, 1); posW_   ->setDecimals(3); posW_   ->setSingleStep(0.01);
    posH_    = new QDoubleSpinBox(this); posH_   ->setRange(0, 1); posH_   ->setDecimals(3); posH_   ->setSingleStep(0.01);
    posLayout->addRow(tr("左"), posLeft_);
    posLayout->addRow(tr("底"),  posBottom_);
    posLayout->addRow(tr("宽度"), posW_);
    posLayout->addRow(tr("高度"),posH_);
    mainLayout->addWidget(posGroup);

    // 标题
    auto* titleGroup = new QGroupBox(tr("标题"), this);
    auto* titleLayout = new QFormLayout(titleGroup);
    titleEdit_ = new QLineEdit(this);
    titleColorBtn_ = new ColorButton(this);
    titleLayout->addRow(tr("文本"), titleEdit_);
    titleLayout->addRow(tr("颜色"), titleColorBtn_);
    titleFontSizeMultiplier_ = new QDoubleSpinBox(this);
    titleFontSizeMultiplier_->setRange(0.1, 5.0);
    titleFontSizeMultiplier_->setSingleStep(0.1);
    titleFontSizeMultiplier_->setDecimals(2);
    titleLayout->addRow(tr("字体缩放"), titleFontSizeMultiplier_);
    mainLayout->addWidget(titleGroup);

    // 字体（作用于整个坐标轴）
    auto* fontGroup = new QGroupBox(tr("字体"), this);
    auto* fontLayout = new QFormLayout(fontGroup);
    fontCombo_ = new QComboBox(this);
    fontCombo_->addItems({"Helvetica", "Arial", "Times New Roman", "Courier New"});
    fontSize_ = new QDoubleSpinBox(this); fontSize_->setRange(4, 48);
    fontLayout->addRow(tr("字体"), fontCombo_);
    fontLayout->addRow(tr("字号"), fontSize_);
    mainLayout->addWidget(fontGroup);

    // X 轴
    auto* xAxisGroup = new QGroupBox(tr("X 轴"), this);
    auto* xAxisLayout = new QFormLayout(xAxisGroup);
    xLabelEdit_ = new QLineEdit(this);
    xMin_ = new QDoubleSpinBox(this); xMin_->setRange(-1e9, 1e9); xMin_->setDecimals(4);
    xMax_ = new QDoubleSpinBox(this); xMax_->setRange(-1e9, 1e9); xMax_->setDecimals(4);
    xVisible_ = new QCheckBox(tr("可见"), this);
    xVisible_->setChecked(true);
    xAxisLayout->addRow(xVisible_);
    xAxisLayout->addRow(tr("标签"), xLabelEdit_);
    xAxisLayout->addRow(tr("最小值"), xMin_);
    xAxisLayout->addRow(tr("最大值"), xMax_);
    mainLayout->addWidget(xAxisGroup);

    // Y 轴
    auto* yAxisGroup = new QGroupBox(tr("Y 轴"), this);
    auto* yAxisLayout = new QFormLayout(yAxisGroup);
    yLabelEdit_ = new QLineEdit(this);
    yMin_ = new QDoubleSpinBox(this); yMin_->setRange(-1e9, 1e9); yMin_->setDecimals(4);
    yMax_ = new QDoubleSpinBox(this); yMax_->setRange(-1e9, 1e9); yMax_->setDecimals(4);
    yVisible_ = new QCheckBox(tr("可见"), this);
    yVisible_->setChecked(true);
    yAxisLayout->addRow(yVisible_);
    yAxisLayout->addRow(tr("标签"), yLabelEdit_);
    yAxisLayout->addRow(tr("最小值"), yMin_);
    yAxisLayout->addRow(tr("最大值"), yMax_);
    mainLayout->addWidget(yAxisGroup);

    // 外观
    auto* appearGroup = new QGroupBox(tr("外观"), this);
    auto* appearLayout = new QFormLayout(appearGroup);
    bgColorBtn_ = new ColorButton(this);
    gridVisible_ = new QCheckBox(tr("显示网格"), this);
    appearLayout->addRow(tr("背景色"), bgColorBtn_);
    appearLayout->addRow(gridVisible_);
    mainLayout->addWidget(appearGroup);

    mainLayout->addStretch();
}

void AxesPropertyPage::load(matplot::axes_type* axes, int /*index*/)
{
    if (!axes) return;

    auto& pos = axes->position();
    posLeft_->setValue(pos[0]);
    posBottom_ ->setValue(pos[1]);
    posW_   ->setValue(pos[2]);
    posH_   ->setValue(pos[3]);

    titleEdit_->setText(QString::fromStdString(axes->title()));
    {
        QString fontName = QString::fromStdString(axes->font());
        int idx = fontCombo_->findText(fontName);
        if (idx >= 0) fontCombo_->setCurrentIndex(idx);
    }
    fontSize_->setValue(axes->font_size() > 0 ? axes->font_size() : 10.0);

    xLabelEdit_->setText(QString::fromStdString(axes->x_axis().label()));
    yLabelEdit_->setText(QString::fromStdString(axes->y_axis().label()));
    auto xl = axes->xlim(); xMin_->setValue(xl[0]); xMax_->setValue(xl[1]);
    auto yl = axes->ylim(); yMin_->setValue(yl[0]); yMax_->setValue(yl[1]);
    xVisible_->setChecked(axes->x_axis().visible());
    yVisible_->setChecked(axes->y_axis().visible());

    gridVisible_->setChecked(axes->grid());

    auto& c = axes->color();
    bgColorBtn_->setColor(QColor::fromRgbF(c[1], c[2], c[3], 1.0 - c[0]));

    auto& tc = axes->title_color();
    titleColorBtn_->setColor(QColor::fromRgbF(tc[1], tc[2], tc[3], 1.0 - tc[0]));

    titleFontSizeMultiplier_->setValue(axes->title_font_size_multiplier());
}

void AxesPropertyPage::apply(matplot::axes_type* axes)
{
    if (!axes) return;

    axes->position({static_cast<float>(posLeft_->value()),
                    static_cast<float>(posBottom_->value()),
                    static_cast<float>(posW_->value()),
                    static_cast<float>(posH_->value())});

    axes->title(titleEdit_->text().toStdString());
    axes->font(fontCombo_->currentText().toStdString());
    axes->font_size(static_cast<float>(fontSize_->value()));

    axes->x_axis().label(xLabelEdit_->text().toStdString());
    axes->y_axis().label(yLabelEdit_->text().toStdString());
    axes->xlim({xMin_->value(), xMax_->value()});
    axes->ylim({yMin_->value(), yMax_->value()});
    axes->x_axis().visible(xVisible_->isChecked());
    axes->y_axis().visible(yVisible_->isChecked());

    axes->grid(gridVisible_->isChecked());

    QColor c = bgColorBtn_->color();
    axes->color({1.0f - static_cast<float>(c.alphaF()),
                 static_cast<float>(c.redF()),
                 static_cast<float>(c.greenF()),
                 static_cast<float>(c.blueF())});

    QColor tc = titleColorBtn_->color();
    axes->title_color({1.0f - static_cast<float>(tc.alphaF()),
                       static_cast<float>(tc.redF()),
                       static_cast<float>(tc.greenF()),
                       static_cast<float>(tc.blueF())});

    axes->title_font_size_multiplier(static_cast<float>(titleFontSizeMultiplier_->value()));
}

// ============ LinePropertyPage ============

LinePropertyPage::LinePropertyPage(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void LinePropertyPage::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);

    visibleCheck_ = new QCheckBox(tr("可见"), this);
    visibleCheck_->setChecked(true);
    mainLayout->addWidget(visibleCheck_);

    auto* nameGroup = new QGroupBox(tr("基本"), this);
    auto* nameLayout = new QFormLayout(nameGroup);
    nameEdit_ = new QLineEdit(this);
    nameLayout->addRow(tr("显示名称"), nameEdit_);
    mainLayout->addWidget(nameGroup);

    auto* lineGroup = new QGroupBox(tr("线条"), this);
    auto* lineLayout = new QFormLayout(lineGroup);
    lineColorBtn_ = new ColorButton(this);
    lineStyleCombo_ = new QComboBox(this);
    // matplot::line_spec::line_style 存在同名函数，无法直接写 using line_style = matplot::line_spec::line_style;
    using line_style = decltype(matplot::line_spec::line_style::solid_line);
    lineStyleCombo_->addItem(tr("实线"),   static_cast<int>(line_style::solid_line));    // solid_line
    lineStyleCombo_->addItem(tr("虚线"),   static_cast<int>(line_style::dashed_line));   // dashed_line
    lineStyleCombo_->addItem(tr("点线"),   static_cast<int>(line_style::dotted_line));   // dotted_line
    lineStyleCombo_->addItem(tr("点划线"), static_cast<int>(line_style::dash_dot_line)); // dash_dot_line
    lineStyleCombo_->addItem(tr("无线"),   static_cast<int>(line_style::none));          // none
    lineWidth_ = new QDoubleSpinBox(this); lineWidth_->setRange(0.1, 10); lineWidth_->setValue(1.0);
    lineLayout->addRow(tr("颜色"), lineColorBtn_);
    lineLayout->addRow(tr("线型"), lineStyleCombo_);
    lineLayout->addRow(tr("线宽"), lineWidth_);
    mainLayout->addWidget(lineGroup);

    auto* markerGroup = new QGroupBox(tr("标记"), this);
    auto* markerLayout = new QFormLayout(markerGroup);
    markerCombo_ = new QComboBox(this);
    // matplot::line_spec::marker_style 存在同名函数，无法直接写 using marker_style = matplot::line_spec::marker_style;
    using marker_style = decltype(matplot::line_spec::marker_style::none);
    markerCombo_->addItem(tr("无"), static_cast<int>(marker_style::none));
    markerCombo_->addItem("o",     static_cast<int>(marker_style::circle));
    markerCombo_->addItem("+",     static_cast<int>(marker_style::plus_sign));
    markerCombo_->addItem("*",     static_cast<int>(marker_style::asterisk));
    markerCombo_->addItem("x",     static_cast<int>(marker_style::cross));
    markerCombo_->addItem("s",     static_cast<int>(marker_style::square));
    markerCombo_->addItem("d",     static_cast<int>(marker_style::diamond));
    markerCombo_->addItem("^",     static_cast<int>(marker_style::upward_pointing_triangle));
    markerCombo_->addItem("v",     static_cast<int>(marker_style::downward_pointing_triangle));
    markerSize_ = new QDoubleSpinBox(this); markerSize_->setRange(1, 20); markerSize_->setValue(6);
    markerColorBtn_ = new ColorButton(this);
    markerFaceBtn_ = new ColorButton(this);
    markerLayout->addRow(tr("样式"), markerCombo_);
    markerLayout->addRow(tr("大小"), markerSize_);
    markerLayout->addRow(tr("颜色"), markerColorBtn_);
    markerLayout->addRow(tr("填充色"), markerFaceBtn_);
    mainLayout->addWidget(markerGroup);

    mainLayout->addStretch();
}

void LinePropertyPage::load(class matplot::line* line, int /*index*/)
{
    if (!line) return;

    nameEdit_->setText(QString::fromStdString(line->display_name()));

    auto& c = line->color();
    lineColorBtn_->setColor(QColor::fromRgbF(c[1], c[2], c[3], 1.0 - c[0]));
    lineWidth_->setValue(line->line_width());

    {
        int ls = static_cast<int>(line->line_spec().line_style());
        int idx = lineStyleCombo_->findData(ls);
        if (idx >= 0) lineStyleCombo_->setCurrentIndex(idx);
    }

    {
        int ms = static_cast<int>(line->marker_style());
        int idx = markerCombo_->findData(ms);
        if (idx >= 0) markerCombo_->setCurrentIndex(idx);
    }
    markerSize_->setValue(line->marker_size());

    auto& mc = line->marker_color();
    markerColorBtn_->setColor(QColor::fromRgbF(mc[1], mc[2], mc[3], 1.0 - mc[0]));
    auto& mf = line->marker_face_color();
    markerFaceBtn_->setColor(QColor::fromRgbF(mf[1], mf[2], mf[3], 1.0 - mf[0]));

    visibleCheck_->setChecked(line->visible());
}

void LinePropertyPage::apply(class matplot::line* line)
{
    if (!line) return;

    line->display_name(nameEdit_->text().toStdString());

    QColor lc = lineColorBtn_->color();
    line->color({1.0f - static_cast<float>(lc.alphaF()),
                 static_cast<float>(lc.redF()),
                 static_cast<float>(lc.greenF()),
                 static_cast<float>(lc.blueF())});
    {
        auto& ls = line->line_spec();
        ls.line_style(static_cast<enum matplot::line_spec::line_style>(lineStyleCombo_->currentData().toInt()));
    }
    line->line_width(lineWidth_->value());

    line->marker_style(static_cast<enum matplot::line_spec::marker_style>(markerCombo_->currentData().toInt()));
    line->marker_size(markerSize_->value());

    QColor mc = markerColorBtn_->color();
    line->marker_color({1.0f - static_cast<float>(mc.alphaF()),
                        static_cast<float>(mc.redF()),
                        static_cast<float>(mc.greenF()),
                        static_cast<float>(mc.blueF())});
    QColor mf = markerFaceBtn_->color();
    // qDebug() << "faceColor:" << mf;
    line->marker_face_color({1.0f - static_cast<float>(mf.alphaF()),
                             static_cast<float>(mf.redF()),
                             static_cast<float>(mf.greenF()),
                             static_cast<float>(mf.blueF())});

    line->visible(visibleCheck_->isChecked());
}

// ============ SurfacePropertyPage ============

SurfacePropertyPage::SurfacePropertyPage(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void SurfacePropertyPage::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);

    auto* nameGroup = new QGroupBox(tr("基本"), this);
    auto* nameLayout = new QFormLayout(nameGroup);
    nameEdit_ = new QLineEdit(this);
    surfaceVisible_ = new QCheckBox(tr("曲面可见"), this);
    surfaceVisible_->setChecked(true);
    nameLayout->addRow(tr("显示名称"), nameEdit_);
    nameLayout->addRow(surfaceVisible_);
    mainLayout->addWidget(nameGroup);

    auto* appearGroup = new QGroupBox(tr("外观"), this);
    auto* appearLayout = new QFormLayout(appearGroup);
    lightingCheck_ = new QCheckBox(tr("光照"), this);
    faceAlpha_ = new QDoubleSpinBox(this); faceAlpha_->setRange(0, 1); faceAlpha_->setValue(1.0);
    edgeColorBtn_ = new ColorButton(this);
    edgeWidth_ = new QDoubleSpinBox(this); edgeWidth_->setRange(0.1, 5); edgeWidth_->setValue(0.5);
    appearLayout->addRow(lightingCheck_);
    appearLayout->addRow(tr("透明度"), faceAlpha_);
    appearLayout->addRow(tr("边缘颜色"), edgeColorBtn_);
    appearLayout->addRow(tr("边缘线宽"), edgeWidth_);
    mainLayout->addWidget(appearGroup);

    auto* cbGroup = new QGroupBox(tr("色条"), this);
    auto* cbLayout = new QFormLayout(cbGroup);
    colorbarVisible_ = new QCheckBox(tr("显示色条"), this);
    cbLayout->addRow(colorbarVisible_);
    mainLayout->addWidget(cbGroup);

    mainLayout->addStretch();
}

void SurfacePropertyPage::load(class matplot::surface* surf, int /*index*/)
{
    if (!surf) return;

    nameEdit_->setText(QString::fromStdString(surf->display_name()));
    surfaceVisible_->setChecked(surf->surface_visible());
    lightingCheck_->setChecked(surf->lighting());
    faceAlpha_->setValue(surf->face_alpha());
    // edge color not directly exposed, use line_spec
    edgeWidth_->setValue(surf->line_width());

    if (auto* parent = surf->parent())
        colorbarVisible_->setChecked(parent->cb_axis().visible());
}

void SurfacePropertyPage::apply(class matplot::surface* surf)
{
    if (!surf) return;

    surf->display_name(nameEdit_->text().toStdString());
    surf->surface_visible(surfaceVisible_->isChecked());
    surf->lighting(lightingCheck_->isChecked());
    surf->face_alpha(faceAlpha_->value());
    surf->line_width(edgeWidth_->value());

    if (auto* parent = surf->parent())
        parent->cb_axis().visible(colorbarVisible_->isChecked());
}

AST_NAMESPACE_END
