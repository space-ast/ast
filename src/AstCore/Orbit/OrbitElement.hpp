///
/// @file      OrbitElement.hpp
/// @brief     轨道根数定义及转换接口
/// @details   提供各种轨道根数（直角坐标、经典轨道根数、修正轨道根数、春分点根数、改进春分点轨道根数）之间的转换函数。
/// @author    axel
/// @date      19.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include "AstCore/Vector.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/Constants.h"
#include <string>
 
AST_NAMESPACE_BEGIN

/*!
    @ingroup   Core
    @defgroup Orbit 轨道计算
    @brief 提供轨道根数、轨道参数计算及转换相关接口。
*/

/*!
    @addtogroup Orbit
    @{
*/


/// @brief 直角坐标
class CartState
{
public:
    Vector3d pos_;   ///< 位置
    Vector3d vel_;   ///< 速度
public:
    /// @brief 获取零状态
    /// @return 零状态
    static CartState Zero() {return CartState{Vector3d::Zero(), Vector3d::Zero()};}

    A_DEF_POD_ITERABLE(double)
    
    /// @brief 获取位置
    /// @return 位置
    const Vector3d& pos() const {return pos_;} 
    Vector3d& pos() {return pos_;}

    /// @brief 获取速度
    /// @return 速度
    const Vector3d& vel() const {return vel_;} 
    Vector3d& vel() {return vel_;}

    double& x() {return pos_.x();}
    double& y() {return pos_.y();}
    double& z() {return pos_.z();}
    double& vx() {return vel_.x();}
    double& vy() {return vel_.y();}
    double& vz() {return vel_.z();}

    double x() const {return pos_.x();}
    double y() const {return pos_.y();}
    double z() const {return pos_.z();}
    double vx() const {return vel_.x();}
    double vy() const {return vel_.y();}
    double vz() const {return vel_.z();}

    /// @brief 转换为字符串
    AST_CORE_API
    std::string toString() const;
};


/// @brief 经典轨道根数
class OrbElem
{
public:
    double	 a_;          ///< 长半轴 
    double	 e_;          ///< 偏心率
    double	 i_;          ///< 轨道倾角 
    double	 raan_;       ///< 升交点赤经 
    double	 argper_;     ///< 近拱点角
    double	 trueA_;      ///< 真近点角
public:
    /// @brief 计算平均角速度变化率
    double getMeanMotion(double gm) const {return aSMAToMeanMotion(getSMA(), gm);}

    double getSMA() const {return a_;}

    double getA() const {return a_;}

    double getE() const {return e_;}

    double getI() const {return i_;}

    double getRAAN() const {return raan_;}

    double getArgPer() const {return argper_;}

    double getTrueA() const {return trueA_;}

    /// @brief 转换为字符串
    AST_CORE_API
    std::string toString() const;
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, a)
    AST_DEF_ACCESS_METHOD(double, e)
    AST_DEF_ACCESS_METHOD(double, i)
    AST_DEF_ACCESS_METHOD(double, raan)
    AST_DEF_ACCESS_METHOD(double, argper)
    AST_DEF_ACCESS_METHOD(double, trueA)
};


/// @brief 修正轨道根数
class ModOrbElem
{
public:
    double rp_;         ///<近拱点半径 
    double e_;          ///<偏心率 
    double i_;          ///<轨道倾角
    double raan_;       ///<升交点赤经
    double argper_;     ///<近拱点角
    double trueA_;      ///<真近点角
public:
    /// @brief 计算半长轴
    double getSMA() const {return rp_ / (1 - e_);}
    
    /// @brief 计算平均角速度
    double getMeanMotion(double gm) const {return aPeriRadToMeanMotion(getPeriRad(), getEcc(), gm);}

    /// @brief 计算周期
    double getPeriod(double gm) const {return kTwoPI / getMeanMotion(gm);}

    /// @brief 计算远拱点半径
    double getApoRad() const{return aPeriRadToApoRad(getPeriRad(), getEcc());}

    /// @brief 计算远拱点高度
    double getApoAlt(double bodyRadius) const{ return aPeriRadToApoAlt(getPeriRad(), getEcc(), bodyRadius);}

    /// @brief 计算近拱点半径
    double getPeriRad() const {return rp_;}

    /// @brief 计算近拱点高度
    double getPeriAlt(double bodyRadius) const{ return getPeriRad() - bodyRadius;}

    /// @brief 计算偏心率
    double getEcc() const {return e_;}

    /// @brief 计算轨道倾角
    double getInc() const {return i_;}

    /// @brief 计算升交点赤经
    double getRAAN() const {return raan_;}

    /// @brief 计算近拱点幅角
    double getArgPeri() const {return argper_;}

    /// @brief 计算真近点角
    double getTrueAnomaly() const {return trueA_;}

    /// @brief 计算平近点角
    double getMeanAnomaly() const {return aTrueToMean(getTrueAnomaly(), getEcc());}

    /// @brief 计算偏近点角
    double getEccAnomaly() const {return aTrueToEcc(getTrueAnomaly(), getEcc());}

    /// @brief 计算纬度幅角
    double getArgLat() const {return aTrueToArgLat(getTrueAnomaly(), getArgPeri());}

    /// @brief 计算过近地点后经过的时间
    double getTimePastPeri(double gm) const{return aTrueToTimePastPeri(getTrueAnomaly(), getSMA(), getEcc(), gm);}

    /// @brief 计算过升交点后经过的时间
    double getTimePastAscNode(double gm) const{return aTrueToTimePastAscNode(getTrueAnomaly(), getArgPeri(), getSMA(), getEcc(), gm);}
    
    /// @brief 计算过升交点时刻
    TimePoint getTimeOfAscNodePassage(const TimePoint& stateEpoch, double gm) const
    {
        return stateEpoch - getTimePastAscNode(gm);
    }

    /// @brief 计算升交点经度
    double getLAN(Axes* inertialAxes, const TimePoint& timeOfAscNodePassage, Axes* bodyFixedAxes) const
    {
        return aRAANToLAN(getRAAN(), inertialAxes, timeOfAscNodePassage, bodyFixedAxes);
    }
    
public:
    /// @brief 计算半长轴
    double getA() const {return getSMA();}

    /// @brief 计算半通径
    double getP() const {return rp_ * (1 + e_);}

    double getE() const {return getEcc();}

    double getI() const {return getInc();}

    double getArgPer() const {return getArgPeri();}

    double getTrueA() const {return getTrueAnomaly();}

    /// @brief 转换为字符串
    AST_CORE_API
    std::string toString() const;
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, rp)
    AST_DEF_ACCESS_METHOD(double, e)
    AST_DEF_ACCESS_METHOD(double, i)
    AST_DEF_ACCESS_METHOD(double, raan)
    AST_DEF_ACCESS_METHOD(double, argper)
    AST_DEF_ACCESS_METHOD(double, trueA)
};


/// @brief 春分点根数
class EquinElem
{
public:
    double a_;          ///< semimajor axis length
    double h_;          ///< e*sin(argper + raan)   omegabar=argper + raan
    double k_;          ///< e*cos(argper + raan)
    double p_;          ///< tan(i/2)*sin(raan)
    double q_;          ///< tan(i/2)*cos(raan)
    double lambda_;     ///< mean longitude = M + raan + argper
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, a)
    AST_DEF_ACCESS_METHOD(double, h)
    AST_DEF_ACCESS_METHOD(double, k)
    AST_DEF_ACCESS_METHOD(double, p)
    AST_DEF_ACCESS_METHOD(double, q)
    AST_DEF_ACCESS_METHOD(double, lambda)
};

/// @brief 改进春分点轨道根数, 180度奇异
class ModEquinElem
{
public:
    double p_;       ///< p = a(1-e^2) 半通径
    double f_;        ///< f = e*cos(argper+RAAN)
    double g_;        ///< g = e*sin(argper+RAAN)
    double h_;        ///< h = tan(i/2)cos(RAAN)
    double k_;        ///< k = tan(i/2)sin(RAAN)
    double L_;        ///< L = RAAN + argper + trueA
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, p)
    AST_DEF_ACCESS_METHOD(double, f)
    AST_DEF_ACCESS_METHOD(double, g)
    AST_DEF_ACCESS_METHOD(double, h)
    AST_DEF_ACCESS_METHOD(double, k)
    AST_DEF_ACCESS_METHOD(double, L)
};



/// @brief 德洛奈根数 
/// @details 
/// 德洛奈根数是天体力学中的一组正则共轭变量
/// 由法国学者Delaunay,C.E.于19世纪60年代研究月球运动时提出
/// 常用于解析轨道理论
/// 依据：
///     《英汉天文学名词》李竞、许邦信主编，上海科技教育出版社，2000
///     《英汉天文学名词》，中国科学技术出版社，2015
class DelaunayElem
{

public:
    double L_;
    double G_;
    double H_;
    double l_;
    double g_;
    double h_;
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, L)
    AST_DEF_ACCESS_METHOD(double, G)
    AST_DEF_ACCESS_METHOD(double, H)
    AST_DEF_ACCESS_METHOD(double, l)
    AST_DEF_ACCESS_METHOD(double, g)
    AST_DEF_ACCESS_METHOD(double, h)
};


/// @brief 经典轨道根数转换为直角坐标
/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t coe2rv(const double* coe, double gm, double* pos, double* vel);

/// @brief 经典轨道根数转换为改进春分点轨道根数
/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t coe2mee(const double* coe, double* mee);

/// @brief 春分点根数转换为直角坐标
/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
AST_CORE_CAPI void ee2rv(const double* ee, double gm, double* pos, double* vel);

/// @brief 改进春分点轨道根数转换为直角坐标
/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
AST_CORE_CAPI void mee2rv(const double* mee, double gm, double* pos, double* vel);

/// @brief 直角坐标转换为改进春分点轨道根数
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t rv2mee(const double* pos, const double* vel, double gm, double* mee);

/// @brief 改进春分点轨道根数转换为经典轨道根数
/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]
/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t mee2coe(const double* mee, double* coe);

/// @brief 直角坐标转换为春分点根数
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]
AST_CORE_CAPI void rv2ee(const double* pos, const double* vel, double gm, double* ee);

/// @brief 直角坐标转换为修正轨道根数
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t rv2moe(const double* pos, const double* vel, double gm, double* moe);

/// @brief 直角坐标转换为经典轨道根数
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t rv2coe(const double* pos, const double* vel, double gm, double* coe);

/// @brief 春分点根数转换为修正轨道根数
/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]
/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t ee2moe(const double* ee, double* moe);

/// @brief 修正轨道根数转换为春分点根数
/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t moe2ee(const double* moe, double* ee);

/// @brief 修正轨道根数转换为经典轨道根数
/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
AST_CORE_CAPI errc_t moe2coe(const double* moe, double* coe);

/// @brief 经典轨道根数转换为修正轨道根数
/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
AST_CORE_CAPI void coe2moe(const double* coe, double* moe);

/// @brief 修正轨道根数转换为直角坐标
/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
AST_CORE_CAPI errc_t moe2rv(const double* moe, double gm, double* pos, double* vel);

/// @brief 修正轨道根数转换为改进春分点轨道根数
/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]
AST_CORE_CAPI void moe2mee(const double* moe, double* mee);

/// @brief 经典轨道根数转换为春分点根数
/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]
AST_CORE_CAPI void coe2ee(const double* coe, double* ee);

/// @brief 春分点根数转换为经典轨道根数
/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]
/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
AST_CORE_CAPI void ee2coe(const double* ee, double* coe);

/// @brief 春分点根数转换为改进春分点轨道根数
/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]
/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]
AST_CORE_CAPI void ee2mee(const double* ee, double* mee);

/// @brief 改进春分点轨道根数转换为春分点根数
/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]
/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]
AST_CORE_CAPI void mee2ee(const double* mee, double* ee);

/// @brief 改进春分点轨道根数转换为修正轨道根数
/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]
/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
AST_CORE_CAPI void mee2moe(const double* mee, double* moe);


/// @brief 经典轨道根数转换为德洛奈根数
/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param gm 引力参数 [m^3/s^2]
/// @param del 输出德洛奈根数, see DelaunayElem
AST_CORE_CAPI errc_t coe2dela(const double* coe, double gm, double* dela);


/// @brief 德洛奈根数转换为经典轨道根数
/// @param del 德洛奈根数, see DelaunayElem
/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
AST_CORE_CAPI errc_t dela2coe(const double* dela, double gm, double* coe);


/// @brief 改进春分点轨道根数转换为直角坐标（类引用版本）
/// @param mee 改进春分点轨道根数
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
AST_CORE_CAPI void aModEquinElemToCart(
    const ModEquinElem& mee, 
    double gm, 
    Vector3d& pos, 
    Vector3d& vel
);

/// @brief 直角坐标转换为改进春分点轨道根数（类引用版本）
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param mee 输出改进春分点轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t aCartToModEquinElem(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    ModEquinElem& mee
);

/// @brief 经典轨道根数转换为改进春分点轨道根数（类引用版本）
/// @param elem 经典轨道根数
/// @param mee 输出改进春分点轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t aOrbElemToModEquinElem(
    const OrbElem& elem,
    ModEquinElem& mee
);

/// @brief 改进春分点轨道根数转换为经典轨道根数（类引用版本）
/// @param mee 改进春分点轨道根数
/// @param elem 输出经典轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t aModEquinElemToOrbElem(
    const ModEquinElem& mee,
    OrbElem& elem
);

/// @brief 直角坐标转换为修正轨道根数（类引用版本）
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param modOrb 输出修正轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t	aCartToModOrbElem(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    ModOrbElem& modOrb);

/// @brief 直角坐标转换为经典轨道根数（类引用版本）
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param elem 输出经典轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI
errc_t aCartToOrbElem     (
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    OrbElem& elem);

/// @brief 春分点根数转换为修正轨道根数（类引用版本）
/// @param equinElem 春分点根数
/// @param modOrb 输出修正轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI
errc_t aEquinElemToModOrb (
    const EquinElem& equinElem,
    ModOrbElem& modOrb);

/// @brief 修正轨道根数转换为春分点根数（类引用版本）
/// @param modOrb 修正轨道根数
/// @param equinElem 输出春分点根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI
errc_t aModOrbToEquinElem (
    const ModOrbElem& modOrb,
    EquinElem& equinElem);

/// @brief 修正轨道根数转换为直角坐标（类引用版本）
/// @param modOrb 修正轨道根数
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
AST_CORE_CAPI
errc_t aModOrbElemToCart  (
    const ModOrbElem& modOrb,
    double gm,
    Vector3d& pos,
    Vector3d& vel);

/// @brief 经典轨道根数转换为直角坐标（类引用版本）
/// @param elem 经典轨道根数
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI
errc_t aOrbElemToCart     (
    const OrbElem& elem,
    double gm,
    Vector3d& pos,
    Vector3d& vel);

/// @brief 直角坐标转换为春分点根数（类引用版本）
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param equinElem 输出春分点根数
AST_CORE_CAPI
void	aCartToEquinElem(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    EquinElem& equinElem);

/// @brief 春分点根数转换为直角坐标（类引用版本）
/// @param equinElem 春分点根数
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
AST_CORE_CAPI
void aEquinElemToCart(
    const EquinElem& equinElem,
    double gm,
    Vector3d& pos,
    Vector3d& vel);


/// @brief 经典轨道根数转换为修正轨道根数（类引用版本）
/// @param elem 经典轨道根数
/// @param gm 引力参数 [m^3/s^2]
/// @param delaunay 输出修正轨道根数
AST_CORE_CAPI 
errc_t aOrbElemToDelaunay(
    const OrbElem& elem,
    double gm,
    DelaunayElem& delaunay);


/// @brief 修正轨道根数转换为经典轨道根数（类引用版本）
/// @param delaunay 修正轨道根数
/// @param gm 引力参数 [m^3/s^2]
/// @param elem 输出经典轨道根数
AST_CORE_CAPI 
errc_t aDelaunayToOrbElem(
    const DelaunayElem& delaunay,
    double gm,
    OrbElem& elem);


/// @brief 直角坐标转换为修正轨道根数（类引用版本）
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @return 修正轨道根数
A_ALWAYS_INLINE
ModOrbElem aCartToModOrbElem(const Vector3d& r, const Vector3d& v, double gm)
{
    ModOrbElem modOrbElem;
    aCartToModOrbElem(r, v, gm, modOrbElem);
    return modOrbElem;
}


/*! @} */


AST_NAMESPACE_END
 
AST_DECL_TYPE_ALIAS(CartState)