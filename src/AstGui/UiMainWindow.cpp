///
/// @file      UiMainWindow.cpp
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

#include "UiMainWindow.hpp"
#include <QLabel>

AST_NAMESPACE_BEGIN


UiMainWindow::UiMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置窗口标题
    setWindowTitle(QStringLiteral("AstGuiApp"));
    
    // 设置窗口大小
    resize(800, 600);
    
    // 创建一个简单的标签
    QLabel *label = new QLabel(QStringLiteral("welcome to ast project"), this);
    label->setAlignment(Qt::AlignCenter);
    
    // 将标签设置为中央部件
    setCentralWidget(label);
}

UiMainWindow::~UiMainWindow()
{
}


AST_NAMESPACE_END
