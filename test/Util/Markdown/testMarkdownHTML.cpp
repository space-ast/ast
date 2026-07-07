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
#include "ast/TestMarkdown.hpp"
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
    parser.finish();

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
// 嵌套混合列表（有序/无序交错，最多四级嵌套）
// ============================================================================

TEST(MarkdownHTMLTest, NestedMixedList)
{
    std::string out = show("nested mixed list", R"(
- 一级无序列表项 A
  1. 二级有序列表项 1
    - 三级无序列表项 a
    - 三级无序列表项 b
  2. 二级有序列表项 2
    - 三级无序列表项 c
      1. 四级有序列表项 i
      2. 四级有序列表项 ii
- 一级无序列表项 B
  1. 二级有序列表项 3
  2. 二级有序列表项 4

1. 一级有序列表项 1
  - 二级无序列表项 X
    1. 三级有序列表项 α
    2. 三级有序列表项 β
  - 二级无序列表项 Y
    1. 三级有序列表项 γ
      - 四级无序列表项 甲
      - 四级无序列表项 乙
2. 一级有序列表项 2
  - 二级无序列表项 Z
)");


    
}

// ============================================================================
// 纯无序列表嵌套（全部 - 开头，最多四级嵌套）
// ============================================================================

TEST(MarkdownHTMLTest, NestedUnorderedList)
{
    std::string out = show("nested unordered list", R"(
- 一级项目 A
  - 二级项目 A1
    - 三级项目 A1a
      - 四级项目 A1a-i
      - 四级项目 A1a-ii
    - 三级项目 A1b
  - 二级项目 A2
- 一级项目 B
  - 二级项目 B1
  - 二级项目 B2
    - 三级项目 B2a
    - 三级项目 B2b
      - 四级项目 B2b-i
)");


    
}

// ============================================================================
// 纯有序列表嵌套（全部 1. 开头，最多四级嵌套）
// ============================================================================

TEST(MarkdownHTMLTest, NestedOrderedList)
{
    std::string out = show("nested ordered list", R"(
1. 第一章
  1. 第一节
    1. 第一小节
      1. 第一点
      2. 第二点
    2. 第二小节
  2. 第二节
2. 第二章
  1. 第一节
  2. 第二节
    1. 第一小节
    2. 第二小节
      1. 第一点
)");


    
}

// ============================================================================
// 引用（Blockquote）— 基于 CommonMark 规范编写
// ============================================================================

/// @brief 基础单行引用
TEST(MarkdownHTMLTest, Blockquote)
{
    std::string out = show("blockquote", "> quoted text\n");
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "</blockquote>"));
    EXPECT_TRUE(has(out, "quoted text"));
}

/// @brief 多行引用：每行以 > 开头，属于同一个引用块
///        CommonMark 例 228: > # Foo\n> bar\n> baz
TEST(MarkdownHTMLTest, BlockquoteMultiLine)
{
    std::string out = show("blockquote multiline",
        "> 第一行引用内容\n"
        "> 第二行引用内容\n"
        "> 第三行引用内容\n");
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "</blockquote>"));
    EXPECT_TRUE(has(out, "第一行引用内容"));
    EXPECT_TRUE(has(out, "第二行引用内容"));
    EXPECT_TRUE(has(out, "第三行引用内容"));
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
    parser.finish();

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


TEST(MarkdownHTMLTest, AllElements)
{
    const char* sample = aMarkdownString();
    std::string markdown = sample;
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed(markdown);
    parser.finish();
    std::string out = html.output();
    ast_printf("%s", out.c_str());

    // ================================================================
    // 15. 转义字符 — 核心回归检测：
    //     反斜杠转义必须正常工作，否则后续所有元素都会被破坏
    // ================================================================

    // 15.1 转义后的字符应作为字面文本出现，而非触发 markdown 格式
    //     注意：所有转义行通过软换行合并到同一个 <p> 中
    EXPECT_TRUE(has(out, "以下字符可以通过反斜杠转义显示"));

    // 15.2 验证反斜杠转义有效 —— 被转义的字符必须出现在输出中
    //     （如果转义失败，* 会触发斜体，# 会触发标题，破坏整个文档结构）
    EXPECT_TRUE(has(out, "* 星号"))     << "escaped \\* should be literal *";
    EXPECT_TRUE(has(out, "` 反引号"))   << "escaped \\` should be literal `";
    EXPECT_TRUE(has(out, "_ 下划线"))   << "escaped \\_ should be literal _";
    EXPECT_TRUE(has(out, "{} 花括号"))  << "escaped \\{\\} should be literal {}";
    EXPECT_TRUE(has(out, "[] 方括号"))  << "escaped \\[\\] should be literal []";
    EXPECT_TRUE(has(out, "() 圆括号"))  << "escaped \\(\\) should be literal ()";
    EXPECT_TRUE(has(out, "# 井号"))     << "escaped \\# should be literal #";
    EXPECT_TRUE(has(out, "+ 加号"))     << "escaped \\+ should be literal +";
    EXPECT_TRUE(has(out, "- 减号"))     << "escaped \\- should be literal -";
    EXPECT_TRUE(has(out, ". 句点"))     << "escaped \\. should be literal .";
    EXPECT_TRUE(has(out, "! 感叹号"))   << "escaped \\! should be literal !";

    // 15.3 转义段不应产生新格式：通过在 15 和 16 之间查找来定位
    //     提取转义段所在段落，确认其中不含格式标签
    {
        size_t sec15End = out.find("16. 组合示例");
        EXPECT_NE(sec15End, std::string::npos);
        std::string escapedSection = out.substr(0, sec15End);

        // 转义段内不应出现由转义字符意外触发的格式标签
        // （斜体/粗体/链接/图片在文档其他位置合法，但在转义段内不应出现）
        size_t lastHr = escapedSection.rfind("<hr>");
        if (lastHr != std::string::npos)
        {
            std::string afterLastHr = escapedSection.substr(lastHr);
            EXPECT_FALSE(has(afterLastHr, "<em>"))
                << "escaped chars in section 15 should NOT trigger <em>";
            EXPECT_FALSE(has(afterLastHr, "<strong>"))
                << "escaped chars in section 15 should NOT trigger <strong>";
            // 注意：<code> 可能因其他原因出现，不与测试
        }
    }

    // ================================================================
    // 16. 组合示例 — 必须出现在转义段之后且结构完整
    // ================================================================

    // 16.1 标题必须包含内容，不能为空
    size_t pos16 = out.find(">16. 组合示例<");
    EXPECT_NE(pos16, std::string::npos)
        << "Section 16 heading '16. 组合示例' must exist with content";
    // 确保在 h2 标签内
    EXPECT_TRUE(has(out, "<h2>16. 组合示例</h2>"))
        << "Section 16 must be an <h2> heading with its content";

    // 16.2 引用块及其内部元素
    size_t posBq = out.find("<blockquote>", pos16);
    EXPECT_NE(posBq, std::string::npos)
        << "Section 16 must contain a <blockquote>";
    EXPECT_TRUE(has(out, "<strong>提示：</strong>"))
        << "bold '提示：' must be present in blockquote";
    EXPECT_TRUE(has(out, "<strong>轨道类型</strong>"))
        << "bold '轨道类型' must be present";
    EXPECT_TRUE(has(out, "<strong>覆盖范围</strong>"))
        << "bold '覆盖范围' must be present";
    EXPECT_TRUE(has(out, "<strong>通信链路</strong>"))
        << "bold '通信链路' must be present";
    EXPECT_TRUE(has(out, "<a href=\"https://example.com\">卫星轨道设计指南</a>"))
        << "link in blockquote must be intact";

    // 16.3 引用块内的代码块
    EXPECT_TRUE(has(out, "coverage_angle = np.arccos"))
        << "inline code in blockquote must be present";

    // ================================================================
    // 17. HTML 元素 — 原始 HTML 标签应被转义
    // ================================================================

    size_t pos17 = out.find(">17. HTML 元素<");
    EXPECT_NE(pos17, std::string::npos)
        << "Section 17 heading must exist with content";
    EXPECT_TRUE(pos17 > pos16)
        << "Section 17 must appear after section 16";
    EXPECT_TRUE(has(out, "<h2>17. HTML 元素</h2>"))
        << "Section 17 must be an <h2> heading";

    // 原始 HTML 标签应被转义
    EXPECT_TRUE(has(out, "&lt;p style="))
        << "raw <p> HTML tag should be escaped";
    EXPECT_TRUE(has(out, "&lt;/p&gt;"))
        << "raw </p> HTML tag should be escaped";
    EXPECT_TRUE(has(out, "&lt;details&gt;"))
        << "raw <details> tag should be escaped";
    EXPECT_TRUE(has(out, "&lt;summary&gt;"))
        << "raw <summary> tag should be escaped";

    // 折叠内容中的列表
    EXPECT_TRUE(has(out, "<li>卫星类型：通信卫星</li>"))
        << "list item in HTML section must exist";
    EXPECT_TRUE(has(out, "<li>轨道类型：地球同步轨道</li>"))
        << "list item in HTML section must exist";
    EXPECT_TRUE(has(out, "<li>覆盖区域：亚太地区</li>"))
        << "list item in HTML section must exist";

    // ================================================================
    // 18. 图表（Mermaid）— 代码块必须正常渲染
    // ================================================================

    size_t pos18 = out.find(">18. 图表");
    EXPECT_NE(pos18, std::string::npos)
        << "Section 18 heading must exist with content";
    EXPECT_TRUE(pos18 > pos17)
        << "Section 18 must appear after section 17";
    EXPECT_TRUE(has(out, "<h2>18. 图表（Mermaid）</h2>"))
        << "Section 18 must be an <h2> heading";

    EXPECT_TRUE(has(out, "<pre><code class=\"language-mermaid\">"))
        << "Mermaid code block must have language-mermaid class";
    EXPECT_TRUE(has(out, "graph TD"))
        << "Mermaid graph content must be present";
    EXPECT_TRUE(has(out, "A[任务需求分析]"))
        << "Mermaid node A must be present";
    EXPECT_TRUE(has(out, "H[在轨测试]"))
        << "Mermaid node H must be present";

    // ================================================================
    // 全局健康检查
    // ================================================================

    // 不应出现空标题（这是转义失败的关键症状：\# 被解析为标题）
    EXPECT_FALSE(has(out, "<h1></h1>"))
        << "Empty <h1> indicates escape failure";
    EXPECT_FALSE(has(out, "<h2></h2>"))
        << "Empty <h2> indicates escape failure";

    // 16-18 的标题不应为空（转义失败时 section 编号会被当作 markdown 格式化掉）
    // 已在上面用完整标签验证通过

    // 不应有裸露的 markdown 格式字符泄漏
    // （如 \` 反引号 如果未正确转义，` 会作为代码段分隔符被吞掉）
    // 此项通过上述正向断言间接覆盖
}

GTEST_MAIN()
