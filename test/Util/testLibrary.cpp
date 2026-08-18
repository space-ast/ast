///
/// @file      testLibrary.cpp
/// @brief     Library 类单元测试
/// @details   测试与 QLibrary 兼容的 Library 类接口
/// @author    axel
/// @date      2026-07-30
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

#include "ast/Library.hpp"
#include "ast/LibraryLoader.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

// 测试默认构造
TEST(Library, DefaultConstructor)
{
    Library lib;
    EXPECT_FALSE(lib.isLoaded());
    EXPECT_TRUE(lib.fileName().empty());
}

// 测试带文件名构造
TEST(Library, ConstructorWithFileName)
{
    Library lib("testlib");
    EXPECT_FALSE(lib.isLoaded());
    EXPECT_EQ(lib.fileName(), "testlib");
}

// 测试 setFileName
TEST(Library, SetFileName)
{
    Library lib;
    lib.setFileName("mylib");
    EXPECT_EQ(lib.fileName(), "mylib");
}

// 测试加载不存在的库
TEST(Library, LoadNonexistent)
{
    Library lib("nonexistent_library_12345");
    errc_t rc = lib.load();
    EXPECT_NE(rc, eNoError);
    EXPECT_FALSE(lib.isLoaded());
}

// 测试加载和卸载标准库
TEST(Library, LoadAndUnload)
{
#ifdef _WIN32
    Library lib("kernel32");
#else
    Library lib("c.so.6");
#endif
    errc_t rc = lib.load();
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(lib.isLoaded());

    rc = lib.unload();
    EXPECT_EQ(rc, eNoError);
    EXPECT_FALSE(lib.isLoaded());
}

// 测试重复卸载
TEST(Library, UnloadTwice)
{
    Library lib;
    // 未加载时卸载应返回错误
    errc_t rc = lib.unload();
    EXPECT_NE(rc, eNoError);
}

// 测试符号解析
TEST(Library, Resolve)
{
#ifdef _WIN32
    Library lib("kernel32");
    auto proc = (void*(*)())lib.resolve("GetLastError");
#else
    Library lib("c.so.6");
    auto proc = lib.resolve("printf");
#endif
    EXPECT_NE(proc, nullptr);
}

// 测试 resolve 自动加载
TEST(Library, ResolveAutoLoad)
{
#ifdef _WIN32
    Library lib("kernel32");
    auto proc = lib.resolve("GetLastError");
#else
    Library lib("c.so.6");
    auto proc = lib.resolve("printf");
#endif
    EXPECT_NE(proc, nullptr);
    EXPECT_TRUE(lib.isLoaded());  // resolve 应自动触发 load
}

// 测试解析不存在的符号
TEST(Library, ResolveNonexistentSymbol)
{
#ifdef _WIN32
    Library lib("kernel32");
#else
    Library lib("c.so.6");
#endif
    errc_t rc = lib.load();
    EXPECT_EQ(rc, eNoError);

    void* proc = lib.resolve("nonexistent_symbol_xyz");
    EXPECT_EQ(proc, nullptr);
}

// 测试 resolve 空指针
TEST(Library, ResolveNullSymbol)
{
    Library lib;
    void* proc = lib.resolve(nullptr);
    EXPECT_EQ(proc, nullptr);
}

// 测试 IsLibrary 静态方法
TEST(Library, IsLibrary)
{
    EXPECT_TRUE(Library::IsLibrary("test.dll"));
    EXPECT_TRUE(Library::IsLibrary("test.so"));
    EXPECT_TRUE(Library::IsLibrary("test.dylib"));
    EXPECT_TRUE(Library::IsLibrary("test.a"));
    EXPECT_TRUE(Library::IsLibrary("test.sl"));
    EXPECT_TRUE(Library::IsLibrary("test.bundle"));
    EXPECT_FALSE(Library::IsLibrary("test.txt"));
    EXPECT_FALSE(Library::IsLibrary("test"));
    EXPECT_FALSE(Library::IsLibrary(""));
}

// 测试 Resolve 静态方法
TEST(Library, StaticResolve)
{
#ifdef _WIN32
    auto proc = (void*(*)())Library::Resolve("kernel32", "GetLastError");
#else
    auto proc = Library::Resolve("c.so.6", "printf");
#endif
    EXPECT_NE(proc, nullptr);
}

// 测试静态 Resolve 空参数
TEST(Library, StaticResolveNullParams)
{
    void* proc = Library::Resolve("", "func");
    EXPECT_EQ(proc, nullptr);

    proc = Library::Resolve("lib", nullptr);
    EXPECT_EQ(proc, nullptr);
}

// 测试 setFileName 在已加载时自动卸载
TEST(Library, SetFileNameAfterLoad)
{
#ifdef _WIN32
    Library lib("kernel32");
#else
    Library lib("c.so.6");
#endif
    lib.load();
    EXPECT_TRUE(lib.isLoaded());

    // 设置新文件名应卸载旧库
    lib.setFileName("otherlib");
    EXPECT_FALSE(lib.isLoaded());
    EXPECT_EQ(lib.fileName(), "otherlib");
}

// 测试析构函数自动卸载
TEST(Library, DestructorUnloads)
{
    // 用指针控制生命周期
    auto* lib = new Library();
#ifdef _WIN32
    lib->setFileName("kernel32");
#else
    lib->setFileName("c.so.6");
#endif
    lib->load();
    EXPECT_TRUE(lib->isLoaded());

    delete lib;  // 析构应自动卸载，不崩溃
    // 如果到达这里没有异常，说明析构正常
}

GTEST_MAIN()
