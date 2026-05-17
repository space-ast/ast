///
/// @file      UiManeuver.hpp
/// @brief     Maneuver 段编辑器
/// @details   发动机选择、姿态设置
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include <QComboBox>

class QLabel;

AST_NAMESPACE_BEGIN

class Maneuver;
class Burn;

/// @brief Maneuver 段编辑器
class AST_GUI_API UiManeuver : public UiObject
{
    Q_OBJECT
public:
    explicit UiManeuver(Object* object, QWidget* parent = nullptr);
    explicit UiManeuver(QWidget* parent = nullptr);
    ~UiManeuver() override;

    void setManeuver(Maneuver* maneuver);
    Maneuver* getManeuver() const;

private:
    void setupUi();
    void refreshBurnInfo();

    QComboBox* burnTypeCombo_ = nullptr;
    QLabel*    burnInfoLabel_ = nullptr;
};

AST_NAMESPACE_END
