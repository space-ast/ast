#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstGUI/UiFilePath.hpp"
#include "AstGUI/UiInteger.hpp"
#include "AstGUI/UiDouble.hpp"
#include "AstGUI/UiQuantity.hpp"
#include "AstGUI/UiGravityForce.hpp"
#include "AstGUI/UiPointMassForce.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>

AST_NAMESPACE_BEGIN

class ThirdBodyForce;
class CelestialBody;

class AST_GUI_API UiThirdBodyForce: public UiObject
{
    Q_OBJECT
public:
    UiThirdBodyForce(Object* object, QWidget *parent = nullptr);
    UiThirdBodyForce(QWidget *parent = nullptr);
    ~UiThirdBodyForce() = default;

    ThirdBodyForce* getThirdBodyForce() const;
    void setThirdBodyForce(ThirdBodyForce* thirdBody);
    void refreshUi();
    void apply();
protected:
    void setupUi();
    void refreshEphemerisSource();
    void refreshAttractionType();
    void refreshCelestialBodies();
private slots:
    void applyTo(ThirdBodyForce* thirdBody);
    void onModeChanged(int index);
private:
    // 布局
    QVBoxLayout* mainLayout_{nullptr};
    
    // 基本信息
    QHBoxLayout* thirdBodyLayout_{nullptr};
    QLabel* thirdBodyLabel_{nullptr};
    QComboBox* thirdBodyCombo_{nullptr};
    QLabel* modeLabel_{nullptr};
    QComboBox* modeCombo_{nullptr};
    
    // 星历来源
    QHBoxLayout* ephemerisLayout_{nullptr};
    QLabel* ephemerisLabel_{nullptr};
    QComboBox* ephemerisCombo_{nullptr};
    
    // 引力场配置
    QWidget* gravityFieldWidget_{nullptr};
    QVBoxLayout* gravityFieldLayout_{nullptr};
    UiGravityForce* gravityForceWidget_{nullptr};
    
    // 点质量配置
    QWidget* pointMassWidget_{nullptr};
    QVBoxLayout* pointMassLayout_{nullptr};
    UiPointMassForce* pointMassForceWidget_{nullptr};
    
    
};

AST_NAMESPACE_END