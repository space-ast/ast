///
/// @file      GUI.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
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

#include "GUI.hpp"
#include <cstdlib>
#include <string>

AST_NAMESPACE_BEGIN

bool aCanDisplayGUI() {
    
    #ifdef _WIN32
        // Windows通常支持GUI
        return true;
    #elif defined(__linux__) || defined(__APPLE__)
        // Unix-like系统：检查DISPLAY环境变量
        const char* display = std::getenv("DISPLAY");
        const char* wayland = std::getenv("WAYLAND_DISPLAY");
        const char* xdg_session = std::getenv("XDG_SESSION_TYPE");
        
        // 如果有显示环境变量，则认为可以显示GUI
        if ((display != nullptr && display[0] != '\0') ||
            (wayland != nullptr && wayland[0] != '\0')) {
            return true;
        }
        
        // 检查是否在图形会话中
        if (xdg_session != nullptr) {
            std::string session(xdg_session);
            return session == "x11" || session == "wayland";
        }
        
        return false;
    #else
        // 其他平台默认返回false
        return false;
    #endif
}


AST_NAMESPACE_END

