/// @file      SnoptAPI.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      27.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。


#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/Logger.hpp"
#include "SnoptAPI.hpp"

AST_NAMESPACE_BEGIN


static SnoptCAPI s_snoptAPI{};

SnoptCAPI* aLoadSnoptCAPI(const char* sharedLibPath)
{
    auto sharedLib = aLoadLibrary(sharedLibPath);
    if (sharedLib) {
        s_snoptAPI.snopta = decltype(&snopta_)(aGetProcAddress(sharedLib, A_STR(snopta_)));
        s_snoptAPI.snmema = decltype(&snmema_)(aGetProcAddress(sharedLib, A_STR(snmema_)));
        s_snoptAPI.snjac = decltype(&snjac_)(aGetProcAddress(sharedLib, A_STR(snjac_)));
        for (auto func : s_snoptAPI) {
            if(func == nullptr)
                return nullptr;
        }
        return &s_snoptAPI;
    }
    return nullptr;
}


AST_NAMESPACE_END
 




