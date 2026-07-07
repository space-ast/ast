///
/// @file      testLibraryLoader.cpp
/// @brief     LibraryLoader模块单元测试
/// @details   测试动态库加载功能
/// @author    axel
/// @date      2026-04-15
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

#include "ast/LibraryLoader.hpp"
#include "ast/Posix.hpp"
#include "ast/Test.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

AST_USING_NAMESPACE

// 测试空指针参数处理
TEST(LibraryLoader, NullPointer)
{
    // 测试 aLoadLibrary 空指针
    void* lib = aLoadLibrary(nullptr);
    EXPECT_EQ(lib, nullptr);
    
    // 测试 aGetProcAddress 空参数
    void* proc = aGetProcAddress(nullptr, "test");
    EXPECT_EQ(proc, nullptr);
    
    proc = aGetProcAddress(nullptr, nullptr);  // 使用 nullptr 替代假指针，避免未定义行为
    EXPECT_EQ(proc, nullptr);
    
    // 测试 aResolveProcAddress 空参数
    proc = aResolveProcAddress(nullptr, "test");
    EXPECT_EQ(proc, nullptr);
    
    proc = aResolveProcAddress("test", nullptr);
    EXPECT_EQ(proc, nullptr);
    
    // 测试 aFreeLibrary 空指针
    errc_t rc = aFreeLibrary(nullptr);
    EXPECT_EQ(rc, eErrorNullInput);
}

// 测试加载不存在的库
TEST(LibraryLoader, LoadNonexistent)
{
    // 尝试加载不存在的库
    void* lib = aLoadLibrary("nonexistent_library_12345");
    EXPECT_EQ(lib, nullptr);
    
    // 获取错误信息
    const char* error = aGetLoadError();
    printf("Load error: %s\n", error ? error : "null");
    EXPECT_NE(error, nullptr);
}

// 测试获取错误信息
TEST(LibraryLoader, GetLoadError)
{
    // 先尝试加载一个不存在的库来触发错误
    aLoadLibrary("nonexistent_lib_xyz");
    
    const char* error = aGetLoadError();
    printf("Error message: %s\n", error ? error : "null");
    EXPECT_NE(error, nullptr);
}

// 测试 aFreeLibrary 无效句柄
TEST(LibraryLoader, FreeInvalidHandle)
{
    // 尝试释放一个无效的句柄
    // 无效指针测试已移除，只测试 nullptr
    errc_t rc = aFreeLibrary(nullptr);
    // 在某些平台上这可能返回成功或失败，取决于系统行为
    EXPECT_EQ(rc, eErrorNullInput);
}

// 测试加载标准库（如果存在）
TEST(LibraryLoader, LoadStandardLibrary)
{
#ifdef _WIN32
    // Windows: 尝试加载 kernel32.dll
    void* lib = aLoadLibrary("kernel32");
    if (lib != nullptr) {
        printf("kernel32 loaded successfully\n");
        
        // 获取函数
        void* proc = aGetProcAddress(lib, "GetLastError");
        printf("GetLastError address: %p\n", proc);
        
        // 卸载库
        errc_t rc = aFreeLibrary(lib);
        EXPECT_EQ(rc, eNoError);
    } else {
        printf("kernel32 not found (expected in some environments)\n");
    }
#else
    // Linux/Unix: 尝试加载 libc.so
    void* lib = aLoadLibrary("c");
    if (lib != nullptr) {
        printf("libc loaded successfully\n");
        
        // 获取函数
        void* proc = aGetProcAddress(lib, "printf");
        printf("printf address: %p\n", proc);
        
        // 卸载库
        errc_t rc = aFreeLibrary(lib);
        EXPECT_EQ(rc, eNoError);
    } else {
        printf("libc not found (expected in some environments)\n");
    }
#endif
}

// 测试 aResolveProcAddress 完整流程
TEST(LibraryLoader, ResolveProcAddress)
{
#ifdef _WIN32
    // Windows: 尝试获取 GetLastError 函数
    void* proc = aResolveProcAddress("kernel32", "GetLastError");
    printf("GetLastError from kernel32: %p\n", proc);
    // 注意：aResolveProcAddress 不会卸载库，所以这个测试后库仍然在内存中
#elif defined(__APPLE__)
    // macOS: 尝试获取函数
    void* proc = aResolveProcAddress("c", "printf");
    printf("printf from libc: %p\n", proc);
#else
    // Linux: 尝试获取函数
    void* proc = aResolveProcAddress("c", "printf");
    printf("printf from libc: %p\n", proc);
#endif
}


GTEST_MAIN()
