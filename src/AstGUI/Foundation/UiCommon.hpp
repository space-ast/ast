///
/// @file      UiCommon.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-04
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
#include <QString>
#include <QIcon>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 获取对象的显示名称
AST_GUI_API QString aUiObjectDisplayName(Object* obj);

AST_GUI_API QIcon aUiIcon(const QString& name);

AST_GUI_API QIcon aUiObjectIcon(const Object* obj);

AST_GUI_API QIcon aUiClassIcon(const QString& className);

A_ALWAYS_INLINE QIcon objectIcon(const Object* obj)
{
    return aUiObjectIcon(obj);
}


/*! @} */

AST_NAMESPACE_END

