///
/// @file      GravityField.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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

#include "GravityField.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/Logger.hpp"
#include <cmath>



AST_NAMESPACE_BEGIN

void aGravityFieldNormalize(GravityField &gf)
{
    gf.normalize();
}

void aGravityFieldUnnormalize(GravityField &gf)
{
    gf.unnormalize();
}

GravityField::GravityField()
    : maxDegree_(0)
    , maxOrder_(0)
    , gm_(0.0)
    , refDistance_(0.0)
    , normalized_(false)
    , includesPermTide_(false)
{

}

err_t GravityField::load(StringView filepath)
{
    BKVParser parser;
    parser.open(filepath);
    if(!parser.isOpen())
        return eErrorInvalidFile;
    StringView firstline = parser.getLine();
    if(firstline.starts_with("COMMENT") || filepath.ends_with(".cof"))
    {
        return loadGMAT(parser);
    }
    else if(firstline.starts_with("stk") || filepath.ends_with(".grv"))
    {
        return loadSTK(parser);
    }
    aError(
        "unsupported gravity field format, checking by first line: %s and filepath: %s", 
        firstline.data(), filepath.data()
    );
    return eErrorParse;
}

/// @brief 计算重力场系数的归一化因子
/// @param n 阶数 degree
/// @param m 次数 order
/// @return 归一化因子
static double gfNormalizeFactor(int n, int m)
{
    /*
    参考：航天器轨道力学理论与方法 附录C 公式C-1-3
    */
    int delta;
    if(m == 0)
        delta = 0;
    else
        delta = 1;

    double factor = 1.0;
    // if(m > 0){
        for(int i = n - m + 1; i <= n + m; i++)
            factor *= i;
    // }
    factor = sqrt(factor / ((1 + delta)*(2 * n + 1)) );
    return factor;
}

void GravityField::normalize()
{
    if(normalized_)
        return;
    normalized_ = true;
    for(int n = 0; n <= maxDegree_; n++)
    {
        for(int m = 0; m <= n; m++)
        {
            double factor = gfNormalizeFactor(n, m);
            snm(n, m) *= factor;
            cnm(n, m) *= factor;
        }
    }
}

void GravityField::unnormalize() 
{
    if(!normalized_)
        return;
    normalized_ = false;
    for(int n = 0; n <= maxDegree_; n++)
    {
        for(int m = 0; m <= n; m++)
        {
            double factor = gfNormalizeFactor(n, m);
            snm(n, m) /= factor;
            cnm(n, m) /= factor;
        }
    }
}

GravityField GravityField::normalized() const
{
    GravityField gf_normalized = *this;
    aGravityFieldNormalize(gf_normalized);
    return gf_normalized;
}

GravityField GravityField::unnormalized() const
{
    GravityField gf_unnormalized = *this;
    aGravityFieldUnnormalize(gf_unnormalized);
    return gf_unnormalized;
}

err_t GravityField::loadSTK(BKVParser &parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    GravityField gf;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin)
        {
            if(aEqualsIgnoreCase(item.value(), "Coefficients"))
            {
                gf.initCoeffMatrices();
                while(1)
                {
                    StringView line = parser.getLineSkipComment();
                    int degree, order;
                    double cnm, snm;
                    int status = sscanf(
                        line.data(), 
                        "%d %d %lf %lf", 
                        &degree, &order, &cnm, &snm
                    );
                    if(status == 4){
                        if(degree <= gf.maxDegree_ && order <= gf.maxOrder_)
                        {
                            gf.snm(degree, order) = snm;
                            gf.cnm(degree, order) = cnm;
                        }else{
                            aError(
                                "Invalid degree or order: %d %d, with max degree %d and max order %d", 
                                degree, order, gf.maxDegree_, gf.maxOrder_
                            );
                            return eErrorParse;
                        }
                    }else{
                        break;
                    }
                }
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            // pass
        }else if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Model"))
            {
                gf.model_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "CentralBody"))
            {
                gf.centralBody_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "Degree"))
            {
                gf.maxDegree_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "Order"))
            {
                gf.maxOrder_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "GM"))
            {
                gf.gm_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "RefDistance"))
            {
                gf.refDistance_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "Normalized"))
            {
                gf.normalized_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "IncludesPermTide"))
            {
                gf.includesPermTide_ = item.value().toBool();
            }
        }
    }while(token != BKVParser::EToken::eEOF);
    *this = std::move(gf);
    return eNoError;
}

err_t GravityField::loadGMAT(BKVParser &parser)
{
    GravityField gf;
    while(1)
    {
        StringView line = parser.getLine();
        if(line.empty()){
            break;
        }
        if(line[0] == 'C')
        {
            // COMMENT line
            continue;
        }
        else if(line.starts_with("POTFIELD"))
        {
            gf.maxDegree_ = aParseInt(line.substr(8, 3));
            gf.maxOrder_ = aParseInt(line.substr(11, 3));
            int cbFlag;
            double normalizedFlag;
            sscanf(line.data() + 14, "%d %lf %lf %lf", &cbFlag, &gf.gm_, &gf.refDistance_, &normalizedFlag);   
            gf.normalized_ = (normalizedFlag != 0.0);
            (void) cbFlag; // @todo: process cbFlag
            gf.initCoeffMatrices();
        }
        else if(line.starts_with("RECOEF"))
        {
            int degree = aParseInt(line.substr(8, 3));
            int order = aParseInt(line.substr(11, 3));
            double cnm, snm;
            int status;
            if(order == 0){
                status = sscanf(line.data() + 17, "%lf", &cnm);
                snm = 0;
            }else{
                status = sscanf(line.data() + 17, "%lf %lf", &cnm, &snm);
            }
            if(status == EOF){
                aError("Invalid coefficient line: %s", line.data());
                return eErrorParse;
            }
            if(degree <= gf.maxDegree_ && order <= gf.maxOrder_)
            {
                gf.snm(degree, order) = snm;
                gf.cnm(degree, order) = cnm;
            }else{
                aError(
                    "Invalid degree or order: %d %d, with max degree %d and max order %d", 
                    degree, order, gf.maxDegree_, gf.maxOrder_
                );
                return eErrorParse;
            }

        }
        else if(line.starts_with("END"))
        {
            // end of coefficients
        }
    }
    *this = std::move(gf);
    return eNoError;
}


void GravityField::initCoeffMatrices()
{
    int size = std::max(maxDegree_, maxOrder_) + 1;
    sinCoeff_.resize(size);
    cosCoeff_.resize(size);
    sinCoeff_.setZero();
    cosCoeff_.setZero();
}



AST_NAMESPACE_END

