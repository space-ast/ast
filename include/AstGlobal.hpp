///
/// @file      AstGlobal.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 根据 Apache 许可证 2.0 版本（"许可证"）授权；
/// 除非符合许可证，否则不得使用此文件。
/// 您可以在以下网址获取许可证副本：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 除非适用法律要求或书面同意，按"原样"分发的软件
/// 没有任何明示或暗示的担保或条件。
/// 请参阅许可证了解特定语言的权限和限制。
 
#pragma once

#include <stddef.h>

/*
 * 宏命名规范：
 * 
 * A_   开头：通用宏
 * AST_ 开头：ast项目专用宏、ast模块相关、ast功能相关宏
 * 
 * */


/// 通用宏

#ifdef _WIN32
#   define A_DECL_EXPORT __declspec(dllexport)
#   define A_DECL_IMPORT __declspec(dllimport)
#else
#   define A_DECL_EXPORT __attribute__((visibility("default")))
#   define A_DECL_IMPORT __attribute__((visibility("default")))
#endif


/// ast项目专用宏

#define AST_NAMESPACE ast
#define AST_NAMESPACE_BEGIN namespace AST_NAMESPACE{
#define AST_NAMESPACE_END   }
#define AST_USING_NAMESPACE using namespace AST_NAMESPACE;
#define AST_PREPEND_NAMESPACE(name) ::AST_NAMESPACE::name

#define AST_DECL_TYPE_ALIAS(name) typedef AST_PREPEND_NAMESPACE(name) A##name;


/// ast项目模块导出声明

#ifdef AST_BUILD_LIB_CORE
#    define AST_CORE_API A_DECL_EXPORT
#else
#    define AST_CORE_API A_DECL_IMPORT
#endif

#define AST_CORE_CAPI extern "C" AST_CORE_API


/// ast项目公共枚举

enum EError
{
	eNoError = 0,   // 没有错误
};

/// ast项目类型前置声明

AST_NAMESPACE_BEGIN

template<typename Scalar, size_t N>
class VectorN;

template<typename Scalar, size_t M, size_t N>
class MatrixMN;

typedef VectorN<double, 3> Vector3d;

typedef MatrixMN<double, 3, 3> Matrix3d;


class Quaternion;

class Euler;

typedef int err_t;

inline void nothing(){}

AST_NAMESPACE_END