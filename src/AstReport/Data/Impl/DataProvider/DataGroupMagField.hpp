///
/// @file      DataGroupMagField.hpp
/// @brief     地磁场数据组
/// @details   IGRF 地磁场参数（NEZ、派生量、L壳、ECF/ECI 各坐标系矢量分量）
/// @author    axel
/// @date      2026-08-11
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
#include "AstReport/DataGroupTimeVar.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/GeodeticPoint.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Span.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


/// @brief 地磁场数据组
/// @details 报告卫星位置处 IGRF 地磁场参数
///          内部存储一律使用国际单位制（T、rad、m）。
class AST_REPORT_API DataGroupMagField : public DataGroupTimeVar
{
public:
    struct Data
    {
        TimePoint time_{};

        // -- NEZ 当地分量 [T] --
        double totalIntensityF_{};     // F  = 总强度
        double northIntensityX_{};     // X  = 北向分量
        double eastIntensityY_{};      // Y  = 东向分量
        double verticalIntensityZ_{};  // Z  = 垂直向下分量

        // -- 推导标量 --
        double horizontalIntensityH_{}; // H = √(X²+Y²) [T]
        double declinationD_{};         // D = atan2(Y,X) [rad]
        double inclinationI_{};         // I = atan2(Z,H) [rad]  向下为正

        // -- L 壳参数 [无量纲] --
        double dipoleL_{};      // Ldip  = 偶极子 L
        double mcIlwainL_{};    // Lm    = McIlwain L
        double absMcIlwainL_{}; // |Lm|

        // -- 磁镜比 --
        double bOverBeq_{};     // B/Beq [无量纲]
        double beq_{};          // Beq [T]  赤道磁场强度

        // -- ECF 矢量分量 [T] --
        double ecfX_{};
        double ecfY_{};
        double ecfZ_{};

        // -- ECI 矢量分量 [T] --
        double eciX_{};
        double eciY_{};
        double eciZ_{};

        // ---- getter ----
        const TimePoint& getTime()                  const { return time_; }
        double           getTotalIntensityF()       const { return totalIntensityF_; }
        double           getNorthIntensityX()       const { return northIntensityX_; }
        double           getEastIntensityY()        const { return eastIntensityY_; }
        double           getVerticalIntensityZ()    const { return verticalIntensityZ_; }
        double           getHorizontalIntensityH()  const { return horizontalIntensityH_; }
        double           getDeclinationD()          const { return declinationD_; }
        double           getInclinationI()          const { return inclinationI_; }
        double           getDipoleL()               const { return dipoleL_; }
        double           getMcIlwainL()             const { return mcIlwainL_; }
        double           getAbsMcIlwainL()          const { return absMcIlwainL_; }
        double           getBOverBeq()              const { return bOverBeq_; }
        double           getBeq()                   const { return beq_; }
        double           getEcfX()                  const { return ecfX_; }
        double           getEcfY()                  const { return ecfY_; }
        double           getEcfZ()                  const { return ecfZ_; }
        double           getEciX()                  const { return eciX_; }
        double           getEciY()                  const { return eciY_; }
        double           getEciZ()                  const { return eciZ_; }
    };
    static DataElements Elements();

    DataGroupMagField();
    ~DataGroupMagField() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
    const DataElements& getElements() const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint() const { return point_.get(); }
    Body*  getBody()  const { return body_.get(); }
    void   setPoint(Point* p) { point_ = p; }
    void   setBody(Body* b)   { body_ = b; }
private:
    WeakPtr<Point> point_{};
    WeakPtr<Body>  body_{};
};


/*! @} */

AST_NAMESPACE_END
