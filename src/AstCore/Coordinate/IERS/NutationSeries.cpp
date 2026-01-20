///
/// @file      NutationSeries.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-10
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

#include "NutationSeries.hpp"
#include "AstCore/FundamentalArguments.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/Math.hpp"
#include <numeric>

AST_NAMESPACE_BEGIN

/// @brief     解析 nutation 项
/// @details   ~
/// @param     line  nutation 项的字符串表示
/// @param     term  解析后的 nutation 项
/// @return    错误码   
static err_t parseTerm(StringView line, int& index, NutationTerm &term)
{
    int status = sscanf(
        line.data(), 
        "%d %lf %lf %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
        &index, &term.sinCoeff(), &term.cosCoeff(), 
        &term.nL(), &term.nLP(), &term.nF(), &term.nD(), &term.nOm(),
        &term.nMe(), &term.nVe(), &term.nE(), &term.nMa(), &term.nJu(), 
        &term.nSa(), &term.nUr(), &term.nNe(), &term.nPa()
    );
    term.sinCoeff() *= 1e-6 * kArcSecToRad;
    term.cosCoeff() *= 1e-6 * kArcSecToRad;
    if(status != 17){
        return eErrorParse;
    }
    return eNoError;
}

/// @brief     解析 nutation 项的索引 j
/// @details   ~
/// @param     line  nutation 项的索引 j 的字符串表示
/// @param     j     解析后的 nutation 项的索引 j
/// @return    错误码   
static err_t parseJList(StringView line, int &j, int& numTerms)
{
    int status = sscanf(
        line.data(), 
        " j = %d %*[^0-9=] = %d", 
        &j, 
        &numTerms
    );
    if(status != 2){
        return eErrorParse;
    }
    return eNoError;
}

/// @brief     检查 nutation series 是否有效
/// @details   ~
/// @param     terms  
/// @param     jlist 
/// @return    是否有效
static bool checkValid(const std::vector<NutationTerm>& terms, const std::vector<int>& jlist)
{
    if(terms.empty() || jlist.empty()){
        aError("invalid nutation series, number of terms: %zu, number of jlist: %zu\n", terms.size(), jlist.size());
        return false;
    }
    int sum = std::accumulate(jlist.begin(), jlist.end(), 0);
    if(sum != terms.size()){
        aError("invalid nutation series, number of terms: %zu, number of jlist: %zu, sum of jlist: %d\n", terms.size(), jlist.size(), sum);
        return false;
    }
    return true;
}

err_t NutationSeries::load(StringView filepath)
{
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        return eErrorInvalidFile;
    }
    StringView line = parser.getLine();
    parser.seek(0, std::ios::beg);
    if(line.starts_with("VERSION")){
        return loadSTK(parser);
    }else{
        return loadIERS(parser);
    }
}

err_t NutationSeries::loadSTK(StringView filepath)
{
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        return eErrorInvalidFile;
    }
    return loadSTK(parser);
}

err_t NutationSeries::loadIERS(StringView filepath)
{
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        return eErrorInvalidFile;
    }
    return loadIERS(parser);
}

double NutationSeries::eval(double t, const FundamentalArguments &fundargs) const
{
    /*

     * Sum_i[C_{s,0})_i * sin(ARG) + C_{c,0})_i * cos(ARG)] 
     * + Sum_i,j=1,4 [C_{s,j})_i * sin(ARG) + C_{c,j})_i * cos(ARG)] * t^j
     * 
     */
    
    double polyPart = polynomial_.eval(t);
    int end = terms_.size();
    if(A_UNLIKELY(end == 0)){
        aError("nutation series is empty, please call `aInitialize()` first");
    }
    double nonPolyPart = 0;
    for(int j=jlist_.size()-1; j>=0; j--){
        double coeff = 0;
        int start = end - jlist_[j];
        for(int i=end-1; i>=start; i--){
            auto& term = terms_[i];
            double arg = 
               term.nL() * fundargs.l() 
             + term.nLP() * fundargs.lP()
             + term.nF() * fundargs.f()
             + term.nD() * fundargs.d()
             + term.nOm() * fundargs.om()
             + term.nMe() * fundargs.lMe()
             + term.nVe() * fundargs.lVe()
             + term.nE() * fundargs.lE()
             + term.nMa() * fundargs.lMa()
             + term.nJu() * fundargs.lJu()
             + term.nSa() * fundargs.lSa()
             + term.nUr() * fundargs.lUr()
             + term.nNe() * fundargs.lNe()
             + term.nPa() * fundargs.pa();
            double sina, cosa;
            sincos(arg, &sina, &cosa);
            coeff += term.sinCoeff() * sina + term.cosCoeff() * cosa;
        }
        end = start;
        nonPolyPart *= t;
        nonPolyPart += coeff;
    }
    return polyPart + nonPolyPart;
}

double NutationSeries::eval(const TimePoint &tp, const FundamentalArguments &fundargs) const
{
    return eval(tp.julianCenturyFromJ2000TT(), fundargs);
}

err_t NutationSeries::loadSTK(BKVParser &parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    int max_polyindex = 0;
    int max_jindex = 0;
    int num_points = 0;
    std::vector<double> polycoeffs;
    std::vector<int> jlist;
    std::vector<NutationTerm> terms;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            // printf("begin %s\n", item.value().data());
            if(aEqualsIgnoreCase(item.value(), "POLYTABLE"))
            {
                for(int i=0; i<=max_polyindex; i++){
                    StringView line = parser.getLine();
                    double val;
                    err_t err = aParseDouble(line, val);
                    if(err != eNoError){
                        aError("expect double, error %d, line %d, %s\n", err, parser.getLineNumber(), line.data());
                        return err;
                    }
                    val *= 1e-6 * kArcSecToRad;
                    polycoeffs.push_back(val);
                }
            }
            else if(aEqualsIgnoreCase(item.value(), "JTABLE"))
            {
                for(int i=0; i<=max_jindex; i++){
                    StringView line = parser.getLine();
                    int val;
                    err_t err = aParseInt(line, val);
                    if(err != eNoError){
                        aError("expect integer, error %d, line %d, %s\n", err, parser.getLineNumber(), line.data());
                        return err;
                    }
                    jlist.push_back(val);
                }
            }else if(aEqualsIgnoreCase(item.value(), "TABLE"))
            {
                for(int i=0; i<num_points; i++){
                    StringView line = parser.getLineSkipComment();
                    NutationTerm term;
                    int index;
                    err_t err = parseTerm(line, index, term);
                    if(err != eNoError){
                        aError("expect 17 terms, error %d, line %d, %s\n", err, parser.getLineNumber(), line.data());
                        return err;
                    }
                    terms.push_back(term);
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            // printf("end %s\n", item.value().data());
        }else if(token == BKVParser::eKeyValue){
            // printf("key %s, value %s\n", item.key().data(), item.value().data());
            if(aEqualsIgnoreCase(item.key(), "MAX_POLYINDEX")){
                max_polyindex = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "MAX_JINDEX")){
                max_jindex = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "NUM_POINTS")){
                num_points = item.value().toInt();
            }
        }
    }while(token != BKVParser::eEOF);
    // printf("max_polyindex=%d, max_jindex=%d, num_points=%d\n", max_polyindex, max_jindex, num_points);
    // printf("%zu, %zu, %zu\n", polycoeffs.size(), jlist.size(), terms.size());
    if(!checkValid(terms, jlist)){
        // aError("invalid nutation series, number of terms: %zu, number of jlist: %zu\n", terms.size(), jlist.size());
        return eErrorParse;
    }

    this->polynomial_.coeffs() = std::move(polycoeffs);
    this->jlist_ = std::move(jlist);
    this->terms_ = std::move(terms);
    return eNoError;
}

err_t NutationSeries::loadIERS(BKVParser &parser)
{
    std::vector<NutationTerm> terms;
    Polynomial polynomial;
    std::vector<int> jlist;
    do{
        StringView line = parser.getLine();
        if(line.empty()){
            continue;
        }
        int index;
        NutationTerm term;
        err_t err = parseTerm(line, index, term);
        if(err == eNoError){
            terms.push_back(term);
        }else{
            int j;
            int numTerms;
            err = parseJList(line, j, numTerms);
            if(err == eNoError){
                jlist.push_back(numTerms);
            }else if(polynomial.coeffs().empty()){
                polynomial.parse(line.to_string());
                if(!polynomial.coeffs().empty()){
                    for(auto& val : polynomial.coeffs()){
                        val *= 1e-6 * kArcSecToRad;
                    }
                }
            }
        }
    }while(!parser.eof());
    
    // printf("%zu, %zu, %zu\n", polynomial.coeffs().size(), jlist.size(), terms.size());
    if(!checkValid(terms, jlist)){
        return eErrorParse;
    }

    this->terms_ = std::move(terms);
    this->polynomial_ = std::move(polynomial);
    this->jlist_ = std::move(jlist);
    
    return eNoError;
}

AST_NAMESPACE_END


