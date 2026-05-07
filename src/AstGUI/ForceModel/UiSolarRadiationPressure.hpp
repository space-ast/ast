#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstGUI/UiDouble.hpp"
#include "AstGUI/UiQuantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>

AST_NAMESPACE_BEGIN

class SolarRadiationPressure;
class CelestialBody;

class AST_GUI_API UiSolarRadiationPressure: public UiObject
{
    Q_OBJECT
public:
    UiSolarRadiationPressure(Object* object, QWidget *parent = nullptr);
    UiSolarRadiationPressure(QWidget *parent = nullptr);
    ~UiSolarRadiationPressure() = default;

    SolarRadiationPressure* getSolarRadiationPressure() const;
    void setSolarRadiationPressure(SolarRadiationPressure* srp);
    void refreshUi();
    void apply();
    void applyTo(SolarRadiationPressure* srp);
protected:
    void setupUi();
    void refreshShadowModel();
    void refreshSunPosition();
    void refreshCelestialBodies();
private slots:
    void addEclipsingBody();
    void removeEclipsingBody();
private:
    // 布局
    QVBoxLayout* mainLayout_{nullptr};
    
    // 模型配置
    QGroupBox* modelGroup_{nullptr};
    QGridLayout* modelLayout_{nullptr};
    QHBoxLayout* typeLayout_{nullptr};
    QLabel* typeLabel_{nullptr};
    QComboBox* typeCombo_{nullptr};
    QHBoxLayout* crLayout_{nullptr};
    QLabel* crLabel_{nullptr};
    UiDouble* crEdit_{nullptr};
    QHBoxLayout* areaMassLayout_{nullptr};
    QLabel* areaMassLabel_{nullptr};
    UiQuantity* areaMassEdit_{nullptr};
    
    // 阴影模型
    QGroupBox* shadowGroup_{nullptr};
    QGridLayout* shadowLayout_{nullptr};
    QHBoxLayout* shadowModelLayout_{nullptr};
    QLabel* shadowModelLabel_{nullptr};
    QComboBox* shadowModelCombo_{nullptr};
    QHBoxLayout* boundaryMitigationLayout_{nullptr};
    QCheckBox* boundaryMitigationCheck_{nullptr};
    QHBoxLayout* sunPositionLayout_{nullptr};
    QLabel* sunPositionLabel_{nullptr};
    QComboBox* sunPositionCombo_{nullptr};
    QHBoxLayout* atmAltLayout_{nullptr};
    QLabel* atmAltLabel_{nullptr};
    UiQuantity* atmAltEdit_{nullptr};
    
    // 遮挡天体
    QGroupBox* eclipsingGroup_{nullptr};
    QHBoxLayout* eclipsingLayout_{nullptr};
    QVBoxLayout* availableLayout_{nullptr};
    QLabel* availableLabel_{nullptr};
    QListWidget* availableList_{nullptr};
    QVBoxLayout* buttonsLayout_{nullptr};
    QPushButton* addButton_{nullptr};
    QPushButton* removeButton_{nullptr};
    QVBoxLayout* assignedLayout_{nullptr};
    QLabel* assignedLabel_{nullptr};
    QListWidget* assignedList_{nullptr};
    
    
};

AST_NAMESPACE_END