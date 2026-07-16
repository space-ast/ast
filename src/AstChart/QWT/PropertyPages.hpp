///
/// @file      PropertyPages.hpp
/// @brief     Figure 元素属性编辑表单页
/// @author    axel
/// @date      2026-06-06
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
#pragma once

#include "AstGlobal.h"
#include <QWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <array>

namespace matplot {
    class axes_type;
    class line;
    class surface;
}

AST_NAMESPACE_BEGIN

/// @brief 辅助：颜色按钮（点击弹出 QColorDialog）
class ColorButton : public QPushButton {
    Q_OBJECT
public:
    explicit ColorButton(QWidget* parent = nullptr);
    ColorButton(const ColorButton&) = delete;
    ColorButton& operator=(const ColorButton&) = delete;
    QColor color() const { return color_; }
    void setColor(const QColor& c);
signals:
    void colorChanged(const QColor& c);
private slots:
    void onClick();
private:
    QColor color_;
};

/// @brief 坐标轴属性编辑页
class AxesPropertyPage : public QWidget {
    Q_OBJECT
public:
    explicit AxesPropertyPage(QWidget* parent = nullptr);
    void load(matplot::axes_type* axes, int index);
    void apply(matplot::axes_type* axes);

private:
    void setupUi();
    QDoubleSpinBox* posLeft_, *posBottom_, *posW_, *posH_;
    QLineEdit* titleEdit_;
    QComboBox* fontCombo_;
    QDoubleSpinBox* fontSize_;
    QDoubleSpinBox* titleFontSizeMultiplier_;
    ColorButton* titleColorBtn_, *bgColorBtn_;
    QLineEdit* xLabelEdit_, *yLabelEdit_;
    QDoubleSpinBox* xMin_, *xMax_, *yMin_, *yMax_;
    QCheckBox* xVisible_, *yVisible_;
    QCheckBox* gridVisible_;
};

/// @brief 折线/散点属性编辑页
class LinePropertyPage : public QWidget {
    Q_OBJECT
public:
    explicit LinePropertyPage(QWidget* parent = nullptr);
    void load(class matplot::line* line, int index);
    void apply(class matplot::line* line);

private:
    void setupUi();
    QLineEdit* nameEdit_;
    ColorButton* lineColorBtn_;
    QComboBox* lineStyleCombo_;
    QDoubleSpinBox* lineWidth_;
    QComboBox* markerCombo_;
    QDoubleSpinBox* markerSize_;
    ColorButton* markerColorBtn_, *markerFaceBtn_;
    QCheckBox* visibleCheck_;
};

/// @brief 曲面属性编辑页
class SurfacePropertyPage : public QWidget {
    Q_OBJECT
public:
    explicit SurfacePropertyPage(QWidget* parent = nullptr);
    void load(matplot::surface* surf, int index);
    void apply(matplot::surface* surf);

private:
    void setupUi();
    QLineEdit* nameEdit_;
    QCheckBox* surfaceVisible_;
    QCheckBox* lightingCheck_;
    QDoubleSpinBox* faceAlpha_;
    ColorButton* edgeColorBtn_;
    QDoubleSpinBox* edgeWidth_;
    QCheckBox* colorbarVisible_;
};

AST_NAMESPACE_END
