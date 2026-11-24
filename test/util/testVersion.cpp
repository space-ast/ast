///
/// @file      testVersion.cpp
/// @brief     测试 Version 类型
/// @details   SemVer.* 为对照 https://semver.org/lang/zh-CN/ 规范编写的用例；
///            其余用例覆盖规范未定义的实现契约
///            SemVer 超集行为（两段版本、v/V 前缀、无修订号）集中在文件末尾。
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

#include "ast/Version.hpp"
#include "ast/Test.h"
#include "AstConfig.h"

AST_USING_NAMESPACE

// ============================================================================
// 第一部分：SemVer 规范测试（TEST(SemVer, ...)）
// 仅依据 https://semver.org/lang/zh-CN/ 规范编写
// ============================================================================

// ============================================================================
// 1. 构造测试 — 通过各部件构造
// ============================================================================

TEST(SemVer, FromParts_Basic)
{
    // 根据 SemVer 规范，X.Y.Z 均为非负整数
    Version v(1, 2, 3);
    EXPECT_TRUE(v.valid());         // major > 0
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 2);
    EXPECT_EQ(v.patch(), 3);
    EXPECT_TRUE(v.prerelease().empty());
    EXPECT_TRUE(v.build().empty());
}

TEST(SemVer, FromParts_ZeroMajor)
{
    // SemVer 规范允许 0.y.z 用于初始开发阶段
    Version v(0, 1, 0);
    EXPECT_EQ(v.major(), 0);
    EXPECT_EQ(v.minor(), 1);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_TRUE(v.valid());
}

TEST(SemVer, FromParts_WithPrerelease)
{
    Version v(1, 0, 0, "alpha");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_STREQ(v.prerelease().c_str(), "alpha");
    EXPECT_TRUE(v.build().empty());
}

TEST(SemVer, FromParts_WithBuild)
{
    Version v(1, 0, 0, {}, "20200101");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_TRUE(v.prerelease().empty());
    EXPECT_STREQ(v.build().c_str(), "20200101");
}

TEST(SemVer, FromParts_WithPrereleaseAndBuild)
{
    Version v(1, 0, 0, "beta", "exp.sha.5114f85");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_STREQ(v.prerelease().c_str(), "beta");
    EXPECT_STREQ(v.build().c_str(), "exp.sha.5114f85");
}

TEST(SemVer, FromParts_DotSeparatedPrerelease)
{
    // 预发布版本号可以使用点分隔的标识符
    Version v(1, 0, 0, "alpha.1");
    EXPECT_STREQ(v.prerelease().c_str(), "alpha.1");
}

TEST(SemVer, FromParts_DotSeparatedBuild)
{
    // 构建版本号可以使用点分隔的标识符
    Version v(1, 0, 0, {}, "1.2.3");
    EXPECT_STREQ(v.build().c_str(), "1.2.3");
}


// ============================================================================
// 2. 解析测试 — 从字符串解析（有效版本）
// ============================================================================

TEST(SemVer, BasicVersion)
{
    // SemVer 核心格式：X.Y.Z，均为非负整数
    auto v = Version::Parse("1.2.3");
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 2);
    EXPECT_EQ(v.patch(), 3);
    EXPECT_TRUE(v.prerelease().empty());
    EXPECT_TRUE(v.build().empty());
}

TEST(SemVer, ZeroMajorVersion)
{
    // 规范允许 0.y.z
    auto v = Version::Parse("0.1.0");
    EXPECT_EQ(v.major(), 0);
    EXPECT_EQ(v.minor(), 1);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_TRUE(v.valid());  // major_ = 0，valid() 返回 true
}

TEST(SemVer, ZeroMinorAndPatch)
{
    auto v = Version::Parse("1.0.0");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
}

TEST(SemVer, LargeVersionNumbers)
{
    // 规范未限制版本号大小
    auto v = Version::Parse("999.999.999");
    EXPECT_EQ(v.major(), 999);
    EXPECT_EQ(v.minor(), 999);
    EXPECT_EQ(v.patch(), 999);
}

TEST(SemVer, WithPrerelease_Alpha)
{
    auto v = Version::Parse("1.0.0-alpha");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_STREQ(v.prerelease().c_str(), "alpha");
    EXPECT_TRUE(v.build().empty());
}

TEST(SemVer, WithPrerelease_DotSeparated)
{
    auto v = Version::Parse("1.0.0-alpha.1");
    EXPECT_STREQ(v.prerelease().c_str(), "alpha.1");
}

TEST(SemVer, WithPrerelease_HyphenInIdentifier)
{
    // 预发布标识符允许包含连接号
    auto v = Version::Parse("1.0.0-x-y-z");
    EXPECT_STREQ(v.prerelease().c_str(), "x-y-z");
}

TEST(SemVer, WithPrerelease_MultipleDots)
{
    auto v = Version::Parse("1.0.0-0.3.7");
    EXPECT_STREQ(v.prerelease().c_str(), "0.3.7");
}

TEST(SemVer, WithPrerelease_Complex)
{
    auto v = Version::Parse("1.0.0-x.7.z.92");
    EXPECT_STREQ(v.prerelease().c_str(), "x.7.z.92");
}

TEST(SemVer, WithBuild)
{
    auto v = Version::Parse("1.0.0+20130313144700");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_TRUE(v.prerelease().empty());
    EXPECT_STREQ(v.build().c_str(), "20130313144700");
}

TEST(SemVer, WithPrereleaseAndBuild)
{
    auto v = Version::Parse("1.0.0-alpha+001");
    EXPECT_STREQ(v.prerelease().c_str(), "alpha");
    EXPECT_STREQ(v.build().c_str(), "001");
}

TEST(SemVer, WithBuild_Complex)
{
    auto v = Version::Parse("1.0.0-beta+exp.sha.5114f85");
    EXPECT_STREQ(v.prerelease().c_str(), "beta");
    EXPECT_STREQ(v.build().c_str(), "exp.sha.5114f85");
}


// ============================================================================
// 3. 解析测试 — 无效/边界情况（依据规范）
// ============================================================================

TEST(SemVer, SingleDigitVersion)
{
    // 规范要求必须为 X.Y.Z 格式，单个数字不符合规范
    auto v = Version::Parse("1");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, LeadingZeroInMajor)
{
    // 规范禁止在前方补零："01.0.0" 不是合法的语义化版本号
    auto v = Version::Parse("01.0.0");
    // 应解析失败（不满足 <numeric identifier> 语法）
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, LeadingZeroInMinor)
{
    auto v = Version::Parse("1.01.0");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, LeadingZeroInPatch)
{
    auto v = Version::Parse("1.0.01");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, LeadingZeroInPrereleaseNumeric)
{
    // 预发布版本号中的数字标识符也不允许补零
    auto v = Version::Parse("1.0.0-01");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, NegativeMajor)
{
    // 版本号必须为非负整数
    auto v = Version::Parse("-1.0.0");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, EmptyString)
{
    auto v = Version::Parse("");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, WhitespaceOnly)
{
    auto v = Version::Parse("   ");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, NonNumericVersion)
{
    auto v = Version::Parse("a.b.c");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, PreReleaseWithLeadingZero)
{
    // 预发布标识符中的纯数字标识符不得有前导零
    auto v = Version::Parse("1.0.0-0.01.0");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, EmptyPrereleaseIdentifier)
{
    // 预发布标识符不能为空，如 "1.0.0-." 或 "1.0.0-alpha..beta"
    auto v = Version::Parse("1.0.0-");
    // 预发布标识符缺失，不合法
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, EmptyPrereleaseIdentifier_Middle)
{
    auto v = Version::Parse("1.0.0-alpha..beta");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, EmptyBuildIdentifier)
{
    // 构建标识符为空："1.0.0+"
    auto v = Version::Parse("1.0.0+");
    // 构建标识符缺失，不合法
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, EmptyPrereleaseIdentifier_TrailingDot)
{
    // 尾随点等价于空标识符："alpha." 中的 "." 后没有标识符
    auto v = Version::Parse("1.0.0-alpha.");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, EmptyBuildIdentifier_TrailingDot)
{
    auto v = Version::Parse("1.0.0+build.");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, EmptyPrereleaseIdentifier_BeforeBuild)
{
    // "." 后直接接 '+'，"alpha." 与构建版本号之间缺标识符
    auto v = Version::Parse("1.0.0-alpha.+b");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, Prerelease_InvalidCharacter)
{
    // 预发布标识符仅允许 [0-9A-Za-z-]
    auto v = Version::Parse("1.0.0-inv@lid");
    EXPECT_FALSE(v.valid());
}

TEST(SemVer, Build_InvalidCharacter)
{
    // 构建标识符仅允许 [0-9A-Za-z-]
    auto v = Version::Parse("1.0.0+build$1");
    EXPECT_FALSE(v.valid());
}


// ============================================================================
// 4. 字符串构造函数测试
// ============================================================================

TEST(SemVer, BasicParsing)
{
    Version v("2.0.0");
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(v.major(), 2);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
}

TEST(SemVer, WithPrerelease)
{
    Version v("2.0.0-rc.1");
    EXPECT_TRUE(v.valid());
    EXPECT_STREQ(v.prerelease().c_str(), "rc.1");
}

TEST(SemVer, InvalidString)
{
    Version v("not-a-version");
    EXPECT_FALSE(v.valid());
}


// ============================================================================
// 5. 优先层级比较 — 版本核心
// ============================================================================

// 根据 SemVer 规范 2.0.0：
// 优先层级由左到右依序比较：主版本号 → 次版本号 → 修订号 → 预发布版本号
// 版本编译信息在比较时被忽略

TEST(SemVer, MajorPrecedence)
{
    EXPECT_LT(Version(1, 0, 0), Version(2, 0, 0));
    EXPECT_GT(Version(2, 0, 0), Version(1, 0, 0));
    EXPECT_LT(Version(0, 9, 9), Version(1, 0, 0));
}

TEST(SemVer, MinorPrecedence)
{
    EXPECT_LT(Version(2, 0, 0), Version(2, 1, 0));
    EXPECT_GT(Version(2, 1, 0), Version(2, 0, 0));
    EXPECT_LT(Version(2, 0, 9), Version(2, 1, 0));
}

TEST(SemVer, PatchPrecedence)
{
    EXPECT_LT(Version(2, 1, 0), Version(2, 1, 1));
    EXPECT_GT(Version(2, 1, 1), Version(2, 1, 0));
    EXPECT_LT(Version(2, 1, 0), Version(2, 1, 9));
}

TEST(SemVer, EqualVersions)
{
    Version a(1, 2, 3);
    Version b(1, 2, 3);
    EXPECT_EQ(a, b);
    EXPECT_TRUE(a.compare(b) == 0);
}

TEST(SemVer, SpecExample_Core)
{
    // 规范示例：1.0.0 < 2.0.0 < 2.1.0 < 2.1.1
    Version v1("1.0.0");
    Version v2("2.0.0");
    Version v3("2.1.0");
    Version v4("2.1.1");

    EXPECT_LT(v1, v2);
    EXPECT_LT(v2, v3);
    EXPECT_LT(v3, v4);
    // 传递性
    EXPECT_LT(v1, v3);
    EXPECT_LT(v1, v4);
    EXPECT_LT(v2, v4);
}


// ============================================================================
// 6. 优先层级比较 — 预发布版本号
// ============================================================================

TEST(SemVer, Prerelease_LowerThanRelease)
{
    // 有预发布版本号的版本，优先层级低于没有预发布版本号的版本
    // 规范示例：1.0.0-alpha < 1.0.0
    EXPECT_LT(Version(1, 0, 0, "alpha"), Version(1, 0, 0));
    EXPECT_GT(Version(1, 0, 0), Version(1, 0, 0, "alpha"));
}

TEST(SemVer, Prerelease_NumericComparison)
{
    // 纯数字标识符以数值高低比较
    // 1.0.0-1 < 1.0.0-2 (数值比较)
    EXPECT_LT(Version(1, 0, 0, "1"), Version(1, 0, 0, "2"));
    // 1.0.0-2 < 1.0.0-10 (数值比较，不是字符串比较)
    EXPECT_LT(Version(1, 0, 0, "2"), Version(1, 0, 0, "10"));
}

TEST(SemVer, Prerelease_HugeNumericComparison)
{
    // 数字标识符按数值比较，不受 int/long 位宽限制
    Version small("1.0.0-99999999999999999999");     // 20 位
    Version large("1.0.0-100000000000000000000");    // 21 位
    EXPECT_LT(small, large);
    EXPECT_GT(large, small);
    EXPECT_EQ(small, Version("1.0.0-99999999999999999999"));
}

TEST(SemVer, Prerelease_AlphaComparison)
{
    // 字母标识符以 ASCII 排序比较
    EXPECT_LT(Version(1, 0, 0, "alpha"), Version(1, 0, 0, "beta"));
}

TEST(SemVer, Prerelease_NumericLowerThanAlpha)
{
    // 规范：数字的标识符比非数字的标识符优先层级低
    // 1.0.0-1 < 1.0.0-alpha
    EXPECT_LT(Version(1, 0, 0, "1"), Version(1, 0, 0, "alpha"));
}

TEST(SemVer, Prerelease_MoreFieldsHigher)
{
    // 栏位比较多的先行版本号优先层级比较高
    // 1.0.0-alpha < 1.0.0-alpha.1
    EXPECT_LT(Version(1, 0, 0, "alpha"), Version(1, 0, 0, "alpha.1"));
}

TEST(SemVer, Prerelease_SpecExample_Full)
{
    // 规范示例完整测试：
    // 1.0.0-alpha < 1.0.0-alpha.1 < 1.0.0-alpha.beta < 1.0.0-beta
    // < 1.0.0-beta.2 < 1.0.0-beta.11 < 1.0.0-rc.1 < 1.0.0
    Version v_alpha    ("1.0.0-alpha");
    Version v_alpha_1  ("1.0.0-alpha.1");
    Version v_alpha_b  ("1.0.0-alpha.beta");
    Version v_beta     ("1.0.0-beta");
    Version v_beta_2   ("1.0.0-beta.2");
    Version v_beta_11  ("1.0.0-beta.11");
    Version v_rc1      ("1.0.0-rc.1");
    Version v_release  ("1.0.0");

    EXPECT_LT(v_alpha, v_alpha_1);
    EXPECT_LT(v_alpha_1, v_alpha_b);
    EXPECT_LT(v_alpha_b, v_beta);
    EXPECT_LT(v_beta, v_beta_2);
    EXPECT_LT(v_beta_2, v_beta_11);
    EXPECT_LT(v_beta_11, v_rc1);
    EXPECT_LT(v_rc1, v_release);
}

TEST(SemVer, Prerelease_DifferentMajor_PreReleaseIrrelevant)
{
    // 主版本号不同时，优先按主版本号比较，预发布版本号不改变主版本号差异
    EXPECT_LT(Version(1, 0, 0, "alpha"), Version(2, 0, 0));
    EXPECT_LT(Version(1, 9, 9, "rc.99"), Version(2, 0, 0, "alpha"));
}

TEST(SemVer, Prerelease_DifferentMinor)
{
    EXPECT_LT(Version(1, 0, 9, "rc.1"), Version(1, 1, 0, "alpha"));
}

TEST(SemVer, Prerelease_SamePreReleaseString)
{
    // 相同的预发布版本号应相等
    Version a(1, 0, 0, "alpha");
    Version b(1, 0, 0, "alpha");
    EXPECT_EQ(a, b);
}

TEST(SemVer, Prerelease_HyphenInIdentifier)
{
    // 标识符中包含连接号的比较（ASCII 排序）
    // '-' 的 ASCII 值为 45，比字母小
    Version v1(1, 0, 0, "a-b");
    Version v2(1, 0, 0, "ab");
    EXPECT_LT(v1, v2);  // '-' (45) < 'b' (98)
}


// ============================================================================
// 7. 优先层级比较 — 构建版本号被忽略
// ============================================================================

TEST(SemVer, BuildMetadata_Ignored)
{
    // 规范：版本编译信息在判断优先层级时可被忽略
    // 1.0.0+001 == 1.0.0+20130313144700 == 1.0.0
    Version v1("1.0.0+001");
    Version v2("1.0.0+20130313144700");
    Version v3("1.0.0");

    EXPECT_EQ(v1, v2);  // 构建版本号不同，但应视为相同优先层级
    EXPECT_EQ(v1, v3);  // 有无构建版本号，应视为相同优先层级
    EXPECT_EQ(v2, v3);
}

TEST(SemVer, BuildMetadata_Ignored_WithPrerelease)
{
    // 构建版本号被忽略，但预发布版本号仍需比较
    Version v1("1.0.0-alpha+001");
    Version v2("1.0.0-alpha+exp.sha.5114f85");
    Version v3("1.0.0-alpha");

    EXPECT_EQ(v1, v2);
    EXPECT_EQ(v1, v3);
    EXPECT_LT(v1, Version("1.0.0"));  // 预发布 < 正式版
}

TEST(SemVer, BuildMetadata_DifferentBuild_SamePrecedence)
{
    // 只有构建版本号不同的两个版本，属于相同的优先层级
    Version v1("2.0.0+build.1");
    Version v2("2.0.0+build.2");
    EXPECT_EQ(v1, v2);
}


// ============================================================================
// 8. 有效性测试
// ============================================================================

TEST(SemVer, ValidVersion)
{
    // major > 0 时有效
    EXPECT_TRUE(Version(1, 0, 0).valid());
    EXPECT_TRUE(Version(2, 5, 3).valid());
    EXPECT_TRUE(Version(99, 0, 0).valid());
}

TEST(SemVer, ZeroMajor_valid)
{
    EXPECT_TRUE(Version(0, 0, 0).valid());
    EXPECT_TRUE(Version(0, 1, 0).valid());
    EXPECT_TRUE(Version(0, 99, 99).valid());
}

TEST(SemVer, ParsedFromInvalidString_Invalid)
{
    EXPECT_FALSE(Version::Parse("abc").valid());
    EXPECT_FALSE(Version::Parse("").valid());
    EXPECT_FALSE(Version::Parse("01.0.0").valid());
}

TEST(SemVer, NegativeMinor_Invalid)
{
    // 次版本号必须 >= 0：负的次版本号序列化后无法解析回等价版本，应视为无效
    Version v(1, -1);
    EXPECT_FALSE(v.valid());
    EXPECT_EQ(v.toString(), "");

    // 与 setMinor 的输入契约一致：设为负值后版本无效
    Version w(1, 2, 3);
    w.setMinor(-1);
    EXPECT_FALSE(w.valid());
    EXPECT_EQ(w.toString(), "");
}

TEST(SemVer, InvalidPrereleaseString_Invalid)
{
    // 构造/setter 传入非法预发布版本号时，版本应无效（toString 为空，无法往返）
    EXPECT_FALSE(Version(1, 0, 0, "01").valid());
    EXPECT_FALSE(Version(1, 0, 0, "a b").valid());
    EXPECT_FALSE(Version(1, 0, 0, "alpha..beta").valid());
    EXPECT_FALSE(Version(1, 0, 0, "alpha.").valid());

    Version v(1, 0, 0);
    v.setPrerelease("01");
    EXPECT_FALSE(v.valid());
    EXPECT_EQ(v.toString(), "");
}

TEST(SemVer, InvalidBuildString_Invalid)
{
    EXPECT_FALSE(Version(1, 0, 0, {}, "x+y").valid());
    EXPECT_FALSE(Version(1, 0, 0, {}, "build.").valid());
    EXPECT_FALSE(Version(1, 0, 0, {}, "a..b").valid());

    Version v(1, 0, 0);
    v.setBuild("x+y");
    EXPECT_FALSE(v.valid());
    EXPECT_EQ(v.toString(), "");
}


// ============================================================================
// 9. 边界情况和特殊字符
// ============================================================================

TEST(SemVer, VersionZero)
{
    // 0.0.0 是一个合法的语义化版本号（valid() 返回 true）
    auto v = Version::Parse("0.0.0");
    EXPECT_EQ(v.major(), 0);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
}

TEST(SemVer, ZeroIsValidNumericIdentifier)
{
    // 规范允许 "0" 作为数字标识符（但不允许前导零）
    auto v = Version::Parse("1.0.0-0");
    EXPECT_TRUE(v.valid());
    EXPECT_STREQ(v.prerelease().c_str(), "0");
}

TEST(SemVer, MaxIntValues)
{
    // 规范的巴科斯范式未对数字大小设置上限
    // 测试较大的 int 值
    int bigMajor = 2147483647;  // INT32_MAX
    Version v(bigMajor, 0, 0);
    EXPECT_EQ(v.major(), bigMajor);
}

TEST(SemVer, PrereleaseWithMixedIdentifiers)
{
    // 混合数字和非数字标识符的预发布版本号
    auto v = Version::Parse("1.0.0-1.alpha.2.beta");
    EXPECT_STREQ(v.prerelease().c_str(), "1.alpha.2.beta");
}

TEST(SemVer, BuildWithDigitsAndLetters)
{
    // 构建版本号可以包含数字和字母
    auto v = Version::Parse("1.0.0+build.123.abc");
    EXPECT_STREQ(v.build().c_str(), "build.123.abc");
}

TEST(SemVer, BuildWithLeadingZeros)
{
    // 构建版本号中的数字标识符可以有前导零（与预发布不同）
    auto v = Version::Parse("1.0.0+001");
    EXPECT_STREQ(v.build().c_str(), "001");
    // 并且版本仍应有效
    EXPECT_TRUE(v.valid());
}


// ============================================================================
// 10. 静态 Parse vs 字符串构造函数一致性
// ============================================================================

TEST(SemVer, ParseVsConstructor)
{
    // static Parse 和 explicit Version(StringView) 应产生相同结果
    auto v1 = Version::Parse("3.2.1-beta+sha.abc");
    Version v2("3.2.1-beta+sha.abc");

    EXPECT_EQ(v1.major(), v2.major());
    EXPECT_EQ(v1.minor(), v2.minor());
    EXPECT_EQ(v1.patch(), v2.patch());
    EXPECT_STREQ(v1.prerelease().c_str(), v2.prerelease().c_str());
    EXPECT_STREQ(v1.build().c_str(), v2.build().c_str());
    EXPECT_EQ(v1, v2);
}


// ============================================================================
// 11. 字符串表示 — toString()
// ============================================================================

TEST(SemVer, BasicCoreFormat)
{
    // 规范格式：MAJOR.MINOR.PATCH
    EXPECT_EQ(Version(1, 2, 3).toString(), "1.2.3");
    EXPECT_EQ(Version(0, 0, 0).toString(), "0.0.0");
    EXPECT_EQ(Version(0, 1, 0).toString(), "0.1.0");
    EXPECT_EQ(Version(10, 20, 30).toString(), "10.20.30");
    EXPECT_EQ(Version(999, 999, 999).toString(), "999.999.999");
}

TEST(SemVer, ToStringWithPrerelease)
{
    EXPECT_EQ(Version(1, 0, 0, "alpha").toString(), "1.0.0-alpha");
    EXPECT_EQ(Version(1, 0, 0, "rc.1").toString(), "1.0.0-rc.1");
    EXPECT_EQ(Version(1, 0, 0, "x.7.z.92").toString(), "1.0.0-x.7.z.92");
}

TEST(SemVer, ToStringWithBuild)
{
    EXPECT_EQ(Version(1, 0, 0, {}, "20200101").toString(), "1.0.0+20200101");
    EXPECT_EQ(Version(1, 0, 0, {}, "exp.sha.5114f85").toString(), "1.0.0+exp.sha.5114f85");
    EXPECT_EQ(Version(1, 0, 0, {}, "001").toString(), "1.0.0+001");
}

TEST(SemVer, ToStringWithPrereleaseAndBuild)
{
    EXPECT_EQ(Version(1, 0, 0, "alpha", "001").toString(), "1.0.0-alpha+001");
    EXPECT_EQ(Version(1, 0, 0, "beta", "exp.sha.5114f85").toString(), "1.0.0-beta+exp.sha.5114f85");
}

TEST(SemVer, FromSetters)
{
    // 通过 setter 逐字段构造的版本也应正确输出
    Version v;
    v.setMajor(5);
    v.setMinor(4);
    v.setPatch(3);
    v.setPrerelease("rc");
    v.setBuild("20200101");
    EXPECT_EQ(v.toString(), "5.4.3-rc+20200101");
}

TEST(SemVer, ParseRoundTrip)
{
    // 合法版本号经 Parse → toString 应原样还原
    const char* const cases[] = {
        "0.0.0",
        "1.2.3",
        "10.20.30",
        "999.999.999",
        "1.0.0-alpha",
        "1.0.0-alpha.1",
        "1.0.0-0.3.7",
        "1.0.0-x.7.z.92",
        "1.0.0-x-y-z.-",
        "1.0.0+20130313144700",
        "1.0.0+001",
        "1.0.0-beta+exp.sha.5114f85",
        "1.0.0-alpha+001",
    };

    for (const char* text : cases)
    {
        Version v = Version::Parse(text);
        ASSERT_TRUE(v.valid()) << "failed to parse: " << text;
        EXPECT_EQ(v.toString(), text) << "round trip: " << text;
    }
}

TEST(SemVer, OutputIsReparsable)
{
    // toString 的输出应能被 Parse 重新解析为等价的版本
    Version original(2, 5, 3, "rc.2", "build.1");
    Version reparsed(original.toString());

    EXPECT_TRUE(reparsed.valid());
    EXPECT_EQ(reparsed.major(), original.major());
    EXPECT_EQ(reparsed.minor(), original.minor());
    EXPECT_EQ(reparsed.patch(), original.patch());
    EXPECT_STREQ(reparsed.prerelease().c_str(), original.prerelease().c_str());
    EXPECT_STREQ(reparsed.build().c_str(), original.build().c_str());
}


// ============================================================================
// 第二部分：实现契约测试（规范未定义，不属于 SemVer 判定）
// ============================================================================

// ============================================================================
// 12. 默认构造与无效版本的表示
// ============================================================================

TEST(VersionConstruct, DefaultConstructor)
{
    Version v;
    // 默认构造：major_ = -1，应为无效版本
    EXPECT_FALSE(v.valid());
    EXPECT_EQ(v.major(), -1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_TRUE(v.prerelease().empty());
    EXPECT_TRUE(v.build().empty());
}

TEST(VersionValid, DefaultConstructed_Invalid)
{
    EXPECT_FALSE(Version().valid());
}

TEST(VersionToString, InvalidVersionReturnsEmpty)
{
    // 无效版本没有字符串表示
    EXPECT_EQ(Version().toString(), "");
    EXPECT_EQ(Version::Parse("").toString(), "");
    EXPECT_EQ(Version::Parse("not-a-version").toString(), "");
    EXPECT_EQ(Version::Parse("01.0.0").toString(), "");
}


// ============================================================================
// 13. Getter / Setter 测试
// ============================================================================

TEST(VersionGetterSetter, Major)
{
    Version v;
    v.setMajor(3);
    EXPECT_EQ(v.major(), 3);
}

TEST(VersionGetterSetter, Minor)
{
    Version v;
    v.setMinor(7);
    EXPECT_EQ(v.minor(), 7);
}

TEST(VersionGetterSetter, Patch)
{
    Version v;
    v.setPatch(15);
    EXPECT_EQ(v.patch(), 15);
}

TEST(VersionGetterSetter, Prerelease)
{
    Version v;
    v.setPrerelease("beta.2");
    EXPECT_STREQ(v.prerelease().c_str(), "beta.2");
}

TEST(VersionGetterSetter, Build)
{
    Version v;
    v.setBuild("sha.abc123");
    EXPECT_STREQ(v.build().c_str(), "sha.abc123");
}

TEST(VersionGetterSetter, ChainSetters)
{
    Version v;
    v.setMajor(5);
    v.setMinor(4);
    v.setPatch(3);
    v.setPrerelease("rc");
    v.setBuild("20200101");

    EXPECT_EQ(v.major(), 5);
    EXPECT_EQ(v.minor(), 4);
    EXPECT_EQ(v.patch(), 3);
    EXPECT_STREQ(v.prerelease().c_str(), "rc");
    EXPECT_STREQ(v.build().c_str(), "20200101");
}

TEST(VersionGetterSetter, ValidityUpdatesAfterMutation)
{
    // 字符串合法性缓存由 setter 就地刷新：把非法值改回合法后，版本应重新有效
    Version v(1, 0, 0);
    EXPECT_TRUE(v.valid());

    v.setMinor(-1);
    EXPECT_FALSE(v.valid());
    v.setMinor(0);
    EXPECT_TRUE(v.valid());

    v.setPrerelease("01");
    EXPECT_FALSE(v.valid());
    v.setPrerelease("alpha");
    EXPECT_TRUE(v.valid());

    v.setBuild("a..b");
    EXPECT_FALSE(v.valid());
    v.setMajor(2);  // 只改主版本号，不应掩盖构建版本号非法
    EXPECT_FALSE(v.valid());
    v.setBuild("a.b");
    EXPECT_TRUE(v.valid());
}


// ============================================================================
// 14. 工程版本号 — aVersion() / aVersionString()
// ============================================================================

TEST(ProjectVersion, StringMatchesCompileTimeMacro)
{
    // aVersionStr() 原样返回编译期宏 AST_VERSION
    EXPECT_STREQ(aVersionStr(), AST_VERSION);
}

TEST(ProjectVersion, MatchesCompileTimeMacro)
{
    // aVersion() 与 AST_VERSION 的解析结果应逐字段一致
    Version expected = Version::Parse(AST_VERSION);
    const Version& actual = aVersion();

    EXPECT_EQ(actual.major(), expected.major());
    EXPECT_EQ(actual.minor(), expected.minor());
    EXPECT_EQ(actual.patch(), expected.patch());
    EXPECT_STREQ(actual.prerelease().c_str(), expected.prerelease().c_str());
    EXPECT_STREQ(actual.build().c_str(), expected.build().c_str());
    EXPECT_EQ(actual, expected);
}

TEST(ProjectVersion, StringAndObjectAgree)
{
    // 文本与对象两条通道不得互相矛盾
    EXPECT_EQ(Version::Parse(aVersionStr()), aVersion());
}

TEST(ProjectVersion, ReturnsReferenceToStableObject)
{
    // 返回的是同一个静态对象，多次调用不应各自解析
    EXPECT_EQ(&aVersion(), &aVersion());
}


// ============================================================================
// 第三部分：SemVer 超集行为（本实现的扩展，规范未定义）
// 两段版本 MAJOR.MINOR、可选 v/V 前缀、patch < 0 表示无修订号
// ============================================================================

TEST(VersionConstruct, FromParts_NoPatch)
{
    // patch < 0 时不包含修订版本号，patch_ 被设为 -1
    // 此时此版本与 patch=0 的版本比较会因数值差异产生特殊排序
    Version v(1, 0, -1);
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), -1);
    // patch=-1 意味着无修订号，其数值比较低于 patch=0
    EXPECT_LT(v, Version(1, 0, 0));
}

TEST(VersionConstruct, NegativePatchMeansNoPatch)
{
    // patch < 0 表示无修订号：toString() 不输出修订号；
    // compare() 比较时与 Parse() 回来的版本相等
    Version v(1, 2, -5);
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(v.toString(), "1.2");
    EXPECT_EQ(Version::Parse(v.toString()), v);

    Version w(1, 2, 3);
    w.setPatch(-2);
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(w, Version(1, 2));
}

TEST(VersionParse, TwoPartVersion)
{
    // "1.2" — MAJOR.MINOR 格式，无修订号
    auto v = Version::Parse("1.2");
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 2);
    EXPECT_EQ(v.patch(), -1);  // 无修订号
}

TEST(VersionParse, VPrefix_Basic)
{
    // "v1.2.3" — 支持可选的 v/V 前缀
    auto v = Version::Parse("v1.2.3");
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 2);
    EXPECT_EQ(v.patch(), 3);
}

TEST(VersionParse, VPrefix_UpperCase)
{
    auto v = Version::Parse("V2.0.0");
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(v.major(), 2);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
}

TEST(VersionParse, VPrefix_WithPrerelease)
{
    auto v = Version::Parse("v1.0.0-rc.1");
    EXPECT_TRUE(v.valid());
    EXPECT_STREQ(v.prerelease().c_str(), "rc.1");
}

TEST(VersionParse, VPrefix_TwoPart)
{
    // v1.2 — 无修订号，也支持 v 前缀
    auto v = Version::Parse("v1.2");
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 2);
    EXPECT_EQ(v.patch(), -1);
}

TEST(VersionToString, NoPatchOmitsPatchField)
{
    // patch < 0 表示无修订号，字符串中只输出 MAJOR.MINOR
    EXPECT_EQ(Version(1, 2, -1).toString(), "1.2");
    EXPECT_EQ(Version(0, 0, -1).toString(), "0.0");
}

TEST(VersionToString, ParseTwoPartRoundTrip)
{
    // MAJOR.MINOR 形式（无修订号）也应原样还原
    EXPECT_EQ(Version::Parse("1.2").toString(), "1.2");
}

TEST(VersionToString, ParseNormalizesVPrefix)
{
    // v/V 前缀不属于版本号本身，toString 不输出
    EXPECT_EQ(Version::Parse("v1.2.3").toString(), "1.2.3");
    EXPECT_EQ(Version::Parse("V2.0.0").toString(), "2.0.0");
    EXPECT_EQ(Version::Parse("v1.0.0-rc.1").toString(), "1.0.0-rc.1");
}

GTEST_MAIN()
