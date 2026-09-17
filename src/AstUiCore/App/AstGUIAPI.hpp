///
/// @file      GuiAPI.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-18
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

class QMainWindow;

AST_NAMESPACE_BEGIN


/// @brief 初始化Gui
/// @return errc_t
AST_UICORE_CAPI errc_t aGUIInit();


/// @brief 初始化Qt应用程序
/// @details 缺少 Qt 平台插件（如 Windows 下的 qwindows.dll）时会降级为 QCoreApplication，
///          此时日志与翻译功能仍可正常使用，但所有 GUI 功能不可用。
/// @return errc_t
AST_UICORE_CAPI errc_t aQAppInit(int argc, char *argv[]);

/// @brief 判断当前是否具备可用的 GUI 环境
/// @details 仅当已成功创建 QApplication（即平台插件可用）时返回 true。
///          返回 false 时不应创建任何 QWidget 派生对象。
/// @return 具备可用 GUI 环境返回 true
AST_UICORE_CAPI bool aGuiAvailable();

/// @brief 创建主窗口
/// @return QMainWindow*，GUI 环境不可用时返回 nullptr
AST_UICORE_CAPI QMainWindow* aUiNewMainWindow();


AST_NAMESPACE_END
