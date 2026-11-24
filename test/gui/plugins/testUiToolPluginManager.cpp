///
/// @file      testUiToolPluginManager.cpp
/// @brief     界面工具插件管理器测试
/// @details   验证元数据解析（多语言、字段校验、坏条目跳过）与刷新接口的幂等性
/// @author    axel
/// @date      2026-11-22
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "ast/Test.hpp"

#ifdef AST_WITH_QT

#include "ast/JsonValue.hpp"
#include "ast/PluginInterface.hpp"
#include "ast/UiLanguage.hpp"
#include "ast/UiToolPluginManager.hpp"
#include "ast/UiToolProvider.hpp"

#include <string>
#include <vector>

AST_USING_NAMESPACE

namespace
{

/// @brief 构造一份只带 JSON 数据的插件元数据
PluginMetadata makeMetadata(const char* json)
{
    PluginMetadata metadata;
    metadata.iid_ = AST_IID_IUiToolProvider;
    metadata.json_ = JsonValue::FromString(json);
    return metadata;
}

} // namespace

TEST(UiToolPluginManager, ResolveTextFallback)
{
    const JsonValue node = JsonValue::FromString(R"({"zh":"中文","en":"English"})");

    EXPECT_EQ(aUiResolveText(node, "zh"), std::string("中文"));
    EXPECT_EQ(aUiResolveText(node, "en"), std::string("English"));

    // 未命中当前语言时回退到英文
    EXPECT_EQ(aUiResolveText(node, "fr"), std::string("English"));

    // 带区域的语言码归一化到主语言
    EXPECT_EQ(aUiResolveText(node, "zh-CN"), std::string("中文"));

    // 纯字符串对所有语言一致
    EXPECT_EQ(aUiResolveText(JsonValue("fixed"), "zh"), std::string("fixed"));

    // 空值与非对象非字符串返回空串
    EXPECT_TRUE(aUiResolveText(JsonValue(), "zh").empty());
    EXPECT_TRUE(aUiResolveText(JsonValue(1), "zh").empty());
}

TEST(UiToolPluginManager, ResolveTextEmptyLangUsesCurrentLanguage)
{
    const JsonValue node = JsonValue::FromString(R"({"zh":"中文","en":"English"})");

    // 不传语言时按当前界面语言解析
    aUiSetLanguage("zh");
    EXPECT_EQ(aUiResolveText(node, ""), std::string("中文"));

    aUiSetLanguage("en");
    EXPECT_EQ(aUiResolveText(node, ""), std::string("English"));

    // 条目接口同样遵循该回退
    std::vector<UiToolPluginEntry> tools;
    UiToolPluginManager::ParseTools("p.dll",
        makeMetadata(R"json({"tools":{"t":{"title":{"zh":"中文标题","en":"English title"}}}})json"),
        tools);
    ASSERT_TRUE(tools.size() == 1);

    aUiSetLanguage("zh");
    EXPECT_EQ(tools[0].title(""), std::string("中文标题"));
    aUiSetLanguage("en");
    EXPECT_EQ(tools[0].title(""), std::string("English title"));

    // 恢复为跟随环境变量/系统区域
    aUiSetLanguage("");
}

TEST(UiToolPluginManager, ParseTools)
{
    const PluginMetadata metadata = makeMetadata(R"json({
        "api": 2,
        "tools": {
            "second": { "title": { "en": "Second", "zh": "第二个" }, "order": 5 },
            "first":  { "title": { "en": "First" }, "tooltip": { "en": "tip" } }
        }
    })json");

    std::vector<UiToolPluginEntry> tools;
    UiToolPluginManager::ParseTools("C:/plugins/AstPluginX.dll", metadata, tools);

    ASSERT_TRUE(tools.size() == 2);

    // 解析顺序跟随 tools 对象的键序（std::map，字母序）
    EXPECT_EQ(tools[0].id(), std::string("first"));
    EXPECT_EQ(tools[1].id(), std::string("second"));

    EXPECT_EQ(tools[1].title("zh"), std::string("第二个"));
    EXPECT_EQ(tools[1].order(), 5);
    EXPECT_EQ(tools[0].api(), 2);
    EXPECT_EQ(tools[0].tooltip("en"), std::string("tip"));
    EXPECT_TRUE(tools[0].iconPath().empty());
}

TEST(UiToolPluginManager, IncompatibleApiMarked)
{
    // 接口版本不一致：条目保留，但状态为 eIncompatible，且记录原因
    std::vector<UiToolPluginEntry> incompatible;
    UiToolPluginManager::ParseTools("p.dll", makeMetadata(R"json({
        "api": 2,
        "tools": { "t": { "title": { "en": "T" } } }
    })json"), incompatible);

    ASSERT_TRUE(incompatible.size() == 1);
    EXPECT_EQ(incompatible[0].api(), 2);
    EXPECT_TRUE(incompatible[0].state() == EUiToolState::eIncompatible);
    EXPECT_FALSE(incompatible[0].error().empty());

    // 版本一致时保持未加载状态
    std::vector<UiToolPluginEntry> compatible;
    UiToolPluginManager::ParseTools("p.dll", makeMetadata(R"json({
        "api": 1,
        "tools": { "t": { "title": { "en": "T" } } }
    })json"), compatible);

    ASSERT_TRUE(compatible.size() == 1);
    EXPECT_TRUE(compatible[0].state() == EUiToolState::eNormal);
    EXPECT_TRUE(compatible[0].error().empty());
}

TEST(UiToolPluginManager, SkipInvalidTools)
{
    const PluginMetadata metadata = makeMetadata(R"json({
        "tools": {
            "bad id":     { "title": { "en": "x" } },
            "no_title":   { "order": 1 },
            "not_object": 3,
            "ok":         { "title": { "en": "OK" } }
        }
    })json");

    std::vector<UiToolPluginEntry> tools;
    UiToolPluginManager::ParseTools("p.dll", metadata, tools);

    ASSERT_TRUE(tools.size() == 1);
    EXPECT_EQ(tools[0].id(), std::string("ok"));
}

TEST(UiToolPluginManager, MissingToolsObject)
{
    std::vector<UiToolPluginEntry> tools;
    UiToolPluginManager::ParseTools("p.dll", makeMetadata(R"({"api":1})"), tools);
    EXPECT_TRUE(tools.empty());

    // 元数据不是 JSON 对象（例如纯文本元数据）时不应产生工具
    PluginMetadata textMetadata;
    textMetadata.json_ = JsonValue("plain");
    UiToolPluginManager::ParseTools("p.dll", textMetadata, tools);
    EXPECT_TRUE(tools.empty());
}

TEST(UiToolPluginManager, RefreshIsIdempotent)
{
    UiToolPluginManager& manager = UiToolPluginManager::Instance();
    EXPECT_EQ(manager.refresh(), eNoError);
    EXPECT_EQ(manager.refresh(), eNoError);

    for (const UiToolPluginEntry& info : manager.tools())
    {
        const EUiToolState state = info.state();
        EXPECT_TRUE(state == EUiToolState::eNormal);
    }
}

TEST(UiToolPluginManager, LanguageOverride)
{
    // 宿主显式设置的语言优先于系统区域设置
    aUiSetLanguage("zh");
    EXPECT_EQ(aUiCurrentLanguage(), std::string("zh"));

    // 带区域的语言码归一化到主语言
    aUiSetLanguage("en-US");
    EXPECT_EQ(aUiCurrentLanguage(), std::string("en"));

    // 传空串恢复为跟随系统
    aUiSetLanguage("");
    EXPECT_FALSE(aUiCurrentLanguage().empty());
}

TEST(UiToolPluginManager, DiscoverExamplePluginWithoutLoading)
{
    // 示例插件与本测试位于同一输出目录；未构建示例插件时跳过本用例
    UiToolPluginManager& manager = UiToolPluginManager::Instance();
    manager.refresh();

    const UiToolPluginEntry* info = manager.tool("hello");
    if (info == nullptr)
        return;

    EXPECT_FALSE(info->title().empty());
    EXPECT_FALSE(info->pluginPath().empty());
    EXPECT_EQ(info->order(), 900);

    EXPECT_TRUE(info->state() == EUiToolState::eNormal);
}

TEST(UiToolPluginManager, LanguageIndependentEntry)
{
    UiToolPluginManager& manager = UiToolPluginManager::Instance();

    aUiSetLanguage("zh");
    manager.refresh();

    const UiToolPluginEntry* info = manager.tool("hello");
    if (info == nullptr)
        return;  // 未构建示例插件时跳过

    // 同一条目可以解析出任意语言的文本
    const std::string zhTitle = info->title("zh");
    const std::string enTitle = info->title("en");
    EXPECT_FALSE(zhTitle.empty());
    EXPECT_FALSE(enTitle.empty());
    EXPECT_NE(zhTitle, enTitle);

    // 切换语言不需要刷新清单，条目指针依然有效，读取时按当前语言解析
    aUiSetLanguage("en");
    EXPECT_EQ(info->title(), enTitle);

    aUiSetLanguage("zh");
    EXPECT_EQ(info->title(), zhTitle);
}

#endif // AST_WITH_QT

GTEST_MAIN()
