///
/// @file      UiDouble.hpp
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

#pragma once

#include "AstGlobal.h"
#include "UiValueEdit.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup GUI
    @{
*/

/// @brief 双精度浮点数输入框
/// @details 用于输入双精度浮点数值
class AST_UICORE_API UiDouble: public UiValueEdit
{
    Q_OBJECT

public:
    UiDouble(QWidget* parent = nullptr);
    
    /// @brief 设置双精度浮点数值
    /// @param value 双精度浮点数值
    void setValue(double value);
    
    /// @brief 获取双精度浮点数值
    /// @return 双精度浮点数值
    double value() const;

signals:
    void valueChanged(double value);
private slots:
    void updateValue();
private:
    double currentValue_{0.0};
};

/*! @} */

AST_NAMESPACE_END