#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstGUI/UiDouble.hpp"
#include "AstGUI/UiQuantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

AST_NAMESPACE_BEGIN

class PointMassForce;

class AST_GUI_API UiPointMassForce: public UiObject
{
    Q_OBJECT
public:
    UiPointMassForce(Object* object, QWidget *parent = nullptr);
    UiPointMassForce(QWidget *parent = nullptr);
    ~UiPointMassForce() = default;

    PointMassForce* getPointMassForce() const;
    void setPointMassForce(PointMassForce* pointMass);
    void refreshUi();
    void apply();
protected:
    void setupUi();
    void refreshGMSource();
private slots:
    void applyTo(PointMassForce* pointMass);
    void onGMSourceChanged(int index);
private:
    // 布局
    QVBoxLayout* mainLayout_{nullptr};
    
    // 引力常数来源
    QHBoxLayout* gmSourceLayout_{nullptr};
    QLabel* gmSourceLabel_{nullptr};
    QComboBox* gmSourceCombo_{nullptr};
    
    // 指定引力常数
    QHBoxLayout* specifiedGMLayout_{nullptr};
    QLabel* specifiedGMLabel_{nullptr};
    UiQuantity* specifiedGMEdit_{nullptr};
};

AST_NAMESPACE_END