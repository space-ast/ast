///
/// @file      Encode.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      28.11.2025
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include <string>
 
 
AST_NAMESPACE_BEGIN

#ifdef _WIN32   

AST_UTIL_CAPI err_t aAnsiToWide(const char* ansi, std::wstring& wide);
AST_UTIL_CAPI err_t aUtf8ToWide(const char* utf8, std::wstring& wide);
AST_UTIL_CAPI err_t aWideToAnsi(const wchar_t* wide, std::string& ansi);
AST_UTIL_CAPI err_t aWideToUtf8(const wchar_t* wide, std::string& utf8);

#endif


AST_NAMESPACE_END
 



