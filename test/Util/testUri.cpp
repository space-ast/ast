/// @file      testUri.cpp
/// @brief     uriopen / aUriFetch 单元测试
/// @details   覆盖普通路径透传、盘符不会被误判成协议、file:// URI（含 localhost 与 percent 解码）、
///            不支持的协议、空输入与打开模式校验、缓存文件夹覆盖，以及一个联网用例
///            （验证 http 下载与持久缓存命中）。
/// @author    axel
/// @date      2026-09-28
/// @copyright 版权所有 (C) 2026-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/AstTestMacro.h"
#include "ast/FileSystem.hpp"
#include "ast/IO.hpp"
#include "ast/RunTime.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>

AST_USING_NAMESPACE

namespace{

const char* const kHttpTestUrl = "https://postman-echo.com/get";
const char* const kCacheDirEnv = AST_ENV_CACHE_DIR;

/// @brief 生成 file:// 形式的绝对路径URI（Windows / Linux 通用）
std::string makeFileUri(const std::string& absPath)
{
    std::string filepath = absPath;
    for (char& c : filepath)
    {
        if (c == '\\')
            c = '/';
    }
    if (filepath.empty() || filepath[0] != '/')
        filepath = "/" + filepath;      // Windows: "E:/x" → "/E:/x"
    return "file://" + filepath;
}

/// @brief 当前工作目录下的绝对路径
std::string absPathInCwd(const std::string& name)
{
    std::error_code ec;
    return fs::absolute(name, ec).string();
}

std::string readAll(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::string readAll(std::FILE* file)
{
    std::string result;
    char buffer[256];
    size_t count = 0;
    while ((count = fread(buffer, 1, sizeof(buffer), file)) > 0)
        result.append(buffer, count);
    return result;
}

void writeAll(const std::string& filepath, const std::string& text)
{
    std::ofstream file(filepath, std::ios::binary | std::ios::trunc);
    file << text;
}

bool isFileEmpty(const std::string& filepath)
{
    std::error_code ec;
    return fs::is_empty(filepath, ec);
}

/// @brief 设置环境变量，返回原来的值
std::string setEnv(const char* name, const char* value)
{
    const char* oldValue = getenv(name);
    std::string saved = oldValue ? oldValue : "";
#ifdef _WIN32
    _putenv_s(name, value ? value : "");
#else
    if (value)
        setenv(name, value, 1);
    else
        unsetenv(name);
#endif
    return saved;
}

void restoreEnv(const char* name, const std::string& value)
{
    if (value.empty())
        setEnv(name, nullptr);
    else
        setEnv(name, value.c_str());
}

}   // namespace


TEST(Uri, PlainPath)
{
    // 普通路径原样透传，不检查文件是否存在
    std::string filepath = "junk";
    errc_t rc = aUriFetch(StringView("UriTest_missing.txt"), filepath);
    EXPECT_EQ(rc, eNoError);
    EXPECT_STREQ(filepath.c_str(), "UriTest_missing.txt");

    // 文件不存在时打不开，并设置 errno
    errno = 0;
    std::FILE* file = uriopen("UriTest_missing.txt", "r");
    EXPECT_TRUE(file == nullptr);
    EXPECT_TRUE(errno != 0);

    // 文件存在时能打开且内容一致
    writeAll("UriTest_plain.txt", "plain-content");
    file = uriopen("UriTest_plain.txt", "rb");
    EXPECT_TRUE(file != nullptr);
    if (file)
    {
        EXPECT_STREQ(readAll(file).c_str(), "plain-content");
        fclose(file);
    }
    fs::remove("UriTest_plain.txt");
}

TEST(Uri, DriveLetterIsNotScheme)
{
    // Windows 盘符不是协议名，路径要原样透传
    std::string filepath;
    EXPECT_EQ(aUriFetch(StringView("C:\\Users\\nobody\\data.txt"), filepath), eNoError);
    EXPECT_STREQ(filepath.c_str(), "C:\\Users\\nobody\\data.txt");

    EXPECT_EQ(aUriFetch(StringView("E:/tmp/a.txt"), filepath), eNoError);
    EXPECT_STREQ(filepath.c_str(), "E:/tmp/a.txt");
}

TEST(Uri, FileUriAbsolute)
{
    const std::string path = absPathInCwd("UriTest_file.txt");
    writeAll(path, "file-uri-content");
    const std::string uri = makeFileUri(path);

    std::string filepath;
    EXPECT_EQ(aUriFetch(StringView(uri), filepath), eNoError);
    EXPECT_TRUE(filepath != uri);
    EXPECT_TRUE(fs::exists(filepath));
    EXPECT_STREQ(readAll(filepath).c_str(), "file-uri-content");

    std::FILE* file = uriopen(uri.c_str(), "r");
    EXPECT_TRUE(file != nullptr);
    if (file)
        fclose(file);

    file = uriopen(uri.c_str(), "rb");
    EXPECT_TRUE(file != nullptr);
    if (file)
        fclose(file);

    fs::remove(path);
}

TEST(Uri, FileUriLocalhostAndPercentDecode)
{
    // 解析结果里不应残留 percent 转义
    std::string filepath;
    EXPECT_EQ(aUriFetch(StringView("file:///UriTest%20dir/x.txt"), filepath), eNoError);
    EXPECT_TRUE(filepath.find('%') == std::string::npos);
    EXPECT_TRUE(filepath.find("UriTest dir") != std::string::npos);

    // 带空格的文件名 + localhost 主机名
    const std::string path = absPathInCwd("UriTest space.txt");
    writeAll(path, "space-content");

    std::string uri = makeFileUri(path);                    // "file:///..."
    uri = "file://localhost" + uri.substr(7);               // 插到 "file://" 之后
    for (size_t pos = 0; (pos = uri.find(' ', pos)) != std::string::npos; pos += 3)
        uri.replace(pos, 1, "%20");

    EXPECT_EQ(aUriFetch(StringView(uri), filepath), eNoError);
    EXPECT_TRUE(fs::exists(filepath));
    EXPECT_STREQ(readAll(filepath).c_str(), "space-content");

    fs::remove(path);
}

TEST(Uri, UnsupportedOrMalformed)
{
    std::string filepath = "junk";
    EXPECT_EQ(aUriFetch(StringView("ftp://example.com/a.txt"), filepath), eErrorUnsupported);
    EXPECT_TRUE(filepath.empty());

    EXPECT_EQ(aUriFetch(StringView("ws://host/socket"), filepath), eErrorUnsupported);
    EXPECT_EQ(aUriFetch(StringView("file://example.com/etc/hosts"), filepath), eErrorUnsupported);
    EXPECT_EQ(aUriFetch(StringView("file://"), filepath), eErrorInvalidParam);
    EXPECT_EQ(aUriFetch(StringView("file:"), filepath), eErrorInvalidParam);
}

TEST(Uri, InvalidInput)
{
    std::string filepath = "junk";
    EXPECT_NE(aUriFetch(StringView(), filepath), eNoError);
    EXPECT_TRUE(filepath.empty());

    EXPECT_EQ(aUriFetch(StringView(""), filepath), eErrorInvalidParam);
    EXPECT_EQ(aUriFetch(StringView("   "), filepath), eErrorInvalidParam);
    EXPECT_TRUE(filepath.empty());
}

TEST(Uri, OpenMode)
{
    // 网络文件只支持只读模式（纯本地判断，不会发起下载）
    errno = 0;
    EXPECT_TRUE(uriopen("https://example.com/a.txt", "w") == nullptr);
    EXPECT_TRUE(errno != 0);
    EXPECT_TRUE(uriopen("https://example.com/a.txt", "wb") == nullptr);
    EXPECT_TRUE(uriopen("https://example.com/a.txt", "r+") == nullptr);
    EXPECT_TRUE(uriopen("https://example.com/a.txt", nullptr) == nullptr);
    EXPECT_TRUE(uriopen(nullptr, "r") == nullptr);

    // 本地文件与 fopen 一致，写模式可用
    const std::string path = absPathInCwd("UriTest_mode.txt");
    const std::string uri = makeFileUri(path);

    std::FILE* file = uriopen(uri.c_str(), "w");
    EXPECT_TRUE(file != nullptr);
    if (file)
    {
        fputs("mode-content", file);
        fclose(file);
    }

    file = uriopen(uri.c_str(), "r");
    EXPECT_TRUE(file != nullptr);
    if (file)
    {
        EXPECT_STREQ(readAll(file).c_str(), "mode-content");
        fclose(file);
    }

    fs::remove(path);
}

TEST(Uri, CacheDirOverride)
{
    const std::string saved = setEnv(kCacheDirEnv, "UriTest_cache_dir");
    {
        const std::string cachedir = aCacheDir();
        EXPECT_FALSE(cachedir.empty());
        EXPECT_TRUE(cachedir.find("UriTest_cache_dir") != std::string::npos);
    }
    restoreEnv(kCacheDirEnv, saved);
}

TEST(Uri, HttpFetchAndCache)
{
    const std::string saved = setEnv(kCacheDirEnv, "UriTest_http_cache");
    fs::remove_all("UriTest_http_cache");
    {
        const std::string url = std::string(kHttpTestUrl) + "?uri_test=1";
        std::string first;
        errc_t rc = aUriFetch(StringView(url), first);
        EXPECT_EQ(rc, eNoError);
        EXPECT_TRUE(fs::exists(first));
        EXPECT_FALSE(isFileEmpty(first));
        printf("cached at: %s\n", first.c_str());

        // 覆写成哨兵值后再次获取：命中缓存，不应重新下载
        writeAll(first, "sentinel");
        std::string second;
        EXPECT_EQ(aUriFetch(StringView(url), second), eNoError);
        EXPECT_STREQ(second.c_str(), first.c_str());
        EXPECT_STREQ(readAll(second).c_str(), "sentinel");

        // query 参与缓存键：不同的 query 是不同的缓存文件
        const std::string otherUrl = std::string(kHttpTestUrl) + "?uri_test=2";
        std::string other;
        EXPECT_EQ(aUriFetch(StringView(otherUrl), other), eNoError);
        EXPECT_TRUE(other != first);
    }
    fs::remove_all("UriTest_http_cache");
    restoreEnv(kCacheDirEnv, saved);
}


GTEST_MAIN();
