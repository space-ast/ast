///
/// @file      testUiVariableList.cpp
/// @brief     UiVariableList 交互式测试
/// @details   ~
/// @author    axel
/// @date      2026-05-24
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

#include "ast/UiVariableList.hpp"
#include "ast/VariableList.hpp"
#include "ast/Variable.hpp"
#include "ast/RunTime.hpp"
#include "ast/StateCartesian.hpp"
#include "ast/StateKeplerian.hpp"
#include "ast/ObjectManager.hpp"
#include "ast/RTTIAPI.hpp"

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QTextEdit>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    aInitialize();

    // ---- 创建对象并注册到 ObjectManager（供浏览属性/计算量时使用） ----
    auto* satCart = StateCartesian::New();
    satCart->setName("SatCart");
    satCart->setX(7000);
    satCart->setY(0);
    satCart->setZ(0);
    satCart->setVx(0);
    satCart->setVy(7.5);
    satCart->setVz(1.0);
    ObjectManager::CurrentInstance().addObject(satCart);

    auto* satKepl = StateKeplerian::New();
    satKepl->setName("SatKepl");
    satKepl->setSMA(7000);
    satKepl->setEcc(0.01);
    satKepl->setInc(98.0);
    satKepl->setRAAN(120.0);
    satKepl->setArgPeri(45.0);
    satKepl->setTrueAnomaly(0.0);
    ObjectManager::CurrentInstance().addObject(satKepl);

    // ---- 创建 VariableList 并预填充测试数据 ----
    auto variableList = std::make_shared<VariableList>();

    auto* var1 = Variable::New();
    var1->setName("x");
    var1->setExpr(1.0);
    var1->setDesc("initial x position");
    variableList->append(var1);

    auto* var2 = Variable::New();
    var2->setName("y");
    var2->setExpr(std::string("x + 2"));
    var2->setDesc("derived y value");
    variableList->append(var2);

    auto* var3 = Variable::New();
    var3->setName("mu");
    var3->setExpr(398600.4418);
    var3->setDesc("gravitational parameter");
    variableList->append(var3);

    // ---- 主窗口 ----
    QMainWindow window;
    auto* central = new QWidget(&window);
    auto* rootLayout = new QHBoxLayout(central);

    // ---- 左侧: UiVariableList ----
    auto* listLayout = new QVBoxLayout;
    auto* listLabel = new QLabel("<b>Variable List</b>", central);
    listLayout->addWidget(listLabel);

    auto* uiVarList = new UiVariableList(central);
    uiVarList->setVariableList(variableList.get());
    listLayout->addWidget(uiVarList);

    rootLayout->addLayout(listLayout, 1);

    // ---- 右侧: 信息面板 ----
    auto* infoLayout = new QVBoxLayout;

    auto* infoLabel = new QLabel("<b>Selection / Events</b>", central);
    infoLayout->addWidget(infoLabel);

    auto* infoText = new QTextEdit(central);
    infoText->setReadOnly(true);
    infoText->setPlaceholderText("Variable selection and list change events will appear here...");
    infoLayout->addWidget(infoText);

    // ---- 测试按钮 ----
    auto* btnLayout = new QHBoxLayout;

    auto* printBtn = new QPushButton("Print All Variables", central);
    btnLayout->addWidget(printBtn);

    auto* addByCodeBtn = new QPushButton("Add var_n by Code", central);
    btnLayout->addWidget(addByCodeBtn);

    auto* clearBtn = new QPushButton("Clear List", central);
    btnLayout->addWidget(clearBtn);

    infoLayout->addLayout(btnLayout);
    rootLayout->addLayout(infoLayout, 1);

    // ---- 信号连接 ----
    QObject::connect(uiVarList, &UiVariableList::variableSelected, [infoText](Variable* var) {
        if (var)
        {
            QString msg = QStringLiteral("Selected: %1 = %2  [%3]")
                .arg(QString::fromStdString(var->name()))
                .arg(QString::fromStdString(var->getExpression()))
                .arg(QString::fromStdString(var->desc()));
            infoText->append(msg);
        }
        else
        {
            infoText->append("Selection cleared");
        }
    });

    QObject::connect(uiVarList, &UiVariableList::variableListChanged, [infoText]() {
        infoText->append("[List modified]");
    });

    // ---- 测试按钮动作 ----
    QObject::connect(printBtn, &QPushButton::clicked, [&variableList, infoText]() {
        infoText->append("--- Current Variables ---");
        for (size_t i = 0; i < variableList->size(); ++i)
        {
            auto* v = variableList->at(i);
            QString msg = QStringLiteral("  [%1] %2 = %3  (%4)")
                .arg(i)
                .arg(QString::fromStdString(v->name()))
                .arg(QString::fromStdString(v->getExpression()))
                .arg(QString::fromStdString(v->desc()));
            infoText->append(msg);
        }
        infoText->append("---");
    });

    int codeVarCount = 0;
    QObject::connect(addByCodeBtn, &QPushButton::clicked, [&variableList, uiVarList, &codeVarCount]() {
        ++codeVarCount;
        auto* var = Variable::New();
        var->setName("var_" + std::to_string(codeVarCount));
        var->setExpr(codeVarCount * 10.0);
        var->setDesc("code-generated variable #" + std::to_string(codeVarCount));
        variableList->append(var);
        uiVarList->refreshUi();
        qDebug() << "Added variable by code:" << QString::fromStdString(var->name());
    });

    QObject::connect(clearBtn, &QPushButton::clicked, [&variableList, uiVarList, infoText]() {
        variableList->clear();
        uiVarList->refreshUi();
        infoText->append("[List cleared]");
    });

    window.setCentralWidget(central);
    window.setWindowTitle("UiVariableList Test");
    window.resize(800, 420);
    window.show();

    return app.exec();
}
