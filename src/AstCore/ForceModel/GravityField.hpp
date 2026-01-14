///
/// @file      GravityField.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstMath/LowerMatrix.hpp"
#include <string>

AST_NAMESPACE_BEGIN

class BKVParser;

class AST_CORE_API GravityField{
public:
    GravityField();
    ~GravityField() = default;

    const std::string& getModelName() const { return model_; }
    const std::string& getCentralBodyName() const { return centralBody_; }
    int getMaxDegree() const { return maxDegree_; }
    int getMaxOrder() const { return maxOrder_; }
    double getGM() const { return gm_; }
    double getRefDistance() const { return refDistance_; }
    bool isNormalized() const { return normalized_; }
    bool isIncludesPermTide() const { return includesPermTide_; }
    
    double getSnm(int n, int m) const;
    double getCnm(int n, int m) const;
    double getSnmNormalized(int n, int m) const;
    double getCnmNormalized(int n, int m) const;
    double getSnmUnnormalized(int n, int m) const;
    double getCnmUnnormalized(int n, int m) const;

    err_t load(StringView filepath);

protected:
    err_t loadSTK(BKVParser& parser);
    err_t loadGMAT(BKVParser& parser);
    double& snm(int n, int m);
    double& cnm(int n, int m);
    void initCoeffMatrices();
protected:
    int maxDegree_;
    int maxOrder_;
    std::string centralBody_;
    std::string model_;
    double gm_;
    double refDistance_;
    bool normalized_;
    bool includesPermTide_;

    LowerMatrixd sinCoeff_;
    LowerMatrixd cosCoeff_;
};

inline double &GravityField::snm(int n, int m)
{
    return sinCoeff_(n, m);
}

inline double &GravityField::cnm(int n, int m)
{
    return cosCoeff_(n, m);
}

inline double GravityField::getSnm(int n, int m) const
{
    return sinCoeff_(n, m);
}

inline double GravityField::getCnm(int n, int m) const
{
    return cosCoeff_(n, m);
}


AST_NAMESPACE_END
