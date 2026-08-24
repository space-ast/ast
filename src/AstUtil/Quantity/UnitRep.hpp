///
/// @file      UnitRep.hpp
/// @brief     单位表示抽象基类
/// @details   
/// @author    axel
/// @date      2026-08-21
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "Dimension.hpp"
#include <memory>
#include <vector>
#include <cmath>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class UnitRep;

enum class EUnitKind: uint8_t{
    eScale,
    eAffine,
    eLogarithmic
};


using UnitRepHandle = std::shared_ptr<UnitRep>;                     ///< 单位表示句柄
using UnitRepPair = std::pair<UnitRepHandle, int>;                  ///< 单位表示子项
using SubUnitList = std::vector<UnitRepPair>;                       ///< 单位表示子项列表
using UnitRepHandleConst = std::shared_ptr<const UnitRep>;          ///< 单位表示句柄（常量）
using UnitRepPairConst = std::pair<UnitRepHandleConst, int>;        ///< 单位表示子项（常量）
using SubUnitListConst = std::vector<UnitRepPairConst>;             ///< 单位表示子项列表（常量）


/// @brief 单位表示
/// @warning 单位表示结构体为项目内部使用，不建议外部直接调用。
class UnitRep{
public:
    UnitRep(EUnitKind kind, StringView name, Dimension dimension, const SubUnitListConst& subunits)
        : kind_{kind}
        , name_{std::string(name)}
        , dimension_{dimension}
        , subUnits_{subunits}
    {}
    UnitRep(EUnitKind kind, StringView name, Dimension dimension)
        : kind_{kind}
        , name_{std::string(name)}
        , dimension_{dimension}
        , subUnits_{}
    {}
    EUnitKind            kind_;          ///< 单位表示类型
    std::string          name_;          ///< 名称
    Dimension            dimension_;     ///< 量纲
    SubUnitListConst     subUnits_;      ///< 子单位列表

    bool operator==(const UnitRep& other) const;
public:
    virtual ~UnitRep() = default;
    virtual double toSI(double value) const = 0;
    virtual double fromSI(double value) const = 0;
    /// @brief 深拷贝单位表示(独立副本)
    virtual UnitRepHandle clone() const = 0;
public:
    EUnitKind kind() const { return kind_; }
    double scale() const;
    bool valid() const;
};


/// 乘法型单位表示: SI = v * scale
class ScaleUnitRep final: public UnitRep
{
public:
    using UnitRep::UnitRep;
    ScaleUnitRep(StringView name, Dimension dimension, double scale)
        : UnitRep(EUnitKind::eScale, name, dimension)
        , scale_{scale}
    {}
    ScaleUnitRep(StringView name, Dimension dimension, const SubUnitListConst& subunits, double scale)
        : UnitRep(EUnitKind::eScale, name, dimension, subunits)
        , scale_{scale}
    {}
public:
    double toSI(double value) const override { return value * scale_;}
    double fromSI(double value) const override { return value / scale_;}
    double scale_{1.0};    ///< 单位缩放因子
    double notused_{0.0};  ///< 未使用参数
public:
    UnitRepHandle clone() const override { return std::make_shared<ScaleUnitRep>(*this); }
};


/// 仿射型单位表示: SI = v * scale + offset
class AffineUnitRep : public UnitRep {
public:
    using UnitRep::UnitRep;
    AffineUnitRep(StringView name, Dimension dimension, double scale, double offset)
        : UnitRep(EUnitKind::eAffine, name, dimension)
        , scale_{scale}
        , offset_{offset}
    {}
public:
    double toSI(double value) const override { return value * scale_ + offset_;}
    double fromSI(double value) const override { return (value - offset_) / scale_;}
    double scale_{1.0};    ///< 单位缩放因子
    double offset_{0.0};   ///< 单位偏移量
public:
    UnitRepHandle clone() const override { return std::make_shared<AffineUnitRep>(*this); }
};


/// 对数型单位表示: SI = reference * 10^(v / factor)
// @todo 未校验 reference/factor 参数：reference<=0 或 factor<=0 会使 toSI/fromSI 输出异常，应加校验并返回错误。
class LogarithmicUnitRep : public UnitRep {
public:
    using UnitRep::UnitRep;
    LogarithmicUnitRep(StringView name, Dimension dimension, double reference, double factor)
        : UnitRep(EUnitKind::eLogarithmic, name, dimension)
        , reference_{reference}
        , factor_{factor}
    {}
public:
    double toSI(double value) const override { return reference_ * std::pow(10.0, value / factor_);}
    double fromSI(double value) const override { return std::log10(value / reference_) * factor_;}
    double reference_{1.0};    ///< 参考值
    double factor_{1.0};       ///< 缩放因子
public:
    UnitRepHandle clone() const override { return std::make_shared<LogarithmicUnitRep>(*this); }
};


inline bool UnitRep::operator==(const UnitRep &rep) const
{
    /*
    单位表示是否相等：比较量纲 + 单位类型 + 两个double成员变量是否相等
    前置条件：必须保证各个单位表示类型的内存大小一致，两个double成员变量的偏移量一致
    该前置条件通过编译期检查(static_assert)来保证
    */
    auto self = static_cast<const ScaleUnitRep*>(this);
    auto other = static_cast<const ScaleUnitRep*>(&rep);
    return dimension_ == other->dimension_ && kind_ == other->kind_ && self->scale_ == other->scale_ && self->notused_ == other->notused_;
}

inline double UnitRep::scale() const
{
    if(kind_ == EUnitKind::eScale){
        return static_cast<const ScaleUnitRep*>(this)->scale_;
    }
    else if(kind_ == EUnitKind::eAffine){
        return static_cast<const AffineUnitRep*>(this)->scale_;
    }
    // @todo 这里需要考虑返回 0 还是 nan 更合适？
    return 0.0;
}

inline bool UnitRep::valid() const
{
    if(kind_ == EUnitKind::eScale){
        return static_cast<const ScaleUnitRep*>(this)->scale_ != 0.0;
    }
    else if(kind_ == EUnitKind::eAffine){
        return static_cast<const AffineUnitRep*>(this)->scale_ != 0.0;
    }
    else //  if(kind_ == EUnitKind::Logarithmic){
    {
        return true;
    }
    // return false;
}

/*! @} */

AST_NAMESPACE_END
