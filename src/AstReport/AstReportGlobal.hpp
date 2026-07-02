///
/// @file      AstReportGlobal.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-02
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


// ---- 枚举定义 ----

/// @brief 报表样式类型
enum class EStyleType : int
{
    eTabular = 0,   ///< 表格报告（.rst 文件）
    eGraph2D = 1,   ///< 2D 图形（.rsg 文件）
    eFixed   = 2,   ///< 固定格式报告
    eGraph3D = 3    ///< 3D 图形
};

/// @brief 数据元素的数据类型
enum class EDataType : int
{
    eDouble    = 0,
    eFloat     = eDouble,
    eInt       = 1,
    eString    = 2,
    eTimePoint = 3,
    eDateTime  = eTimePoint
};

/// @brief 图形线型
enum class ELineStyle : int
{
    eSolid   = 0,   ///< 实线
    eDash    = 1,   ///< 虚线
    eDot     = 2,   ///< 点线
    eDashDot = 3    ///< 点划线
};

/// @brief 图形点标记样式
enum class EPointStyle : int
{
    eNone     = 0,
    eSquare   = 1,
    eCircle   = 2,
    eDiamond  = 3,
    eTriangle = 4,
    eCross    = 5
};

/// @brief 图形填充图案
enum class EFillPattern : int
{
    eNone  = 0,
    eSolid = 1
};

/// @brief Section 展开方式
enum class EExpandMethod : int
{
    eNone     = 0,
    eAll      = 1,
    eSelected = 2
};


/*! @} */

AST_NAMESPACE_END
