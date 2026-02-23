#include "AstTest/AstTestMacro.h"
#include "AstUtil/IO.hpp"
#include "AstCore/RunTime.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

AST_USING_NAMESPACE

void testPrintf()
{
    printf("----------------------\n");
    wprintf(L"😊😀\n");
    printf(u8"😊😀\n");
    printf(u8"中文utf8\n");
    fprintf(stdout, u8"中文utf8\n");
    printf("中文gbk\n");
    printf(u8"محتوى عربي\n");
    posix::printf(u8"😊😀 ast\n");
    posix::printf(u8"中文utf8 ast\n");
    posix::fprintf(stdout, u8"中文utf8 ast\n");
    posix::printf(u8"محتوى عربي\n");
    printf("----------------------\n");
}

TEST(IO, printf)
{
    testPrintf();

    {
        setlocale(LC_ALL, ".UTF-8");
        auto locale = std::setlocale(LC_ALL, nullptr);
        printf("Locale name: %s\n", locale);
    }
    
    testPrintf();

    {
        // 在Windows平台上设置控制台和标准库编码为UTF-8
        #ifdef _WIN32
        // 设置控制台I/O编码为UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        #endif
    }

    testPrintf();
}

TEST(IO, getFilePath)
{
    std::string datadir = aDataDirGet();
    EXPECT_FALSE(datadir.empty());
    datadir += "/README.md";
    FILE* file = posix::fopen(datadir.c_str(), "r");
    EXPECT_TRUE(file != nullptr);

    std::string filepath;
    auto ret = aGetFilePath(file, filepath);
    printf("filepath: %s\n", filepath.c_str());
    EXPECT_EQ(ret, eNoError);
    EXPECT_FALSE(filepath.empty());
    FILE* file2 = posix::fopen(filepath.c_str(), "r");
    EXPECT_TRUE(file2 != nullptr);
}


GTEST_MAIN();

