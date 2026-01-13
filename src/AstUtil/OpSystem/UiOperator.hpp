///
/// @file      UiOperator.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

AST_NAMESPACE_BEGIN

class Object;
class Class;
class UiWidget;



typedef UiWidget* (*UiWidgetFactory)(Object* object);


/// @brief     注册编辑对象的UI组件工厂
/// @details   ~
/// @param     type    对象类型
/// @param     factory UI组件工厂函数指针
/// @return    err_t  错误码
AST_UTIL_CAPI err_t aUiRegEditWidget(Class* type, UiWidgetFactory factory);


/// @brief     编辑对象
/// @details   ~
/// @param     object 要编辑的对象
/// @return    err_t  错误码
AST_UTIL_CAPI err_t aUiEditObject(Object *object);


/// @brief     创建编辑对象的UI组件
/// @details   ~
/// @param     object 要编辑的对象
/// @return    UiWidget*  编辑对象的UI组件指针
AST_UTIL_CAPI UiWidget* aUiNewEditWidget(Object* object);



/// @brief     选择对象
/// @details   ~
/// @param     typeName 对象类型名
/// @return    Object*  选择的对象指针
AST_UTIL_CAPI Object* aUiSelectObject(StringView typeName);



AST_NAMESPACE_END
