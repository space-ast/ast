///
/// @file      testUiInitialState.cpp
/// @brief     交互式测试 UiInitialState — 状态类型切换、笛卡尔/开普勒编辑
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

#include "AstGUI/UiInitialState.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>

using namespace ast;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // ---- 构造测试数据 ----
    auto* init = aNewObject<InitialState>();
    init->setName("TestInitialState");

    auto* scState = SpacecraftState::NewDefault();
    // SpacecraftState::NewDefault 创建 Keplerian 状态

    // ---- 主窗口 ----
    QMainWindow window;
    auto* central = new QWidget(&window);
    auto* rootLayout = new QVBoxLayout(central);

    // ---- UiInitialState 编辑器 ----
    auto* editor = new UiInitialState(central);
    // 绑定 InitialState（会读取其 outputState 并初始化编辑器）
    editor->setInitialState(init);

    // ---- 信息标签 ----
    auto* infoLabel = new QLabel(central);
    infoLabel->setWordWrap(true);
    infoLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    infoLabel->setMinimumHeight(60);

    // ---- 操作按钮 ----
    auto* btnGroup = new QGroupBox(QStringLiteral("操作"), central);
    auto* btnLayout = new QHBoxLayout(btnGroup);

    auto* printBtn = new QPushButton(QStringLiteral("打印状态信息"), central);
    auto* verifyBtn = new QPushButton(QStringLiteral("验证 getInitialState"), central);

    btnLayout->addWidget(printBtn);
    btnLayout->addWidget(verifyBtn);
    btnLayout->addStretch();

    // ---- 组装界面 ----
    rootLayout->addWidget(new QLabel(QStringLiteral("InitialState 编辑器 — 切换状态类型查看效果"), central));
    rootLayout->addWidget(editor);
    rootLayout->addWidget(infoLabel);
    rootLayout->addWidget(btnGroup);

    // ---- 连接 ----
    QObject::connect(printBtn, &QPushButton::clicked, [init, scState, infoLabel]() {
        auto* os = scState->getOrbitState();
        if (!os)
        {
            infoLabel->setText(QStringLiteral("orbitState 为空"));
            return;
        }

        QString info;
        info += QStringLiteral("InitialState 名称: %1\n").arg(QString::fromStdString(init->getName()));

        EStateType t = scState->getStateType();
        info += QStringLiteral("状态类型: %1\n").arg(t == EStateType::eCartesian ? "Cartesian" : "Keplerian");

        if (auto* cart = aobject_cast<StateCartesian*>(os))
        {
            info += QStringLiteral("位置: (%1, %2, %3) m\n")
                .arg(cart->x()).arg(cart->y()).arg(cart->z());
            info += QStringLiteral("速度: (%1, %2, %3) m/s\n")
                .arg(cart->vx()).arg(cart->vy()).arg(cart->vz());
        }
        else if (auto* kep = aobject_cast<StateKeplerian*>(os))
        {
            ModOrbElem oe;
            kep->getState(oe);
            info += QStringLiteral("SMA: %1 m, Ecc: %2, Inc: %3 deg\n")
                .arg(oe.getSMA()).arg(oe.getEcc()).arg(oe.getInc());
        }

        infoLabel->setText(info);
        qDebug() << info;
    });

    QObject::connect(verifyBtn, &QPushButton::clicked, [editor, init, infoLabel]() {
        auto* got = editor->getInitialState();
        if (got == init)
            infoLabel->setText(QString::fromUtf8("✓ getInitialState() 返回正确对象"));
        else
            infoLabel->setText(QString::fromUtf8("✗ getInitialState() 返回错误对象"));
    });

    // 初始显示信息
    printBtn->click();

    window.setCentralWidget(central);
    window.setWindowTitle(QStringLiteral("UiInitialState — 交互测试"));
    window.resize(550, 650);
    window.show();

    return app.exec();
}
