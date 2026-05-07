///
/// @file      UiThirdBodyForceList.hpp
/// @brief     三体引力模型列表界面
/// @author    axel
/// @date      2026-04-23
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstCore/ThirdBodyForce.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstGUI/UiThirdBodyForce.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>

AST_NAMESPACE_BEGIN

class AST_GUI_API UiThirdBodyForceList: public UiObject
{
    Q_OBJECT
public:
    UiThirdBodyForceList(Object* object, QWidget *parent = nullptr);
    UiThirdBodyForceList(QWidget *parent = nullptr);
    ~UiThirdBodyForceList() = default;

    HPOPForceModel* getHPOPForceModel() const;
    void setHPOPForceModel(HPOPForceModel* hpop);
    void refreshUi();
    void apply();
protected:
    void setupUi();
private slots:
    void onAddThirdBody();
    void onRemoveThirdBody();
    void onThirdBodySelected(int index);
private:
    // 布局
    QVBoxLayout* mainLayout_{nullptr};
    QHBoxLayout* topLayout_{nullptr};
    QListWidget* thirdBodyList_{nullptr};
    QVBoxLayout* buttonLayout_{nullptr};
    QPushButton* addButton_{nullptr};
    QPushButton* removeButton_{nullptr};
    QGroupBox* detailGroup_{nullptr};
    QVBoxLayout* detailLayout_{nullptr};
    UiThirdBodyForce* thirdBodyWidget_{nullptr};
    
    // 数据
    HPOPForceModel* hpopForceModel_{nullptr};
};

AST_NAMESPACE_END