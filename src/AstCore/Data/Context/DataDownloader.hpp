///
/// @file      DataDownloader.hpp
/// @brief     数据文件夹下载器
/// @details   提供整包下载并安装 data 数据文件夹的能力，供 aInitialize 在缺少合法
///            数据文件夹时调用。
/// @author    axel
/// @date      2026-08-26
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/

/// @brief 下载并安装数据文件夹
/// @details 从 gitcode 或 github 下载 data 仓库的整包压缩包，
///          并解压到动态库所在文件夹下的 data/ 目录。
///          优先尝试 gitcode，下载失败时自动回退到 github。
/// @param dataDir 数据文件夹路径
/// @return eNoError 表示成功，否则返回错误码
AST_CORE_CAPI errc_t aDownloadData(StringView dataDir = StringView());

/*! @} */

AST_NAMESPACE_END
