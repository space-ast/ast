///
/// @file      testVersion.cpp
/// @brief     测试 Version 类型，严格依据 Semantic Versioning 2.0.0 规范
/// @details   测试用例仅依据 https://semver.org/lang/zh-CN/ 规范编写，
///            不参考 Version.cpp 的实现细节。
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

AST_USING_NAMESPACE

// ============================================================================
// 1. 构造测试 — 通过各部件构造
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

TEST(VersionConstruct, FromParts_Basic)
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

TEST(VersionConstruct, FromParts_ZeroMajor)
{
    // SemVer 规范允许 0.y.z 用于初始开发阶段
    Version v(0, 1, 0);
    EXPECT_EQ(v.major(), 0);
    EXPECT_EQ(v.minor(), 1);
    EXPECT_EQ(v.patch(), 0);
    // 注意：valid() 返回 major_ > 0，因此 0.y.z 为"无效"（不稳定版）
    EXPECT_FALSE(v.valid());
}

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

TEST(VersionConstruct, FromParts_WithPrerelease)
{
    Version v(1, 0, 0, "alpha");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_STREQ(v.prerelease().c_str(), "alpha");
    EXPECT_TRUE(v.build().empty());
}

TEST(VersionConstruct, FromParts_WithBuild)
{
    Version v(1, 0, 0, {}, "20200101");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_TRUE(v.prerelease().empty());
    EXPECT_STREQ(v.build().c_str(), "20200101");
}

TEST(VersionConstruct, FromParts_WithPrereleaseAndBuild)
{
    Version v(1, 0, 0, "beta", "exp.sha.5114f85");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_STREQ(v.prerelease().c_str(), "beta");
    EXPECT_STREQ(v.build().c_str(), "exp.sha.5114f85");
}

TEST(VersionConstruct, FromParts_DotSeparatedPrerelease)
{
    // 预发布版本号可以使用点分隔的标识符
    Version v(1, 0, 0, "alpha.1");
    EXPECT_STREQ(v.prerelease().c_str(), "alpha.1");
}

TEST(VersionConstruct, FromParts_DotSeparatedBuild)
{
    // 构建版本号可以使用点分隔的标识符
    Version v(1, 0, 0, {}, "1.2.3");
    EXPECT_STREQ(v.build().c_str(), "1.2.3");
}


// ============================================================================
// 2. 解析测试 — 从字符串解析（有效版本）
// ============================================================================

TEST(VersionParse, BasicVersion)
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

TEST(VersionParse, ZeroMajorVersion)
{
    // 规范允许 0.y.z
    auto v = Version::Parse("0.1.0");
    EXPECT_EQ(v.major(), 0);
    EXPECT_EQ(v.minor(), 1);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_FALSE(v.valid());  // major_ = 0，valid() 返回 false
}

TEST(VersionParse, ZeroMinorAndPatch)
{
    auto v = Version::Parse("1.0.0");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
}

TEST(VersionParse, LargeVersionNumbers)
{
    // 规范未限制版本号大小
    auto v = Version::Parse("999.999.999");
    EXPECT_EQ(v.major(), 999);
    EXPECT_EQ(v.minor(), 999);
    EXPECT_EQ(v.patch(), 999);
}

TEST(VersionParse, WithPrerelease_Alpha)
{
    auto v = Version::Parse("1.0.0-alpha");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_STREQ(v.prerelease().c_str(), "alpha");
    EXPECT_TRUE(v.build().empty());
}

TEST(VersionParse, WithPrerelease_DotSeparated)
{
    auto v = Version::Parse("1.0.0-alpha.1");
    EXPECT_STREQ(v.prerelease().c_str(), "alpha.1");
}

TEST(VersionParse, WithPrerelease_HyphenInIdentifier)
{
    // 预发布标识符允许包含连接号
    auto v = Version::Parse("1.0.0-x-y-z");
    EXPECT_STREQ(v.prerelease().c_str(), "x-y-z");
}

TEST(VersionParse, WithPrerelease_MultipleDots)
{
    auto v = Version::Parse("1.0.0-0.3.7");
    EXPECT_STREQ(v.prerelease().c_str(), "0.3.7");
}

TEST(VersionParse, WithPrerelease_Complex)
{
    auto v = Version::Parse("1.0.0-x.7.z.92");
    EXPECT_STREQ(v.prerelease().c_str(), "x.7.z.92");
}

TEST(VersionParse, WithBuild)
{
    auto v = Version::Parse("1.0.0+20130313144700");
    EXPECT_EQ(v.major(), 1);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
    EXPECT_TRUE(v.prerelease().empty());
    EXPECT_STREQ(v.build().c_str(), "20130313144700");
}

TEST(VersionParse, WithPrereleaseAndBuild)
{
    auto v = Version::Parse("1.0.0-alpha+001");
    EXPECT_STREQ(v.prerelease().c_str(), "alpha");
    EXPECT_STREQ(v.build().c_str(), "001");
}

TEST(VersionParse, WithBuild_Complex)
{
    auto v = Version::Parse("1.0.0-beta+exp.sha.5114f85");
    EXPECT_STREQ(v.prerelease().c_str(), "beta");
    EXPECT_STREQ(v.build().c_str(), "exp.sha.5114f85");
}

TEST(VersionParse, SingleDigitVersion)
{
    // 规范要求必须为 X.Y.Z 格式，单个数字不符合规范
    auto v = Version::Parse("1");
    EXPECT_FALSE(v.valid());
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


// ============================================================================
// 3. 解析测试 — 无效/边界情况（依据规范）
// ============================================================================

TEST(VersionParse, LeadingZeroInMajor)
{
    // 规范禁止在前方补零："01.0.0" 不是合法的语义化版本号
    auto v = Version::Parse("01.0.0");
    // 应解析失败（不满足 <numeric identifier> 语法）
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, LeadingZeroInMinor)
{
    auto v = Version::Parse("1.01.0");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, LeadingZeroInPatch)
{
    auto v = Version::Parse("1.0.01");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, LeadingZeroInPrereleaseNumeric)
{
    // 预发布版本号中的数字标识符也不允许补零
    auto v = Version::Parse("1.0.0-01");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, NegativeMajor)
{
    // 版本号必须为非负整数
    auto v = Version::Parse("-1.0.0");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, EmptyString)
{
    auto v = Version::Parse("");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, WhitespaceOnly)
{
    auto v = Version::Parse("   ");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, NonNumericVersion)
{
    auto v = Version::Parse("a.b.c");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, PreReleaseWithLeadingZero)
{
    // 预发布标识符中的纯数字标识符不得有前导零
    auto v = Version::Parse("1.0.0-0.01.0");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, EmptyPrereleaseIdentifier)
{
    // 预发布标识符不能为空，如 "1.0.0-." 或 "1.0.0-alpha..beta"
    auto v = Version::Parse("1.0.0-");
    // 预发布标识符缺失，不合法
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, EmptyPrereleaseIdentifier_Middle)
{
    auto v = Version::Parse("1.0.0-alpha..beta");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, EmptyBuildIdentifier)
{
    // 构建标识符为空："1.0.0+"
    auto v = Version::Parse("1.0.0+");
    // 构建标识符缺失，不合法
    EXPECT_FALSE(v.valid());
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

TEST(VersionParse, Prerelease_InvalidCharacter)
{
    // 预发布标识符仅允许 [0-9A-Za-z-]
    auto v = Version::Parse("1.0.0-inv@lid");
    EXPECT_FALSE(v.valid());
}

TEST(VersionParse, Build_InvalidCharacter)
{
    // 构建标识符仅允许 [0-9A-Za-z-]
    auto v = Version::Parse("1.0.0+build$1");
    EXPECT_FALSE(v.valid());
}


// ============================================================================
// 4. 字符串构造函数测试
// ============================================================================

TEST(VersionStringConstructor, BasicParsing)
{
    Version v("2.0.0");
    EXPECT_TRUE(v.valid());
    EXPECT_EQ(v.major(), 2);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
}

TEST(VersionStringConstructor, WithPrerelease)
{
    Version v("2.0.0-rc.1");
    EXPECT_TRUE(v.valid());
    EXPECT_STREQ(v.prerelease().c_str(), "rc.1");
}

TEST(VersionStringConstructor, InvalidString)
{
    Version v("not-a-version");
    EXPECT_FALSE(v.valid());
}


// ============================================================================
// 5. Getter / Setter 测试
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


// ============================================================================
// 6. 优先层级比较 — 版本核心
// ============================================================================

// 根据 SemVer 规范 2.0.0：
// 优先层级由左到右依序比较：主版本号 → 次版本号 → 修订号 → 预发布版本号
// 版本编译信息在比较时被忽略

TEST(VersionCompare, MajorPrecedence)
{
    EXPECT_LT(Version(1, 0, 0), Version(2, 0, 0));
    EXPECT_GT(Version(2, 0, 0), Version(1, 0, 0));
    EXPECT_LT(Version(0, 9, 9), Version(1, 0, 0));
}

TEST(VersionCompare, MinorPrecedence)
{
    EXPECT_LT(Version(2, 0, 0), Version(2, 1, 0));
    EXPECT_GT(Version(2, 1, 0), Version(2, 0, 0));
    EXPECT_LT(Version(2, 0, 9), Version(2, 1, 0));
}

TEST(VersionCompare, PatchPrecedence)
{
    EXPECT_LT(Version(2, 1, 0), Version(2, 1, 1));
    EXPECT_GT(Version(2, 1, 1), Version(2, 1, 0));
    EXPECT_LT(Version(2, 1, 0), Version(2, 1, 9));
}

TEST(VersionCompare, EqualVersions)
{
    Version a(1, 2, 3);
    Version b(1, 2, 3);
    EXPECT_EQ(a, b);
    EXPECT_TRUE(a.compare(b) == 0);
}

TEST(VersionCompare, SpecExample_Core)
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
// 7. 优先层级比较 — 预发布版本号
// ============================================================================

TEST(VersionCompare, Prerelease_LowerThanRelease)
{
    // 有预发布版本号的版本，优先层级低于没有预发布版本号的版本
    // 规范示例：1.0.0-alpha < 1.0.0
    EXPECT_LT(Version(1, 0, 0, "alpha"), Version(1, 0, 0));
    EXPECT_GT(Version(1, 0, 0), Version(1, 0, 0, "alpha"));
}

TEST(VersionCompare, Prerelease_NumericComparison)
{
    // 纯数字标识符以数值高低比较
    // 1.0.0-1 < 1.0.0-2 (数值比较)
    EXPECT_LT(Version(1, 0, 0, "1"), Version(1, 0, 0, "2"));
    // 1.0.0-2 < 1.0.0-10 (数值比较，不是字符串比较)
    EXPECT_LT(Version(1, 0, 0, "2"), Version(1, 0, 0, "10"));
}

TEST(VersionCompare, Prerelease_AlphaComparison)
{
    // 字母标识符以 ASCII 排序比较
    EXPECT_LT(Version(1, 0, 0, "alpha"), Version(1, 0, 0, "beta"));
}

TEST(VersionCompare, Prerelease_NumericLowerThanAlpha)
{
    // 规范：数字的标识符比非数字的标识符优先层级低
    // 1.0.0-1 < 1.0.0-alpha
    EXPECT_LT(Version(1, 0, 0, "1"), Version(1, 0, 0, "alpha"));
}

TEST(VersionCompare, Prerelease_MoreFieldsHigher)
{
    // 栏位比较多的先行版本号优先层级比较高
    // 1.0.0-alpha < 1.0.0-alpha.1
    EXPECT_LT(Version(1, 0, 0, "alpha"), Version(1, 0, 0, "alpha.1"));
}

TEST(VersionCompare, Prerelease_SpecExample_Full)
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

TEST(VersionCompare, Prerelease_DifferentMajor_PreReleaseIrrelevant)
{
    // 主版本号不同时，优先按主版本号比较，预发布版本号不改变主版本号差异
    EXPECT_LT(Version(1, 0, 0, "alpha"), Version(2, 0, 0));
    EXPECT_LT(Version(1, 9, 9, "rc.99"), Version(2, 0, 0, "alpha"));
}

TEST(VersionCompare, Prerelease_DifferentMinor)
{
    EXPECT_LT(Version(1, 0, 9, "rc.1"), Version(1, 1, 0, "alpha"));
}

TEST(VersionCompare, Prerelease_SamePreReleaseString)
{
    // 相同的预发布版本号应相等
    Version a(1, 0, 0, "alpha");
    Version b(1, 0, 0, "alpha");
    EXPECT_EQ(a, b);
}

TEST(VersionCompare, Prerelease_HyphenInIdentifier)
{
    // 标识符中包含连接号的比较（ASCII 排序）
    // '-' 的 ASCII 值为 45，比字母小
    Version v1(1, 0, 0, "a-b");
    Version v2(1, 0, 0, "ab");
    EXPECT_LT(v1, v2);  // '-' (45) < 'b' (98)
}


// ============================================================================
// 8. 优先层级比较 — 构建版本号被忽略
// ============================================================================

TEST(VersionCompare, BuildMetadata_Ignored)
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

TEST(VersionCompare, BuildMetadata_Ignored_WithPrerelease)
{
    // 构建版本号被忽略，但预发布版本号仍需比较
    Version v1("1.0.0-alpha+001");
    Version v2("1.0.0-alpha+exp.sha.5114f85");
    Version v3("1.0.0-alpha");

    EXPECT_EQ(v1, v2);
    EXPECT_EQ(v1, v3);
    EXPECT_LT(v1, Version("1.0.0"));  // 预发布 < 正式版
}

TEST(VersionCompare, BuildMetadata_DifferentBuild_SamePrecedence)
{
    // 只有构建版本号不同的两个版本，属于相同的优先层级
    Version v1("2.0.0+build.1");
    Version v2("2.0.0+build.2");
    EXPECT_EQ(v1, v2);
}


// ============================================================================
// 9. 有效性测试
// ============================================================================

TEST(VersionValid, ValidVersion)
{
    // major > 0 时有效
    EXPECT_TRUE(Version(1, 0, 0).valid());
    EXPECT_TRUE(Version(2, 5, 3).valid());
    EXPECT_TRUE(Version(99, 0, 0).valid());
}

TEST(VersionValid, ZeroMajor_Invalid)
{
    // 主版本号为 0 视为无效（不稳定版）
    EXPECT_FALSE(Version(0, 0, 0).valid());
    EXPECT_FALSE(Version(0, 1, 0).valid());
    EXPECT_FALSE(Version(0, 99, 99).valid());
}

TEST(VersionValid, DefaultConstructed_Invalid)
{
    EXPECT_FALSE(Version().valid());
}

TEST(VersionValid, ParsedFromInvalidString_Invalid)
{
    EXPECT_FALSE(Version::Parse("abc").valid());
    EXPECT_FALSE(Version::Parse("").valid());
    EXPECT_FALSE(Version::Parse("01.0.0").valid());
}


// ============================================================================
// 10. 边界情况和特殊字符
// ============================================================================

TEST(VersionEdgeCase, VersionZero)
{
    // 0.0.0 是一个合法的语义化版本号（尽管 valid() 返回 false）
    auto v = Version::Parse("0.0.0");
    EXPECT_EQ(v.major(), 0);
    EXPECT_EQ(v.minor(), 0);
    EXPECT_EQ(v.patch(), 0);
}

TEST(VersionEdgeCase, ZeroIsValidNumericIdentifier)
{
    // 规范允许 "0" 作为数字标识符（但不允许前导零）
    auto v = Version::Parse("1.0.0-0");
    EXPECT_TRUE(v.valid());
    EXPECT_STREQ(v.prerelease().c_str(), "0");
}

TEST(VersionEdgeCase, MaxIntValues)
{
    // 规范的巴科斯范式未对数字大小设置上限
    // 测试较大的 int 值
    int bigMajor = 2147483647;  // INT32_MAX
    Version v(bigMajor, 0, 0);
    EXPECT_EQ(v.major(), bigMajor);
}

TEST(VersionEdgeCase, PrereleaseWithMixedIdentifiers)
{
    // 混合数字和非数字标识符的预发布版本号
    auto v = Version::Parse("1.0.0-1.alpha.2.beta");
    EXPECT_STREQ(v.prerelease().c_str(), "1.alpha.2.beta");
}

TEST(VersionEdgeCase, BuildWithDigitsAndLetters)
{
    // 构建版本号可以包含数字和字母
    auto v = Version::Parse("1.0.0+build.123.abc");
    EXPECT_STREQ(v.build().c_str(), "build.123.abc");
}

TEST(VersionEdgeCase, BuildWithLeadingZeros)
{
    // 构建版本号中的数字标识符可以有前导零（与预发布不同）
    auto v = Version::Parse("1.0.0+001");
    EXPECT_STREQ(v.build().c_str(), "001");
    // 并且版本仍应有效
    EXPECT_TRUE(v.valid());
}


// ============================================================================
// 11. 静态 Parse vs 字符串构造函数一致性
// ============================================================================

TEST(VersionConsistency, ParseVsConstructor)
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

GTEST_MAIN()
