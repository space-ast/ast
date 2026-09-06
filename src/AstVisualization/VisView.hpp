///
/// @file      VisView.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-06
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "VisGroup.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 可视化视图：场景的顶级容器
/// @details 持有所有可视化对象的分组，高层 API（trajectory/groundTrack/planet）
///          通过虚函数或 friend 机制把对象挂进 objects_。
class AST_VISUALIZATION_API VisView: public ObjectNamed
{
public:
    VisView();

    /// @brief 场景对象分组
    VisGroup* objects() const { return objects_.get(); }

private:
    SharedPtr<VisGroup> objects_;
};


/*! @} */

AST_NAMESPACE_END
