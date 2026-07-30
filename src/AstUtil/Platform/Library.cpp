///
/// @file      Library.cpp
/// @brief     动态库加载类实现
/// @details   ~
/// @author    axel
/// @date      30.07.2026
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

#include "Library.hpp"
#include "LibraryLoader.hpp"

AST_NAMESPACE_BEGIN

Library::Library()
    : handle_(nullptr)
{
}

Library::Library(StringView fileName)
    : handle_(nullptr)
    , fileName_(fileName.data(), fileName.size())
{
}

Library::~Library()
{
    if (handle_) {
        aFreeLibrary(handle_);
        handle_ = nullptr;
    }
}

errc_t Library::load()
{
    if (fileName_.empty()) {
        return eErrorNullInput;
    }

    // 先加载新库，然后卸载旧库，利用操作系统的库引用计数机制，库始终驻留内存
    // 避免了先卸载再加载时因计数归零导致库被移除，进而可能影响当前正在执行的代码
    auto newhandle = aLoadLibrary(fileName_.c_str());
    if (!newhandle) {
        return eError;
    }

    if (handle_) {
        unload();
    }

    handle_ = newhandle;

    return eNoError;
}

errc_t Library::unload()
{
    if (!handle_) {
        return eErrorNullInput;
    }

    errc_t rc = aFreeLibrary(handle_);
    handle_ = nullptr;
    return rc;
}

void* Library::resolve(const char* symbol)
{
    if (!symbol) {
        return nullptr;
    }

    // 如果尚未加载，自动加载
    if (!handle_) {
        errc_t rc = load();
        if (rc != eNoError) {
            return nullptr;
        }
    }

    return aGetProcAddress(handle_, symbol);
}

void Library::setFileName(StringView fileName)
{
    // 如果已有已加载的库，先卸载
    if (handle_) {
        unload();
    }
    fileName_.assign(fileName.data(), fileName.size());
}

bool Library::IsLibrary(StringView fileName)
{
    return aIsLibrary(fileName);
}

void* Library::Resolve(StringView fileName, const char* symbol)
{
    return aResolveProcAddress(std::string(fileName).c_str(), symbol);
}

AST_NAMESPACE_END
