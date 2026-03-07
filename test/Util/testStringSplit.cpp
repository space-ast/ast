///
/// @file      testStringSplit.cpp
/// @brief      
/// @details   
/// @author    axel
/// @date      2026-03-06
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

#include "AstUtil/StringSplit.hpp"
#include "AstTest/Test.h"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <array>
#include <deque>
#include <list>
#include <utility>

AST_USING_NAMESPACE

// 测试辅助函数
void ExpectElementsAre(const std::vector<StringView>& actual, const std::vector<std::string>& expected) {
    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        ASSERT_EQ(std::string(actual[i].data(), actual[i].size()), expected[i]);
    }
}

void ExpectElementsAre(const std::vector<std::string>& actual, const std::vector<std::string>& expected) {
    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        ASSERT_EQ(actual[i], expected[i]);
    }
}


// 测试边缘情况
TEST(StringSplit, EdgeCases) {
    {
        // 空字符串
        std::vector<std::string> v = aStrSplit("a,", ',');
        ExpectElementsAre(v, {"a", ""});
    }
    {
        // 空字符串
        std::vector<std::string> v = aStrSplit("", ',');
        ExpectElementsAre(v, {""});
    }

    {
        // 单个字符
        std::vector<std::string> v = aStrSplit("a", ',');
        ExpectElementsAre(v, {"a"});
    }

    {
        // 只有分隔符
        std::vector<std::string> v = aStrSplit(",", ',');
        ExpectElementsAre(v, {"", ""});
    }

    {
        // 连续分隔符
        std::vector<std::string> v = aStrSplit("a,,b", ',');
        ExpectElementsAre(v, {"a", "", "b"});
    }
}

// 测试API示例
TEST(StringSplit, APIExamples) {
    {
        // 开头和结尾的空字符串
        std::vector<std::string> v = aStrSplit((",a,b,c,"), ',');
        ExpectElementsAre(v, {"", "a", "b", "c", ""});
    }
    
    {
        // 使用字符分隔符
        std::vector<std::string> v = aStrSplit(("a,b,c"), ',');
        ExpectElementsAre(v, {"a", "b", "c"});

        // 使用字符串分隔符
        std::vector<std::string> v2 = aStrSplit(("a,b,c"), ",");
        ExpectElementsAre(v2, {"a", "b", "c"});
    }

    {
        // 使用字符串分隔符
        const std::vector<std::string> v = aStrSplit(("a=>b=>c"), "=>");
        ExpectElementsAre(v, {"a", "b", "c"});
    }

    {
        // 返回StringView
        std::vector<StringView> v = aStrSplit(("a,b,c"), ',');
        ExpectElementsAre(v, {"a", "b", "c"});
    }

    

    {
        // 未找到分隔符
        std::vector<std::string> v = aStrSplit(("abc"), ',');
        ExpectElementsAre(v, {"abc"});
    }

    {
        // 空字符串分隔符
        std::vector<std::string> v = aStrSplit(("abc"), "");
        ExpectElementsAre(v, {"a", "b", "c"});
    }

    {
        // 范围for循环
        std::string s = "x,x,x,x,x,x,x";
        int count = 0;
        for (StringView sp : aStrSplit(s, ',')) {
            ASSERT_EQ(std::string(sp.data(), sp.size()), "x");
            count++;
        }
        ASSERT_EQ(count, 7);
    }
}

// 测试分隔符类型
TEST(StringSplit, Delimiters) {
    {
        // ByChar
        std::vector<std::string> v = aStrSplit(("a,b,c"), ByChar(','));
        ExpectElementsAre(v, {"a", "b", "c"});
    }

    {
        // ByString
        std::vector<std::string> v = aStrSplit(("a,b,c"), ByString(","));
        ExpectElementsAre(v, {"a", "b", "c"});
    }

    {
        // ByAnyChar
        std::vector<std::string> v = aStrSplit(("a,b;c"), ByAnyChar(",;"));
        ExpectElementsAre(v, {"a", "b", "c"});
    }

    {
        // ByLength
        std::vector<std::string> v = aStrSplit(("abcdefg"), ByLength(3));
        ExpectElementsAre(v, {"abc", "def", "g"});
    }

    {
        // MaxSplits
        std::vector<std::string> v = aStrSplit(("a,b,c"), MaxSplits(',', 1));
        ExpectElementsAre(v, {"a", "b,c"});
    }
    {
        // ByRepeatedWhitespace
        std::vector<std::string> v = aStrSplit(("a b c"), ByRepeatedWhitespace());
        ExpectElementsAre(v, {"a", "b", "c"});
    }
    {
        // ByRepeatedWhitespace
        std::vector<std::string> v = aStrSplit(("a \n b c \r"), ByRepeatedWhitespace());
        ExpectElementsAre(v, {"a", "b", "c", ""});
    }
    {
        // ByRepeatedChar
        std::vector<std::string> v = aStrSplit(("a,,b"), ByRepeatedChar(','));
        ExpectElementsAre(v, {"a", "b"});
    }
}

// 测试谓词
TEST(StringSplit, Predicates) {
    const char kTestChars[] = ",a, ,b,";

    {
        // 无谓词
        auto splitter = aStrSplit((kTestChars), ',');
        std::vector<std::string> v = splitter;
        ExpectElementsAre(v, {"", "a", " ", "b", ""});
    }

    {
        // AllowEmpty
        auto splitter = aStrSplit((kTestChars), ',', AllowEmpty());
        std::vector<std::string> v = splitter;
        ExpectElementsAre(v, {"", "a", " ", "b", ""});
    }

    {
        // SkipEmpty
        auto splitter = aStrSplit((kTestChars), ',', SkipEmpty());
        std::vector<std::string> v = splitter;
        ExpectElementsAre(v, {"a", " ", "b"});
    }

    {
        // SkipWhitespace
        auto splitter = aStrSplit((kTestChars), ',', SkipWhitespace());
        std::vector<std::string> v = splitter;
        ExpectElementsAre(v, {"a", "b"});
    }
}

// 测试容器转换
TEST(StringSplit, ContainerConversion) {
    {
        // 转换为std::vector
        std::vector<std::string> v = aStrSplit("a,b,c", ',');
        ExpectElementsAre(v, {"a", "b", "c"});
    }

    {
        // 转换为std::set
        std::set<std::string> s = aStrSplit("a,b,c,a,b,c", ',');
        ASSERT_EQ(s.size(), 3);
        ASSERT_TRUE(s.find("a") != s.end());
        ASSERT_TRUE(s.find("b") != s.end());
        ASSERT_TRUE(s.find("c") != s.end());
    }

    
}


// 测试迭代器
TEST(StringSplit, Iterator) {
    auto splitter = aStrSplit("a,b,c", ',');
    auto it = splitter.begin();
    auto end = splitter.end();

    ASSERT_TRUE(it != end);
    ASSERT_EQ(std::string(it->data(), it->size()), "a");
    ++it;
    ASSERT_TRUE(it != end);
    ASSERT_EQ(std::string(it->data(), it->size()), "b");
    ++it;
    ASSERT_TRUE(it != end);
    ASSERT_EQ(std::string(it->data(), it->size()), "c");
    ++it;
    ASSERT_TRUE(it == end);
}

// 测试智能分割
TEST(StringSplit, SmartSplit) {
    std::map<std::string, std::string> m;
    for (StringView sp : aStrSplit("a=b=c,d=e,f=,g", ',')) {
        auto pair = aStrSplit(sp, MaxSplits('=', 1));
        auto it = pair.begin();
        if (it != pair.end()) {
            std::string key(it->data(), it->size());
            ++it;
            std::string value = it != pair.end() ? std::string(it->data(), it->size()) : "";
            m[key] = value;
        }
    }
    ASSERT_EQ(m.size(), 4);
    ASSERT_EQ(m["a"], "b=c");
    ASSERT_EQ(m["d"], "e");
    ASSERT_EQ(m["f"], "");
    ASSERT_EQ(m["g"], "");
}

GTEST_MAIN()