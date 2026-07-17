///
/// @file      UiCelestialBody.hpp
/// @brief     天体编辑界面
/// @details   参考STK Central Bodies属性窗口设计
/// @author    Aist
/// @date      2026-04-10
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstGUI/UiQuantity.hpp"

class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;

AST_NAMESPACE_BEGIN

class CelestialBody;

class AST_GUI_API UiCelestialBody: public UiObject
{
    Q_OBJECT
public:
    UiCelestialBody(Object* object, QWidget* parent = nullptr);
    UiCelestialBody(QWidget* parent = nullptr);
    ~UiCelestialBody() = default;
    UiCelestialBody(const UiCelestialBody&) = delete;
    UiCelestialBody& operator=(const UiCelestialBody&) = delete;

    void refreshUi();
    void apply();
    
    void setCelestialBody(CelestialBody* body);
    CelestialBody* getCelestialBody() const;

signals:
    void celestialBodyChanged(CelestialBody* body);

private slots:
    void onGravityModelDetails();
    void onOrientationDetails();

private:
    // 基本信息
    QLineEdit* nameEdit_{nullptr};
    
    // 物理参数
    UiQuantity* gmEdit_{nullptr};
    QLabel* parentLabel_{nullptr};
    QLabel* childrenLabel_{nullptr};
    
    // 引力模型
    QComboBox* gravityModelCombo_{nullptr};
    QPushButton* gravityModelDetailsBtn_{nullptr};
    
    // 形状
    QComboBox* shapeCombo_{nullptr};
    UiQuantity* semiMajorAxisEdit_{nullptr};
    UiQuantity* semiIntermediateAxisEdit_{nullptr};
    UiQuantity* semiMinorAxisEdit_{nullptr};
    
    // 姿态
    QComboBox* orientationCombo_{nullptr};
    QPushButton* orientationDetailsBtn_{nullptr};
    
    // 星历
    QComboBox* ephemerisCombo_{nullptr};
    QLabel* spiceIdLabel_{nullptr};
};

AST_NAMESPACE_END
