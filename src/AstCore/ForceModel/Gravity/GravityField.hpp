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
#include "AstCore/TimePoint.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

class GravityField;
class BKVParser;

/// @brief 归一化重力场
/// @param gf 待归一化的重力场
AST_CORE_CAPI void aGravityFieldNormalize(GravityField& gf);

/// @brief 反归一化重力场
/// @param gf 待反归一化的重力场
AST_CORE_CAPI void aGravityFieldUnnormalize(GravityField& gf);

/// @brief 重力场头信息
/// @details 包含重力场的基本信息，如最大阶数、最大次数、中心天体名称等。
class AST_CORE_API GravityFieldHead
{
public:
    errc_t load(StringView filepath, StringView dirpath=StringView());
    
    /// @brief 获取重力场的中心天体引力常数
    double getGM() const { return gm_; }

    /// @brief 获取重力场的参考系名称
    const std::string& referenceFrame() const { return referenceFrame_; }

    /// @brief 获取重力场的参考半径
    double refDistance() const { return refDistance_; }
protected:
    int maxDegree_{0};                      ///< 最大阶数
    int maxOrder_{0};                       ///< 最大次数
    std::string centralBody_{};             ///< 中心天体名称
    std::string model_{};                   ///< 重力场模型名称
    std::string referenceFrame_{};          ///< 重力场的参考系名称：例如月球的重力场参考系 PrincipalAxes_421、PrincipalAxes_403等，默认为天体固连系
    double gm_{0};                          ///< 中心天体引力常数
    double refDistance_{0};                 ///< 参考距离
    bool normalized_{false};                ///< 是否归一化
    bool includesPermTide_{false};          ///< 是否包含永久潮汐: 若为true，则重力场为零潮汐模型(zero-tide)，保留了永久性隆起；反之则为无潮汐模型(tide-free)，不包含任何潮汐变形
};

/// @brief 重力场长期变化
class GravityFieldSecularVariations
{
public:
    struct Variation
    {
        bool isSin_{false};                ///< 是否是正弦项
        int degree_{0};                    ///< 阶数 n
        int order_{0};                     ///< 次数 m
        double linearRate_{0};             ///< 线性变化率[1/年]
        double originalCoefficient_{0};    ///< 原始系数
    };
public:
    bool normalized_{false};                 ///< 该系数是否是归一化系数
    TimePoint referenceEpoch_{};             ///< 参考时间点
    std::vector<Variation> variations_{};     ///< 长期变化系数列表
};

/// @brief 重力场系数
/// @details 包含重力场的头信息和系数矩阵，如Sn、Cn等。
class AST_CORE_API GravityField: protected GravityFieldHead
{
public:
    using SecularVariations = GravityFieldSecularVariations;
    GravityField();
    ~GravityField() = default;

    using GravityFieldHead::referenceFrame;
    using GravityFieldHead::refDistance;
    
    using GravityFieldHead::maxDegree_;
    using GravityFieldHead::maxOrder_;
    using GravityFieldHead::centralBody_;
    using GravityFieldHead::model_;
    using GravityFieldHead::referenceFrame_;
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
    void setModelName(StringView model) { model_ = std::string(model); }
    
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
    
    /// @brief 获取中心天体引力常数
    /// @return 中心天体引力常数
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

    /// @brief 施加永久潮汐C20修正（无潮汐 → 零潮汐）
    /// @param k20 2阶位Love数
    /// @note 仅对无潮汐模型(includesPermTide_==false)生效，已是零潮汐模型则不做任何操作。
    ///       **当前仅支持地球**，A₀和H₀为日月对地球的潮汐势参数，
    ///       非地球天体会发出警告并跳过修正。
    ///       修正公式: ΔC̄20 = A₀·H₀·k20
    ///       来源: IERS 2010 TN36 第6章 第6.2.2节 公式(6.14) (p.88)
    ///       其中 A₀=4.4228×10⁻⁸, H₀=−0.31460, k20名义值=0.30190 (表6.3)
    void applyPermanentTideC20Correction(double k20);

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

#ifndef SWIG // 还没解决好swig封装StringView的问题，暂时屏蔽

    /// @brief 从文件加载重力场
    /// @param model 重力场模型文件路径，或者模型名称
    /// @param dirpath 可选参数，重力场模型文件所在目录路径，默认从当前程序执行目录和地球目录中查找
    /// @return 加载状态
    errc_t load(StringView model, StringView dirpath=StringView{});

    /// @brief 从文件加载重力场
    /// @param model 重力场模型文件路径，或者模型名称
    /// @param maxLoadDegree 最大加载阶数
    /// @param maxLoadOrder 最大加载次数
    /// @param dirpath 可选参数，重力场模型文件所在目录路径，默认从当前程序执行目录和地球目录中查找
    /// @return 加载状态
    errc_t load(StringView model, int maxLoadDegree, int maxLoadOrder, StringView dirpath=StringView{});
    
#endif

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

    /// @brief 更新重力场系数变化
    /// @param epoch 时间点
    void updateVariations(const TimePoint& epoch);
    
    /// @brief 获取重力场长期变化
    SecularVariations& secularVariations() { return secularVariations_; }
public:
    double& snm(int n, int m);
    double& cnm(int n, int m);
    /// @brief 初始化系数矩阵
    void initCoeffMatrices();
protected:
    LowerMatrixd sinCoeff_{};                ///< Snm系数
    LowerMatrixd cosCoeff_{};                ///< Cnm系数
    SecularVariations secularVariations_{};  ///< 重力场长期变化
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
