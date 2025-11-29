///
/// @file      AstGlobal.h
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
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

#include <stddef.h>         // for size_t


/*
 * 宏命名规范：
 * 
 * A_   开头：通用宏，也能用于其他工程
 * AST_ 开头：ast项目专用宏、ast模块相关、ast功能相关宏
 * 
 * */


/// 通用宏

// 判断是否是 Microsoft Visual C++
#if defined(_MSC_VER)
#    define A_COMP_MSVC _MSC_VER
#else
#    define A_COMP_MSVC 0
#endif

// 判断是否是Intel icc compiler
#if defined(__INTEL_COMPILER)
#    define A_COMP_ICC __INTEL_COMPILER
#else
#    define A_COMP_ICC 0
#endif

// 判断编译器是否是与GCC兼容
#ifdef __GNUC__
#    define A_COMP_GNUC (__GNUC__*10+__GNUC_MINOR__)
#else
#    define A_COMP_GNUC 0
#endif

#if defined(__clang__)
#define A_COMP_CLANG (__clang_major__*100+__clang_minor__)
#else
#define A_COMP_CLANG 0
#endif

#if defined(__MINGW32__)
#define A_COMP_MINGW 1
#else
#define A_COMP_MINGW 0
#endif

// deprecated c++17特性
#if _HAS_CXX17 || __cplusplus>201402L
#define A_DEPRECATED [[deprecated]] // 废弃函数
#else
#define A_DEPRECATED // 废弃函数
#endif


// __forceinline
#ifndef A_STRONG_INLINE
#if A_COMP_MSVC || A_COMP_ICC
#define A_STRONG_INLINE __forceinline
#else
#define A_STRONG_INLINE inline
#endif
#endif

// __attribute__((always_inline)) __forceinline
#if A_COMP_GNUC
#define A_ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define A_ALWAYS_INLINE A_STRONG_INLINE
#endif

// DLL 导出导入

#ifdef _WIN32
#   define A_DECL_EXPORT __declspec(dllexport)
#   define A_DECL_IMPORT __declspec(dllimport)
#else
#   define A_DECL_EXPORT __attribute__((visibility("default")))
#   define A_DECL_IMPORT __attribute__((visibility("default")))
#endif


// extern "C"

#ifdef __cplusplus
#   define A_DECL_EXTERN_C extern "C"
#else
#   define A_DECL_EXTERN_C
#endif


// thread local storage

#ifdef thread_local
#  define A_THREAD_LOCAL thread_local
#elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#  define A_THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER)  
#  define A_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) 
#  define A_THREAD_LOCAL __thread
#endif


#define A_STR(S) #S


/// 为类型定义迭代器标准函数
#define A_DEF_ITERABLE(Scalar, Data, Size)                                      \
    size_t size() const noexcept{ return (Size) ;}                              \
    Scalar* data() noexcept{return (Data);}                                     \
    Scalar const* data() const{return (Data);}                                  \
    Scalar* begin() noexcept{ return data(); }                                  \
    Scalar* end() noexcept{ return (Data) + (Size); }                           \
    Scalar const * begin() const noexcept{ return data(); }                      \
    Scalar const* end() const noexcept{ return (Data) + (Size); }               \
    Scalar const* cbegin() const noexcept{ return data(); }                     \
    Scalar const* cend() const noexcept{ return (Data) + (Size); }              \
    Scalar operator[](size_t idx) const noexcept{return data()[idx];}           \
    Scalar& operator[](size_t idx) noexcept{return data()[idx];}                \


/// 为POD类型定义迭代器标准函数
#define A_DEF_POD_ITERABLE(Scalar)                                      \
    A_DEF_ITERABLE(Scalar, (Scalar*)this, sizeof(*this)/sizeof(Scalar))




/// ast项目专用宏
#ifdef __cplusplus
#   define _AST ::ast:: 
#	define AST_NAMESPACE ast
#	define AST_NAMESPACE_BEGIN namespace AST_NAMESPACE{
#	define AST_NAMESPACE_END   }
#	define AST_USING_NAMESPACE using namespace AST_NAMESPACE;
#	define AST_PREPEND_NAMESPACE(name) ::AST_NAMESPACE::name
#	define AST_DECL_TYPE_ALIAS(name) typedef AST_PREPEND_NAMESPACE(name) A##name;
#else
#   define _AST
#	define AST_NAMESPACE 
#	define AST_NAMESPACE_BEGIN 
#	define AST_NAMESPACE_END   
#	define AST_USING_NAMESPACE 
#	define AST_PREPEND_NAMESPACE(name) name
#	define AST_DECL_TYPE_ALIAS(name)
#endif



// 定义访问函数
#define AST_DEF_ACCESS_METHOD(TYPE, NAME) TYPE NAME() const{return NAME##_;} TYPE& NAME(){return NAME##_;}


// AST 对象运行时元信息
#define AST_OBJECT(TYPE) // @todo

/// ast项目模块导出声明

#ifdef AST_BUILD_LIB_CORE
#    define AST_CORE_API A_DECL_EXPORT
#else
#    define AST_CORE_API A_DECL_IMPORT
#endif
#define AST_CORE_CAPI A_DECL_EXTERN_C AST_CORE_API

#ifdef AST_BUILD_LIB_OPT
#    define AST_OPT_API A_DECL_EXPORT
#else
#    define AST_OPT_API A_DECL_IMPORT
#endif
#define AST_OPT_CAPI  A_DECL_EXTERN_C AST_OPT_API

#ifdef AST_BUILD_LIB_UTIL
#    define AST_UTIL_API A_DECL_EXPORT
#else
#    define AST_UTIL_API A_DECL_IMPORT
#endif
#define AST_UTIL_CAPI A_DECL_EXTERN_C AST_UTIL_API

#ifdef AST_BUILD_LIB_MOCK
#    define AST_MOCK_API A_DECL_EXPORT
#else
#    define AST_MOCK_API A_DECL_IMPORT
#endif
#define AST_MOCK_CAPI A_DECL_EXTERN_C AST_MOCK_API

AST_NAMESPACE_BEGIN

/// ast项目公共枚举

typedef enum EError
{
	eNoError = 0,       ///< 没有错误
    eErrorNullInput,    ///< 输入参数是空指针
    eErrorInvalidParam, ///< 非法输入参数
    eErrorNotInit,      ///< 没有初始化
    eErrorInvalidFile,  ///< 文件格式错误
} AEError;

/// ast项目类型前置声明

#ifdef __cplusplus
template<typename _Scalar, size_t N>
class VectorN;

template<typename _Scalar, size_t M, size_t N>
class MatrixMN;

typedef VectorN<double, 3> Vector3d;

typedef MatrixMN<double, 3, 3> Matrix3d;


class Quaternion;

class Euler;

class Object;
class Type;

class AbsTime;

class System;
class Axes;
class Point;

#endif

typedef int err_t;


inline void nothing(){}

AST_NAMESPACE_END


AST_DECL_TYPE_ALIAS(Vector3d)
