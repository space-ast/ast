/// @file      LibraryLoader.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      16.11.2025
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
 
 
#include "LibraryLoader.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/Path.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

// 平台特定的头文件包含
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#if defined(__linux__)
#include <link.h>   // dlinfo / struct link_map
#endif
#endif
 
AST_NAMESPACE_BEGIN


#ifdef A_WASM

void* aLoadLibrary(const char*)
{
    aError(_("WASM 不支持加载库"));
    return nullptr;
}

void* aGetProcAddress(void*, const char*)
{
    aError(_("WASM 不支持获取函数地址"));
    return nullptr;
}

void* aResolveProcAddress(const char* , const char* )
{
    aError(_("WASM 不支持获取函数地址"));
    return nullptr;
}

errc_t aFreeLibrary(void*)
{
    aError(_("WASM 不支持释放库"));
    return eError;
}

errc_t aFreeLibraryOnExit(void*)
{
    aError(_("WASM 不支持释放库"));
    return eError;
}

const char* aGetLoadError()
{
    aError(_("WASM 不支持库加载器"));
    return "WASM not support library loader";
}
#else

#if defined(_WIN32) || defined(_WIN64)
errc_t aAddLibrarySearchDirectory(StringView path)
{
    using AddDllDirectoryFunc = decltype(&AddDllDirectory);
    static AddDllDirectoryFunc addDllDirectory = (AddDllDirectoryFunc) aResolveProcAddress("kernel32.dll", "AddDllDirectory");
    if (!addDllDirectory) return eErrorUnsupported;
	std::wstring widePath = aMakeWin32Path(path);
	auto cookie = addDllDirectory(widePath.c_str());
    if (cookie == 0) return eError;
    return eNoError;
}

#endif

void* _aLoadLibraryFromName(const char* filename)
{
#if defined(_WIN32) || defined(_WIN64)
    /**
     * @bug 从通过 aAddLibrarySearchDirectory 添加的目录中加载 DLL 时，如果其依赖仅存在于 PATH 中，则可能加载失败。
     *
     *      第一次尝试使用 LOAD_LIBRARY_SEARCH_DEFAULT_DIRS，该策略不包含 PATH。
     *      回退的 LoadLibraryW 使用标准搜索路径，该路径不包含通过aAddLibrarySearchDirectory 注册的目录。
     *      没有任何一次尝试同时搜索这两个位置。
     *
     *      同样的缺口也适用于基于路径加载时的同级依赖，以及仅存在于 PATH 的依赖（参见 _aLoadLibraryFromPath）
     */
	std::wstring wideFilename;
	aUtf8ToWide(filename, wideFilename);
    static_assert(sizeof(void*) >= sizeof(HMODULE), "void* must be at least as large as HMODULE");
    // 优先按 LOAD_LIBRARY_SEARCH_* 搜索：
    // 可命中通过 aAddLibrarySearchDirectory 登记的目录（默认搜索顺序不会查这些目录）。
    if (HMODULE handle = LoadLibraryExW(wideFilename.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
        return handle;
    // 回退到默认搜索顺序：
    // 依赖库可能只存在于 PATH 中，指定 LOAD_LIBRARY_SEARCH_* 时 PATH 会被排除，因此需要这次回退。
    return LoadLibraryW(wideFilename.c_str());
#else
    // Linux/Unix平台
    // RTLD_LAZY: 延迟绑定，只在需要时解析符号
    // RTLD_LOCAL: 符号不与其他模块共享
	const int flag = RTLD_LAZY | RTLD_LOCAL;
    return dlopen(filename, flag);
#endif
}


void* _aLoadLibraryFromPath(StringView filepath)
{
#if defined(_WIN32) || defined(_WIN64)
	std::wstring widePath = aMakeWin32Path(filepath);
    static_assert(sizeof(void*) >= sizeof(HMODULE), "void* must be at least as large as HMODULE");
    // 优先按 LOAD_LIBRARY_SEARCH_* 搜索：
    // 可命中动态库同目录的依赖，以及通过 aAddLibrarySearchDirectory 登记的目录。
    if (HMODULE handle = LoadLibraryExW(widePath.c_str(), nullptr,
            LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
        return handle;
    // 回退到默认搜索顺序：
    // 依赖库可能只存在于 PATH 中，指定 LOAD_LIBRARY_SEARCH_* 时 PATH 会被排除，因此需要这次回退。
    return LoadLibraryW(widePath.c_str());
#else
    std::string path(filepath);
    return _aLoadLibraryFromName(path.c_str());
#endif
}


void* aLoadLibrary(const char* filepath)
{
	if (!filepath) return nullptr;
	return aLoadLibrary(StringView(filepath));
}


void* aLoadLibrary(StringView filepath)
{
	if (filepath.empty()) return nullptr;

#if defined(_WIN32) || defined(_WIN64)
	static const char* kLibSuffix    = ".dll";
	static const char* kLibPrefixes[] = {"", "lib"};  // lib 前缀用于支持 mingw 平台
#else
	static const char* kLibSuffix    = ".so";
	static const char* kLibPrefixes[] = {"", "lib"};
#endif

	// 含目录分隔符 → 路径，直接加载
	if (filepath.find('/') != StringView::npos
#ifdef _WIN32
		|| filepath.find('\\') != StringView::npos
#endif
	) {
		return _aLoadLibraryFromPath(filepath);
	}

	// 文件名是否已经以平台扩展名结尾？
	const bool alreadyHasSuffix = filepath.ends_with(kLibSuffix);

	// 试探前缀组合，必要时追加平台扩展名
	for (const char* prefix : kLibPrefixes) {
		std::string name = std::string(prefix) + std::string(filepath);

		if (void* h = _aLoadLibraryFromName(name.c_str()))
			return h;                     // 1) 不加后缀

		if (!alreadyHasSuffix) {
			name += kLibSuffix;
			if (void* h = _aLoadLibraryFromName(name.c_str()))
				return h;                 // 2) 加平台后缀
		}
	}
	return nullptr;
}


void* aGetProcAddress(void* lib, const char* funcName)
{
    if (!lib || !funcName) {
        return nullptr;
    }

#if defined(_WIN32) || defined(_WIN64)
    // Windows平台
    return (void*)GetProcAddress((HMODULE)lib, funcName);
#else
    // Linux/Unix平台
    return dlsym(lib, funcName);
#endif
}

/// @brief 库句柄 RAII 类，用于自动卸载已加载的库
class LibraryHolder
{
public:
    LibraryHolder(void* lib) : handle_(lib) {}
    ~LibraryHolder() { aFreeLibrary(handle_); }
    LibraryHolder(const LibraryHolder& other) = delete;
    LibraryHolder& operator=(const LibraryHolder& other) = delete;
    LibraryHolder(LibraryHolder&& other) : handle_(other.handle_) { other.handle_ = nullptr; }
    LibraryHolder& operator=(LibraryHolder&& other) { std::swap(handle_, other.handle_); return *this; }
private:
    void* handle_{ nullptr };
};

// 记录已加载的库，在程序结束时自动卸载
static std::vector<LibraryHolder> loadedLibraries;
static std::mutex loadedLibrariesMutex;


void* aResolveProcAddress(const char* filepath, const char* funcName)
{
    if (!filepath || !funcName) {
        return nullptr;
    }

    // 加载动态库
    void* library = aLoadLibrary(filepath);
    if (!library) {
        return nullptr;
    }

    // 获取函数指针
    void* procAddress = aGetProcAddress(library, funcName);

    if (!procAddress) {
        // 符号没找到，说明这个库不是我们要的，句柄留着没用，直接卸掉
        aFreeLibrary(library);
        return nullptr;
    }

    // 记录已加载的库，在程序结束时自动卸载
    aFreeLibraryOnExit(library);

    return procAddress;
}


errc_t aFreeLibraryOnExit(void* lib)
{
    if (!lib)
        return eErrorNullInput;
    std::lock_guard<std::mutex> lock(loadedLibrariesMutex);
    loadedLibraries.emplace_back(lib);
    return eNoError;
}


errc_t aFreeLibrary(void* lib)
{
    if (!lib) {
        return eErrorNullInput;
    }

#if defined(_WIN32) || defined(_WIN64)
    // Windows平台
    if(FreeLibrary((HMODULE)lib) == 0){
        return eErrorInvalidParam;
    }
    return eNoError;
#else
    // Linux/Unix平台
    if(dlclose(lib) != 0){
        return eErrorInvalidParam;
    }
    return eNoError;
#endif
}


const char* aGetLoadError()
{
#if defined(_WIN32) || defined(_WIN64)
    // Windows平台 
    // @todo：这里返回的是简单的错误码，实际使用时可能需要格式化错误消息
    static char buffer[32];
    DWORD errorCode = GetLastError();
    snprintf(buffer, sizeof(buffer), "Error code: %lu", errorCode);
    return buffer;
#else
    // Linux/Unix平台
    return dlerror();
#endif
}

#endif // A_WASM


std::string aGetModuleFilePath(void* lib)
{
#ifdef A_WASM
    (void)lib;
    return std::string();
#elif defined(_WIN32) || defined(_WIN64)
    if (!lib) {
        return std::string();
    }

    // GetModuleFileNameW 在缓冲区不足时返回缓冲区大小，需要动态扩容
    std::wstring buffer(MAX_PATH, L'\0');
    for (;;) {
        DWORD len = ::GetModuleFileNameW((HMODULE)lib, &buffer[0], static_cast<DWORD>(buffer.size()));
        if (len == 0) {
            return std::string();
        }
        if (len < buffer.size()) {
            buffer.resize(len);
            break;
        }
        if (buffer.size() >= 0x8000) {   // Windows 路径长度上限
            return std::string();
        }
        buffer.resize(buffer.size() * 2);
    }
    return aWideToUtf8(buffer.c_str());
#elif defined(__linux__)
    if (!lib) {
        return std::string();
    }

    // dladdr 只接受符号地址，句柄要用 dlinfo 取 link_map
    struct link_map* map = nullptr;
    if (::dlinfo(lib, RTLD_DI_LINKMAP, &map) != 0 || !map || !map->l_name || !map->l_name[0]) {
        return std::string();
    }

    std::string path = map->l_name;
    // dlopen 传入的是不带路径的文件名时，l_name 也只是文件名，尽量还原成绝对路径
    if (path[0] != '/') {
        if (char* resolved = ::realpath(path.c_str(), nullptr)) {
            path = resolved;
            ::free(resolved);
        }
    }
    return path;
#else
    (void)lib;
    aWarning(_("当前平台不支持获取动态库路径"));
    return std::string();
#endif
}


bool aIsLibrary(StringView fileName)
{
#if defined(_WIN32) || defined(_WIN64)
    return fileName.ends_with(".dll");
#elif defined(__linux__)
    return fileName.ends_with(".so");
#elif defined(__APPLE__)
    return fileName.ends_with(".dylib");
#else
    aWarning(_("未知平台或该平台不支持动态库"));
    return false;
#endif
}

AST_NAMESPACE_END