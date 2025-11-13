#pragma once

/*
 * 宏命名规范：
 * 
 * A_   开头：通用宏
 * AST_ 开头：AST工程专用宏、模块相关、功能相关宏
 * 
 * */


#ifdef _WIN32
#   define A_DECL_EXPORT __declspec(dllexport)
#   define A_DECL_IMPORT __declspec(dllimport)
#else
#   define A_DECL_EXPORT __attribute__((visibility("default")))
#   define A_DECL_IMPORT __attribute__((visibility("default")))
#endif

#define AST_NAMESPACE ast
#define AST_NAMESPACE_BEGIN namespace AST_NAMESPACE{
#define AST_NAMESPACE_END   };
#define AST_USING_NAMESPACE using namespace AST_NAMESPACE;
#define AST_PREPEND_NAMESPACE(name) ::AST_NAMESPACE::name

#define AST_DECL_TYPE_ALIAS(name) typedef AST_PREPEND_NAMESPACE(name) A##name;
