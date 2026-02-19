///
/// @file      WorkingDirectory.hpp
/// @brief     
/// @details   
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/PosixExt.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Util
    @{
*/


/// @brief 工作目录切换器
/// @details 用于在作用域内临时切换工作目录，退出作用域后自动恢复原目录。
class WorkingDirectory
{
public:
    WorkingDirectory(StringView path)
    {
        oldpath_ = posix::getcwd();
        /// @todo: 需要考虑如何避免创建临时std::string对象 
        posix::chdir(path.to_string().c_str());
    }
    ~WorkingDirectory()
    {
        posix::chdir(oldpath_.c_str());
    }
protected:
    std::string oldpath_;
};


/*! @} */

AST_NAMESPACE_END
