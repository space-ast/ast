///
/// @file      PosixExt.hpp
/// @brief     扩展Posix接口
/// @details   本文件包含了一些扩展Posix接口的函数，
///            用于解决一些Posix接口的不足
///            以便使用更加现代化的C++接口
/// @author    axel
/// @date      2026-02-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
#include "Posix.hpp"
#include <string>
#include <limits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Platform
    @{
*/

namespace posix
{
#if defined(PATH_MAX)
constexpr size_t kMaxPath = PATH_MAX;
#elif defined (MAX_PATH)
constexpr size_t kMaxPath = MAX_PATH;
#elif defined(_MAX_PATH)
constexpr size_t kMaxPath = _MAX_PATH;
#elif defined(_POSIX_PATH_MAX)
constexpr size_t kMaxPath = _POSIX_PATH_MAX;
#else
constexpr size_t kMaxPath = 255; // default max path length
#endif

    /// @brief 获取当前工作目录
    /// @return 当前工作目录的路径
    A_ALWAYS_INLINE std::string getcwd()
    {
        char buffer[kMaxPath];
        if (posix::getcwd(buffer, sizeof(buffer)) != nullptr) {
            return buffer;
        }
        return std::string();
    }
#if defined(_WIN32) || defined(AST_PARSED_BY_DOXYGEN)
    /// @brief 检查文件状态是否为目录
    /// @param st 文件状态结构体
    /// @return 如果是目录则返回true，否则返回false
    A_ALWAYS_INLINE
    bool isdir(const stat& st)
    {
        return (_S_IFDIR & st.st_mode) != 0;
    }
#else
    A_ALWAYS_INLINE
    bool isdir(const struct stat& st)
    {
        return S_ISDIR(st.st_mode);
    }
#endif
}

/*! @} */

AST_NAMESPACE_END
