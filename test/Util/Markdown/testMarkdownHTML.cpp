///
/// @file      testMarkdownHTML.cpp
/// @brief     Markdown HTML 渲染器单元测试
/// @details   使用 MarkdownParser + MarkdownHTML 将综合 Markdown 文档转为 HTML 输出，
///            验证各块级/行内元素的 HTML 标签生成是否正确。
/// @author    axel
/// @date      2026-06-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "AstGlobal.h"
#include "ast/MarkdownParser.hpp"
#include "ast/MarkdownHTML.hpp"
#include "ast/IO.hpp"
#include "ast/Test.h"

#include <cstdio>
#include <fstream>

AST_USING_NAMESPACE

// ============================================================================
// 辅助工具
// ============================================================================

/// @brief 使用 MarkdownParser + MarkdownHTML 渲染 markdown 为 HTML
static std::string renderHtml(const std::string& markdown)
{
    MarkdownHTML html;
    MarkdownParser parser(html);

    // 逐行 feed，模拟流式输入
    parser.feed(markdown);
    parser.flush();

    return html.output();
}

/// @brief 检查子串是否存在
static bool has(const std::string& s, const std::string& sub)
{
    return s.find(sub) != std::string::npos;
}

/// @brief 渲染并打印，返回 HTML 字符串
static std::string show(const char* label, const std::string& markdown)
{
    std::string out = renderHtml(markdown);

    ast_printf("\n[%s]\n", label);
    ast_printf("[input]\n%s", markdown.c_str());
    if (!markdown.empty() && markdown.back() != '\n') ast_printf("\n");
    ast_printf("[output]\n%s", out.c_str());
    if (!out.empty() && out.back() != '\n') ast_printf("\n");

    return out;
}

// ============================================================================
// 标题
// ============================================================================

TEST(MarkdownHTMLTest, Heading)
{
    std::string out = show("heading", "# 一级标题\n## 二级标题\n### 三级\n");
    EXPECT_TRUE(has(out, "<h1>"));
    EXPECT_TRUE(has(out, "</h1>"));
    EXPECT_TRUE(has(out, "<h2>"));
    EXPECT_TRUE(has(out, "</h2>"));
    EXPECT_TRUE(has(out, "<h3>"));
    EXPECT_TRUE(has(out, "</h3>"));
    EXPECT_TRUE(has(out, "一级标题"));
}

// ============================================================================
// 段落
// ============================================================================

TEST(MarkdownHTMLTest, Paragraph)
{
    std::string out = show("paragraph", "Hello World\n\nSecond paragraph.\n");
    EXPECT_TRUE(has(out, "<p>"));
    EXPECT_TRUE(has(out, "</p>"));
    EXPECT_TRUE(has(out, "Hello World"));
    EXPECT_TRUE(has(out, "Second paragraph"));
}

// ============================================================================
// 粗体 / 斜体
// ============================================================================

TEST(MarkdownHTMLTest, BoldItalic)
{
    std::string out = show("bold+italic", "Hello **world** and *italic* text\n");
    EXPECT_TRUE(has(out, "<strong>world</strong>"));
    EXPECT_TRUE(has(out, "<em>italic</em>"));
}

TEST(MarkdownHTMLTest, BoldItalicTriple)
{
    std::string out = show("triple ***", "***bold italic***\n");
    // *** 歧义消除：解析器先匹配 ** (粗体)，剩余 * 为字面文本
    EXPECT_TRUE(has(out, "<strong>"));
    EXPECT_TRUE(has(out, "bold italic"));
    EXPECT_TRUE(has(out, "</strong>"));
}

// ============================================================================
// 行内代码
// ============================================================================

TEST(MarkdownHTMLTest, InlineCode)
{
    std::string out = show("inline code", "Use `printf()` function\n");
    EXPECT_TRUE(has(out, "<code>printf()</code>"));
}

// ============================================================================
// 链接
// ============================================================================

TEST(MarkdownHTMLTest, Link)
{
    std::string out = show("link", "See [GitHub](https://github.com)\n");
    EXPECT_TRUE(has(out, "<a href=\"https://github.com\">"));
    EXPECT_TRUE(has(out, "GitHub"));
    EXPECT_TRUE(has(out, "</a>"));
}

// ============================================================================
// 图片
// ============================================================================

TEST(MarkdownHTMLTest, Image)
{
    std::string out = show("image", "![logo](https://example.com/img.png)\n");
    EXPECT_TRUE(has(out, "<img src=\"https://example.com/img.png\""));
    EXPECT_TRUE(has(out, "alt=\"logo\""));
}

// ============================================================================
// 无序列表
// ============================================================================

TEST(MarkdownHTMLTest, UnorderedList)
{
    std::string out = show("unordered list",
        "- item A\n- item B\n- item C\n");
    EXPECT_TRUE(has(out, "<ul>"));
    EXPECT_TRUE(has(out, "</ul>"));
    EXPECT_TRUE(has(out, "<li>item A</li>"));
    EXPECT_TRUE(has(out, "<li>item B</li>"));
    EXPECT_TRUE(has(out, "<li>item C</li>"));
}

// ============================================================================
// 有序列表
// ============================================================================

TEST(MarkdownHTMLTest, OrderedList)
{
    std::string out = show("ordered list",
        "1. first\n2. second\n3. third\n");
    EXPECT_TRUE(has(out, "<ol>"));
    EXPECT_TRUE(has(out, "</ol>"));
    EXPECT_TRUE(has(out, "<li>first</li>"));
    EXPECT_TRUE(has(out, "<li>second</li>"));
    EXPECT_TRUE(has(out, "<li>third</li>"));
}

// ============================================================================
// 引用
// ============================================================================

TEST(MarkdownHTMLTest, Blockquote)
{
    std::string out = show("blockquote", "> quoted text\n");
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "</blockquote>"));
    EXPECT_TRUE(has(out, "quoted text"));
}

// ============================================================================
// 分割线
// ============================================================================

TEST(MarkdownHTMLTest, HorizontalRule)
{
    std::string out = show("HR", "before\n---\nafter\n");
    EXPECT_TRUE(has(out, "<hr>"));
}

// ============================================================================
// 代码块
// ============================================================================

TEST(MarkdownHTMLTest, FencedCodeBlock)
{
    std::string out = show("code block",
        "```cpp\nint x = 42;\n```\n");
    EXPECT_TRUE(has(out, "<pre><code"));
    EXPECT_TRUE(has(out, "language-cpp"));
    EXPECT_TRUE(has(out, "</code></pre>"));
    EXPECT_TRUE(has(out, "int x = 42;"));
}

TEST(MarkdownHTMLTest, CodeBlockNoLang)
{
    std::string out = show("code block no lang",
        "```\nplain code\n```\n");
    EXPECT_TRUE(has(out, "<pre><code>"));
    EXPECT_FALSE(has(out, "language-"));
    EXPECT_TRUE(has(out, "plain code"));
}

// ============================================================================
// HTML 转义
// ============================================================================

TEST(MarkdownHTMLTest, HtmlEscaping)
{
    std::string out = show("escaping", "Use `<div>` for layout\n");
    EXPECT_TRUE(has(out, "&lt;div&gt;"));
    EXPECT_FALSE(has(out, "<div>"));
}

TEST(MarkdownHTMLTest, AmpersandEscaping)
{
    std::string out = show("ampersand", "A & B & C\n");
    EXPECT_TRUE(has(out, "A &amp; B &amp; C"));
}

// ============================================================================
// 综合 Markdown 渲染示例
// ============================================================================

TEST(MarkdownHTMLTest, RenderAllElements)
{
    const char* sample = R"md(
# Markdown → HTML 综合示例

## 1. 标题层级

### 1.1 三级标题

---

## 2. 文本样式

- **粗体文本**
- *斜体文本*
- ***粗斜体文本***
- `行内代码`

---

## 3. 列表

### 3.1 无序列表

- 苹果
- 香蕉
- 樱桃

### 3.2 有序列表

1. 第一步：打开软件
2. 第二步：创建场景
3. 第三步：添加对象

### 3.3 嵌套列表

- 水果
  1. 苹果
  2. 香蕉
- 蔬菜
  - 白菜
  - 萝卜

---

## 4. 引用

> 这是单行引用

> 这是多行引用
> 可以包含多行内容
>
> > 这是嵌套引用

---

## 5. 代码块

### Python 示例

```python
import numpy as np

def calculate_orbit(altitude, inclination):
    """
    计算卫星轨道参数
    """
    R_earth = 6371
    mu = 398600.4418
    r = R_earth + altitude
    v = np.sqrt(mu / r)
    return {'radius': r, 'velocity': v}

# 计算ISS轨道参数
iss = calculate_orbit(altitude=408, inclination=51.6)
```

### 行内代码

在终端中运行 `npm install` 安装依赖。

---

## 6. 链接

- [GitHub](https://github.com)
- [NASA](https://www.nasa.gov)

---

## 7. 图片

![卫星轨道示意图](https://via.placeholder.com/400x200/3498db/ffffff?text=Orbit)

---

## 8. 组合示例

> **提示：** 在航天任务设计中，需要综合考虑以下因素：
>
> 1. **轨道类型** - 包括 LEO、MEO、GEO 等
> 2. **覆盖范围** - 卫星对地面的覆盖区域
>
> ```python
> # 计算覆盖范围
> coverage_angle = np.arccos(R_earth / (R_earth + altitude))
> ```
>
> 更多信息请参考 [卫星轨道设计指南](https://example.com)。
)md";

    // 去掉开头的换行
    std::string markdown = sample;

    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed(markdown);
    parser.flush();

    std::string out = html.output();

    ast_printf("\n");
    ast_printf("================================================================\n");
    ast_printf("  综合 Markdown → HTML 渲染示例\n");
    ast_printf("================================================================\n");
    ast_printf("\n[HTML OUTPUT]\n%s", out.c_str());
    if (!out.empty() && out.back() != '\n')
        ast_printf("\n");
    ast_printf("================================================================\n");

    // ================================================================
    // 验证关键 HTML 元素
    // ================================================================

    // 标题
    EXPECT_TRUE(has(out, "<h1>Markdown → HTML 综合示例</h1>"));
    EXPECT_TRUE(has(out, "<h2>1. 标题层级</h2>"));
    EXPECT_TRUE(has(out, "<h3>1.1 三级标题</h3>"));

    // 分割线
    EXPECT_TRUE(has(out, "<hr>"));

    // 无序列表
    EXPECT_TRUE(has(out, "<ul>"));
    EXPECT_TRUE(has(out, "</ul>"));
    EXPECT_TRUE(has(out, "<li>苹果</li>"));
    EXPECT_TRUE(has(out, "<li>香蕉</li>"));

    // 有序列表
    EXPECT_TRUE(has(out, "<ol>"));
    EXPECT_TRUE(has(out, "</ol>"));
    EXPECT_TRUE(has(out, "<li>第一步：打开软件</li>"));

    // 文本样式
    EXPECT_TRUE(has(out, "<strong>粗体文本</strong>"));
    EXPECT_TRUE(has(out, "<em>斜体文本</em>"));
    EXPECT_TRUE(has(out, "<code>行内代码</code>"));

    // 引用
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "</blockquote>"));

    // 代码块
    EXPECT_TRUE(has(out, "<pre><code class=\"language-python\">"));
    EXPECT_TRUE(has(out, "</code></pre>"));
    EXPECT_TRUE(has(out, "import numpy as np"));
    EXPECT_TRUE(has(out, "def calculate_orbit"));

    // 行内代码
    EXPECT_TRUE(has(out, "<code>npm install</code>"));

    // 链接
    EXPECT_TRUE(has(out, "<a href=\"https://github.com\">GitHub</a>"));
    EXPECT_TRUE(has(out, "<a href=\"https://www.nasa.gov\">NASA</a>"));

    // 图片
    EXPECT_TRUE(has(out, "<img src=\"https://via.placeholder.com/400x200/3498db/ffffff?text=Orbit\""));
    EXPECT_TRUE(has(out, "alt=\"卫星轨道示意图\""));

    // 组合：引用中的粗体
    EXPECT_TRUE(has(out, "<strong>提示：</strong>"));

    // 组合：引用中的链接
    EXPECT_TRUE(has(out, "<a href=\"https://example.com\">卫星轨道设计指南</a>"));

    // 嵌套引用
    int bqCount = 0;
    for (size_t pos = 0; (pos = out.find("<blockquote>", pos)) != std::string::npos; ++pos)
        ++bqCount;
    EXPECT_GE(bqCount, 2) << "Should have at least 2 blockquotes (including nested)";

    // ================================================================
    // 将 HTML 写入文件，便于在浏览器中验证
    // ================================================================
    std::string htmlFull = "<!DOCTYPE html>\n<html>\n<head>\n"
        "<meta charset=\"utf-8\">\n"
        "<title>Markdown → HTML 测试输出</title>\n"
        "<style>\n"
        "body { font-family: -apple-system, sans-serif; max-width: 800px; margin: 2em auto; "
        "line-height: 1.6; color: #333; }\n"
        "pre { background: #f5f5f5; padding: 1em; border-radius: 4px; overflow-x: auto; }\n"
        "code { background: #f5f5f5; padding: 2px 4px; border-radius: 3px; }\n"
        "pre code { background: none; padding: 0; }\n"
        "blockquote { border-left: 4px solid #ccc; margin: 0; padding-left: 1em; color: #666; }\n"
        "img { max-width: 100%; }\n"
        "table { border-collapse: collapse; }\n"
        "td, th { border: 1px solid #ddd; padding: 8px; }\n"
        "hr { border: none; border-top: 1px solid #ddd; }\n"
        "</style>\n"
        "</head>\n<body>\n";
    htmlFull += out;
    htmlFull += "\n</body>\n</html>\n";

    std::string filePath = "testMarkdownHTML_output.html";
    {
        std::ofstream ofs(filePath);
        if (ofs.is_open())
        {
            ofs << htmlFull;
            ast_printf("\n[FILE] HTML written to: %s\n", filePath.c_str());
        }
    }
}

GTEST_MAIN()
