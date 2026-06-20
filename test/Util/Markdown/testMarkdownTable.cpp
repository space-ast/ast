///
/// @file      testMarkdownTable.cpp
/// @brief     MarkdownTable 单元测试
/// @author    axel
/// @date      2026-06-15
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "AstGlobal.h"
//#include "ast/MarkdownTable.hpp"
//#include "ast/Markdown.hpp"
#include "ast/MarkdownRenderer.hpp"
#include "ast/IO.hpp"
#include "ast/Test.h"
#include <gtest/gtest.h>
#include <cstdio>
#include <string>

AST_USING_NAMESPACE

// ============================================================================
// 目视验证 — 打印所有表格样式到终端
// ============================================================================
TEST(MarkdownTableTest, VisualPrintAll)
{
    ast_printf("\n===== 基础表格 =====\n");
    {
        const char* input =
            "| 参数 | 数值 | 单位 |\n"
            "|------|-----:|:----:|\n"
            "| 轨道高度 | 500.5 | km |\n"
            "| 倾角 | 97.4 | 度 |\n"
            "| 半长轴 | 6878.14 | km |\n";
        std::string r = aMarkdownANSI(input);
        ast_printf("%s\n", r.c_str());
        EXPECT_FALSE(r.empty());
    }

    ast_printf("\n===== 简单英文表格 =====\n");
    {
        const char* input =
            "| Name | Value |\n"
            "|------|-------|\n"
            "| foo  | 123   |\n"
            "| bar  | 456   |\n";
        std::string r = aMarkdownANSI(input);
        ast_printf("%s\n", r.c_str());
        EXPECT_FALSE(r.empty());
    }

    ast_printf("\n===== 行内样式 =====\n");
    {
        const char* input =
            "| 样式 | 示例 |\n"
            "|------|------|\n"
            "| **粗体** | *斜体* |\n"
            "| `代码` | [链接](url) |\n";
        std::string r = aMarkdownANSI(input);
        ast_printf("%s\n", r.c_str());
        EXPECT_FALSE(r.empty());
    }

    ast_printf("\n===== 多列对齐 =====\n");
    {
        const char* input =
            "| 序号 | 名称 | 描述 | 备注 |\n"
            "|:----:|------|------|------|\n"
            "| 1 | Alpha | 姿态控制系统 | 正常 |\n"
            "| 2 | Beta | 推进系统 | 待检查 |\n"
            "| 3 | Gamma | 通信链路 | 正常 |\n";
        std::string r = aMarkdownANSI(input);
        ast_printf("%s\n", r.c_str());
        EXPECT_FALSE(r.empty());
    }

    ast_printf("\n===== 单列表格 =====\n");
    {
        const char* input =
            "| 任务名称 |\n"
            "|----------|\n"
            "| 嫦娥七号 |\n"
            "| 天问三号 |\n";
        std::string r = aMarkdownANSI(input);
        ast_printf("%s\n", r.c_str());
        EXPECT_FALSE(r.empty());
    }

    ast_printf("\n===== 只有表头无数据 =====\n");
    {
        const char* input =
            "| A | B | C |\n"
            "|---|---|---|\n";
        std::string r = aMarkdownANSI(input);
        ast_printf("%s\n", r.c_str());
        EXPECT_FALSE(r.empty());
    }
}

// ============================================================================
// 基础表格渲染
// ============================================================================
TEST(MarkdownTableTest, BasicTable)
{
    const char* input =
        "| Name | Value |\n"
        "|------|-------|\n"
        "| foo  | 123   |\n"
        "| bar  | 456   |\n";

    std::string result = aMarkdownANSI(input);
    EXPECT_FALSE(result.empty());

    // 应包含框线字符
    EXPECT_NE(result.find("\xe2\x94\x8c"), std::string::npos);  // ┌
    EXPECT_NE(result.find("\xe2\x94\x82"), std::string::npos);  // │
    EXPECT_NE(result.find("\xe2\x94\x94"), std::string::npos);  // └

    // 应包含表头和数据
    EXPECT_NE(result.find("Name"), std::string::npos);
    EXPECT_NE(result.find("foo"), std::string::npos);
    EXPECT_NE(result.find("123"), std::string::npos);
}

// ============================================================================
// 对齐方式
// ============================================================================
TEST(MarkdownTableTest, Alignments)
{
    const char* input =
        "| Left | Center | Right |\n"
        "|------|:------:|------:|\n"
        "| a    | b      | c     |\n";

    std::string result = aMarkdownANSI(input);
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("Left"), std::string::npos);
    EXPECT_NE(result.find("Center"), std::string::npos);
    EXPECT_NE(result.find("Right"), std::string::npos);
}

// ============================================================================
// 中文表格
// ============================================================================
TEST(MarkdownTableTest, ChineseTable)
{
    const char* input =
        "| 参数 | 数值 | 单位 |\n"
        "|------|------|------|\n"
        "| 轨道高度 | 500 | km |\n";

    std::string result = aMarkdownANSI(input);
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("参数"), std::string::npos);
    EXPECT_NE(result.find("轨道高度"), std::string::npos);
}

// ============================================================================
// 无效输入
// ============================================================================
TEST(MarkdownTableTest, InvalidInput)
{
    EXPECT_TRUE(aMarkdownANSI("").empty());
}

// ============================================================================
// 单列表格
// ============================================================================
TEST(MarkdownTableTest, SingleColumn)
{
    const char* input =
        "| Item |\n"
        "|------|\n"
        "| one  |\n"
        "| two  |\n";

    std::string result = aMarkdownANSI(input);
    EXPECT_FALSE(result.empty());
}

// ============================================================================
// 行内样式
// ============================================================================
TEST(MarkdownTableTest, InlineStyles)
{
    const char* input =
        "| Name | Description |\n"
        "|------|-------------|\n"
        "| **bold** | *italic* text |\n"
        "| `code` | ~~strike~~ |\n";

    std::string result = aMarkdownANSI(input);
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("\033[1m"), std::string::npos);   // bold
    EXPECT_NE(result.find("\033[3m"), std::string::npos);   // italic
}

// ============================================================================
// 流式集成测试 — 通过 Markdown 渲染器处理含表格的文本
// ============================================================================
TEST(MarkdownTableTest, StreamingIntegration)
{
    // 模拟 ChatConsole 的流式场景：文本 + 表格 + 文本
    const char* input =
        "这是一段普通文本。\n"
        "\n"
        "| 参数 | 数值 |\n"
        "|------|------|\n"
        "| A    | 1    |\n"
        "| B    | 2    |\n"
        "\n"
        "表格后的文本。\n";

    std::string result = aMarkdownANSI(input);
    ast_printf("%s\n", result.c_str());

    // 表格部分应包含框线字符，证明被渲染了
    EXPECT_NE(result.find("\xe2\x94\x8c"), std::string::npos);  // ┌
    EXPECT_NE(result.find("\xe2\x94\x82"), std::string::npos);  // │

    // 表格前后的文本也应该正常输出
    EXPECT_NE(result.find("普通文本"), std::string::npos);
    EXPECT_NE(result.find("表格后的文本"), std::string::npos);

    // 表格内容应存在
    EXPECT_NE(result.find("参数"), std::string::npos);
    EXPECT_NE(result.find("A"), std::string::npos);
}

// ============================================================================
// 非表格行首 | — 不应被误判为表格
// ============================================================================
TEST(MarkdownTableTest, PipeLineNotTable)
{
    // 只有一行 |，没有分隔行 → 不是表格，应原样输出
    std::string result = aMarkdownANSI("| 这不是表格\n\n下一段\n");
    ast_printf("%s\n", result.c_str());

    // 不应包含框线字符
    EXPECT_EQ(result.find("\xe2\x94\x8c"), std::string::npos);
    // | 行应该原样出现
    EXPECT_NE(result.find("这不是表格"), std::string::npos);
    EXPECT_NE(result.find("下一段"), std::string::npos);
}

GTEST_MAIN()

