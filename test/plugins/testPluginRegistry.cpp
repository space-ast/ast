///
/// @file      testPluginRegistry.cpp
/// @brief     插件注册表基础行为测试
/// @details   验证扫描/查找接口在无插件情况下的边界行为
/// @author    axel
/// @date      2026-10-14
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "ast/Test.hpp"
#include "ast/PluginRegistry.hpp"
#include "ast/PluginLoader.hpp"
#include "ast/FileSystem.hpp"
#include "ast/Logger.hpp"
#include <fstream>
#include <string>
#include <vector>

AST_USING_NAMESPACE

namespace
{

/// @brief 当前平台的动态库扩展名（与 aIsLibrary 的判定保持一致）
const char* librarySuffix()
{
#if defined(_WIN32)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

/// @brief 在指定目录下创建一个空文件，文件名以给定的扩展名结尾
/// @return 创建成功返回 true
bool createFileWithSuffix(const fs::path& dir, const std::string& stem)
{
    std::ofstream file((dir / (stem + librarySuffix())).string(), std::ios::trunc);
    return file.is_open();
}

/// @brief 创建一个空的临时目录
/// @return 创建失败返回空路径
fs::path makeTempDir(const std::string& name)
{
    const fs::path dir = fs::temp_directory_path() / name;
    fs::remove_all(dir);
    if (!fs::create_directory(dir))
        return fs::path();
    return dir;
}

} // namespace

TEST(PluginRegistry, ScanInvalidPath)
{
    // 空路径
    EXPECT_EQ(PluginRegistry::Instance()->scanDirectory(StringView()), eErrorNullInput);

    // 不存在的目录
    const std::string missing =
        (fs::temp_directory_path() / "ast_plugin_missing_dir_9f2a").string();
    EXPECT_EQ(PluginRegistry::Instance()->scanDirectory(missing), eErrorInvalidFile);
}

TEST(PluginRegistry, EmptyDirScanAndMissLookup)
{
    PluginRegistry* registry = PluginRegistry::Instance();

    const fs::path dir = fs::temp_directory_path() / "ast_plugin_empty_dir_9f2a";
    fs::remove_all(dir);
    if (!fs::create_directory(dir))
    {
        aWarning(_("无法创建临时目录，跳过本用例"));
        return;
    }

    // 空目录：扫描成功，但不产生任何条目
    EXPECT_EQ(registry->scanDirectory(dir.string()), eNoError);
    EXPECT_EQ(registry->entryCount(), static_cast<size_t>(0));

    // 未命中接口：keys 为空、find 返回 nullptr
    std::vector<std::string> keys;
    registry->keys("com.example.NoSuchInterface", keys);
    EXPECT_TRUE(keys.empty());
    EXPECT_TRUE(registry->find("com.example.NoSuchInterface", "anything") == nullptr);

    // 重复扫描同一目录是幂等的
    EXPECT_EQ(registry->scanDirectory(dir.string()), eNoError);
    EXPECT_EQ(registry->entryCount(), static_cast<size_t>(0));

    fs::remove_all(dir);
}

TEST(PluginRegistry, SkipByPrefixAndLoadFailure)
{
    PluginRegistry* registry = PluginRegistry::Instance();
    const size_t entriesBefore = registry->entryCount();
    const size_t pluginsBefore = registry->pluginCount();

    // 目录里放一个只有扩展名像动态库的文件：
    // 名字前缀不匹配时应被跳过，不产生任何索引
    const fs::path dir = makeTempDir("ast_plugin_prefix_dir_9f2a");
    ASSERT_FALSE(dir.empty());
    ASSERT_TRUE(createFileWithSuffix(dir, "NotAPlugin"));

    EXPECT_EQ(registry->scanDirectory(dir.string(), "AstPlugin"), eNoError);
    EXPECT_EQ(registry->entryCount(), entriesBefore);

    // 前缀匹配但内容不是动态库：加载失败，同样不产生索引
    const fs::path dir2 = makeTempDir("ast_plugin_broken_dir_9f2a");
    ASSERT_FALSE(dir2.empty());
    ASSERT_TRUE(createFileWithSuffix(dir2, "AstPluginBroken"));

    EXPECT_EQ(registry->scanDirectory(dir2.string()), eNoError);
    EXPECT_EQ(registry->entryCount(), entriesBefore);
    EXPECT_EQ(registry->pluginCount(), pluginsBefore);

    fs::remove_all(dir);
    fs::remove_all(dir2);
}

TEST(PluginRegistry, MultiPrefixScan)
{
    PluginRegistry* registry = PluginRegistry::Instance();
    const size_t entriesBefore = registry->entryCount();
    const size_t pluginsBefore = registry->pluginCount();

    // 一次扫描同时接受无前缀与带 lib 前缀的插件名
    const fs::path dir = makeTempDir("ast_plugin_multi_prefix_dir_9f2a");
    ASSERT_FALSE(dir.empty());
    ASSERT_TRUE(createFileWithSuffix(dir, "AstPluginFake"));
    ASSERT_TRUE(createFileWithSuffix(dir, "libAstPluginFake"));
    ASSERT_TRUE(createFileWithSuffix(dir, "NotAPlugin"));

    const StringView prefixes[] = {"AstPlugin", "libAstPlugin"};
    EXPECT_EQ(registry->scanDirectory(dir.string(), prefixes), eNoError);

    // 空文件不是有效插件：加载失败，不产生索引
    EXPECT_EQ(registry->entryCount(), entriesBefore);
    EXPECT_EQ(registry->pluginCount(), pluginsBefore);

    fs::remove_all(dir);
}

TEST(PluginLoader, RejectNonPluginLibrary)
{
    // 空路径
    PluginLoader emptyLoader{StringView()};
    EXPECT_EQ(emptyLoader.readMetadata(), eErrorNullInput);
    EXPECT_EQ(emptyLoader.load(), eErrorNullInput);
    EXPECT_FALSE(emptyLoader.isPlugin());
    EXPECT_FALSE(emptyLoader.isLoaded());
    EXPECT_TRUE(emptyLoader.instance() == nullptr);

    // 不存在的文件
    PluginLoader missingLoader{"no_such_plugin_library"};
    EXPECT_EQ(missingLoader.readMetadata(), eErrorInvalidFile);
    EXPECT_EQ(missingLoader.load(), eErrorInvalidFile);
    EXPECT_FALSE(missingLoader.isPlugin());
    EXPECT_FALSE(missingLoader.isLoaded());

    // 存在但不是插件的动态库：文件中没有 .astplug 元数据，读取阶段即拒绝，不加载动态库
    const std::string libPath = aLibPath();
    ASSERT_FALSE(libPath.empty());
    PluginLoader moduleLoader{StringView(libPath.c_str())};
    EXPECT_EQ(moduleLoader.readMetadata(), eErrorInvalidFile);
    EXPECT_FALSE(moduleLoader.isPlugin());
    EXPECT_FALSE(moduleLoader.isLoaded());

    // load() 会先读取元数据；失败结果被缓存，instance() 同样拿不到根对象
    EXPECT_EQ(moduleLoader.load(), eErrorInvalidFile);
    EXPECT_FALSE(moduleLoader.isLoaded());
    EXPECT_TRUE(moduleLoader.instance() == nullptr);
}

GTEST_MAIN()
