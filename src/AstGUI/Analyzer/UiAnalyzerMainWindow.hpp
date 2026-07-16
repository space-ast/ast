///
/// @file      UiAnalyzerMainWindow.hpp
/// @brief     Analyzer 模块主窗口（Ribbon 风格菜单）
/// @details   顶部 Ribbon 菜单 + 中央 UiStudyWorkbench 编辑区 + 底部状态栏
/// @author    axel
/// @date      2026-06-03
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
#include <QIcon>
#include <QMainWindow>
#include <QStyle>

class QTabBar;
class QStackedWidget;
class QToolButton;

AST_NAMESPACE_BEGIN

class UiStudyWorkbench;
class UiSweepStudy;
class StudyWorkbench;
class SweepStudy;

/// @brief Analyzer 模块主窗口，使用 Ribbon 风格菜单
class AST_GUI_API UiAnalyzerMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit UiAnalyzerMainWindow(QWidget* parent = nullptr);
    ~UiAnalyzerMainWindow() override;

    UiAnalyzerMainWindow(const UiAnalyzerMainWindow&) = delete;
    UiAnalyzerMainWindow& operator=(const UiAnalyzerMainWindow&) = delete;

    /// @brief 获取内部 UiStudyWorkbench 编辑器
    UiStudyWorkbench* studyWorkbenchEditor() const { return basicEditor_; }

    /// @brief 获取内部 UiSweepStudy 编辑器
    UiSweepStudy* sweepStudyEditor() const { return traverseEditor_; }

    /// @brief 设置要编辑的 StudyWorkbench
    void setStudyWorkbench(StudyWorkbench* analyzer);

    /// @brief 设置要编辑的 SweepStudy
    void setSweepStudy(SweepStudy* analyzer);

private:
    void setupUi();

    QWidget* setupRibbon();
    QWidget* createRibbonPage(int index);
    QToolButton* createRibbonButton(const QString& text, QStyle::StandardPixmap icon,
                                     QWidget* parent);
    QToolButton* createRibbonButton(const QString& text, const QIcon& icon,
                                     QWidget* parent);

    // Ribbon
    QTabBar*        ribbonTabBar_ = nullptr;
    QStackedWidget* ribbonStack_ = nullptr;

    // 中央编辑器
    QStackedWidget*        editorStack_ = nullptr;
    UiStudyWorkbench*       basicEditor_ = nullptr;
    UiSweepStudy* traverseEditor_ = nullptr;
};

AST_NAMESPACE_END
