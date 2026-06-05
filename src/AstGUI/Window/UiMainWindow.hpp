///
/// @file      UiMainWindow.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include <QIcon>
#include <QMainWindow>
#include <QStyle>

class QTabBar;
class QStackedWidget;
class QToolButton;
class QLabel;
class QDockWidget;
class QTabWidget;

AST_NAMESPACE_BEGIN

class UiObjectTree;

class UiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UiMainWindow(QWidget *parent = nullptr);
    ~UiMainWindow();

private:
    void setupUi();

    QWidget* setupRibbon();
    QWidget* createRibbonPage(int index);
    QToolButton* createRibbonButton(const QString& text, const QIcon& icon,
                                     QWidget* parent);

    void setupObjectDock();
    void setupStatusBar();

    // Ribbon
    QTabBar* ribbonTabBar_ = nullptr;
    QStackedWidget* ribbonStack_ = nullptr;

    // Central tabs
    QTabWidget* centralTabs_ = nullptr;

    // Object dock
    QDockWidget* objectDock_ = nullptr;
    UiObjectTree* objectTree_ = nullptr;

    // Status bar
    QLabel* statusReadyLabel_ = nullptr;
};

AST_NAMESPACE_END
