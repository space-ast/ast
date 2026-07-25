///
/// @file      CompressorInterface.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-24
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
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// 压缩器接口
/// @details 定义了压缩器的基本行为，包括压缩文件或目录。
class AST_UTIL_API CompressorInterface
{
public:
    virtual ~CompressorInterface() = default;

    /// @brief 压缩文件或目录
    /// @param source 要压缩的文件或目录的路径
    /// @param target 压缩目标路径
    /// @param curdir 工作目录，source 在归档中的路径相对于此目录计算
    ///               （空串=从 source 父目录推断；等于 source=归档根为".")
    /// @return errc_t 错误码
    virtual errc_t compress(StringView source, StringView target, StringView curdir = {}) const = 0;

    /// @brief 检查该压缩实现是否在当前环境中可用
    /// @details 例如：基于 Shell COM 的实现检查是否是 Windows，
    /// 基于系统命令的实现检查是否安装了对应的命令行工具
    /// @return true 如果可用，false 否则
    virtual bool isSupported() const { return true; }

    /// @brief 检查该压缩实现是否能处理指定源文件/目录并生成目标格式
    /// @details 默认返回 true；子类可重写以检查目标扩展名或源文件类型
    /// @param source 待压缩的源文件或目录路径
    /// @param target 压缩目标路径（用于判断目标格式）
    /// @return true 如果能处理，false 否则
    virtual bool canCompress(StringView source, StringView target) const;
};


/*! @} */

AST_NAMESPACE_END
