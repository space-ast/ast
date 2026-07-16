///
/// @file      UiPilotConsole.hpp
/// @brief     内嵌控制台控件 — 在目标应用内提供命令输入界面
/// @author    axel
/// @date      2026-06-12
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
#include <QWidget>
#include <QList>

class QTextEdit;
class QLineEdit;
class QMainWindow;

AST_NAMESPACE_BEGIN

class PilotCommander;

/*!
    @addtogroup UiPilot
    @{
*/

/// @brief 内嵌控制台控件
/// @details 提供一个 QTextEdit（输出） + QLineEdit（输入）的命令面板。
///          可独立使用，也可通过 dockInto() 附加到 QMainWindow 底部。
class AST_UIPILOT_API UiPilotConsole : public QWidget
{
    Q_OBJECT

public:
    /// @brief 构造函数
    /// @param commander 命令调度器
    /// @param parent 父控件
    explicit UiPilotConsole(PilotCommander* commander, QWidget* parent = nullptr);

    /// @brief 析构函数
    ~UiPilotConsole() override;
    UiPilotConsole(const UiPilotConsole&) = delete;
    UiPilotConsole& operator=(const UiPilotConsole&) = delete;

    /// @brief 向输出区域追加文本
    void appendOutput(const QString& text);

    /// @brief 查找主窗口并 dock 到其底部
    /// @return 是否成功 dock
    bool dockInto(QMainWindow* mainWindow);

    /// @brief 自动查找并附加到第一个可用的 QMainWindow
    /// @return 是否成功
    bool autoDock();

    /// @brief 显示/隐藏
    void setVisible(bool visible) override;

private Q_SLOTS:
    /// @brief 用户按下 Enter 键
    void onCommandEntered();

    /// @brief 命令执行后有输出
    void onCommanderOutput(const QString& text);

Q_SIGNALS:
    /// @brief 控制台请求关闭（隐藏）
    void closeRequested();

private:
    void setupUi();
    void executeCommand(const QString& cmd);

    PilotCommander* commander_;
    QTextEdit*      outputView_;
    QLineEdit*      inputLine_;
};

/*! @} */

AST_NAMESPACE_END
