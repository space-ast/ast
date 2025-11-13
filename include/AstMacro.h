#pragma once

/*
 * 宏命名规范：
 * 
 * A_   开头：通用宏
 * AST_ 开头：模板相关、功能相关宏
 * 
 * */


#ifdef _WIN32
#   define A_DECL_EXPORT __declspec(dllexport)
#   define A_DECL_IMPORT __declspec(dllimport)
#else
#   define A_DECL_EXPORT __attribute__((visibility("default")))
#   define A_DECL_IMPORT __attribute__((visibility("default")))
#endif

#define AST_NAMESPACE_BEGIN namespace ast{
#define AST_NAMESPACE_END   };
#define AST_USING_NAMESPACE using namespace ast;


