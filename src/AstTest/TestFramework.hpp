///
/// @file      TestFramework.hpp
/// @brief     轻量的、头文件式、兼容 GoogleTest 的测试框架。
/// @details   提供单元测试所依赖的 GoogleTest 接口子集，让 test/ 目录下的
///            测试可以编译运行，而不必依赖外部的 gtest 库：
///              - TEST() / TEST_F() 测试与夹具注册
///              - EXPECT_* / ASSERT_* 断言宏（支持 "<<" 流式输出）
///              - GTEST_SKIP()、GTEST_MAIN()、RUN_ALL_TESTS()
///              - testing::Test、testing::InitGoogleTest()、testing::GTEST_FLAG()
///
///            本框架有意保持头文件式且使用 C++11：每个测试源码文件由构建系统
///            单独编译为一个可执行文件，因此测试注册表和运行器可以完全放在
///            此头文件中，无需额外的库。
///
/// @author    axel
/// @date      2026-09-01
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目。
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

#pragma once

// 兼容 GoogleTest 的传递包含：以前很多测试在未显式 include 的情况下使用了这些
// 标准库头（例如 std::set<char*>）。内置框架在 gtest/gtest.h 处被包含，因此需
// 提供等价的标准库头，以免测试因其先前依赖 gtest 的传递包含而编译失败。
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <utility>
#include <memory>
#include <map>
#include <set>
#include <deque>
#include <list>
#include <tuple>
#include <algorithm>
#include <type_traits>
#include <typeinfo>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <streambuf>
#include <locale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <limits>
#include <exception>

namespace ast {
namespace testing {

/// @brief TEST_F 夹具的基类。按需覆写 SetUp()/TearDown()。
class Test {
public:
    virtual ~Test() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
    virtual void TestBody() = 0;
};

// 兼容 GoogleTest 的开关访问方式：testing::GTEST_FLAG(name) 展开为裸标识符
// FLAGS_gtest_<name>（与 GoogleTest 相同的技巧），从而保留调用方书写的
// "testing::" 前缀。采用内部链接，每个翻译单元各有一份副本；每个测试
// 可执行文件都是单一翻译单元。
static bool FLAGS_gtest_catch_exceptions = true;

#define GTEST_FLAG(name) FLAGS_gtest_##name

/// @brief 初始化（极简的）测试运行器。参数被忽略。
inline void InitGoogleTest(int* argc, char** argv) {
    (void)argc;
    (void)argv;
}

namespace internal {

/// @brief 一个已注册的测试用例。
struct TestCase {
    std::string suite;
    std::string name;
    std::function<void()> run;
    int failures = 0;
    bool skipped = false;
    std::vector<std::string> trace;  ///< SCOPED_TRACE 栈
};

/// @brief 全局测试注册表（整个程序仅一份）。
inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> list;
    return list;
}

/// @brief 当前正在执行的测试（用于归因失败）。
inline TestCase*& current_test() {
    static TestCase* t = nullptr;
    return t;
}

/// @brief 在静态初始化阶段注册一个测试。
struct TestRegistrar {
    TestRegistrar(const char* suite, const char* name, std::function<void()> run) {
        TestCase t;
        t.suite = suite;
        t.name = name;
        t.run = std::move(run);
        registry().push_back(std::move(t));
    }
};

/// @brief 由致命断言（ASSERT_*）抛出，用于终止测试体。
struct FatalTestException {};

/// @brief 由 GTEST_SKIP 抛出，用于跳过当前测试。
struct SkippedTestException {};

/// @brief 在当前测试上记录一次失败并打印消息（含 SCOPED_TRACE 栈）。
inline void report_current_failure(const std::string& msg) {
    if (TestCase* t = current_test()) {
        ++t->failures;
        for (size_t i = 0; i < t->trace.size(); ++i) {
            std::fprintf(stderr, "  %s\n", t->trace[i].c_str());
        }
    }
    std::fputs(msg.c_str(), stderr);
    std::fputc('\n', stderr);
}

/// @brief 将当前测试标记为跳过。
inline void mark_skipped() {
    if (TestCase* t = current_test()) {
        t->skipped = true;
    }
}

// 若某个类型的值可流式输出则输出之，否则为空操作。避免对未定义 operator<<
// 的自定义类型使用 "<<" 时报编译错误。
template <typename T>
auto stream_value(std::ostream& os, const T& v, int) -> decltype(os << v, void()) {
    os << v;
}
template <typename T>
void stream_value(std::ostream&, const T&, long) {}

/// @brief SCOPED_TRACE 的 RAII 辅助类：构造时压入一条跟踪信息，析构时弹出。
class ScopedTrace {
public:
    ScopedTrace(const char* file, int line, const std::string& msg) {
        if (TestCase* t = current_test()) {
            t->trace.push_back(std::string(file) + ":" + std::to_string(line) + ": " + msg);
        }
    }
    ~ScopedTrace() {
        if (TestCase* t = current_test()) {
            if (!t->trace.empty()) t->trace.pop_back();
        }
    }
};

/// @brief GTEST_SKIP() << "msg" 的临时对象：支持 "<<" 流式输出，其析构函数将
///        当前测试标记为跳过并抛出 SkippedTestException 以终止测试体。
class SkipHelper {
public:
    template <typename T>
    SkipHelper& operator<<(const T& v) {
        stream_value(stream_, v, 0);
        return *this;
    }

    ~SkipHelper() noexcept(false) {
        (void)stream_.str();  // 读取流，避免 stream_ 因只写不读而被告警
        mark_skipped();
        throw SkippedTestException();
    }

private:
    std::ostringstream stream_;
};

/// @brief FAIL() << "msg" 的临时对象：支持 "<<" 流式输出，其析构函数上报一次
///        致命失败并抛出 FatalTestException 以终止测试体。
class FailHelper {
public:
    FailHelper(const char* file, int line) : file_(file), line_(line) {}

    template <typename T>
    FailHelper& operator<<(const T& v) {
        stream_value(stream_, v, 0);
        return *this;
    }

    ~FailHelper() noexcept(false) {
        std::ostringstream os;
        os << file_ << ':' << line_ << ": Failure\n  FAIL()";
        const std::string extra = stream_.str();
        if (!extra.empty()) {
            os << "\n  Extra: " << extra;
        }
        report_current_failure(os.str());
        throw FatalTestException();
    }

private:
    const char* file_;
    int line_;
    std::ostringstream stream_;
};

/// @brief 由 EXPECT_*/ASSERT_* 返回的临时对象；累加可选的 "<<" 消息；其析构
///        函数负责上报结果，并在致命断言时抛出 FatalTestException 以终止
///        包围的测试体。
class AssertionHelper {
public:
    AssertionHelper(bool ok, const char* file, int line, const char* expr, bool fatal)
        : ok_(ok), file_(file), line_(line), expr_(expr), fatal_(fatal) {}

    template <typename T>
    AssertionHelper& operator<<(const T& v) {
        if (!ok_) {
            stream_value(stream_, v, 0);
        }
        return *this;
    }

    explicit operator bool() const { return ok_; }

    // 可能抛出 FatalTestException（致命断言时），因此不声明为 noexcept。
    ~AssertionHelper() noexcept(false) {
        if (ok_) return;
        std::ostringstream os;
        os << file_ << ':' << line_ << ": Failure\n  Expression: " << expr_;
        const std::string extra = stream_.str();
        if (!extra.empty()) {
            os << "\n  Extra: " << extra;
        }
        report_current_failure(os.str());
        if (fatal_) {
            throw FatalTestException();
        }
    }

private:
    bool ok_;
    const char* file_;
    int line_;
    const char* expr_;
    bool fatal_;
    std::ostringstream stream_;
};

// 比较辅助函数。

inline bool streq(const char* a, const char* b) {
    if (!a || !b) return a == b;
    return std::strcmp(a, b) == 0;
}
template <typename A, typename B>
inline bool streq(const A& a, const B& b) {
    return std::string(a) == std::string(b);
}

inline bool double_eq(double a, double b) {
    if (a == b) return true;
    if (std::isnan(a) || std::isnan(b)) return std::isnan(a) && std::isnan(b);
    double scale = std::fabs(a);
    if (std::fabs(b) > scale) scale = std::fabs(b);
    if (scale < 1.0) scale = 1.0;
    return std::fabs(a - b) <= scale * 4.0 * std::numeric_limits<double>::epsilon();
}

inline bool is_near(double a, double b, double eps) {
    return std::fabs(a - b) <= std::fabs(eps);
}

/// @brief 运行全部已注册测试，并返回失败数量。
inline int run_all_tests() {
    std::vector<TestCase>& list = registry();
    int total = 0, failed = 0, skipped = 0;

    for (size_t i = 0; i < list.size(); ++i) {
        TestCase& tc = list[i];
        ++total;
        tc.failures = 0;
        tc.skipped = false;
        current_test() = &tc;

        std::printf("[ RUN      ] %s.%s\n", tc.suite.c_str(), tc.name.c_str());
        const bool caught = FLAGS_gtest_catch_exceptions;
        try {
            tc.run();
        } catch (const FatalTestException&) {
            // 致命断言（ASSERT_*）的内部控制流，始终吞掉，以便该测试按失败
            // 计数并让运行器继续执行其余测试。
        } catch (const SkippedTestException&) {
            // 跳过标记已由 GTEST_SKIP 写入，直接按跳过计数。
        } catch (const std::exception& e) {
            if (caught) {
                report_current_failure(std::string("stray exception: ") + e.what());
            } else {
                throw;
            }
        } catch (...) {
            if (caught) {
                report_current_failure("stray non-std exception");
            } else {
                throw;
            }
        }
        current_test() = nullptr;

        if (tc.skipped) {
            ++skipped;
            std::printf("[  SKIPPED ] %s.%s\n", tc.suite.c_str(), tc.name.c_str());
        } else if (tc.failures > 0) {
            ++failed;
            std::printf("[  FAILED  ] %s.%s\n", tc.suite.c_str(), tc.name.c_str());
        } else {
            std::printf("[       OK ] %s.%s\n", tc.suite.c_str(), tc.name.c_str());
        }
    }

    std::printf("\n[==========] %d test(s) ran.\n", total);
    std::printf("[  PASSED  ] %d test(s).\n", total - failed - skipped);
    if (skipped) std::printf("[  SKIPPED ] %d test(s).\n", skipped);
    if (failed) std::printf("[  FAILED  ] %d test(s).\n", failed);
    return failed;
}

} // namespace internal
} // namespace testing
} // namespace ast

// 兼容 GoogleTest 的词法别名：把 "testing" 映射到 ast::testing，
// 使既有测试中 testing::Test / testing::InitGoogleTest() 等无需改动。
namespace testing = ast::testing;

/// @brief 运行全部已注册测试（返回失败数量）。
inline int RUN_ALL_TESTS() { return ::testing::internal::run_all_tests(); }

// -----------------------------------------------------------------------------
// 公开宏
// -----------------------------------------------------------------------------

/// @brief 跳过当前测试（支持 "<<" 流式输出）。
#define GTEST_SKIP() (::testing::internal::SkipHelper())

/// @brief 记录一条跟踪信息，便于定位失败位置。
#define SCOPED_TRACE(message) \
    ::testing::internal::ScopedTrace _gtest_scoped_trace_##__LINE__(__FILE__, __LINE__, (message))

/// @brief 标记为成功（无操作）。
#define SUCCEED() do { } while (0)

/// @brief 立即令当前测试失败（支持 "<<" 流式输出）。
#define FAIL() (::testing::internal::FailHelper(__FILE__, __LINE__))

// 展开为临时 AssertionHelper，从而支持 "<<" 链式输出，并由析构函数上报结果。
#define AST_GTEST_ASSERT_(expr, exprtext, fatal) \
    (::testing::internal::AssertionHelper((expr), __FILE__, __LINE__, (exprtext), (fatal)))

#define EXPECT_EQ(a, b) AST_GTEST_ASSERT_((a) == (b), #a " == " #b, false)
#define ASSERT_EQ(a, b) AST_GTEST_ASSERT_((a) == (b), #a " == " #b, true)
#define EXPECT_NE(a, b) AST_GTEST_ASSERT_((a) != (b), #a " != " #b, false)
#define ASSERT_NE(a, b) AST_GTEST_ASSERT_((a) != (b), #a " != " #b, true)
#define EXPECT_LT(a, b) AST_GTEST_ASSERT_((a) < (b), #a " < " #b, false)
#define ASSERT_LT(a, b) AST_GTEST_ASSERT_((a) < (b), #a " < " #b, true)
#define EXPECT_GT(a, b) AST_GTEST_ASSERT_((a) > (b), #a " > " #b, false)
#define ASSERT_GT(a, b) AST_GTEST_ASSERT_((a) > (b), #a " > " #b, true)
#define EXPECT_LE(a, b) AST_GTEST_ASSERT_((a) <= (b), #a " <= " #b, false)
#define ASSERT_LE(a, b) AST_GTEST_ASSERT_((a) <= (b), #a " <= " #b, true)
#define EXPECT_GE(a, b) AST_GTEST_ASSERT_((a) >= (b), #a " >= " #b, false)
#define ASSERT_GE(a, b) AST_GTEST_ASSERT_((a) >= (b), #a " >= " #b, true)

#define EXPECT_TRUE(v) AST_GTEST_ASSERT_(static_cast<bool>(v), #v, false)
#define ASSERT_TRUE(v) AST_GTEST_ASSERT_(static_cast<bool>(v), #v, true)
#define EXPECT_FALSE(v) AST_GTEST_ASSERT_(!static_cast<bool>(v), #v, false)
#define ASSERT_FALSE(v) AST_GTEST_ASSERT_(!static_cast<bool>(v), #v, true)

#define EXPECT_NEAR(a, b, eps) AST_GTEST_ASSERT_(::testing::internal::is_near((a), (b), (eps)), #a " near " #b, false)
#define ASSERT_NEAR(a, b, eps) AST_GTEST_ASSERT_(::testing::internal::is_near((a), (b), (eps)), #a " near " #b, true)
#define EXPECT_DOUBLE_EQ(a, b) AST_GTEST_ASSERT_(::testing::internal::double_eq((a), (b)), #a " == " #b, false)
#define ASSERT_DOUBLE_EQ(a, b) AST_GTEST_ASSERT_(::testing::internal::double_eq((a), (b)), #a " == " #b, true)
#define EXPECT_FLOAT_EQ(a, b) AST_GTEST_ASSERT_(::testing::internal::double_eq((double)(a), (double)(b)), #a " == " #b, false)
#define ASSERT_FLOAT_EQ(a, b) AST_GTEST_ASSERT_(::testing::internal::double_eq((double)(a), (double)(b)), #a " == " #b, true)
#define EXPECT_STREQ(a, b) AST_GTEST_ASSERT_(::testing::internal::streq((a), (b)), #a " == " #b, false)
#define ASSERT_STREQ(a, b) AST_GTEST_ASSERT_(::testing::internal::streq((a), (b)), #a " == " #b, true)

// 抛异常检查：此类检查无法使用 "<<" 流式输出，因此采用语句宏。
#define AST_GTEST_EXPECT_THROW_(stmt, ex, fatal) \
    do { \
        bool ok_ = false; \
        try { (void)(stmt); } \
        catch (const ex&) { ok_ = true; } \
        catch (...) {} \
        if (!ok_) { \
            ::testing::internal::report_current_failure( \
                std::string("Expected exception ") + #ex + " from: " + #stmt); \
            if (fatal) return; \
        } \
    } while (0)
#define EXPECT_THROW(stmt, ex) AST_GTEST_EXPECT_THROW_(stmt, ex, false)
#define ASSERT_THROW(stmt, ex) AST_GTEST_EXPECT_THROW_(stmt, ex, true)

#define AST_GTEST_EXPECT_NO_THROW_(stmt, fatal) \
    do { \
        try { (void)(stmt); } \
        catch (...) { \
            ::testing::internal::report_current_failure( \
                std::string("Unexpected exception from: ") + #stmt); \
            if (fatal) return; \
        } \
    } while (0)
#define EXPECT_NO_THROW(stmt) AST_GTEST_EXPECT_NO_THROW_(stmt, false)
#define ASSERT_NO_THROW(stmt) AST_GTEST_EXPECT_NO_THROW_(stmt, true)

// TEST：注册一个裸测试函数。
#define TEST(suite, name) \
    static void suite##_##name##_TestBody(); \
    static ::testing::internal::TestRegistrar suite##_##name##_Reg( \
        #suite, #name, [] { suite##_##name##_TestBody(); }); \
    static void suite##_##name##_TestBody()

// TEST_F：注册一个夹具（必须继承自 ::testing::Test）。测试类覆写 TestBody()；
// SetUp()/TearDown() 通过公开的基类接口调用，因此即使是私有覆写也能正确分发。
#define TEST_F(fixture, name) \
    class fixture##_##name##_Test : public fixture { \
    public: \
        void TestBody() override; \
    }; \
    static ::testing::internal::TestRegistrar fixture##_##name##_Reg( \
        #fixture, #name, [] { \
            ::testing::Test* obj_ = new fixture##_##name##_Test(); \
            obj_->SetUp(); \
            try { obj_->TestBody(); } \
            catch (const ::testing::internal::FatalTestException&) {} \
            catch (const ::testing::internal::SkippedTestException&) {} \
            obj_->TearDown(); \
            delete obj_; \
        }); \
    void fixture##_##name##_Test::TestBody()
