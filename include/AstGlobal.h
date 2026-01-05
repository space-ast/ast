///
/// @file      AstGlobal.h
/// @brief     
/// @details   ~
/// @author    axel
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



/*
 * 宏命名规范：
 * 
 * 前缀 A_          ：通用宏，也能用于其他工程
 * 前缀 AST_        ：ast项目专用宏、ast模块相关、ast功能相关宏
 * 前缀 _           ：内部宏
 * 
 * 前缀 AST_WITH_   ：是否带有某个第三方库，例如eigen、boost、fmt、ipopt等
 * 前缀 AST_ENABLE_ ：是否启用某个特定功能，通常是性能、调试、特性的开关，例如调试信息、缓存、日志、AVX2指令集等
 * 前缀 AST_USE_    ：是否使用某个实现方法，通常是可选、可替换的实现方式，例如不同的算法、不同的数据结构
 * 前缀 AST_HAS_    ：是否具有某个特定功能，通常是指示标准库是否有某功能，例如是否有某个函数、是否有某个类型
 * */


// 通用宏见"AstCompiler.h"
#include "AstCompiler.h"

#include <stddef.h>         // for size_t
#include <stdint.h>         // for uint32_t


// 下面是ast项目专用宏，用于控制ast项目的行为，你可以根据需要定义或注释掉这些宏
// 有些宏定义会改变项目的二进制接口(ABI)，使用不同宏定义编译的库文件互相不兼容
// 请确保整个项目中使用一致的配置，避免混用不同配置的库文件和头文件


#define AST_ENABLE_NAMESPACE                     // [影响ABI]是否使用命名空间
// #define AST_ENABLE_OVERRIDE_STDLIB            // 是否允许覆盖标准库的一些函数
// #define AST_ENABLE_DATETIME_FORMAT_RFC        // 是否启用RFC系列的其他日期时间格式化，例如RFC 1123、RFC 2822等
// #define AST_ENABLE_REDUNDANT                  // 是否启用冗余功能

// #define AST_USE_STD_STRING_VIEW               // [影响ABI]是否使用 std::string_view ，否则使用内置的string_view
// #define AST_USE_STD_FILESYSTEM                // 是否使用 std::filesystem，如果存在的话，否则使用内置的filesystem
// #define AST_USE_STD_FILESYSTEM_EXPERIMENTAL   // 是否选择使用 std::experimental::filesystem 的c++实验特性，如果存在的话


/// ast项目专用宏
#if defined AST_ENABLE_NAMESPACE && defined __cplusplus 
#   define _AST ::ast:: 
#	define AST_NAMESPACE ast
#	define AST_NAMESPACE_BEGIN namespace AST_NAMESPACE{
#	define AST_NAMESPACE_END   }
#	define AST_USING_NAMESPACE using namespace AST_NAMESPACE;
#	define AST_PREPEND_NAMESPACE(name) ::AST_NAMESPACE::name
#	define AST_DECL_TYPE_ALIAS(name) typedef AST_PREPEND_NAMESPACE(name) A##name;
#else
#   define _AST ::
#	define AST_NAMESPACE 
#	define AST_NAMESPACE_BEGIN 
#	define AST_NAMESPACE_END   
#	define AST_USING_NAMESPACE 
#	define AST_PREPEND_NAMESPACE(name) ::name
#	define AST_DECL_TYPE_ALIAS(name)
#endif



// 定义访问函数
#define AST_DEF_ACCESS_METHOD(TYPE, NAME) TYPE NAME() const{return NAME##_;} TYPE& NAME(){return NAME##_;}


// AST 对象运行时元信息
#define AST_OBJECT(TYPE) Class* staticType();

#ifdef AST_PROPERTIES_MARK
#   define AST_PROPERTIES AST_PROPERTIES_MARK public
#else
#   define AST_PROPERTIES public
#endif
#define properties AST_PROPERTIES


// 字符串宏，用于在编译时将字符串转换为ast项目内部运行时编码
// 当前ast项目内部运行时采用utf-8编码，所有字符串字面量都需要在编译时转换为utf-8编码
// 但是考虑到为了有可能会采用其他编码，例如utf-16等，所以这里保留宏定义
#define aText(x) u8 ## x


// ast项目脚本模块导出声明
#ifdef AST_BUILD_LIB_SCRIPT
#    define AST_SCRIPT_API A_DECL_EXPORT
#else
#    define AST_SCRIPT_API A_DECL_IMPORT
#endif
#define AST_SCRIPT_CAPI A_DECL_EXTERN_C AST_SCRIPT_API


// ast项目核心模块导出声明
#ifdef AST_BUILD_LIB_CORE
#    define AST_CORE_API A_DECL_EXPORT
#else
#    define AST_CORE_API A_DECL_IMPORT
#endif
#define AST_CORE_CAPI A_DECL_EXTERN_C AST_CORE_API

// ast项目优化模块导出声明
#ifdef AST_BUILD_LIB_OPT
#    define AST_OPT_API A_DECL_EXPORT
#else
#    define AST_OPT_API A_DECL_IMPORT
#endif
#define AST_OPT_CAPI  A_DECL_EXTERN_C AST_OPT_API

// ast项目数学模块导出声明
#ifdef AST_BUILD_LIB_MATH
#    define AST_MATH_API A_DECL_EXPORT
#else
#    define AST_MATH_API A_DECL_IMPORT
#endif
#define AST_MATH_CAPI A_DECL_EXTERN_C AST_MATH_API

// ast项目工具模块导出声明
#ifdef AST_BUILD_LIB_UTIL
#    define AST_UTIL_API A_DECL_EXPORT
#else
#    define AST_UTIL_API A_DECL_IMPORT
#endif
#define AST_UTIL_CAPI A_DECL_EXTERN_C AST_UTIL_API

// ast项目模拟模块导出声明
#ifdef AST_BUILD_LIB_MOCK
#    define AST_MOCK_API A_DECL_EXPORT
#else
#    define AST_MOCK_API A_DECL_IMPORT
#endif
#define AST_MOCK_CAPI A_DECL_EXTERN_C AST_MOCK_API

AST_NAMESPACE_BEGIN

/// ast项目公共枚举


/// @brief 错误码
typedef enum EError
{
	eNoError = 0,       ///< 没有错误
    eErrorNullInput,    ///< 输入参数是空指针
    eErrorInvalidParam, ///< 非法输入参数
    eErrorNotInit,      ///< 没有初始化
    eErrorInvalidFile,  ///< 文件格式错误
    eErrorParse,        ///< 解析错误
    eErrorReadonly,     ///< 只读属性
} AEError;



/// ast项目类型前置声明

/*
 * @note
 * 下面这些别名用于类型元信息标注，例如：
 * ```
 * class Demo{
 * properties:
 *   time_d   timestamp;        // 时间量纲
 *   angle_d  heading;          // 角度量纲
 *   length_d distance;         // 长度量纲
 * };
 * ```
 * 通过这些标注，反射工具可以识别这些类型，提取其属性元信息，然后生成相应代码，例如动态反射、序列化
 * 使用 `_d` 后缀，表示 dimension 和 double，避免了使用 `_t` 后缀与标准库 `time_t` 的冲突
 */ 
typedef double length_d, mass_d, time_d, area_d, speed_d, force_d, energy_d, power_d, angle_d, angvel_d; 


typedef int err_t;           ///< 错误码类型
typedef double ImpreciseJD;  ///< 儒略日(注意单个double的数值精度不够)
typedef double ImpreciseMJD; ///< 简约儒略日(注意单个double的数值精度不够)
typedef uint32_t Color;      ///< 颜色值


#ifdef __cplusplus
template<typename _Scalar, size_t N>
class VectorN;

template<typename _Scalar, size_t M, size_t N>
class MatrixMN;

typedef VectorN<double, 3> Vector3d;

typedef MatrixMN<double, 3, 3> Matrix3d;

template<typename _Char>
class StringViewBasic;

typedef StringViewBasic<char>     StringView;


class Quaternion;

class Euler;

class Object;
class Class;
using Type = Class;

class AbsTime;
class TimePoint;
class JulianDate;
class ModJulianDate;

class System;
class Axes;
class Point;

#endif


inline void nothing(){}

AST_NAMESPACE_END


AST_DECL_TYPE_ALIAS(Vector3d)
