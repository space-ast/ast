///
/// @file      UiHPOPForceModel.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-22
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

#include "UiHPOPForceModel.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstGUI/UiDragForce.hpp"
#include "AstGUI/UiSolarRadiationPressure.hpp"
#include "AstGUI/UiGravityForce.hpp"
#include "AstGUI/UiThirdBodyForce.hpp"
#include <QVBoxLayout>
#include <QTabWidget>

AST_NAMESPACE_BEGIN

UiHPOPForceModel::UiHPOPForceModel(Object* object, QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiHPOPForceModel");
    setupUi();
    if (object) {
        setObject(object);
        setHPOPForceModel(dynamic_cast<HPOPForceModel*>(object));
    }
}

UiHPOPForceModel::UiHPOPForceModel(QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiHPOPForceModel");
    setupUi();
}

void UiHPOPForceModel::setupUi()
{
    // 主布局
    mainLayout_ = new QVBoxLayout(this);
    
    // 标签页
    tabWidget_ = new QTabWidget(this);
    mainLayout_->addWidget(tabWidget_);
    
    // 引力模型标签页
    gravityTab_ = new QWidget(this);
    gravityTabLayout_ = new QVBoxLayout(gravityTab_);
    tabWidget_->addTab(gravityTab_, "引力模型");
    
    gravityWidget_ = new UiGravityForce(gravityTab_);
    gravityTabLayout_->addWidget(gravityWidget_);
    
    // 三体引力标签页
    thirdBodyTab_ = new QWidget(this);
    thirdBodyTabLayout_ = new QVBoxLayout(thirdBodyTab_);
    tabWidget_->addTab(thirdBodyTab_, "三体引力");
    
    thirdBodyWidget_ = new UiThirdBodyForceList(thirdBodyTab_);
    thirdBodyTabLayout_->addWidget(thirdBodyWidget_);
    
    // 大气阻力标签页
    dragTab_ = new QWidget(this);
    dragTabLayout_ = new QVBoxLayout(dragTab_);
    tabWidget_->addTab(dragTab_, "大气阻力");
    
    dragWidget_ = new UiDragForce(dragTab_);
    dragTabLayout_->addWidget(dragWidget_);
    
    // 太阳光压标签页
    srpTab_ = new QWidget(this);
    srpTabLayout_ = new QVBoxLayout(srpTab_);
    tabWidget_->addTab(srpTab_, "太阳光压");
    
    srpWidget_ = new UiSolarRadiationPressure(srpTab_);
    srpTabLayout_->addWidget(srpWidget_);
}

HPOPForceModel* UiHPOPForceModel::getHPOPForceModel() const
{
    return dynamic_cast<HPOPForceModel*>(getObject());
}

void UiHPOPForceModel::setHPOPForceModel(HPOPForceModel* hpop)
{
    if (hpop) {
        setObject(hpop);
        refreshUi();
    }
}

void UiHPOPForceModel::refreshUi()
{
    auto hpop = getHPOPForceModel();
    if (!hpop) return;
    
    // 刷新大气阻力
    dragWidget_->setDragForce(&hpop->drag());
    
    // 刷新太阳辐射压
    srpWidget_->setSolarRadiationPressure(&hpop->srp());
    
    // 刷新三体引力
    thirdBodyWidget_->setHPOPForceModel(hpop);
    thirdBodyWidget_->refreshUi();
}

void UiHPOPForceModel::apply()
{
    auto hpop = getHPOPForceModel();
    if (hpop) {
        applyTo(hpop);
    }
}

void UiHPOPForceModel::applyTo(HPOPForceModel* hpop)
{
    if (!hpop) return;
    
    // 应用大气阻力
    dragWidget_->applyTo(&hpop->drag());
    
    // 应用太阳辐射压
    srpWidget_->applyTo(&hpop->srp());
    
    // 应用三体引力
    thirdBodyWidget_->apply();
}

AST_NAMESPACE_END