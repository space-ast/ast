///
/// @file      Polynomial.cpp
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

#include "Polynomial.hpp"
#include "AstUtil/ParseFormat.hpp"
#include <cctype> // for isdigit
#include <stdexcept> // for stod, stoul


AST_NAMESPACE_BEGIN


err_t aParsePolynomial(StringView content, StringView varname, std::vector<double> &coeffsOut)
{
    if (content.empty())
    {
        return eErrorNullInput;
    }

    std::vector<double> coeffs;
    size_t i = 0;
    const size_t len = content.size();

    while (i < len)
    {
        // 跳过空格
        while (i < len && std::isspace(static_cast<unsigned char>(content[i])))
        {
            ++i;
        }

        if (i >= len)
        {
            break;
        }

        // 解析符号
        double sign = 1.0;
        if (content[i] == '+' || content[i] == '-')
        {
            if (content[i] == '-')
            {
                sign = -1.0;
            }
            ++i;
        }

        // 跳过空格
        while (i < len && std::isspace(static_cast<unsigned char>(content[i])))
        {
            ++i;
        }

        if (i >= len)
        {
            return eErrorParse;
        }

        // 解析系数
        double coeff = 1.0;
        bool has_coeff = false;
        if (isdigit(content[i]) || content[i] == '.')
        {
            size_t start = i;
            while (i < len && (isdigit(content[i]) || content[i] == '.'))
            {
                ++i;
            }
            coeff = aParseDouble(content.substr(start, i - start));
            has_coeff = true;
        }

        // 跳过空格
        while (i < len && std::isspace(static_cast<unsigned char>(content[i])))
        {
            ++i;
        }

        // 解析变量和指数
        size_t exp = 0;
        if (i < len && isalpha(content[i]) )
        {
            size_t start = i;
            while (i < len && isalpha(content[i]))
            {
                ++i;
            }
            if(varname.empty()){
                varname = content.substr(start, i-start);
            }else{
                if(content.substr(start, i-start) != varname){
                    return eErrorParse;
                }
            }
            
            // 解析指数
            exp = 1;

            // 跳过空格
            while (i < len && std::isspace(static_cast<unsigned char>(content[i])))
            {
                ++i;
            }

            // 解析指数
            if (i < len && content[i] == '^')
            {
                ++i;

                // 跳过空格
                while (i < len && std::isspace(static_cast<unsigned char>(content[i])))
                {
                    ++i;
                }

                if (i >= len || !isdigit(content[i]))
                {
                    return eErrorParse;
                }

                size_t start = i;
                while (i < len && isdigit(content[i]))
                {
                    ++i;
                }
                exp = aParseInt(content.substr(start, i - start));
                auto peekchar = content[i];
                if(peekchar == '.'){
                    aError("Invalid exponent format, expect integer.");
                    return eErrorParse;
                }
                // if(peekchar != ' ' && peekchar != '\n' && peekchar != '\t' && peekchar != '+' && peekchar != '-')
                // {
                //     return eErrorParse;
                // }
            }
        }
        else if (!has_coeff)
        {
            // 没有系数也没有变量，无效项
            return eErrorParse;
        }

        // 设置系数
        coeff *= sign;
        if (exp >= coeffs.size())
        {
            coeffs.resize(exp + 1, 0.0);
        }
        coeffs[exp] += coeff;
    }

    coeffsOut = std::move(coeffs);
    return eNoError;
}

err_t aParsePolynomial(StringView content, std::vector<double> &coeffs)
{
    // 动态识别变量名
    return aParsePolynomial(content, StringView(), coeffs);
}

std::string aFormatPolynomial(const std::vector<double> &coeffs, StringView varname)
{
    std::string result;
    bool first = true;
    for (int i = (int)coeffs.size() - 1; i >= 0; --i)
    {
        std::string numstr = aFormatDouble(coeffs[i]);
        if(numstr.back() == '.'){
            numstr.push_back('0');
        }
        if(first){
            first = false;
            result = numstr;
        }
        else
        {
            if(numstr[0] == '-'){
                result += " - " + numstr.substr(1);
            }else{
                result += " + " + numstr;
            }
        }
        if (i > 0)
        {
            result += " " + varname.to_string();
            result += "^" + aFormatInt(i);
        }
    }
    return result;
}


AST_NAMESPACE_END