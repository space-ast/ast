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
#include "AstUtil/StringView.hpp"
#include <string>

AST_NAMESPACE_BEGIN

class GravityField;
class BKVParser;

/// @brief 归一化重力场
/// @param gf 待归一化的重力场
void aGravityFieldNormalize(GravityField& gf);

/// @brief 反归一化重力场
/// @param gf 待反归一化的重力场
void aGravityFieldUnnormalize(GravityField& gf);

/// @brief 重力场头信息
/// @details 包含重力场的基本信息，如最大阶数、最大次数、中心天体名称等。
class AST_CORE_API GravityFieldHead
{
public:
    err_t load(StringView filepath);
public:
    int maxDegree_{0};                      ///< 最大阶数
    int maxOrder_{0};                       ///< 最大次数
    std::string centralBody_;               ///< 中心天体名称
    std::string model_;                     ///< 重力场模型名称
    double gm_{0};                          ///< 中心天体重力常数
    double refDistance_{0};                 ///< 参考距离
    bool normalized_{false};                ///< 是否归一化
    bool includesPermTide_{false};          ///< 是否包含潮汐
};


/// @brief 重力场系数
/// @details 包含重力场的头信息和系数矩阵，如Sn、Cn等。
class AST_CORE_API GravityField: protected GravityFieldHead
{
public:
    GravityField();
    ~GravityField() = default;

    using GravityFieldHead::maxDegree_;
    using GravityFieldHead::maxOrder_;
    using GravityFieldHead::centralBody_;
    using GravityFieldHead::model_;
    using GravityFieldHead::gm_;
    using GravityFieldHead::refDistance_;
    using GravityFieldHead::normalized_;
    using GravityFieldHead::includesPermTide_;


    /// @brief 获取重力场头信息
    /// @return 重力场头信息
    const GravityFieldHead& getHead() const { return *this; }

    /// @brief 获取重力场模型名称
    /// @return 重力场模型名称
    const std::string& getModelName() const { return model_; }
    void setModelName(StringView model) { model_ = model.to_string(); }
    
    /// @brief 获取中心天体名称
    /// @return 中心天体名称
    const std::string& getCentralBodyName() const { return centralBody_; }

    /// @brief 获取最大阶数
    /// @return 最大阶数
    /// @return 最大阶数
    int getMaxDegree() const { return maxDegree_; }
    /// @brief 获取最大次数
    /// @return 最大次数
    int getMaxOrder() const { return maxOrder_; }

    /// @brief 检查阶数和次数是否有效
    /// @param degree 阶数
    /// @param order 次数
    /// @return 是否有效
    bool isValidDegreeOrder(int degree, int order) const;
    
    /// @brief 获取中心天体重力常数
    /// @return 中心天体重力常数
    double getGM() const { return gm_; }

    /// @brief 获取参考距离
    /// @return 参考距离
    double getRefDistance() const { return refDistance_; }

    /// @brief 是否归一化
    /// @return 是否归一化
    bool isNormalized() const { return normalized_; }
    
    /// @brief 是否包含潮汐
    /// @return 是否包含潮汐
    bool isIncludesPermTide() const { return includesPermTide_; }
    
    /// @brief 获取Snm系数
    /// @param n 阶数
    /// @param m 次数
    /// @return Snm系数
    double getSnm(int n, int m) const;
    
    /// @brief 获取Cnm系数
    /// @param n 阶数
    /// @param m 次数
    /// @return Cnm系数
    double getCnm(int n, int m) const;
    
    double getSnmNormalized(int n, int m) const;
    double getCnmNormalized(int n, int m) const;
    double getSnmUnnormalized(int n, int m) const;
    double getCnmUnnormalized(int n, int m) const;

    /// @brief 获取Jn项
    double getJn(int n) const;

    /// @brief 从文件加载重力场
    /// @param model 重力场模型文件路径，或者模型名称
    /// @return 加载状态
    err_t load(StringView model);

    /// @brief 从文件加载重力场
    /// @param model 重力场模型文件路径，或者模型名称
    /// @param maxLoadDegree 最大加载阶数
    /// @param maxLoadOrder 最大加载次数
    /// @return 加载状态
    err_t load(StringView model, int maxLoadDegree, int maxLoadOrder);
    
    /// @brief 归一化重力场
    void normalize();

    /// @brief 反归一化重力场
    void unnormalize() ;

    /// @brief 获取归一化后的重力场
    /// @return 归一化后的重力场
    GravityField normalized() const;

    /// @brief 获取反归一化后的重力场
    /// @return 反归一化后的重力场
    GravityField unnormalized() const;
    
public:
    double& snm(int n, int m);
    double& cnm(int n, int m);
    /// @brief 初始化系数矩阵
    void initCoeffMatrices();
protected:
    
    LowerMatrixd sinCoeff_;         ///< Snm系数
    LowerMatrixd cosCoeff_;         ///< Cnm系数    
};

inline double &GravityField::snm(int n, int m)
{
    return sinCoeff_(n, m);
}

inline double &GravityField::cnm(int n, int m)
{
    return cosCoeff_(n, m);
}

inline bool GravityField::isValidDegreeOrder(int degree, int order) const
{
    return degree <= this->getMaxDegree() && order <= this->getMaxOrder() 
            && degree >= order && order >= 0 && degree >= 0;
}

inline double GravityField::getSnm(int n, int m) const
{
    if(A_LIKELY(isValidDegreeOrder(n, m)))
        return sinCoeff_(n, m);
    return 0;
}

inline double GravityField::getCnm(int n, int m) const
{
    if(A_LIKELY(isValidDegreeOrder(n, m)))
        return cosCoeff_(n, m);
    return 0;
}


AST_NAMESPACE_END
