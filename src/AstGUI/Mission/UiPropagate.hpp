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
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstGUI/UiQuantity.hpp"
#include <QCheckBox>

class QPushButton;

AST_NAMESPACE_BEGIN

class Propagate;
class HPOP;
class UiEventDetectorList;

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
    void onMaxTimeChanged();
    void onMaxTimeEnabledChanged(bool checked);
    void onConfigureForceModel();
    void onConfigureIntegrator();

private:
    void setupUi();
    void refreshFromPropagate();

    UiQuantity*  maxTimeEdit_ = nullptr;
    QCheckBox*   useMaxTimeCheck_ = nullptr;
    QPushButton* forceModelBtn_ = nullptr;
    QPushButton* integratorBtn_ = nullptr;
    UiEventDetectorList* eventDetectorList_ = nullptr;
};

AST_NAMESPACE_END
