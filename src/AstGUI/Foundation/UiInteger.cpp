///
/// @file      UiInteger.cpp
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

#include "UiInteger.hpp"

AST_NAMESPACE_BEGIN

UiInteger::UiInteger(QWidget* parent)
    : UiValueEdit(parent)
{
    setText(QString::number(currentValue_));
    connect(this, &QLineEdit::editingFinished, this, &UiInteger::updateValue);
}

void UiInteger::setValue(int value)
{
    currentValue_ = value;
    setText(QString::number(value));
    setNormal();
}

int UiInteger::value() const
{
    return currentValue_;
}

void UiInteger::updateValue()
{
    QString text = this->text();
    bool ok;
    int value = text.toInt(&ok);
    if (ok)
    {
        if (value != currentValue_)
        {
            currentValue_ = value;
            emit valueChanged(value);
        }
        // 解析成功，恢复正常状态
        setNormal();
    }
    else
    {
        // 显示错误提示
        setError(tr("请输入正确的整数值"));
    }
}

AST_NAMESPACE_END