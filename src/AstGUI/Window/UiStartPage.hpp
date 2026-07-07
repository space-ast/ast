///
/// @file      UiStartPage.hpp
/// @brief     SpaceAST 起始页
/// @details   提供启动后的新建、示例和学习入口
/// @author    axel
/// @date      2026-05-27
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
#include <QString>
#include <QStyle>
#include <QVector>
#include <QWidget>

class QToolButton;

AST_NAMESPACE_BEGIN

class AST_GUI_API UiStartPage : public QWidget
{
    Q_OBJECT

public:
    explicit UiStartPage(QWidget* parent = nullptr);
    ~UiStartPage() override;

signals:
    void newTaskRequested();
    void openTaskRequested();
    void exampleRequested(const QString& name);
    void cardActivated(const QString& id);

private:
    void setupUi();

    QWidget* createSidebar();
    QWidget* createMainArea();
    QWidget* createNewTab();
    QWidget* createExampleTab();
    QWidget* createLearningTab();
    QWidget* createScrollPage(QWidget* content);
    QWidget* createSection(const QString& title, const QVector<QWidget*>& cards);
    QWidget* createCard(const QString& id, const QString& iconName, QStyle::StandardPixmap fallbackIcon,
                        const QString& title, const QString& description);
    QToolButton* createSidebarButton(const QString& text, const QString& iconName,
                                     QStyle::StandardPixmap fallbackIcon);
};

AST_NAMESPACE_END
