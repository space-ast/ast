///
/// @file      ExtractorInterface.hpp
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 解压接口
/// @details 定义了解压器的基本行为，包括解压压缩文件或目录。
class AST_UTIL_API ExtractorInterface
{
public:
    virtual ~ExtractorInterface() = default;

    /// @brief 解压压缩文件或目录
    /// @param source 压缩文件或目录的路径
    /// @param target 解压目标路径
    /// @return errc_t 错误码
    virtual errc_t extract(StringView source, StringView target) const = 0;

    /// @brief 检查该解压实现是否在当前环境中可用
    /// @details 例如：基于 Shell COM 的实现检查是否是 Windows，
    /// 基于系统命令的实现检查是否安装了对应的命令行工具
    /// @return true 如果可用，false 否则
    virtual bool isSupported() const { return true; }

    /// @brief 检查该解压实现是否能处理指定文件
    /// @details 默认返回 true；子类可重写以检查文件扩展名或魔数
    /// @param source 待解压的文件路径
    /// @return true 如果能处理，false 否则
    virtual bool canExtract(StringView source) const;
};


/*! @} */

AST_NAMESPACE_END
