///
/// @file      testUiObjectTree.cpp
/// @brief     测试 UiObjectTree 通用对象树控件
/// @details   创建一组对象并注册到 ObjectManager，验证树形显示和选择功能
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

#include "ast/UiObjectTree.hpp"
#include "ast/UiObjectTreeItem.hpp"
#include "ast/StateKeplerian.hpp"
#include "ast/StateCartesian.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/RunTime.hpp"
#include "ast/SpacecraftState.hpp"
#include "ast/Scenario.hpp"
#include "ast/Satellite.hpp"
#include "ast/Facility.hpp"
#include "ast/Sensor.hpp"
#include "ast/ObjectManager.hpp"
#include "ast/Propagate.hpp"
#include "ast/Maneuver.hpp"
#include "ast/Sequence.hpp"
#include "ast/TargeterSequence.hpp"
#include "ast/InitialState.hpp"
#include "ast/RTTIAPI.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QDebug>

AST_USING_NAMESPACE

int main(int argc, char* argv[])
{
    aInitialize();
    printf("testUiObjectTree.cpp\n");
    QApplication app(argc, argv);

    // 创建原有通用对象树并注册到 ObjectManager
    auto* rootObj = StateCartesian::New();
    rootObj->setName("RootCartesian");
    rootObj->setX(1000);
    rootObj->setY(2000);
    rootObj->setZ(3000);
    rootObj->setVx(10);
    rootObj->setVy(20);
    rootObj->setVz(30);
    ObjectManager::CurrentInstance().addObject(rootObj);

    auto* child1 = StateKeplerian::New();
    child1->setName("ChildKeplerian-A");
    child1->setSMA(7000);
    child1->setEcc(0.1);
    ObjectManager::CurrentInstance().addObject(child1);
    ObjectManager::CurrentInstance().setParentScope(child1, rootObj);

    auto* child2 = StateKeplerian::New();
    child2->setName("ChildKeplerian-B");
    child2->setSMA(42000);
    child2->setEcc(0.01);
    ObjectManager::CurrentInstance().addObject(child2);
    ObjectManager::CurrentInstance().setParentScope(child2, rootObj);

    auto* grandchild = StateCartesian::New();
    grandchild->setName("GrandchildCartesian");
    grandchild->setX(4000);
    grandchild->setY(5000);
    grandchild->setZ(6000);
    grandchild->setVx(100);
    grandchild->setVy(200);
    grandchild->setVz(300);
    ObjectManager::CurrentInstance().addObject(grandchild);
    ObjectManager::CurrentInstance().setParentScope(grandchild, child1);

    // 创建场景对象树并注册到 ObjectManager
    auto* scenario = new Scenario();
    scenario->setName("Scenario3");
    ObjectManager::CurrentInstance().addObject(scenario);

    auto* geo90w = Satellite::New();
    geo90w->setName("GEO_90W");
    ObjectManager::CurrentInstance().addObject(geo90w);
    ObjectManager::CurrentInstance().setParentScope(geo90w, scenario);

    auto* chase = Satellite::New();
    chase->setName("Chase");
    ObjectManager::CurrentInstance().addObject(chase);
    ObjectManager::CurrentInstance().setParentScope(chase, scenario);

    auto* geo105w = Satellite::New();
    geo105w->setName("GEO_105W");
    ObjectManager::CurrentInstance().addObject(geo105w);
    ObjectManager::CurrentInstance().setParentScope(geo105w, scenario);

    auto* target = Satellite::New();
    target->setName("Target");
    ObjectManager::CurrentInstance().addObject(target);
    ObjectManager::CurrentInstance().setParentScope(target, scenario);

    auto* sunSync = Satellite::New();
    sunSync->setName("SunSync_400km");
    ObjectManager::CurrentInstance().addObject(sunSync);
    ObjectManager::CurrentInstance().setParentScope(sunSync, scenario);

    auto* facility = new Facility();
    facility->setName("VBar");
    facility->setLatitude(39.9);
    facility->setLongitude(116.4);
    facility->setAltitude(0.05);
    ObjectManager::CurrentInstance().addObject(facility);
    ObjectManager::CurrentInstance().setParentScope(facility, scenario);

    auto* sensor = new Sensor();
    sensor->setName("EOIR_Sensor");
    ObjectManager::CurrentInstance().addObject(sensor);
    ObjectManager::CurrentInstance().setParentScope(sensor, geo90w);

    auto* groundSensor = new Sensor();
    groundSensor->setName("VBar_Antenna");
    ObjectManager::CurrentInstance().addObject(groundSensor);
    ObjectManager::CurrentInstance().setParentScope(groundSensor, facility);

    auto* access = new Facility();
    access->setName("Access");
    ObjectManager::CurrentInstance().addObject(access);
    ObjectManager::CurrentInstance().setParentScope(access, scenario);

    auto* deckAccess = new Facility();
    deckAccess->setName("DeckAccess");
    ObjectManager::CurrentInstance().addObject(deckAccess);
    ObjectManager::CurrentInstance().setParentScope(deckAccess, scenario);

    auto* satState = StateKeplerian::New();
    satState->setName("OrbitState");
    satState->setSMA(42164);
    satState->setEcc(0.0001);
    ObjectManager::CurrentInstance().addObject(satState);
    ObjectManager::CurrentInstance().setParentScope(satState, geo90w);

    // 创建嵌套 Mission 对象树（通过 Sequence::addCommand 构建父子关系）
    auto* mission = new Sequence();
    mission->setName(u8"TLI_Mission");
    ObjectManager::CurrentInstance().addObject(mission);

    auto* initialState = new InitialState();
    initialState->setName(u8"LEO_Initial");
    mission->addCommand(initialState);

    auto* tliTargeter = new TargeterSequence();
    tliTargeter->setName(u8"TLI_Targeter");
    mission->addCommand(tliTargeter);

    auto* tliBurn = new Maneuver();
    tliBurn->setName(u8"TLI_Burn");
    tliTargeter->addCommand(tliBurn);

    auto* coastToMoon = new Propagate();
    coastToMoon->setName(u8"Coast_To_Moon");
    tliTargeter->addCommand(coastToMoon);

    auto* loiBurn = new Maneuver();
    loiBurn->setName(u8"LOI_Burn");
    tliTargeter->addCommand(loiBurn);

    auto* lunarPhase = new Sequence();
    lunarPhase->setName(u8"LunarPhase");
    mission->addCommand(lunarPhase);

    auto* lunarOrbit = new Propagate();
    lunarOrbit->setName(u8"Lunar_Orbit");
    lunarPhase->addCommand(lunarOrbit);

    auto* returnBurn = new Maneuver();
    returnBurn->setName(u8"Return_Burn");
    lunarPhase->addCommand(returnBurn);

    // 主窗口
    QMainWindow window;
    window.setWindowTitle("UiObjectTree Test");
    window.resize(1000, 700);

    auto* centralWidget = new QWidget(&window);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    // 左侧：全部对象树 + 子树面板
    auto* treePanel = new QWidget(centralWidget);
    auto* treeLayout = new QVBoxLayout(treePanel);
    treeLayout->setContentsMargins(0, 0, 0, 0);

    auto* fullTreeLabel = new QLabel("全部对象树", treePanel);
    auto* objectTree = new UiObjectTree(treePanel);
    auto* refreshBtn = new QPushButton("刷新全部", treePanel);
    treeLayout->addWidget(fullTreeLabel);
    treeLayout->addWidget(objectTree);
    treeLayout->addWidget(refreshBtn);

    auto* separator = new QFrame(treePanel);
    separator->setFrameShape(QFrame::HLine);
    treeLayout->addWidget(separator);

    auto* subTreeLabel = new QLabel(u8"子树（以 Scenario3 为根）", treePanel);
    auto* subtreeTree = new UiObjectTree(treePanel);
    subtreeTree->setRootItem(new UiObjectTreeItem(scenario));
    subtreeTree->setRootVisible(true);
    auto* subtreeRefreshBtn = new QPushButton(u8"刷新子树", treePanel);
    treeLayout->addWidget(subTreeLabel);
    treeLayout->addWidget(subtreeTree);

    auto* ctrlRow1 = new QHBoxLayout();
    auto* rootScenarioBtn = new QPushButton(u8"根=Scenario3", treePanel);
    auto* rootGeoBtn = new QPushButton(u8"根=GEO_90W", treePanel);
    auto* rootChildBtn = new QPushButton(u8"根=ChildKeplerian-A", treePanel);
    ctrlRow1->addWidget(rootScenarioBtn);
    ctrlRow1->addWidget(rootGeoBtn);
    ctrlRow1->addWidget(rootChildBtn);
    treeLayout->addLayout(ctrlRow1);

    auto* ctrlRow2 = new QHBoxLayout();
    auto* rootNoneBtn = new QPushButton(u8"显示全部", treePanel);
    auto* rootVisibleCheck = new QCheckBox(u8"显示根节点", treePanel);
    rootVisibleCheck->setChecked(true);
    ctrlRow2->addWidget(rootNoneBtn);
    ctrlRow2->addWidget(rootVisibleCheck);
    ctrlRow2->addStretch();
    treeLayout->addLayout(ctrlRow2);

    auto* ctrlRow3 = new QHBoxLayout();
    auto* rootMissionBtn = new QPushButton(u8"根=TLI_Mission", treePanel);
    auto* rootTargeterBtn = new QPushButton(u8"根=TLI_Targeter", treePanel);
    auto* rootLunarPhaseBtn = new QPushButton(u8"根=LunarPhase", treePanel);
    ctrlRow3->addWidget(rootMissionBtn);
    ctrlRow3->addWidget(rootTargeterBtn);
    ctrlRow3->addWidget(rootLunarPhaseBtn);
    treeLayout->addLayout(ctrlRow3);
    treeLayout->addWidget(subtreeRefreshBtn);

    // 右侧：选中对象信息
    auto* infoPanel = new QWidget(centralWidget);
    auto* infoLayout = new QVBoxLayout(infoPanel);

    auto* infoTitle = new QLabel(u8"选中对象信息", infoPanel);
    auto* infoLabel = new QLabel(u8"(未选择)", infoPanel);
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoTitle);
    infoLayout->addWidget(infoLabel);
    infoLayout->addStretch(1);

    mainLayout->addWidget(treePanel, 2);
    mainLayout->addWidget(infoPanel, 3);

    window.setCentralWidget(centralWidget);

    objectTree->refresh();
    subtreeTree->refresh();

    auto showObjectInfo = [infoLabel](Object* obj) {
        if (obj)
        {
            QString text = QString(u8"名称: %1\n类型: %2\nID: %3")
                .arg(QString::fromStdString(obj->getName()))
                .arg(QString::fromStdString(obj->typeName()))
                .arg(obj->getID());
            infoLabel->setText(text);
            qDebug() << "Selected:" << obj->getName().c_str()
                     << "Type:" << obj->typeName().c_str()
                     << "ID:" << obj->getID();
        }
        else
        {
            infoLabel->setText(u8"(未选择)");
        }
    };

    QObject::connect(objectTree, &UiObjectTree::objectSelected, infoLabel, showObjectInfo);
    QObject::connect(subtreeTree, &UiObjectTree::objectSelected, infoLabel, showObjectInfo);

    QObject::connect(refreshBtn, &QPushButton::clicked, objectTree, [objectTree]() {
        objectTree->refresh();
        qDebug() << "Full tree refreshed.";
    });

    QObject::connect(subtreeRefreshBtn, &QPushButton::clicked, subtreeTree, [subtreeTree]() {
        subtreeTree->refresh();
        qDebug() << "Subtree refreshed.";
    });

    QObject::connect(rootScenarioBtn, &QPushButton::clicked, subtreeTree, [subtreeTree, subTreeLabel, scenario]() {
        subtreeTree->setRootItem(new UiObjectTreeItem(scenario));
        subTreeLabel->setText(u8"子树（以 Scenario3 为根）");
        subtreeTree->refresh();
    });

    QObject::connect(rootGeoBtn, &QPushButton::clicked, subtreeTree, [subtreeTree, subTreeLabel, geo90w]() {
        subtreeTree->setRootItem(new UiObjectTreeItem(geo90w));
        subTreeLabel->setText(u8"子树（以 GEO_90W 为根）");
        subtreeTree->refresh();
    });

    QObject::connect(rootChildBtn, &QPushButton::clicked, subtreeTree, [subtreeTree, subTreeLabel, child1]() {
        subtreeTree->setRootItem(new UiObjectTreeItem(child1));
        subTreeLabel->setText(u8"子树（以 ChildKeplerian-A 为根）");
        subtreeTree->refresh();
    });

    QObject::connect(rootNoneBtn, &QPushButton::clicked, subtreeTree, [subtreeTree, subTreeLabel]() {
        subtreeTree->setRootItem(nullptr);
        subTreeLabel->setText(u8"子树（显示全部）");
        subtreeTree->refresh();
    });

    QObject::connect(rootVisibleCheck, &QCheckBox::toggled, subtreeTree, [subtreeTree](bool checked) {
        subtreeTree->setRootVisible(checked);
        subtreeTree->refresh();
        qDebug() << "Root visible:" << checked;
    });

    QObject::connect(rootMissionBtn, &QPushButton::clicked, subtreeTree, [subtreeTree, subTreeLabel, mission]() {
        subtreeTree->setRootItem(new UiObjectTreeItem(mission));
        subTreeLabel->setText(u8"子树（以 TLI_Mission 为根）");
        subtreeTree->refresh();
    });

    QObject::connect(rootTargeterBtn, &QPushButton::clicked, subtreeTree, [subtreeTree, subTreeLabel, tliTargeter]() {
        subtreeTree->setRootItem(new UiObjectTreeItem(tliTargeter));
        subTreeLabel->setText(u8"子树（以 TLI_Targeter 为根）");
        subtreeTree->refresh();
    });

    QObject::connect(rootLunarPhaseBtn, &QPushButton::clicked, subtreeTree, [subtreeTree, subTreeLabel, lunarPhase]() {
        subtreeTree->setRootItem(new UiObjectTreeItem(lunarPhase));
        subTreeLabel->setText(u8"子树（以 LunarPhase 为根）");
        subtreeTree->refresh();
    });

    window.show();
    return app.exec();
}
