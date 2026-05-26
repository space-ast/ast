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

#include "AstGUI/UiObjectTree.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstSim/Scenario.hpp"
#include "AstSim/Satellite.hpp"
#include "AstSim/Facility.hpp"
#include "AstSim/Sensor.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
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

    // 主窗口
    QMainWindow window;
    window.setWindowTitle("UiObjectTree Test");
    window.resize(800, 500);

    auto* centralWidget = new QWidget(&window);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    // 左侧：对象树
    auto* treePanel = new QWidget(centralWidget);
    auto* treeLayout = new QVBoxLayout(treePanel);
    treeLayout->setContentsMargins(0, 0, 0, 0);

    auto* treeLabel = new QLabel("对象树 (ObjectManager)", treePanel);
    auto* objectTree = new UiObjectTree(treePanel);
    auto* refreshBtn = new QPushButton("刷新", treePanel);

    treeLayout->addWidget(treeLabel);
    treeLayout->addWidget(objectTree);
    treeLayout->addWidget(refreshBtn);

    // 右侧：选中对象信息
    auto* infoPanel = new QWidget(centralWidget);
    auto* infoLayout = new QVBoxLayout(infoPanel);

    auto* infoTitle = new QLabel("选中对象信息", infoPanel);
    auto* infoLabel = new QLabel("(未选择)", infoPanel);
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoTitle);
    infoLayout->addWidget(infoLabel);
    infoLayout->addStretch(1);

    mainLayout->addWidget(treePanel, 1);
    mainLayout->addWidget(infoPanel, 2);

    window.setCentralWidget(centralWidget);

    // 初始加载
    objectTree->refresh();

    // 连接信号
    QObject::connect(objectTree, &UiObjectTree::objectSelected, infoLabel, [infoLabel](Object* obj) {
        if (obj)
        {
            QString text = QString("名称: %1\n类型: %2\nID: %3")
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
            infoLabel->setText("(未选择)");
        }
    });

    QObject::connect(refreshBtn, &QPushButton::clicked, objectTree, [objectTree]() {
        objectTree->refresh();
        qDebug() << "Tree refreshed.";
    });

    window.show();
    return app.exec();
}
