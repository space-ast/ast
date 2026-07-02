///
/// @file      Version.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-30
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Version.hpp"
#include <cstdlib>
#include <cctype>
#include <climits>

AST_NAMESPACE_BEGIN

namespace
{
    /// @brief 判断字符是否为合法的 semver 标识符字符 [0-9A-Za-z-]
    inline bool isSemverChar(char c)
    {
        return (c >= '0' && c <= '9')
            || (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z')
            || c == '-';
    }

    /// @brief 解析一个非负整数（不允前导零），成功返回 true 并推进指针
    bool parseSemverInt(const char*& p, const char* end, int& value)
    {
        if (p >= end || !isdigit(static_cast<unsigned char>(*p)))
            return false;

        // 禁止前导零（"0" 本身除外）
        if (*p == '0' && p + 1 < end && isdigit(static_cast<unsigned char>(*(p + 1))))
            return false;

        char* next = nullptr;
        long val = strtol(p, &next, 10);
        if (next == p || val < 0 || val > INT_MAX)
            return false;

        value = static_cast<int>(val);
        p = next;
        return true;
    }

    /// @brief 解析一段预发布标识符，成功返回 true 并推进指针
    /// @note 预发布标识符：非空，仅含 [0-9A-Za-z-]；纯数字时禁止前导零
    ///       '.' 和 '+' 为预发布标识符的终止符
    bool parsePrereleaseId(const char*& p, const char* end, std::string& out)
    {
        if (p >= end)
            return false;

        const char* start = p;
        while (p < end && *p != '.' && *p != '+')
        {
            if (!isSemverChar(*p))
                return false;
            ++p;
        }

        size_t len = static_cast<size_t>(p - start);
        if (len == 0)
            return false;  // 标识符不能为空

        // 如果标识符是纯数字，禁止前导零
        if (*start == '0' && len > 1)
        {
            bool allDigits = true;
            for (const char* c = start; c < p; ++c)
            {
                if (!isdigit(static_cast<unsigned char>(*c)))
                {
                    allDigits = false;
                    break;
                }
            }
            if (allDigits)
                return false;  // 纯数字标识符有前导零
        }

        if (!out.empty())
            out += '.';
        out.append(start, len);
        return true;
    }

    /// @brief 解析一段构建标识符，成功返回 true 并推进指针
    /// @note 构建标识符：非空，仅含 [0-9A-Za-z-]（数字允许前导零）
    bool parseBuildId(const char*& p, const char* end, std::string& out)
    {
        if (p >= end)
            return false;

        const char* start = p;
        while (p < end && *p != '.')
        {
            if (!isSemverChar(*p))
                return false;
            ++p;
        }

        size_t len = static_cast<size_t>(p - start);
        if (len == 0)
            return false;  // 标识符不能为空

        if (!out.empty())
            out += '.';
        out.append(start, len);
        return true;
    }
}  // anonymous namespace

Version::Version(StringView version)
{
    *this = Parse(version);
}

Version::Version(int major, int minor, int patch, StringView prerelease, StringView build)
    : major_(major)
    , minor_(minor)
    , patch_(patch)
    , prerelease_(prerelease.data(), prerelease.size())
    , build_(build.data(), build.size())
{
}

Version Version::Parse(StringView version)
{
    Version result;
    result.patch_ = -1;  // 默认无修订号，解析到 .PATCH 时覆盖

    if (version.empty())
        return result;

    const char* p = version.data();
    const char* end = p + version.size();

    // 允许可选的 'v' / 'V' 前缀（如 "v1.2.3"）
    if (p < end && (*p == 'v' || *p == 'V'))
        ++p;

    // 解析 MAJOR.MINOR[.PATCH]
    if (!parseSemverInt(p, end, result.major_))
        return result;

    if (p >= end || *p != '.')
    {
        result.major_ = -1;
        return result;
    }
    ++p;

    if (!parseSemverInt(p, end, result.minor_))
    {
        result.major_ = -1;
        return result;
    }

    // 可选的 .PATCH（不存在时 patch_ 保持 -1，表示无修订号）
    if (p < end && *p == '.')
    {
        ++p;
        if (!parseSemverInt(p, end, result.patch_))
        {
            result.major_ = -1;
            return result;
        }
    }

    // 可选的 -预发布版本号
    if (p < end && *p == '-')
    {
        ++p;
        while (p < end && *p != '+')
        {
            if (!parsePrereleaseId(p, end, result.prerelease_))
            {
                result.major_ = -1;
                return result;
            }
            if (p < end && *p == '.')
                ++p;
        }
        // 检查 '-' 后面至少有一个有效标识符
        if (result.prerelease_.empty())
        {
            result.major_ = -1;
            return result;
        }
    }

    // 可选的 +构建版本号
    if (p < end && *p == '+')
    {
        ++p;
        while (p < end)
        {
            if (!parseBuildId(p, end, result.build_))
            {
                result.major_ = -1;
                return result;
            }
            if (p < end && *p == '.')
                ++p;
        }
        // 检查 '+' 后面至少有一个有效标识符
        if (result.build_.empty())
        {
            result.major_ = -1;
            return result;
        }
    }

    // 不允许尾部有未解析的字符
    if (p != end)
    {
        result.major_ = -1;
        return result;
    }

    return result;
}

int Version::compare(const Version& other) const
{
    // 逐级比较主版本号、次版本号、修订版本号
    if (major_ != other.major_)
        return major_ > other.major_ ? 1 : -1;

    if (minor_ != other.minor_)
        return minor_ > other.minor_ ? 1 : -1;

    if (patch_ != other.patch_)
        return patch_ > other.patch_ ? 1 : -1;

    // 预发布版本号优先级（依据 semver 2.0.0 §11）：
    // 无预发布版本号的版本 > 有预发布版本号的版本
    if (prerelease_.empty() && !other.prerelease_.empty())
        return 1;
    if (!prerelease_.empty() && other.prerelease_.empty())
        return -1;

    // 双方均有预发布版本号 — 逐段比较点号分隔的标识符
    if (!prerelease_.empty() && !other.prerelease_.empty())
    {
        const std::string& a = prerelease_;
        const std::string& b = other.prerelease_;

        size_t ai = 0, bi = 0;
        while (ai < a.size() || bi < b.size())
        {
            // 标识符段数少的一方优先级更低
            if (ai >= a.size())
                return -1;
            if (bi >= b.size())
                return 1;

            // 取出下一段标识符
            size_t aDot = a.find('.', ai);
            size_t bDot = b.find('.', bi);
            if (aDot == std::string::npos) aDot = a.size();
            if (bDot == std::string::npos) bDot = b.size();

            std::string aId = a.substr(ai, aDot - ai);
            std::string bId = b.substr(bi, bDot - bi);

            // 判断标识符是否为纯数字
            auto isNumeric = [](const std::string& s) -> bool {
                if (s.empty()) return false;
                for (size_t i = 0; i < s.size(); ++i)
                    if (!isdigit(static_cast<unsigned char>(s[i])))
                        return false;
                return true;
            };

            bool aNum = isNumeric(aId);
            bool bNum = isNumeric(bId);

            if (aNum && bNum)
            {
                // 同为数字标识符，按数值比较（通过 strtol 忽略前导零）
                long aVal = strtol(aId.c_str(), nullptr, 10);
                long bVal = strtol(bId.c_str(), nullptr, 10);
                if (aVal != bVal)
                    return aVal < bVal ? -1 : 1;
            }
            else if (aNum)
            {
                // 数字标识符优先级低于非数字标识符
                return -1;
            }
            else if (bNum)
            {
                return 1;
            }
            else
            {
                // 同为非数字标识符，按字典序（ASCII 排序）比较
                int cmp = aId.compare(bId);
                if (cmp != 0)
                    return cmp < 0 ? -1 : 1;
            }

            ai = aDot + 1;
            bi = bDot + 1;
        }
    }

    // 构建版本号不影响优先级（semver 2.0.0 §10）
    return 0;
}

AST_NAMESPACE_END
