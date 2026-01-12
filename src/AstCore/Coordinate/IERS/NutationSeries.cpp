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
#include "AstUtil/BKVParser.hpp"

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
        "j = %d %*[^0-9=] = %d", 
        &j, 
        &numTerms
    );
    if(status != 2){
        return eErrorParse;
    }
    return eNoError;
}

err_t NutationSeries::load(StringView filepath)
{
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        return eErrorInvalidFile;
    }
    

    return err_t();
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
        if(token == BKVParser::eBegin){
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
        }else if(token == BKVParser::eEnd){
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
    }while(token != BKVParser::eError);
    // printf("max_polyindex=%d, max_jindex=%d, num_points=%d\n", max_polyindex, max_jindex, num_points);
    // printf("%zu, %zu, %zu\n", polycoeffs.size(), jlist.size(), terms.size());
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
            }
        }
    }while(!parser.eof());
    
    // printf("%zu, %zu, %zu\n", polynomial.coeffs().size(), jlist.size(), terms.size());
    
    this->terms_ = std::move(terms);
    this->polynomial_ = std::move(polynomial);
    this->jlist_ = std::move(jlist);
    
    return eNoError;
}

AST_NAMESPACE_END


