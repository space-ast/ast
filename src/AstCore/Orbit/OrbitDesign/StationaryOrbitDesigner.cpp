///
/// @file      StationaryOrbitDesigner.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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

#include "StationaryOrbitDesigner.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstCore/OrbitDesign.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/Zeros.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

using namespace literals;

StationaryOrbitDesigner::StationaryOrbitDesigner()
    : StationaryOrbitDesigner(getDefaultCelestialBody())
{

}

StationaryOrbitDesigner::StationaryOrbitDesigner(CelestialBody *body) 
    : BaseOrbitDesigner(body)
    , subsatellitePoint_(-100_deg)
    , inclination_(0_deg)
{
    if(body && !body->isEarth())
    {
        aError("StationaryOrbitDesigner currently only support Earth.");
    }
}

err_t StationaryOrbitDesigner::getOrbitState(ModOrbElem &orbElem) const
{
    const double gm = getGM();
    const double j2 = getJ2();
    const double rb = getBodyRadius();
    
    double period = 0;
    if(celestialBody_ && celestialBody_->isEarth()){
        #if 0
        // @fixme 这里应该计算TOD到GTOD的旋转速率
        KinematicRotation rot;
        aICRFToECFTransform(orbitEpoch_, rot);
        double rotRate = rot.getRotationRate().norm();
        period = kTwoPI / rotRate;
        #else
        period = 86164.09054;
        #endif
    }else{
        /// @todo 目前只能计算地球轨道，这里需要增加对其他天体的支持
    }
    
    // const double inc = inclination_;
    auto func = [gm, j2, rb, period](double a) -> double
    {
        /*!
        @note 
        如果地球同步轨道倾角不为0，根据轨道根数长期摄动项，其升交点赤经一定存在漂移率
        在这种情况下，即使是在理想的J2长期摄动模型下，也无法始终保持在某点上空静止

        下面只考虑倾角为0和偏心率为0的情况，
        根据轨道根数长期摄动项，此时J2项的raan和meanA的漂移率相互抵消
        所以下面公式中仅通过argper的漂移率来计算周期

        @todo 这里应该可以采用解析公式直接计算得到半长轴
        */
       
        const double ecc = 0.0;
        const double inc = 0.0;
        double n = aSMajAxToMeanMotn(a, gm);
        double p = kTwoPI / ( n + aArgPerRate(gm, j2, rb, a, ecc, inc));
        return p - period;
    };
    SolverStats stats{};
    double atwobody = aPeriodToSMajAx(period, gm);
    double a = brentq(func, 1, atwobody * 1.5, 1e-12, 1e-12, 100, stats);

    if(stats.error_num == 0){
        orbElem.rp_ = a;
        orbElem.e_ = 0.0;
        orbElem.i_ = inclination_;
        orbElem.raan_ = 0_deg;
        orbElem.argper_ = 0_deg;
        orbElem.trueA_ = 0_deg;
        return eNoError;
    }else{
        aError("StationaryOrbitDesigner failed to solve semimajor axis.");
        return -1;
    }
}

AST_NAMESPACE_END

