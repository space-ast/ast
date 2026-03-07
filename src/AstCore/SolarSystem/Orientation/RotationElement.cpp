///
/// @file      RotationElement.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-20
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

#include "RotationElement.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Constants.h"
#include <cmath>


AST_NAMESPACE_BEGIN


double RotationElement::evaluate(double t_or_d,double t, double d) const
{
    double angle = 0;
    if(isSine_){
        for(auto& coeff : coefficients_){
            angle += coeff.amplitude_ * sin(coeff.constant_ + coeff.rate_ * d + coeff.rateDot_ * t * t);
        }
    }else{
        for(auto& coeff : coefficients_){
            angle += coeff.amplitude_ * cos(coeff.constant_ + coeff.rate_ * d + coeff.rateDot_ * t * t);
        }
    }
    angle += this->constant_ + this->rate_ * t_or_d + this->rateDot_ * t * t;
    return angle;
}

double RotationElement::evaluateZero() const
{
    double angle = 0;
    if(isSine_){
        for(auto& coeff : coefficients_){
            angle += coeff.amplitude_ * sin(coeff.constant_ );
        }
    }else{
        for(auto& coeff : coefficients_){
            angle += coeff.amplitude_ * cos(coeff.constant_ );
        }
    }
    angle += this->constant_;
    return angle;
}

double RotationElement::evaluatePoly(double t_or_d, double t) const
{
    return this->constant_ + this->rate_ * t_or_d + this->rateDot_ * t * t;
}

double RotationElement::evaluatePolyZero() const
{
    return this->constant_;
}

err_t RotationElement::load(BKVParser &parser)
{
    BKVItemView item;
    BKVParser::EToken token;
    RotationElement element{};
    int numOfTerms = 0;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Constant"))
            {
                element.constant_ = item.value().toDouble() * kDegToRad;
            }
            else if(aEqualsIgnoreCase(item.key(), "Rate"))
            {
                element.rate_ = item.value().toDouble() * kDegToRad;
            }
            else if(aEqualsIgnoreCase(item.key(), "RateDot")){
                element.rateDot_ = item.value().toDouble() * kDegToRad;
            }
            else if(aEqualsIgnoreCase(item.key(), "NumberOfTerms")){
                numOfTerms = item.value().toInt();
                if(numOfTerms < 0)
                {
                    aError("numberOfTerms must be greater than or equal to 0.");
                    return eErrorInvalidParam;
                }
                element.coefficients_.reserve(numOfTerms);
            }else if(aEqualsIgnoreCase(item.key(), "SineOrCosine")){
                if(aEqualsIgnoreCase(item.value(), "Sin")){
                    element.isSine_ = true;
                }else if(aEqualsIgnoreCase(item.value(), "Cos")){
                    element.isSine_ = false;
                }else{
                    aError("SineOrCosine must be Sine or Cosine.");
                    return eErrorInvalidParam;
                }
            }else if(aEqualsIgnoreCase(item.key(), "Coefficients")){
                // 解析系数
                for(int i=0;i<numOfTerms;i++){
                    StringView line = parser.getLine();
                    Coefficient coeff{};
                    int status = sscanf(line.data(), "%lf %lf %lf %lf", 
                        &coeff.amplitude_, &coeff.constant_, 
                        &coeff.rate_, &coeff.rateDot_
                    );
                    // 转换为弧度
                    coeff.amplitude_ *= kDegToRad;
                    coeff.constant_ *= kDegToRad;
                    coeff.rate_ *= kDegToRad;
                    coeff.rateDot_ *= kDegToRad;

                    if(status != 4){
                        aError("failed to parse coefficient line %d: %.*s", i, (int)line.size(), line.data());
                        return eErrorInvalidParam;
                    }
                    element.coefficients_.push_back(coeff);
                }
            }
        }
        else if(token == BKVParser::eBlockBegin)
        {
            
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "RotationalElement")){
                break;
            }
            else if(aEqualsIgnoreCase(item.value(), "Coefficients"))
            {
                // pass
            }
        }
    }while(token != BKVParser::eEOF);

#if 0
    // 检查rateDot是否为0
    {
        if(element.rateDot_ != 0.0){
            aError("currently, non-zero rateDot is not supported.");
            return eErrorInvalidParam;
        }
        for(const auto &coeff : element.coefficients_){
            if(coeff.rateDot_ != 0.0){
                aError("currently, non-zero rateDot in coefficient is not supported.");
                return eErrorInvalidParam;
            }
        }
    }
#endif

    *this = std::move(element);

    return eNoError;
}


AST_NAMESPACE_END

