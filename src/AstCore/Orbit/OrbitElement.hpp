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
 
AST_NAMESPACE_BEGIN


/// @brief 直角坐标
class CartState
{
public:
    Vector3d pos_;   ///< 位置
    Vector3d vel_;   ///< 速度
public:
    A_DEF_POD_ITERABLE(double)
    const Vector3d& pos() const {return pos_;} 
    Vector3d& pos() {return pos_;}
    const Vector3d& vel() const {return vel_;} 
    Vector3d& vel() {return vel_;}
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
    /// @brief 计算平均运动数
    double getMeanMotion(double gm) const {return sqrt(gm / pow(getA(), 3));}
    
    /// @brief 计算半长轴
    double getA() const {return rp_ / (1 - e_);}

    /// @brief 计算半通径
    double getP() const {return rp_ * (1 + e_);}

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



/// @brief 经典轨道根数转换为直角坐标
/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t coe2rv(const double* coe, double gm, double* pos, double* vel);

/// @brief 经典轨道根数转换为改进春分点轨道根数
/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t coe2mee(const double* coe, double* mee);

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
AST_CORE_CAPI err_t rv2mee(const double* pos, const double* vel, double gm, double* mee);

/// @brief 改进春分点轨道根数转换为经典轨道根数
/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]
/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t mee2coe(const double* mee, double* coe);

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
AST_CORE_CAPI err_t rv2moe(const double* pos, const double* vel, double gm, double* moe);

/// @brief 直角坐标转换为经典轨道根数
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t rv2coe(const double* pos, const double* vel, double gm, double* coe);

/// @brief 春分点根数转换为修正轨道根数
/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]
/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t ee2moe(const double* ee, double* moe);

/// @brief 修正轨道根数转换为春分点根数
/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t moe2ee(const double* moe, double* ee);

/// @brief 修正轨道根数转换为经典轨道根数
/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
AST_CORE_CAPI err_t moe2coe(const double* moe, double* coe);

/// @brief 经典轨道根数转换为修正轨道根数
/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
AST_CORE_CAPI void coe2moe(const double* coe, double* moe);

/// @brief 修正轨道根数转换为直角坐标
/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
AST_CORE_CAPI err_t moe2rv(const double* moe, double gm, double* pos, double* vel);

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
AST_CORE_CAPI err_t aCartToModEquinElem(
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    ModEquinElem& mee
);

/// @brief 经典轨道根数转换为改进春分点轨道根数（类引用版本）
/// @param elem 经典轨道根数
/// @param mee 输出改进春分点轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t aOrbElemToModEquinElem(
    const OrbElem& elem,
    ModEquinElem& mee
);

/// @brief 改进春分点轨道根数转换为经典轨道根数（类引用版本）
/// @param mee 改进春分点轨道根数
/// @param elem 输出经典轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t aModEquinElemToOrbElem(
    const ModEquinElem& mee,
    OrbElem& elem
);

/// @brief 直角坐标转换为修正轨道根数（类引用版本）
/// @param pos 位置矢量 [m]
/// @param vel 速度矢量 [m/s]
/// @param gm 引力参数 [m^3/s^2]
/// @param modOrb 输出修正轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI err_t	aCartToModOrbElem(
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
err_t aCartToOrbElem     (
    const Vector3d& pos,
    const Vector3d& vel,
    double gm,
    OrbElem& elem);

/// @brief 春分点根数转换为修正轨道根数（类引用版本）
/// @param equinElem 春分点根数
/// @param modOrb 输出修正轨道根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI
err_t aEquinElemToModOrb (
    const EquinElem& equinElem,
    ModOrbElem& modOrb);

/// @brief 修正轨道根数转换为春分点根数（类引用版本）
/// @param modOrb 修正轨道根数
/// @param equinElem 输出春分点根数
/// @return 错误码，成功返回eNoError
AST_CORE_CAPI
err_t aModOrbToEquinElem (
    const ModOrbElem& modOrb,
    EquinElem& equinElem);

/// @brief 修正轨道根数转换为直角坐标（类引用版本）
/// @param modOrb 修正轨道根数
/// @param gm 引力参数 [m^3/s^2]
/// @param pos 输出位置矢量 [m]
/// @param vel 输出速度矢量 [m/s]
AST_CORE_CAPI
err_t aModOrbElemToCart  (
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
err_t aOrbElemToCart     (
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

AST_NAMESPACE_END
 
AST_DECL_TYPE_ALIAS(CartState)