///
/// @file      ObjectEditRegistry.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
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
#include <unordered_map>

class QWidget;

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class Object;
class Class;

typedef QWidget* (*FNewEditWidget)(Object *object);

class AST_UICORE_API ObjectEditRegistry
{
    using EditWidgetMap = std::unordered_map<Class*, FNewEditWidget>;
public:
    ObjectEditRegistry();
    explicit ObjectEditRegistry(bool shouldRegistEditWidget);
    ~ObjectEditRegistry() = default;

    static ObjectEditRegistry& Instance();

    /// @brief     注册编辑控件
    /// @details   ~
    /// @param     cls     类指针
    /// @param     newEditWidget 控件创建函数指针
    void registerEditWidget(Class *cls, FNewEditWidget newEditWidget);

    /// @brief     创建编辑控件
    /// @details   ~
    /// @param     object 要编辑的对象
    /// @return    QWidget*  编辑控件指针
    QWidget* newEditWidget(Object *object);
protected:
    EditWidgetMap editWidgetMap_{};
};


/// @brief     注册编辑控件
/// @details   ~
/// @param     cls     类指针
/// @param     newEditWidget 控件创建函数指针
AST_UICORE_CAPI void aUiRegisterEditWidget(Class *cls, FNewEditWidget newEditWidget);


/// @brief     创建编辑控件
/// @details   ~
/// @param     object 要编辑的对象
/// @return    QWidget*  编辑控件指针
AST_UICORE_CAPI QWidget* aUiNewEditWidget(Object *object);


/*! @} */

AST_NAMESPACE_END
