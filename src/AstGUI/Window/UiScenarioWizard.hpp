///
/// @file      UiScenarioWizard.hpp
/// @brief     新建场景向导对话框
/// @details   用于创建新任务场景，包含场景名称、描述、存储目录、
///            起止时间以及中心天体选择。
/// @author    axel
/// @date      2026-09-07
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
#include "AstCore/TimePoint.hpp"
#include <QDialog>
#include <QString>

class QLineEdit;
class QTextEdit;
class QComboBox;
class QPushButton;
class QLabel;

AST_NAMESPACE_BEGIN

class UiTimePoint;

/// @brief 新建场景向导对话框
class AST_GUI_API UiScenarioWizard : public QDialog
{
    Q_OBJECT

public:
    explicit UiScenarioWizard(QWidget* parent = nullptr);
    ~UiScenarioWizard() override;
    UiScenarioWizard(const UiScenarioWizard&) = delete;
    UiScenarioWizard& operator=(const UiScenarioWizard&) = delete;

    /// @brief 场景名称
    QString scenarioName() const;

    /// @brief 场景描述
    QString scenarioDescription() const;

    /// @brief 场景存储目录
    QString scenarioLocation() const;

    /// @brief 场景起始时间点（绝对时间）
    TimePoint startTime() const;

    /// @brief 场景终止时间点（绝对时间）
    TimePoint stopTime() const;

    /// @brief 中心天体名称（如 "Earth"）
    QString centralBody() const;

private:
    void setupUi();
    void setupConnections();
    void applyTimeDefaults();
    void browseLocation();

private:
    QLineEdit*   nameEdit_ = nullptr;
    QTextEdit*   descEdit_ = nullptr;
    QLineEdit*   locationEdit_ = nullptr;
    UiTimePoint* startEdit_ = nullptr;
    UiTimePoint* stopEdit_ = nullptr;
    QComboBox*   centralBodyCombo_ = nullptr;
    QPushButton* okButton_ = nullptr;
    QPushButton* cancelButton_ = nullptr;
};

AST_NAMESPACE_END
