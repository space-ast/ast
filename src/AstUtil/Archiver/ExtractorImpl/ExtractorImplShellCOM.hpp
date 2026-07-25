///
/// @file      ExtractorImplShellCOM.hpp
/// @brief     Windows Shell COM 解压后端
/// @details   通过 IShellDispatch 直接 COM 接口解压 ZIP 文件。
///           使用 Folder::Items + FolderItems 枚举 + CopyHere + ParseName 轮询。
/// @author    axel
/// @date      2026-07-25
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

#include "AstUtil/ExtractorInterface.hpp"
#include "AstUtil/StringView.hpp"

// 仅 Windows 平台
#if defined(_WIN32) && !defined(SWIG)


#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/// Windows Shell COM 解压后端
/// @details 利用 IShellDispatch 直接 COM 接口解压 ZIP 文件：
/// 1. IShellDispatch::NameSpace(zip) 获取 zip 内部 Folder
/// 2. Folder::Items 获取所有项集合
/// 3. IShellDispatch::NameSpace(dir) 获取目标目录 Folder
/// 4. 目标 Folder::CopyHere 一次性解压全部内容
/// 5. ParseName 轮询等待各项写入完成
class AST_UTIL_API ExtractorImplShellCOM : public ExtractorInterface
{
public:
    static ExtractorImplShellCOM& Instance();

    ExtractorImplShellCOM() = default;
    virtual ~ExtractorImplShellCOM() = default;

    virtual errc_t extract(StringView source, StringView target) const override;
    virtual bool canExtract(StringView source) const override;

};

AST_NAMESPACE_END

#endif // _WIN32
