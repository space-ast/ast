///
/// @file      MSISBase.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-25
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
#include "AtmosphereBase.hpp"
#include "AstCore/ConstantSpaceWeather.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include <type_traits>
#include <array>


typedef struct msisrecord msistype;
typedef struct lpolyrecord lpolytype;
typedef struct fitrecord fittype;
typedef struct lsqvrecord lsqvtype;



AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief MSIS86大气模型基类
class AST_CORE_API MSISBase : public AtmosphereBase
{
public:
    class WorkSpace;
    using ApArray = std::array<double, 7>;

    MSISBase(Frame* frame, BodyShape* bodyShape, double f107Daily, double f107Average, double ap);
    MSISBase(Frame* frame, BodyShape* bodyShape, SpaceWeatherProvider* spaceWeather);
    ~MSISBase();

    void setSpaceWeatherProvider(SpaceWeatherProvider* spaceWeather);
    void setConstantSpaceWeather(double f107Daily, double f107Average, double ap);
    bool useDailyAp() const { return useDailyAp_; }
    void setUseDailyAp(bool useDailyAp);

    static SpaceWeatherProvider* NewConstantSpaceWeather(double f107Daily, double f107Average, double ap);
protected:
    WorkSpace& workSpace() const { return *reinterpret_cast<WorkSpace*>(&storage_); }
    msistype& msis() const;
    lpolytype& lpoly() const;
    fittype& fit() const;
    lsqvtype& lsqv() const;
protected:
    struct MSISParam
    {
        int dayOfYear;
        double secOfDay;
        double lst;
        double alt; // [km]
        double lat; // [deg]
        double lon; // [deg]
        double f107A;
        double f107;
        ApArray ap;
        static constexpr int mass = 48;
    };
    static void getTimeParam(const TimePoint& tp, double lon, int& dayOfYear, double& secOfDay, double& lst);
    void getMSISParam(const TimePoint& tp, const Vector3d& posInBodyFixed, MSISParam& param) const;
    void getSpaceWeather(const TimePoint& tp, double& f107, double& f107Average, ApArray& aparray) const;
protected:
    ScopedPtr<SpaceWeatherProvider> spaceWeather_;    ///< 空间天气数据源
private:
    mutable std::aligned_storage<57000>::type storage_{};
    bool useDailyAp_{false};                        ///< 是否使用每日Ap值，否则使用3小时Ap值
};

/*! @} */

AST_NAMESPACE_END
