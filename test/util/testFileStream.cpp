#include "ast/AstTestMacro.h"
#include "ast/FileStream.hpp"
#include "ast/FileSystem.hpp"
#include <iterator>
#include <string>

AST_USING_NAMESPACE

/// @brief ast::ifstream/ast::ofstream 应以 UTF-8 编码解释文件路径
TEST(FileStream, utf8Path)
{
    // <temp>/ast_utf8_测试/星历.txt
    std::error_code ec;
    fs::path tmpdir = fs::temp_directory_path(ec);
    ASSERT_FALSE(ec);
    fs::path dir  = tmpdir / "ast_utf8_\xe6\xb5\x8b\xe8\xaf\x95";
    fs::path file = dir / "\xe6\x98\x9f\xe5\x8e\x86.txt";

    fs::remove_all(dir);
    ASSERT_TRUE(fs::create_directories(dir, ec));
    ASSERT_FALSE(ec);

    const std::string content = "hello \xe4\xb8\xad\xe6\x96\x87";   // hello 中文

    {
        cxx::ofstream out(file.string(), std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(out.is_open());
        out.write(content.data(), static_cast<std::streamsize>(content.size()));
        ASSERT_TRUE(out.good());
    }

    {
        cxx::ifstream in(file.string(), std::ios::binary);
        ASSERT_TRUE(in.is_open());
        const std::string read((std::istreambuf_iterator<char>(in)),
                                std::istreambuf_iterator<char>());
        EXPECT_EQ(read, content);
    }

    fs::remove_all(dir);
}

GTEST_MAIN();
