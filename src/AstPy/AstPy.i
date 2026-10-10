%module AstPy

%begin %{
#define Py_LIMITED_API 0x03050000
%}

%{
#include "AstAI/AstAI.hpp"
#include "AstAnalyzer/AstAnalyzer.hpp"
#include "AstCalc/AstCalc.hpp"
#include "AstCmd/AstCmd.hpp"
#include "AstCore/AstCore.hpp"
#include "AstLoader/AstLoader.hpp"
#include "AstMath/AstMath.hpp"
#include "AstMock/AstMock.hpp"
#include "AstOCP/AstOCP.hpp"
#include "AstOpt/AstOpt.hpp"
#include "AstScript/AstScript.hpp"
#include "AstSim/AstSim.hpp"
#include "AstSPICE/AstSPICE.hpp"
#include "AstUtil/AstUtil.hpp"
#include "AstWeather/AstWeather.hpp"
// 屏蔽 Windows 头文件中的 min/max 宏，否则会破坏 ast::propagate_nan::min / ::max 的封装
#undef min
#undef max
%}

%include "std_vector.i"
%include "std_string.i"

%rename(__str__) *::toString;
%rename(__getitem__) *::operator();
%rename(__getitem__) *::operator[];


// %typemap(in) ast::StringView() {
//     const char *data = PyUnicode_AsUTF8($input);
//     Py_ssize_t len  = PyUnicode_GetLength($input);
//     if (!data) {
//         // 如果转换失败，data为NULL，需处理异常
//         SWIG_fail;
//     }
//     $1 = ast::StringView(data, len); 
// }


%import "../../include/AstCompiler.h"
%import "../../include/AstGlobal.h"
%import "../AstUtil/String/StringView.hpp"
%import "../AstUtil/RTTI/Object.hpp"
%import "../AstUtil/RTTI/Reflect/Attribute/Attribute.hpp"

%import "../AstMath/NLE/SolverStats.h"


%ignore ast::TimeInterval::discretize;
%ignore ast::TimePointRange;
%ignore ast::DoubleRange;

// va_list 在 Linux x86-64 等平台上被定义为数组类型（__va_list_tag[1]），
// SWIG 生成的 `arg = *temp;` 属于对数组整体赋值，C++ 无法编译。
// 这些 C 风格可变参数接口本身也无法在 Python 侧安全构造 va_list，直接忽略。
%ignore ast::aScript_FormartErrStringV;
%ignore ast::aLogMessageV;
%ignore ast::ast_vprintf;
%ignore ast::cvprintf;
%ignore ast::cvfprintf;

// 屏蔽 posix 命名空间（文件/目录/字符串的底层系统接口），
// 其参数多为 FILE*/stat 等不透明句柄，在 Python 侧既无法构造也无使用价值。
%ignore ast::posix;

%include "AstAllHeaders.i"


%template(Vector3d) ast::VectorN<double, 3>;
