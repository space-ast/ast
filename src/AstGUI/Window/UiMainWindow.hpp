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
#include <QMainWindow>
#include <QStyle>

class QTabBar;
class QStackedWidget;
class QToolButton;
class QFrame;
class QSlider;
class QLabel;
class QDockWidget;
class QTabWidget;
class QTreeWidget;

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
    void applyTheme(const QString& name);

    QWidget* setupRibbon();
    QWidget* createRibbonPage(int index);
    QToolButton* createRibbonButton(const QString& text, const QString& iconName,
                                     QStyle::StandardPixmap stdIcon, QWidget* parent);

    void setupCentralCanvas();
    void setupObjectDock();
    void setupTimeDock();
    void setupStatusBar();

    // Ribbon
    QTabBar* ribbonTabBar_ = nullptr;
    QStackedWidget* ribbonStack_ = nullptr;

    // Object dock
    QDockWidget* objectDock_ = nullptr;
    UiObjectTree* objectTree_ = nullptr;
    QTreeWidget* groupTree_ = nullptr;

    // Central
    QFrame* canvasFrame_ = nullptr;

    // Time dock
    QDockWidget* timeDock_ = nullptr;
    QSlider* timeSlider_ = nullptr;
    QLabel* startEpochLabel_ = nullptr;
    QLabel* currentEpochLabel_ = nullptr;
    QLabel* endEpochLabel_ = nullptr;

    // Status bar
    QLabel* statusReadyLabel_ = nullptr;
    QLabel* statusSceneLabel_ = nullptr;
    QLabel* statusObjectLabel_ = nullptr;
    QLabel* statusCoordLabel_ = nullptr;
    QLabel* statusUnitLabel_ = nullptr;
    QLabel* statusTimeLabel_ = nullptr;

    QString currentTheme_;
};

AST_NAMESPACE_END
