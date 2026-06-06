///
/// @file      PropertyPages.cpp
/// @brief     Figure 元素属性编辑表单页实现
/// @author    axel
/// @date      2026-06-06
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#include "PropertyPages.hpp"
#include <matplot/core/axes_type.h>
#include <matplot/axes_objects/line.h>
#include <matplot/axes_objects/surface.h>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QColorDialog>
#include <QFont>
#include <QLabel>
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
    QColor c = QColorDialog::getColor(color_, this);
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
    posLeft_ = new QDoubleSpinBox(this); posLeft_->setRange(0, 1); posLeft_->setDecimals(3);
    posTop_  = new QDoubleSpinBox(this); posTop_ ->setRange(0, 1); posTop_ ->setDecimals(3);
    posW_    = new QDoubleSpinBox(this); posW_   ->setRange(0, 1); posW_   ->setDecimals(3);
    posH_    = new QDoubleSpinBox(this); posH_   ->setRange(0, 1); posH_   ->setDecimals(3);
    posLayout->addRow(tr("Left"), posLeft_);
    posLayout->addRow(tr("Top"),  posTop_);
    posLayout->addRow(tr("Width"), posW_);
    posLayout->addRow(tr("Height"),posH_);
    mainLayout->addWidget(posGroup);

    // 标题
    auto* titleGroup = new QGroupBox(tr("标题"), this);
    auto* titleLayout = new QFormLayout(titleGroup);
    titleEdit_ = new QLineEdit(this);
    fontCombo_ = new QComboBox(this);
    fontCombo_->addItems({"Helvetica", "Arial", "Times New Roman", "Courier New"});
    fontSize_ = new QDoubleSpinBox(this); fontSize_->setRange(4, 48);
    titleColorBtn_ = new ColorButton(this);
    titleLayout->addRow(tr("标题"), titleEdit_);
    titleLayout->addRow(tr("字体"), fontCombo_);
    titleLayout->addRow(tr("字号"), fontSize_);
    titleLayout->addRow(tr("颜色"), titleColorBtn_);
    mainLayout->addWidget(titleGroup);

    // 轴
    auto* axisGroup = new QGroupBox(tr("坐标轴"), this);
    auto* axisLayout = new QFormLayout(axisGroup);
    xLabelEdit_ = new QLineEdit(this);
    yLabelEdit_ = new QLineEdit(this);
    xMin_ = new QDoubleSpinBox(this); xMin_->setRange(-1e9, 1e9); xMin_->setDecimals(4);
    xMax_ = new QDoubleSpinBox(this); xMax_->setRange(-1e9, 1e9); xMax_->setDecimals(4);
    yMin_ = new QDoubleSpinBox(this); yMin_->setRange(-1e9, 1e9); yMin_->setDecimals(4);
    yMax_ = new QDoubleSpinBox(this); yMax_->setRange(-1e9, 1e9); yMax_->setDecimals(4);
    xVisible_ = new QCheckBox(tr("X 轴可见"), this);
    yVisible_ = new QCheckBox(tr("Y 轴可见"), this);
    xVisible_->setChecked(true);
    yVisible_->setChecked(true);
    axisLayout->addRow(tr("X 标签"), xLabelEdit_);
    axisLayout->addRow(tr("Y 标签"), yLabelEdit_);
    axisLayout->addRow(tr("X 范围"), xMin_);
    axisLayout->addRow("", xMax_);
    axisLayout->addRow(tr("Y 范围"), yMin_);
    axisLayout->addRow("", yMax_);
    axisLayout->addRow(xVisible_);
    axisLayout->addRow(yVisible_);
    mainLayout->addWidget(axisGroup);

    // 外观
    auto* appearGroup = new QGroupBox(tr("外观"), this);
    auto* appearLayout = new QFormLayout(appearGroup);
    bgColorBtn_ = new ColorButton(this);
    gridVisible_ = new QCheckBox(tr("显示网格"), this);
    boxVisible_ = new QCheckBox(tr("显示边框"), this);
    boxVisible_->setChecked(true);
    appearLayout->addRow(tr("背景色"), bgColorBtn_);
    appearLayout->addRow(gridVisible_);
    appearLayout->addRow(boxVisible_);
    mainLayout->addWidget(appearGroup);

    mainLayout->addStretch();
}

void AxesPropertyPage::load(matplot::axes_type* axes, int /*index*/)
{
    if (!axes) return;

    auto& pos = axes->position();
    posLeft_->setValue(pos[0]);
    posTop_ ->setValue(pos[1]);
    posW_   ->setValue(pos[2]);
    posH_   ->setValue(pos[3]);

    titleEdit_->setText(QString::fromStdString(axes->title()));
    fontSize_->setValue(axes->font_size() > 0 ? axes->font_size() : 10.0);

    xLabelEdit_->setText(QString::fromStdString(axes->x_axis().label()));
    yLabelEdit_->setText(QString::fromStdString(axes->y_axis().label()));
    auto xl = axes->xlim(); xMin_->setValue(xl[0]); xMax_->setValue(xl[1]);
    auto yl = axes->ylim(); yMin_->setValue(yl[0]); yMax_->setValue(yl[1]);
    xVisible_->setChecked(axes->x_axis().visible());
    yVisible_->setChecked(axes->y_axis().visible());

    gridVisible_->setChecked(axes->grid());
    boxVisible_->setChecked(axes->box());

    auto& c = axes->color();
    bgColorBtn_->setColor(QColor::fromRgbF(c[1], c[2], c[3], 1.0 - c[0]));
}

void AxesPropertyPage::apply(matplot::axes_type* axes)
{
    if (!axes) return;

    axes->position({static_cast<float>(posLeft_->value()),
                    static_cast<float>(posTop_->value()),
                    static_cast<float>(posW_->value()),
                    static_cast<float>(posH_->value())});

    axes->title(titleEdit_->text().toStdString());
    axes->font_size(static_cast<float>(fontSize_->value()));

    axes->x_axis().label(xLabelEdit_->text().toStdString());
    axes->y_axis().label(yLabelEdit_->text().toStdString());
    axes->xlim({xMin_->value(), xMax_->value()});
    axes->ylim({yMin_->value(), yMax_->value()});
    axes->x_axis().visible(xVisible_->isChecked());
    axes->y_axis().visible(yVisible_->isChecked());

    axes->grid(gridVisible_->isChecked());
    axes->box(boxVisible_->isChecked());

    QColor c = bgColorBtn_->color();
    axes->color({1.0f - static_cast<float>(c.alphaF()),
                 static_cast<float>(c.redF()),
                 static_cast<float>(c.greenF()),
                 static_cast<float>(c.blueF())});
}

// ============ LinePropertyPage ============

LinePropertyPage::LinePropertyPage(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void LinePropertyPage::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);

    auto* nameGroup = new QGroupBox(tr("基本"), this);
    auto* nameLayout = new QFormLayout(nameGroup);
    nameEdit_ = new QLineEdit(this);
    nameLayout->addRow(tr("显示名称"), nameEdit_);
    mainLayout->addWidget(nameGroup);

    auto* lineGroup = new QGroupBox(tr("线条"), this);
    auto* lineLayout = new QFormLayout(lineGroup);
    lineColorBtn_ = new ColorButton(this);
    lineStyleCombo_ = new QComboBox(this);
    lineStyleCombo_->addItems({tr("实线"), tr("虚线"), tr("点线"), tr("点划线"), tr("无线")});
    lineWidth_ = new QDoubleSpinBox(this); lineWidth_->setRange(0.1, 10); lineWidth_->setValue(1.0);
    lineLayout->addRow(tr("颜色"), lineColorBtn_);
    lineLayout->addRow(tr("线型"), lineStyleCombo_);
    lineLayout->addRow(tr("线宽"), lineWidth_);
    mainLayout->addWidget(lineGroup);

    auto* markerGroup = new QGroupBox(tr("标记"), this);
    auto* markerLayout = new QFormLayout(markerGroup);
    markerCombo_ = new QComboBox(this);
    markerCombo_->addItems({tr("无"), "o", "+", "*", "x", "s", "d", "^", "v"});
    markerSize_ = new QDoubleSpinBox(this); markerSize_->setRange(1, 20); markerSize_->setValue(6);
    markerColorBtn_ = new ColorButton(this);
    markerFaceBtn_ = new ColorButton(this);
    markerLayout->addRow(tr("样式"), markerCombo_);
    markerLayout->addRow(tr("大小"), markerSize_);
    markerLayout->addRow(tr("颜色"), markerColorBtn_);
    markerLayout->addRow(tr("填充色"), markerFaceBtn_);
    mainLayout->addWidget(markerGroup);

    visibleCheck_ = new QCheckBox(tr("可见"), this);
    visibleCheck_->setChecked(true);
    mainLayout->addWidget(visibleCheck_);

    mainLayout->addStretch();
}

void LinePropertyPage::load(matplot::line* line, int /*index*/)
{
    if (!line) return;

    nameEdit_->setText(QString::fromStdString(line->display_name()));

    auto& c = line->color();
    lineColorBtn_->setColor(QColor::fromRgbF(c[1], c[2], c[3], 1.0 - c[0]));
    lineWidth_->setValue(line->line_width());

    int ls = static_cast<int>(line->line_spec().line_style());
    lineStyleCombo_->setCurrentIndex(ls);

    int ms = static_cast<int>(line->marker_style());
    markerCombo_->setCurrentIndex(ms + 1);  // +1 because index 0 is "无"
    markerSize_->setValue(line->marker_size());

    auto& mc = line->marker_color();
    markerColorBtn_->setColor(QColor::fromRgbF(mc[1], mc[2], mc[3], 1.0 - mc[0]));
    auto& mf = line->marker_face_color();
    markerFaceBtn_->setColor(QColor::fromRgbF(mf[1], mf[2], mf[3], 1.0 - mf[0]));

    visibleCheck_->setChecked(line->visible());
}

void LinePropertyPage::apply(matplot::line* line)
{
    if (!line) return;

    line->display_name(nameEdit_->text().toStdString());

    QColor lc = lineColorBtn_->color();
    line->color({1.0f - static_cast<float>(lc.alphaF()),
                 static_cast<float>(lc.redF()),
                 static_cast<float>(lc.greenF()),
                 static_cast<float>(lc.blueF())});
    // 使用 decltype 推导枚举类型，避免 unity build 中类型名不可见的问题
    static const int lsMap[] = {1, 2, 3, 4, 0}; // none=0,solid=1,dashed=2,dotted=3,dash_dot=4
    {
        auto& ls = line->line_spec();
        using LS = std::decay<decltype(ls.line_style())>::type;
        ls.line_style(static_cast<LS>(lsMap[lineStyleCombo_->currentIndex()]));
    }
    line->line_width(lineWidth_->value());

    if (markerCombo_->currentIndex() == 0) {
        line->marker_style("none");
    } else {
        static const char* msNames[] = {"o", "+", "*", "x", "s", "d", "^", "v"};
        int idx = markerCombo_->currentIndex() - 1;
        if (idx >= 0 && idx < 8)
            line->marker_style(msNames[idx]);
    }
    line->marker_size(markerSize_->value());

    QColor mc = markerColorBtn_->color();
    line->marker_color({1.0f - static_cast<float>(mc.alphaF()),
                        static_cast<float>(mc.redF()),
                        static_cast<float>(mc.greenF()),
                        static_cast<float>(mc.blueF())});
    QColor mf = markerFaceBtn_->color();
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

void SurfacePropertyPage::load(matplot::surface* surf, int /*index*/)
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

void SurfacePropertyPage::apply(matplot::surface* surf)
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
