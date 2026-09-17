///
/// @file      UiValueEdit.hpp
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
#include <QLineEdit>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup GUI
    @{
*/

/// @brief 数值输入框基类
/// @details 提供错误提示和恢复正常的公共逻辑
class AST_UICORE_API UiValueEdit: public QLineEdit
{
public:
    explicit UiValueEdit(QWidget* parent = nullptr);
    
    /// @brief 设置错误状态
    /// @param errorMessage 错误信息
    void setError(const QString& errorMessage);
    
    /// @brief 恢复正常状态
    void setNormal();
};

/*! @} */

AST_NAMESPACE_END