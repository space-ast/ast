///
/// @file      testUiEventDetector.cpp
/// @brief     交互式测试 UiEventDetector — 事件检测器属性编辑
/// @author    axel
/// @date      2026-05-18
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

#include "ast/UiEventDetectorEditor.hpp"
#include "ast/UiEventDetector.hpp"
#include "ast/EventDetector.hpp"
#include "ast/DetectorDuration.hpp"
#include "ast/DetectorPeriapsis.hpp"
#include "ast/DetectorUserSelect.hpp"
#include "ast/RTTIAPI.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>

using namespace ast;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // ---- 构造测试数据 ----
    auto* det1 = aNewObject<DetectorDuration>();
    det1->setName("DurationDet");
    det1->setDuration(3600.0);

    auto* det2 = aNewObject<DetectorPeriapsis>();
    det2->setName("PeriapsisDet");

    auto* det3 = aNewObject<DetectorUserSelect>();
    det3->setName("UserSelectDet");

    // ---- 主窗口 ----
    QMainWindow window;
    auto* central = new QWidget(&window);
    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(9, 9, 9, 9);
    rootLayout->setSpacing(6);

    // ---- 检测器选择 ----
    auto* topBar = new QHBoxLayout();
    topBar->addWidget(new QLabel(QStringLiteral("选择检测器:"), central));
    auto* selector = new QComboBox(central);
    selector->addItem(QStringLiteral("DetectorDuration"), 0);
    selector->addItem(QStringLiteral("DetectorPeriapsis"), 1);
    selector->addItem(QStringLiteral("DetectorUserSelect"), 2);
    topBar->addWidget(selector);
    topBar->addStretch();
    rootLayout->addLayout(topBar);

    // ---- UiEventDetectorEditor ----
    auto* editor = new UiEventDetectorEditor(central);
    rootLayout->addWidget(editor, 1);

    // ---- 信息标签 ----
    auto* infoLabel = new QLabel(central);
    infoLabel->setWordWrap(true);
    infoLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    infoLabel->setMinimumHeight(60);
    infoLabel->setMaximumHeight(120);
    rootLayout->addWidget(infoLabel);

    // ---- 操作按钮 ----
    auto* btnRow = new QHBoxLayout();
    auto* printBtn = new QPushButton(QStringLiteral("打印属性"), central);
    auto* verifyBtn = new QPushButton(QStringLiteral("验证 getDetector"), central);
    btnRow->addWidget(printBtn);
    btnRow->addWidget(verifyBtn);
    btnRow->addStretch();
    rootLayout->addLayout(btnRow);

    // ---- 当前选中检测器 ----
    EventDetector* currentDet = det1;

    QObject::connect(selector, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [&](int idx) {
            switch (idx) {
            case 0: currentDet = det1; break;
            case 1: currentDet = det2; break;
            case 2: currentDet = det3; break;
            }
            editor->setDetector(currentDet);
        });

    QObject::connect(printBtn, &QPushButton::clicked, [&]() {
        auto* det = editor->getDetector();
        if (!det) {
            infoLabel->setText(QStringLiteral("detector 为空"));
            return;
        }
        QString info;
        info += QStringLiteral("类型: %1\n").arg(QString::fromStdString(det->getType()->name()));
        info += QStringLiteral("激活: %1  重复: %2  阈值: %3  目标值: %4\n")
            .arg(det->active() ? "是" : "否")
            .arg(det->repeatCount())
            .arg(det->threshold())
            .arg(det->goal());
        if (auto* dur = aobject_cast<DetectorDuration*>(det))
            info += QStringLiteral("时长: %1 s\n").arg(dur->duration());
        infoLabel->setText(info);
        qDebug() << info;
    });

    QObject::connect(verifyBtn, &QPushButton::clicked, [&]() {
        auto* got = editor->getDetector();
        infoLabel->setText(got == currentDet
            ? QString::fromUtf8("✓ getDetector() 返回正确对象")
            : QString::fromUtf8("✗ getDetector() 返回错误对象"));
    });

    editor->setDetector(det1);
    printBtn->click();

    window.setCentralWidget(central);
    window.setWindowTitle(QStringLiteral("UiEventDetector — 交互测试"));
    window.resize(500, 500);
    window.show();

    return app.exec();
}
