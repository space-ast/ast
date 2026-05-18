///
/// @file      UiQuantity.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-26
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "UiQuantity.hpp"
#include "AstUtil/QuantityParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/UnitManager.hpp"
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QPolygon>
#include <QPixmap>

AST_NAMESPACE_BEGIN

static QIcon cachedArrowIcon()
{
    static QIcon icon = []() {
        const int size = 12;
        QPixmap pix(size * 2, size * 2);
        pix.setDevicePixelRatio(2);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QApplication::palette().color(QPalette::Text));
        p.setPen(Qt::NoPen);
        QPolygonF tri;
        const double m = size * 0.25;
        tri << QPointF(m,       size * 0.35)
            << QPointF(size / 2.0, size - m)
            << QPointF(size - m, size * 0.35);
        p.drawPolygon(tri);
        p.end();
        return QIcon(pix);
    }();
    return icon;
}

UiQuantity::UiQuantity(QWidget* parent)
    : UiValueEdit(parent)
{
    actionSwitchUnit_ = new QAction(cachedArrowIcon(), QString(), this);
    actionSwitchUnit_->setToolTip(tr("切换单位"));
    addAction(actionSwitchUnit_, QLineEdit::TrailingPosition);
    connect(actionSwitchUnit_, &QAction::triggered, this, &UiQuantity::showUnitMenu);

    connect(this, &QLineEdit::editingFinished, this, &UiQuantity::updateQuantity);
}

void UiQuantity::setQuantity(const Quantity& quantity)
{
    currentQuantity_ = quantity;
    setText(QString::fromUtf8(currentQuantity_.toString().c_str()));
    setNormal();
}

Quantity UiQuantity::getQuantity() const
{
    return currentQuantity_;
}

double UiQuantity::getMagnitude() const
{
    return currentQuantity_.magnitude();
}

void UiQuantity::setMagnitude(double value)
{
    currentQuantity_.setMagnitude(value);
    this->setQuantity(currentQuantity_);
}

Unit UiQuantity::getUnit() const
{
    return currentQuantity_.unit();
}

void UiQuantity::setUnit(const Unit& unit)
{
    currentQuantity_.setUnit(unit);
    this->setQuantity(currentQuantity_);
}

void UiQuantity::changeUnit(const Unit& unit)
{
    currentQuantity_.changeUnit(unit);
    this->setQuantity(currentQuantity_);
}

double UiQuantity::getValueSI() const
{
    return currentQuantity_.getValueSI();
}

void UiQuantity::setValueSI(double value)
{
    currentQuantity_.setValueSI(value);
    this->setQuantity(currentQuantity_);
}

double UiQuantity::getValueInUnit(const Unit& unit) const
{
    return currentQuantity_.getValueInUnit(unit);
}

void UiQuantity::setValueUnit(double value, const Unit& unit)
{
    currentQuantity_.setValueUnit(value, unit);
    this->setQuantity(currentQuantity_);
}

void UiQuantity::setDimension(Dimension dim)
{
    if(this->dimension() == dim){
        return;
    }
    if (Unit* siUnit = aUnitGetSI(dim))
    {
        currentQuantity_.changeUnit(*siUnit);
    }
    else
    {
        aError("dimension %s has no SI unit", dim.symbol().c_str());
    }
}

void UiQuantity::showUnitMenu()
{
    updateQuantity();

    std::vector<Unit> units = aUnitGetByDimension(currentQuantity_.dimension());

    QMenu menu(this);
    if (units.empty())
    {
        QAction* emptyAction = menu.addAction(tr("(无可用单位)"));
        emptyAction->setEnabled(false);
    }
    else
    {
        for (size_t i = 0; i < units.size(); ++i)
        {
            const Unit& u = units[i];
            QString name = QString::fromStdString(u.name());
            if (name.isEmpty())
                name = QStringLiteral("<空>");
            QAction* action = menu.addAction(name);
            action->setData(static_cast<qulonglong>(i));
            if (u.getScale() == currentQuantity_.unit().getScale())
            {
                action->setCheckable(true);
                action->setChecked(true);
            }
        }
    }

    QPoint pos = mapToGlobal(QPoint(width() - menu.sizeHint().width(), height()));
    QAction* selected = menu.exec(pos);

    if (selected && selected->isEnabled() && !units.empty())
    {
        size_t idx = selected->data().toULongLong();
        changeUnit(units[idx]);
    }
}

void UiQuantity::updateQuantity()
{
    QString text = this->text();
    Quantity newQuantity;
    errc_t rc = aQuantityParse(text.toUtf8().data(), newQuantity);
    if(rc){
        aError("failed to parse quantity: %s", text.toUtf8().data());
        setError(tr("数量值格式错误或者单位不支持"));
    }else if(dimensionLocked_ && this->dimension() != newQuantity.dimension()){
        setError(tr("量纲不匹配，期望的量纲为 %1")
            .arg(QString::fromStdString(this->dimension().symbol())));
    }else{
        currentQuantity_ = newQuantity;
        setNormal();
        emit quantityChanged(currentQuantity_);
    }
}


AST_NAMESPACE_END