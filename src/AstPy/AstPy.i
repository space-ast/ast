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


%ignore ast::TimeInterval::discrete;
%ignore ast::TimeInterval::DiscreteTimePointRange;
%ignore ast::TimeInterval::DiscreteEpochSecondRange;

%include "AstAllHeaders.i"


%template(Vector3d) ast::VectorN<double, 3>;
