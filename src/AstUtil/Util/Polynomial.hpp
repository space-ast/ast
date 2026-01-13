///
/// @file      Polynomial.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-11
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

class Polynomial;

/// @brief 解析多项式表达式
/// @param content 多项式表达式字符串，例如 "2x^2 + 3x - 1"
/// @param varname 变量名，例如 "x"
/// @param coeffs 输出参数，用于存储解析后的系数
/// @return 错误码
AST_UTIL_API err_t aParsePolynomial(StringView content, StringView varname, std::vector<double>& coeffs);


/// @brief 解析多项式表达式
/// @param content 多项式表达式字符串，例如 "2x^2 + 3x - 1"
/// @param coeffs 输出参数，用于存储解析后的系数
/// @return 错误码
AST_UTIL_API err_t aParsePolynomial(StringView content, std::vector<double>& coeffs);


/// @brief 将系数向量格式化为多项式表达式字符串
/// @param coeffs 系数向量，例如 {-1.0, 3.0, 2.0} 表示 2x^2 + 3x - 1
/// @param varname 变量名，例如 "x"
/// @return 多项式表达式字符串，例如 "2x^2 + 3x - 1"
AST_UTIL_API std::string aFormatPolynomial(const std::vector<double>& coeffs, StringView varname);


/// @brief 多项式类
class Polynomial
{
public:
    Polynomial() = default;
    Polynomial(const std::initializer_list<double>& init)
        : coeffs_{init}
    {}
    Polynomial(const std::vector<double>& coeff)
        : coeffs_{coeff}
    {}
    ~Polynomial() = default;
    
    /// @brief 解析多项式表达式
    /// @param content 多项式表达式字符串，例如 "2x^2 + 3x - 1"
    /// @param varname 变量名，例如 "x"
    /// @return 错误码
    err_t parse(StringView content, StringView varname){
        return aParsePolynomial(content, varname, coeffs_);
    }
    
    /// @brief 解析多项式表达式（默认变量名 "x"）
    /// @param content 多项式表达式字符串，例如 "2x^2 + 3x - 1"
    /// @return 错误码
    err_t parse(StringView content){
        return aParsePolynomial(content, coeffs_);
    }

    /// @brief 将多项式表达式转换为字符串
    /// @param varname 变量名
    /// @return 多项式表达式字符串
    std::string toString(StringView varname)
    {
        return aFormatPolynomial(coeffs_, varname);
    }

    const std::vector<double>& coeffs() const { return coeffs_; }
    std::vector<double>& coeffs() { return coeffs_; }
    void setCoeffs(const std::vector<double>& coeffs){ coeffs_ = coeffs; }
    void setCoeffs(std::vector<double>&& coeffs){ coeffs_ = std::move(coeffs); }
    
    /// @brief 计算多项式在给定值x处的函数值
    /// @param x 给定值
    /// @return 函数值
    double eval(double x) const;
protected:
    std::vector<double> coeffs_;
};


// Horner法则/秦九韶算法计算多项式值
inline double Polynomial::eval(double x) const
{
    if(coeffs_.size() == 0){
        return 0;
    }
    else if(coeffs_.size() == 1)
    {
        return coeffs_[0];
    }
    else{
        // 使用 Horner 法则计算多项式值
        double temp = coeffs_.back();
        for (int i = (int)coeffs_.size() - 2; i >= 0; --i) {
            temp *= x;
            temp += coeffs_[i];
        }
        return temp;
    }
}

A_ALWAYS_INLINE err_t aParsePolynomial(StringView content, Polynomial& poly)
{
    return poly.parse(content);
}

A_ALWAYS_INLINE err_t aParsePolynomial(StringView content, StringView varname, Polynomial& poly)
{
    return poly.parse(content, varname);
}

AST_NAMESPACE_END