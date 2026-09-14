///
/// @file      OrbitElement.hpp
/// @brief     轨道根数定义及转换接口
/// @details   提供各种轨道根数（直角坐标、经典轨道根数、修正轨道根数、春分点根数、改进春分点轨道根数、球坐标根数、B平面根数）之间的转换函数。
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
#include "AstUtil/Constants.hpp"
#include <string>
#include <cmath>                // for std::isfinite
 
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
/// @note  该类保持聚合类型, 不额外定义构造函数。
///        GCC 会对 CartState 的隐式默认构造函数报-Weffc++ 警告, 因此在类定义处局部抑制该警告。
#if defined(A_GCC) || defined(A_CLANG)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Weffc++"
#endif
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
    const Vector3d& position() const {return pos_;} 
    Vector3d& position() {return pos_;}

    /// @brief 获取速度
    /// @return 速度
    const Vector3d& vel() const {return vel_;} 
    Vector3d& vel() {return vel_;}
    const Vector3d& velocity() const {return vel_;} 
    Vector3d& velocity() {return vel_;}

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
public:
    /// @brief 判断状态分量是否全部为有限值
    bool isFinite() const{
        for (auto val: *this)
            if (!std::isfinite(val))
                return false;
        return true;
    }
};
#if defined(A_GCC) || defined(A_CLANG)
#   pragma GCC diagnostic pop
#endif


/// @brief 经典轨道根数
class OrbElem
{
// 设置为public使类型为聚合类型
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
// 设置为public使类型为聚合类型
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

    /// @brief 计算近拱点经度
    double getLongitudeOfPeri() const {return aArgPeriToLongPeri(getArgPeri(), getRAAN());}

    /// @brief 计算平均经度
    double getMeanLongitude() const {return getLongitudeOfPeri() + getMeanAnomaly();}

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
// 设置为public使类型为聚合类型
public:
    double a_;          ///< semimajor axis length (半长轴)
    double h_;          ///< e*sin(argper + RAAN)   omegabar=argper + RAAN
    double k_;          ///< e*cos(argper + RAAN)
    double p_;          ///< tan(i/2)*sin(RAAN)
    double q_;          ///< tan(i/2)*cos(RAAN)
    double lambda_;     ///< mean longitude = M + RAAN + argper (平经度)
public:
    /// @brief 平经度
    double meanLongitude() const {return lambda_;}
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
// 设置为public使类型为聚合类型
public:
    double p_;        ///< p = a(1-e^2) 半通径
    double f_;        ///< f = e*cos(argper + RAAN)
    double g_;        ///< g = e*sin(argper + RAAN)
    double h_;        ///< h = tan(i/2)*cos(RAAN)
    double k_;        ///< k = tan(i/2)*sin(RAAN)
    double L_;        ///< L = RAAN + argper + trueA (真经度)
public:
    /// @brief 真经度
    double trueLongitude() const {return L_;}
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
// 设置为public使类型为聚合类型
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


/// @brief B平面根数
/// @details
/// B平面主要用于描述双曲线到达轨迹(月球或行星借力飞越)相对中心天体的状态。
/// B平面是垂直于入渐近线方向的平面, 位于该平面内的B矢量给出入渐近线与B平面的交点,
/// B矢量通常以平面内另外两个矢量R、T的分量表示。
/// 其中R、T由参考向量N决定: T = S x N, R = S x T (S为入渐近线方向)
/// @note  当偏心率e小于1(椭圆轨道)时, 入渐近线不存在, 给出退化结果, 参见 aCartToBPlane 的说明。
class BPlaneElem
{
// 设置为public使类型为聚合类型
public:
    double ra_;         ///< 入渐近线赤经(B平面法向量的赤经) [rad]
    double dec_;        ///< 入渐近线赤纬(B平面法向量的赤纬) [rad]
    double bDotR_;      ///< B矢量在R轴上的分量 B·R [m]
    double bDotT_;      ///< B矢量在T轴上的分量 B·T [m]
    double c3_;         ///< 特征能量 C3 = v^2 - 2*gm/r [m^2/s^2]
    double trueA_;      ///< 真近点角 [rad]
public:
    /// @brief 转换为字符串
    AST_CORE_API
    std::string toString() const;
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, ra)
    AST_DEF_ACCESS_METHOD(double, dec)
    AST_DEF_ACCESS_METHOD(double, bDotR)
    AST_DEF_ACCESS_METHOD(double, bDotT)
    AST_DEF_ACCESS_METHOD(double, c3)
    AST_DEF_ACCESS_METHOD(double, trueA)
};


/// @brief 球坐标轨道根数
/// @details
/// 用赤经、赤纬、地心距描述位置, 用飞行航迹角、航迹方位角、速度大小描述速度。
/// 各分量均在参考坐标系的轴系下定义:
///     ra  = atan2(y, x)           [rad] 赤经, 自x轴逆时针量
///     dec = atan2(z, hypot(x, y)) [rad] 赤纬, 范围[-π/2, π/2]
///     r   = |pos|                 [m]   地心距
///     fpa = atan2(v_r, v_h)       [rad] 航迹角, 速度矢量与当地水平面的夹角, 向上为正
///     azi = atan2(v_e, v_n)       [rad] 航迹方位角, 自当地北向东为正
///     v   = |vel|                 [m/s] 速度大小
/// @note  极点(|dec|=π/2)处赤经与航迹方位角奇异: 正向转换取ra=0, 仍保持精确往返;
///        零速度时航迹角与航迹方位角无定义, 正向转换失败。
class SphericalElem
{
// 设置为public使类型为聚合类型
public:
    double ra_;         ///< 赤经 [rad]
    double dec_;        ///< 赤纬 [rad]
    double r_;          ///< 地心距 [m]
    double fpa_;        ///< 航迹角 [rad]
    double azi_;        ///< 航迹方位角 [rad]
    double v_;          ///< 速度大小 [m/s]
public:
    /// @brief 获取赤经
    double getRA() const {return ra_;}

    /// @brief 获取赤纬
    double getDec() const {return dec_;}

    /// @brief 获取地心距
    double getRadius() const {return r_;}

    /// @brief 获取飞行航迹角(水平飞行航迹角)
    double getFltPathAng() const {return fpa_;}

    /// @brief 获取航迹方位角
    double getFltPathAzi() const {return azi_;}

    /// @brief 获取速度大小
    double getVel() const {return v_;}

    /// @brief 转换为字符串
    AST_CORE_API
    std::string toString() const;
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, ra)
    AST_DEF_ACCESS_METHOD(double, dec)
    AST_DEF_ACCESS_METHOD(double, r)
    AST_DEF_ACCESS_METHOD(double, fpa)
    AST_DEF_ACCESS_METHOD(double, azi)
    AST_DEF_ACCESS_METHOD(double, v)
};


/// @brief 混合球坐标轨道根数
/// @details
/// 位置用天体固连系下的大地坐标描述, 速度由惯性速度矢量描述:
///     lon = atan2(y, x)           [rad] 大地经度(天体固连系), 自本初子午线向东为正
///     lat = 大地纬度               [rad] 椭球法线与赤道面的夹角, 范围[-π/2, π/2]
///     alt = 高度                   [m]   在参考椭球之上为正, 沿椭球法线量
///     fpa = atan2(v_r, v_h)       [rad] 水平航迹角, 惯性速度矢量与当地水平面的夹角, 向上为正
///     azi = atan2(v_e, v_n)       [rad] 航迹方位角, 自当地北向东为正
///     v   = |vel|                 [m/s] 惯性速度大小
/// @note  位置分量由固连系下的大地坐标给出, 需要参考形状(椭球, 见 @ref BodyShape)与固连系/惯性系之间的旋转(见 @ref Rotation);
///        速度分量则完全在参考系(惯性系)下定义, 与椭球和固连系无关, 
///        当地水平面与当地北向由位置矢量在该系下的赤经确定(同 @ref SphericalElem), 不含固连系的牵连速度, 故 v 为惯性速度大小。
///        参见 @ref aCartToMixedSpherical 和 @ref aMixedSphericalToCart
class MixedSphericalElem
{
// 设置为public使类型为聚合类型
public:
    double lon_;        ///< 大地经度 [rad] (天体固连系)
    double lat_;        ///< 大地纬度 [rad] (天体固连系)
    double alt_;        ///< 高度 [m] (相对参考椭球, 沿椭球法线)
    double fpa_;        ///< 水平航迹角 [rad]
    double azi_;        ///< 航迹方位角 [rad]
    double v_;          ///< 惯性速度大小 [m/s]
public:
    /// @brief 获取大地经度
    double getLon() const {return lon_;}

    /// @brief 获取大地纬度
    double getLat() const {return lat_;}

    /// @brief 获取高度
    double getAlt() const {return alt_;}

    /// @brief 获取飞行航迹角(水平飞行航迹角)
    double getFltPathAng() const {return fpa_;}

    /// @brief 获取垂直飞行航迹角(水平飞行航迹角的余角)
    double getVertFltPathAng() const {return kHalfPI - fpa_;}

    /// @brief 获取航迹方位角
    double getFltPathAzi() const {return azi_;}

    /// @brief 获取速度大小
    double getVel() const {return v_;}

    /// @brief 转换为字符串
    AST_CORE_API
    std::string toString() const;
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, lon)
    AST_DEF_ACCESS_METHOD(double, lat)
    AST_DEF_ACCESS_METHOD(double, alt)
    AST_DEF_ACCESS_METHOD(double, fpa)
    AST_DEF_ACCESS_METHOD(double, azi)
    AST_DEF_ACCESS_METHOD(double, v)
};


/// @brief 大地坐标根数
/// @details
/// 位置与速度均在天体固连系下以大地坐标及其变化率描述:
///     lon = atan2(y, x)           [rad]   大地经度(天体固连系), 自本初子午线向东为正
///     lat                          [rad]   大地纬度, 椭球法线与赤道面的夹角, 范围[-π/2, π/2]
///     alt                          [m]     高度, 在参考椭球之上为正, 沿椭球法线量
///     lonRate                      [rad/s] 大地经度变化率
///     latRate                      [rad/s] 大地纬度变化率
///     altRate                      [m/s]   高度变化率
/// @note  与 @ref MixedSphericalElem 的关键区别在于速度也完全在固连系下定义:
///        vel = (N+alt)cos(lat)·lonRate·e + (M+alt)·latRate·n + altRate·u ,
///        其中 e、n、u 为当地东、北、天单位矢量, N、M 为卯酉圈与子午圈曲率半径。
///        因此 vel 为零(如地球同步轨道)时三个变化率亦为零, 与惯性速度无关。
///        需要参考形状(椭球, 见 @ref BodyShape)以及参考系与固连系之间的运动学变换(见 @ref KinematicTransform)。
///        极点(lat=±π/2)处经度率奇异, 正向转换取lonRate=0(约定), 与 @ref SphericalElem 的赤经一致。
///        参见 @ref aCartToGeodetic 和 @ref aGeodeticToCart
class GeodeticElem
{
// 设置为public使类型为聚合类型
public:
    double lat_;        ///< 大地纬度 [rad] (天体固连系)
    double lon_;        ///< 大地经度 [rad] (天体固连系)
    double alt_;        ///< 高度 [m] (相对参考椭球, 沿椭球法线)
    double latRate_;    ///< 大地纬度变化率 [rad/s] (天体固连系)
    double lonRate_;    ///< 大地经度变化率 [rad/s] (天体固连系)
    double altRate_;    ///< 高度变化率 [m/s] (天体固连系)
public:
    /// @brief 获取大地经度
    double getLon() const {return lon_;}

    /// @brief 获取大地纬度
    double getLat() const {return lat_;}

    /// @brief 获取高度
    double getAlt() const {return alt_;}

    /// @brief 获取大地经度变化率
    double getLonRate() const {return lonRate_;}

    /// @brief 获取大地纬度变化率
    double getLatRate() const {return latRate_;}

    /// @brief 获取高度变化率
    double getAltRate() const {return altRate_;}

    /// @brief 转换为字符串
    AST_CORE_API
    std::string toString() const;
public:
    A_DEF_POD_ITERABLE(double)
    AST_DEF_ACCESS_METHOD(double, lon)
    AST_DEF_ACCESS_METHOD(double, lat)
    AST_DEF_ACCESS_METHOD(double, alt)
    AST_DEF_ACCESS_METHOD(double, lonRate)
    AST_DEF_ACCESS_METHOD(double, latRate)
    AST_DEF_ACCESS_METHOD(double, altRate)
};


/// @brief 直角坐标转换为球坐标根数
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param sph 输出球坐标根数
/// @return 错误码，成功返回eNoError
/// @note  位置或速度为零矢量时无法确定赤经、赤纬、航迹角与航迹方位角, 返回eErrorInvalidParam
AST_CORE_API errc_t aCartToSpherical(
    const Vector3d& pos,
    const Vector3d& vel,
    SphericalElem& sph
);


/// @brief 球坐标根数转换为直角坐标
/// @param sph 球坐标根数
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
/// @return 错误码，成功返回eNoError
/// @note  地心距不大于零或速度小于零时返回eErrorInvalidParam
AST_CORE_API errc_t aSphericalToCart(
    const SphericalElem& sph,
    Vector3d& pos,
    Vector3d& vel
);


/// @brief 直角坐标转换为混合球坐标根数
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param framelToFixed 位置速度参考系到天体固连系的旋转, 仅用于位置分量
/// @param shape 参考形状(椭球), 仅用于位置分量
/// @param mixedSph 输出混合球坐标根数
/// @return 错误码，成功返回eNoError
/// @note  位置或速度为零矢量时无法确定经度、纬度、航迹角与航迹方位角, 返回eErrorInvalidParam;
///        航迹角与航迹方位角完全由 pos、vel 在惯性系下确定, 与 inertialToFixed、shape 无关,
///        见 @ref MixedSphericalElem。
AST_CORE_API errc_t aCartToMixedSpherical(
    const Vector3d& pos,
    const Vector3d& vel,
    const Rotation& framelToFixed,
    const BodyShape& shape,
    MixedSphericalElem& mixedSph
);


/// @brief 混合球坐标根数转换为直角坐标
/// @param mixedSph 混合球坐标根数
/// @param fixedToframe 天体固连系到参考系的旋转, 需与生成mixedSph时所用的旋转互逆, 仅用于位置分量
/// @param shape 参考形状(椭球), 仅用于位置分量
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
/// @return 错误码，成功返回eNoError
/// @note  速度小于零或位置退化到参考椭球中心时返回eErrorInvalidParam
AST_CORE_API errc_t aMixedSphericalToCart(
    const MixedSphericalElem& mixedSph,
    const Rotation& fixedToframe,
    const BodyShape& shape,
    Vector3d& pos,
    Vector3d& vel
);


/// @brief 天体固连系直角坐标转换为大地坐标根数
/// @param pos 天体固连系下的位置矢量 [m]
/// @param vel 天体固连系下的速度矢量 [m/s]
/// @param shape 参考形状(椭球)
/// @param geodetic 输出大地坐标根数
/// @return 错误码，成功返回eNoError
/// @note  与 @ref aCartToMixedSpherical 不同, 这里的 pos 与 vel 均为天体固连系下的量,
///        故不需要旋转参数; 参考系与固连系之间的换算由调用方(见 @ref StateGeodetic )负责。
///        位置或速度为零矢量时无法确定经度、纬度或变化率, 返回eErrorInvalidParam;
///        极点(lat=±π/2)处经度率奇异, 取lonRate=0(约定); 参考形状不支持该转换时透传其错误码。
AST_CORE_API errc_t aCartToGeodetic(
    const Vector3d& pos,
    const Vector3d& vel,
    const BodyShape& shape,
    GeodeticElem& geodetic
);


/// @brief 大地坐标根数转换为天体固连系直角坐标
/// @param geodetic 大地坐标根数
/// @param shape 参考形状(椭球)
/// @param pos 输出天体固连系下的位置矢量 [m]
/// @param vel 输出天体固连系下的速度矢量 [m/s]
/// @return 错误码，成功返回eNoError
/// @note  速度为天体固连系下的速度, 不包含任何牵连速度;
///        大地纬度超出[-π/2, π/2]或位置退化到参考椭球中心附近时返回eErrorInvalidParam;
///        参考形状不支持该转换时透传其错误码。
AST_CORE_API errc_t aGeodeticToCart(
    const GeodeticElem& geodetic,
    const BodyShape& shape,
    Vector3d& pos,
    Vector3d& vel
);


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


/// @brief B平面参考向量类型
/// @details B平面内的R、T两轴由参考向量N决定: T = unit(S x N), R = S x T,
///          其中S为入渐近线方向。不同的参考向量会给出不同的R、T轴。
enum class EBPlaneRefVector
{
    eBodyOrbitNormal,           ///< 天体绕其父天体运动的轨道法向
    eSpacecraftOrbitNormal,     ///< 航天器自身的轨道法向
    eBodyPole,                  ///< 天体的自转轴
    eCustom,                    ///< 自定义参考向量
};



/// @brief 直角坐标转换为B平面根数
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param refVector 参考向量(任意非零矢量), 用于确定R、T轴, 见 EBPlaneRefVector
/// @param bPlane 输出B平面根数
/// @return 错误码，成功返回eNoError
/// @note  当偏心率e大于1时, 按B平面的定义计算;
///        e不大于1时入渐近线不存在, 此时退化处理:
///        |B|取椭圆半短轴 a*sqrt(1-e^2), 入渐近线方向S取偏心率向量方向,
///        偏心率向量为零(圆轨道)时取坐标系的+X轴。
AST_CORE_CAPI errc_t aCartToBPlane(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    const Vector3d& refVector,
    BPlaneElem& bPlane
);


/// @brief B平面根数转换为直角坐标
/// @param bPlane B平面根数
/// @param gm 引力参数 [m^3/s^2]
/// @param refVector 参考向量(任意非零矢量), 需与生成bPlane时一致
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI errc_t aBPlaneToCart(
    const BPlaneElem& bPlane,
    double gm,
    const Vector3d& refVector,
    Vector3d& pos,
    Vector3d& vel
);


/*! @} */


AST_NAMESPACE_END
 
AST_DECL_TYPE_ALIAS(CartState)