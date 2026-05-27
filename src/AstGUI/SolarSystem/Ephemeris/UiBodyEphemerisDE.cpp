///
/// @file      UiBodyEphemerisDE.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-04
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

#include "AstGUI/UiBodyEphemerisDE.hpp"
#include "AstCore/BodyEphemerisDE.hpp"
#include "AstGUI/UiInteger.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

AST_NAMESPACE_BEGIN

UiBodyEphemerisDE::UiBodyEphemerisDE(Object *object, QWidget *parent)
    : UiBodyEphemerisDE(parent)
{
    setBodyEphemerisDE(dynamic_cast<BodyEphemerisDE*>(object));
}

UiBodyEphemerisDE::UiBodyEphemerisDE(QWidget *parent) : UiObject(parent)
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // JPL索引
    QHBoxLayout* jplIndexLayout = new QHBoxLayout();
    QLabel* jplIndexLabel = new QLabel(tr("JPL索引"), this);
    jplIndexEdit_ = new UiInteger(this);
    jplIndexEdit_->setValue(0);
    jplIndexLayout->addWidget(jplIndexLabel);
    jplIndexLayout->addWidget(jplIndexEdit_);
    mainLayout->addLayout(jplIndexLayout);
    
    setLayout(mainLayout);

    connect(jplIndexEdit_, &UiInteger::valueChanged, this, &UiBodyEphemerisDE::apply);
}

void UiBodyEphemerisDE::refreshUi()
{
    if(auto ephemeris = getBodyEphemerisDE()){
        // 设置JPL索引值
        jplIndexEdit_->setValue(ephemeris->getJplIndex());
    }
}

void UiBodyEphemerisDE::apply()
{
    if (auto ephemeris = getBodyEphemerisDE())
    {
        applyTo(ephemeris);
        emit bodyEphemerisDEChanged(ephemeris);
    }
}

void UiBodyEphemerisDE::applyTo(BodyEphemerisDE *ephemeris)
{
    // 从界面上获取JPL索引值
    ephemeris->setJplIndex(jplIndexEdit_->value());
}

void UiBodyEphemerisDE::setBodyEphemerisDE(BodyEphemerisDE* ephemeris)
{
    if (ephemeris)
    {
        setObject(ephemeris);
        refreshUi();
    }
}

BodyEphemerisDE* UiBodyEphemerisDE::getBodyEphemerisDE() const
{
    return dynamic_cast<BodyEphemerisDE*>(getObject());
}

AST_NAMESPACE_END