///
/// @file      AstGlobal.hpp
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

#ifdef __cplusplus
#   define A_DECL_EXTERN_C extern "C"
#else
#   define A_DECL_EXTERN_C
#endif


#define A_DEF_ITERABLE(Scalar, Data, Size)                              \
    size_t size() const noexcept{ return (Size) ;}                      \
    Scalar* data() noexcept{return (Data);}                             \
    const Scalar* data() const{return (Data);}                          \
    Scalar* begin() noexcept{ return data(); }                          \
    Scalar* end() noexcept{ return (Data) + (Size); }                   \
    const Scalar* begin() const noexcept{ return data(); }              \
    const Scalar* end() const noexcept{ return (Data) + (Size); }       \
    const Scalar* cbegin() const noexcept{ return data(); }             \
    const Scalar* cend() const noexcept{ return (Data) + (Size); }      \
    Scalar operator[](size_t idx) const noexcept{return data()[idx];}   \
    Scalar& operator[](size_t idx) noexcept{return data()[idx];}        \

#define A_DEF_POD_ITERABLE(Scalar)                                      \
    A_DEF_ITERABLE(Scalar, (Scalar*)this, sizeof(*this)/sizeof(Scalar))



#if __cplusplus >= 201402L

#define COHEAD_LOCAL_BUFFER(T, buf, size)                               \
  auto __cohead_local_buffer_ ## buf = std::make_unique<T []> (size);     \
  T *buf = __cohead_local_buffer_ ## buf.get ();            

#else

#define COHEAD_LOCAL_BUFFER(T, buf, size)                                 \
  std::unique_ptr<T []> __cohead_local_buffer_ ## buf { new T [size] };   \
  T *buf = __cohead_local_buffer_ ## buf.get ();

#endif

#define COHEAD_LOCAL_BUFFER_INIT(T, buf, size, value)                   \
  COHEAD_LOCAL_BUFFER (T, buf, size);                                   \
  std::fill_n (buf, size, value);


/// ast项目专用宏
#ifdef __cplusplus
#	define AST_NAMESPACE ast
#	define AST_NAMESPACE_BEGIN namespace AST_NAMESPACE{
#	define AST_NAMESPACE_END   }
#	define AST_USING_NAMESPACE using namespace AST_NAMESPACE;
#	define AST_PREPEND_NAMESPACE(name) ::AST_NAMESPACE::name
#	define AST_DECL_TYPE_ALIAS(name) typedef AST_PREPEND_NAMESPACE(name) A##name;
#else
#	define AST_NAMESPACE 
#	define AST_NAMESPACE_BEGIN 
#	define AST_NAMESPACE_END   
#	define AST_USING_NAMESPACE 
#	define AST_PREPEND_NAMESPACE(name) name
#	define AST_DECL_TYPE_ALIAS(name)
#endif



/// ast项目模块导出声明

#ifdef AST_BUILD_LIB_CORE
#    define AST_CORE_API A_DECL_EXPORT
#    define AST_OPT_API  A_DECL_EXPORT
#else
#    define AST_CORE_API A_DECL_IMPORT
#    define AST_OPT_API  A_DECL_IMPORT
#endif

#define AST_CORE_CAPI A_DECL_EXTERN_C AST_CORE_API
#define AST_OPT_CAPI  A_DECL_EXTERN_C AST_OPT_API


AST_NAMESPACE_BEGIN

/// ast项目公共枚举

typedef enum EError
{
	eNoError = 0,   // 没有错误
} AEError;

/// ast项目类型前置声明

#ifdef __cplusplus
template<typename Scalar, size_t N>
class VectorN;

template<typename Scalar, size_t M, size_t N>
class MatrixMN;

typedef VectorN<double, 3> Vector3d;

typedef MatrixMN<double, 3, 3> Matrix3d;


class Quaternion;

class Euler;

#endif

typedef int err_t;


inline void nothing(){}

AST_NAMESPACE_END


AST_DECL_TYPE_ALIAS(Vector3d)
