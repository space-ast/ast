#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstGUI/UiFilePath.hpp"
#include "AstGUI/UiInteger.hpp"
#include "AstGUI/UiDouble.hpp"
#include "AstGUI/UiQuantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>

AST_NAMESPACE_BEGIN

class GravityForce;

class AST_GUI_API UiGravityForce: public UiObject
{
    Q_OBJECT
public:
    UiGravityForce(Object* object, QWidget *parent = nullptr);
    UiGravityForce(QWidget *parent = nullptr);
    ~UiGravityForce() = default;

    GravityForce* getGravityForce() const;
    void setGravityForce(GravityForce* gravity);
    void refreshUi();
    void apply();
protected:
    void setupUi();
    void refreshModel();
    void refreshSolidTideType();
private slots:
    void applyTo(GravityForce* gravity);
private:
    // 布局
    QVBoxLayout* mainLayout_{nullptr};
    QGridLayout* gridLayout_{nullptr};
    
    // 中心天体重力场配置
    QGroupBox* centralBodyGravityGroup_{nullptr};
    QGridLayout* centralBodyGravityLayout_{nullptr};
    QHBoxLayout* modelLayout_{nullptr};
    QLabel* modelLabel_{nullptr};
    QComboBox* modelCombo_{nullptr};
    UiFilePath* modelFileEdit_{nullptr};
    QHBoxLayout* degreeLayout_{nullptr};
    QLabel* degreeLabel_{nullptr};
    UiInteger* degreeEdit_{nullptr};
    QHBoxLayout* orderLayout_{nullptr};
    QLabel* orderLabel_{nullptr};
    UiInteger* orderEdit_{nullptr};
    QHBoxLayout* secularVariationsLayout_{nullptr};
    QCheckBox* secularVariationsCheck_{nullptr};
    
    // 固体潮汐配置
    QGroupBox* solidTidesGroup_{nullptr};
    QGridLayout* solidTidesLayout_{nullptr};
    QHBoxLayout* solidTideTypeLayout_{nullptr};
    QLabel* solidTideTypeLabel_{nullptr};
    QComboBox* solidTideTypeCombo_{nullptr};
    QHBoxLayout* timeDependentLayout_{nullptr};
    QCheckBox* timeDependentCheck_{nullptr};
    QHBoxLayout* minAmplitudeLayout_{nullptr};
    QLabel* minAmplitudeLabel_{nullptr};
    UiQuantity* minAmplitudeEdit_{nullptr};
    QHBoxLayout* truncateLayout_{nullptr};
    QCheckBox* truncateCheck_{nullptr};
    
    // 海洋潮汐配置
    QGroupBox* oceanTidesGroup_{nullptr};
    QGridLayout* oceanTidesLayout_{nullptr};
    QHBoxLayout* useOceanTidesLayout_{nullptr};
    QCheckBox* useOceanTidesCheck_{nullptr};
    QHBoxLayout* degreeOceanLayout_{nullptr};
    QLabel* degreeOceanLabel_{nullptr};
    UiInteger* degreeOceanEdit_{nullptr};
    QHBoxLayout* orderOceanLayout_{nullptr};
    QLabel* orderOceanLabel_{nullptr};
    UiInteger* orderOceanEdit_{nullptr};
    QHBoxLayout* minAmplitudeOceanLayout_{nullptr};
    QLabel* minAmplitudeOceanLabel_{nullptr};
    UiQuantity* minAmplitudeOceanEdit_{nullptr};
    

};

AST_NAMESPACE_END