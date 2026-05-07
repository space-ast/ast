///
/// @file      Resolve.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-17
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 获取对象的坐标系
/// @param obj 对象指针
/// @param frameName 坐标系名称
/// @return 坐标系指针
/// @note 如果坐标系不存在，返回 nullptr
AST_CORE_API Frame* aObject_GetFrame(Object* obj, StringView frameName);

#if 0 // 移动到 AstUtil 工程

/// @brief 解析对象
/// @param value 对象路径/名称/表达式
/// @param type 对象类型，默认 nullptr 表示不指定类型
/// @return 对象指针
/// @note 如果对象不存在，返回 nullptr
AST_CORE_API Object* aResolveObject(StringView value, Class* type=nullptr);

#endif

/// @brief 解析天体
/// @param name 天体名称
/// @return 天体指针
/// @note 如果天体不存在，返回 nullptr
AST_CORE_API Body* aResolveBody(StringView name);


/// @brief 解析坐标系
/// @param name 坐标系名称
/// @return 坐标系指针
/// @note 如果坐标系不存在，返回 nullptr
AST_CORE_API Frame* aResolveFrame(StringView name);


/// @brief 解析坐标轴
/// @param name 坐标轴名称
/// @return 坐标轴指针
/// @note 如果坐标轴不存在，返回 nullptr
AST_CORE_API Axes* aResolveAxes(StringView name);


/// @brief 解析点
/// @param name 点名称
/// @return 点指针
/// @note 如果点不存在，返回 nullptr
AST_CORE_API Point* aResolvePoint(StringView name);


/*! @} */

AST_NAMESPACE_END
