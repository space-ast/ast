///
/// @file      UiQuantity.hpp
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

#pragma once

#include "AstGlobal.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMetaType>
#include "UiValueEdit.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/Unit.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup GUI
    @{
*/

/// @brief 数量输入框
/// @details 用于输入数量值，支持数值和单位选择。
class AST_GUI_API UiQuantity: public UiValueEdit
{
    Q_OBJECT

public:
    explicit UiQuantity(QWidget* parent = nullptr);
    
    /// @brief 设置数量值
    /// @param quantity 数量值
    void setQuantity(const Quantity& quantity);
    
    /// @brief 获取数量值
    /// @return 数量值
    Quantity getQuantity() const;

    /// @brief 获取数值大小
    /// @return 数值大小
    double getMagnitude() const;

    /// @brief 设置数值大小
    /// @param value 数值大小
    void setMagnitude(double value);

    /// @brief 获取当前单位
    /// @return 当前单位
    Unit getUnit() const;

    /// @brief 设置当前单位
    /// @details 不会改变当前数量值的数值大小，仅改变单位表示
    /// 例如将1000m转换为1000km，将1000kg转换为1000t等
    /// @param unit 当前单位
    void setUnit(const Unit& unit);

    /// @brief 切换当前单位
    /// @details 切换当前单位后，数量值会自动转换为该单位
    /// 例如将1000m转换为1km，将1000kg转换为1t等
    /// @param unit 当前单位
    void changeUnit(const Unit& unit);

    /// @brief 获取国际单位表示的值
    /// @return 国际单位表示的值
    double getValueSI() const;
    
    /// @brief 获取国际单位表示的值
    /// @return 国际单位表示的值
    double value() const{return getValueSI();}

    /// @brief 设置国际单位下的值
    /// @details 只会改变数量值的数值大小，不会改变数量值的单位
    /// @param value 国际单位下的值
    void setValueSI(double value);

    /// @brief 设置国际单位下的值
    /// @param value 国际单位下的值
    void setValue(double valueInSI){setValueSI(valueInSI);}

    /// @brief 获取指定单位下的值
    /// @param unit 指定单位
    /// @return 指定单位下的值
    double getValueInUnit(const Unit& unit) const;    

    /// @brief 设置指定单位下的值
    /// @details 只会改变数量值的数值大小，不会改变数量值的单位
    /// @param value 指定单位下的值
    /// @param unit 指定单位
    void setValueInUnit(double value, const Unit& unit);

    /// @brief 设置数值大小和单位
    /// @param value 数值大小
    /// @param unit 单位
    void setValueUnit(double value, const Unit& unit);
signals:
    void quantityChanged(const Quantity& quantity);
private:
    void updateQuantity();
private:
    Quantity currentQuantity_;
};

/*! @} */

AST_NAMESPACE_END