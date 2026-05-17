///
/// @file      UiPropagate.hpp
/// @brief     Propagate 段编辑器
/// @details   预报器选择、时间设置、事件检测器列表
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstGUI/UiQuantity.hpp"
#include <QCheckBox>
#include <QComboBox>

class QListWidget;

AST_NAMESPACE_BEGIN

class Propagate;
class HPOP;

/// @brief Propagate 段编辑器
class AST_GUI_API UiPropagate : public UiObject
{
    Q_OBJECT
public:
    explicit UiPropagate(Object* object, QWidget* parent = nullptr);
    explicit UiPropagate(QWidget* parent = nullptr);
    ~UiPropagate() override;

    void setPropagate(Propagate* prop);
    Propagate* getPropagate() const;

private slots:
    void onMinTimeChanged();
    void onMaxTimeChanged();
    void onMaxTimeEnabledChanged(bool checked);

private:
    void setupUi();
    void refreshFromPropagate();

    QComboBox*   propagatorTypeCombo_ = nullptr;
    UiQuantity*  minTimeEdit_ = nullptr;
    UiQuantity*  maxTimeEdit_ = nullptr;
    QCheckBox*   useMaxTimeCheck_ = nullptr;
    QListWidget* eventDetectorList_ = nullptr;
};

AST_NAMESPACE_END
