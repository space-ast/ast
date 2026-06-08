///
/// @file      UiObject.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-28
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
#include "AstUtil/Object.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 与对象关联的 QWidget
class AST_GUI_API UiObject: public QWidget
{
    Q_OBJECT
public:
    explicit UiObject(QWidget *parent = nullptr);
    explicit UiObject(Object* object, QWidget *parent = nullptr);
    ~UiObject() = default;

    /// @brief 获取关联的对象
    /// @return 关联的对象指针
    /// @note 如果对象已被销毁，返回 nullptr
    Object* getObject() const{return object_.get();}

    /// @brief 设置关联的对象
    /// @param object 关联的对象指针
    void setObject(Object* object);
private:
    WeakPtr<Object> object_;
};


/*! @} */

AST_NAMESPACE_END
