///
/// @file      NRLMSIS00.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-08
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
#include "Atmosphere.hpp"
#include <type_traits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class NRLMSISE;

/// @brief NRLMSIS00大气模型
class AST_CORE_API NRLMSIS00 final: public Atmosphere
{
public:
    NRLMSIS00(Frame* ecf, BodyShape* bodyShape, double f107Daily, double f107Average, double ap);
    ~NRLMSIS00() override;

    double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const override;

    Frame* getFrame() const override{return earthFixedFrame_;}

    /// @brief 是否使用近似高度计算
    bool useApproximateAltitude() const {return useApproximateAltitude_;}

    /// @brief 设置是否使用近似高度计算
    void setUseApproximateAltitude(bool useApproximateAltitude) {useApproximateAltitude_ = useApproximateAltitude;}

private:
    NRLMSISE& nrlmsise() const {return *reinterpret_cast<NRLMSISE*>(&storage_);}
private:
    mutable std::aligned_storage<624>::type storage_;
private:
    Frame* earthFixedFrame_{nullptr};
    BodyShape* bodyShape_{nullptr};
    double F107Daily_{0.0};
    double F107Average_{0.0};
    double ap_{0.0};
    bool useApproximateAltitude_{false};
};


/*! @} */

AST_NAMESPACE_END

