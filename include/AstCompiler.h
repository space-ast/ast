///
/// @file      AstCompiler.h
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      1.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
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
 
#pragma once
 

// 编译器检测宏

// 判断是否是clang
#if defined(__clang__)
#  define A_CLANG __clang__
#endif

// 判断是否是gcc
#if defined(__GNUC__) && !defined(__clang__)
#  define A_GCC __GNUC__
#endif

// 判断是否是intel icc
#if defined(__INTEL_COMPILER)
#    define A_ICC __INTEL_COMPILER
#endif

// 判断是否是mingw
#if defined(__MINGW32__)
#   define A_MINGW __MINGW32__
#endif

// 判断是否是vc
#if defined(_MSC_VER)
#  define A_MSVC _MSC_VER
#endif

// 内置函数检测
#if defined(__has_builtin)
#  define A_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
#  define A_HAS_BUILTIN(builtin) 0
#endif

#if defined(__has_attribute)
#  define A_HAS_ATTRIBUTE(attribute) __has_attribute(attribute)
#else
#  define A_HAS_ATTRIBUTE(attribute) 0
#endif

// 分支预测宏
#if A_HAS_BUILTIN(__builtin_expect) || defined(A_GCC) || defined(A_CLANG)
#  define A_LIKELY(expr)   __builtin_expect(!!(expr), 1)
#  define A_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#  define A_LIKELY(expr)   (expr)
#  define A_UNLIKELY(expr) (expr)
#endif

#if A_HAS_BUILTIN(__builtin_unpredictable)
#  define A_UNPREDICTABLE(expr) __builtin_unpredictable(!!(expr))
#else
#  define A_UNPREDICTABLE(expr) (expr)
#endif

// 内联控制宏

#if A_MSVC || A_ICC
#    define A_STRONG_INLINE __forceinline
#else
#    define A_STRONG_INLINE inline
#endif


#if A_HAS_ATTRIBUTE(always_inline) || defined(A_GCC) || defined(A_CLANG)
#  define A_ALWAYS_INLINE __attribute__((__always_inline__)) inline
#else
#  define A_ALWAYS_INLINE A_STRONG_INLINE
#endif

#if A_HAS_ATTRIBUTE(noinline) || defined(A_GCC) || defined(A_CLANG)
#  define A_NO_INLINE __attribute__((__noinline__))
#elif defined(A_MSVC)
#  define A_NO_INLINE __declspec(noinline)
#else
#  define A_NO_INLINE
#endif


// DLL 导出导入

#ifdef _WIN32
#   define A_DECL_EXPORT __declspec(dllexport)
#   define A_DECL_IMPORT __declspec(dllimport)
#else
#   define A_DECL_EXPORT __attribute__((visibility("default")))
#   define A_DECL_IMPORT __attribute__((visibility("default")))
#endif


// 按C函数名称导入导出，不使用c++修饰名： extern "C"

#ifdef __cplusplus
#   define A_DECL_EXTERN_C extern "C"
#else
#   define A_DECL_EXTERN_C
#endif


// 线程本地存储 thread local storage

#if defined(thread_local) || defined(__cplusplus) && __cplusplus >= 201103L
#  define A_THREAD_LOCAL thread_local
#elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#  define A_THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER)  
#  define A_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) 
#  define A_THREAD_LOCAL __thread
#endif


// 字符串化

#define A_STR(S) #S



// 函数属性宏
#if A_HAS_ATTRIBUTE(const) || defined(A_GCC) || defined(A_CLANG)
#  define A_CONST __attribute__((__const__))
#else
#  define A_CONST
#endif

#if A_HAS_ATTRIBUTE(pure) || defined(A_GCC) || defined(A_CLANG)
#  define A_PURE __attribute__((__pure__))
#else
#  define A_PURE
#endif

#if A_HAS_ATTRIBUTE(noreturn) || defined(A_GCC) || defined(A_CLANG)
#  define A_NORETURN __attribute__((__noreturn__))
#elif defined(A_MSVC)
#  define A_NORETURN __declspec(noreturn)
#else
#  define A_NORETURN
#endif

#if A_HAS_ATTRIBUTE(malloc) || defined(A_GCC) || defined(A_CLANG)
#  define A_MALLOC __attribute__((__malloc__))
#else
#  define A_MALLOC
#endif

#if A_HAS_ATTRIBUTE(warn_unused_result) || defined(A_GCC) || defined(A_CLANG)
#  define A_NODISCARD __attribute__((__warn_unused_result__))
#elif defined(A_MSVC) && (_MSC_VER >= 1700)
#  define A_NODISCARD _Check_return_
#else
#  define A_NODISCARD
#endif

// 优化提示
#if A_HAS_BUILTIN(__builtin_assume)
#  define A_ASSUME(expr) __builtin_assume(expr)
#elif defined(A_MSVC)
#  define A_ASSUME(expr) __assume(expr)
#else
#  define A_ASSUME(expr) ((void)((expr) ? 1 : 0))
#endif

#if A_HAS_BUILTIN(__builtin_unreachable) || defined(A_GCC) || defined(A_CLANG)
#  define A_UNREACHABLE() __builtin_unreachable()
#elif defined(A_MSVC)
#  define A_UNREACHABLE() __assume(0)
#else
#  define A_UNREACHABLE() 
#endif



// 弃用标记
#if A_HAS_ATTRIBUTE(deprecated) || defined(A_GCC) || defined(A_CLANG)
#  define A_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#elif defined(A_MSVC)
#  define A_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#   if _HAS_CXX17 || __cplusplus>201402L
#       define A_DEPRECATED(msg) [[deprecated]]
#   else
#       define A_DEPRECATED(msg)
#   endif
#endif


// 类型安全检查
#if A_HAS_ATTRIBUTE(nonnull) || defined(A_GCC) || defined(A_CLANG)
#  define A_NON_NULL(...) __attribute__((__nonnull__(__VA_ARGS__)))
#else
#  define A_NON_NULL(...)
#endif

// 限制指针
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#  define A_RESTRICT restrict
#elif defined(A_MSVC)
#  define A_RESTRICT __restrict
#elif defined(A_GCC) || defined(A_CLANG)
#  define A_RESTRICT __restrict__
#else
#  define A_RESTRICT
#endif




/// 为类型定义迭代器标准函数
#define A_DEF_ITERABLE(Scalar, Data, Size)                                      \
    size_t size() const noexcept{ return (Size) ;}                              \
    Scalar* data() noexcept{return (Data);}                                     \
    Scalar const* data() const{return (Data);}                                  \
    Scalar* begin() noexcept{ return data(); }                                  \
    Scalar* end() noexcept{ return (Data) + (Size); }                           \
    Scalar const * begin() const noexcept{ return data(); }                     \
    Scalar const* end() const noexcept{ return (Data) + (Size); }               \
    Scalar const* cbegin() const noexcept{ return data(); }                     \
    Scalar const* cend() const noexcept{ return (Data) + (Size); }              \
    Scalar operator[](size_t idx) const noexcept{return data()[idx];}           \
    Scalar& operator[](size_t idx) noexcept{return data()[idx];}                \


/// 为POD类型定义迭代器标准函数
#define A_DEF_POD_ITERABLE(Scalar)                                      \
    A_DEF_ITERABLE(Scalar, (Scalar*)this, sizeof(*this)/sizeof(Scalar))

