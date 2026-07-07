///
/// @file      AstUiDataUpdateGlobal.hpp
/// @brief     AstUiDataUpdate 模块全局定义
/// @author    axel
/// @date      2026-06-30
/// @copyright 版权所有 (C) 2026-present, ast项目.

#pragma once

#include "AstGlobal.h"

#ifdef AST_BUILD_LIB_UIDATAUPDATE
#  define AST_UIDATAUPDATE_API A_DECL_EXPORT
#else
#  define AST_UIDATAUPDATE_API A_DECL_IMPORT
#endif

AST_NAMESPACE_BEGIN

/*!
    @defgroup UiDataUpdate 数据更新界面
    @brief 提供动态数据文件更新的 Qt 对话框。
*/

/*! @} */

AST_NAMESPACE_END
