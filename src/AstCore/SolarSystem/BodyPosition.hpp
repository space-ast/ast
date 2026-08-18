///
/// @file      BodyPosition.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-02
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 计算天体在参考天体的惯性坐标系中的位置，不考虑光行差和像差
/// @param[in] body 天体指针
/// @param[in] referenceBody 参考天体指针
/// @param[out] posInReferenceBodyInertial 天体在参考天体的惯性坐标系中的位置
/// @return 错误码
AST_CORE_CAPI errc_t aBodyInBodyInertial(const TimePoint& time, Body* body, Body* referenceBody, Vector3d& posInReferenceBodyInertial);


/// @brief 计算天体在参考天体的固连系中的位置，不考虑光行差和像差
/// @param[in] body 天体指针
/// @param[in] referenceBody 参考天体指针
/// @param[out] posInReferenceBodyFixed 天体在参考天体的固连系中的位置
/// @return 错误码
AST_CORE_CAPI errc_t aBodyInBodyFixed(const TimePoint& time, Body* body, Body* referenceBody, Vector3d& posInReferenceBodyFixed);


/// @brief 计算天体在站心坐标系下的方位角、仰角和距离
/// @param[in] body 天体指针
/// @param[in] referenceBody 参考天体(即观察者所在的天体)
/// @param[in] observerPosition 观察者在观察天体上的大地坐标
/// @param[out] aer 天体在站心坐标系下的方位角、仰角和距离
/// @return 错误码
AST_CORE_CAPI errc_t aBodyAERInTopocentric(const TimePoint& time, Body* body, Body* referenceBody, const GeodeticPoint& observerPosition, AER& aer);


/// @brief 计算太阳在站心坐标系下的方位角、仰角和距离
/// @param[in] referenceBody 参考天体(即观察者所在的天体)
/// @param[in] observerPosition 观察者在观察天体上的大地坐标
/// @param[out] aer 太阳在站心坐标系下的方位角、仰角和距离
/// @return 错误码
AST_CORE_CAPI errc_t aSunAERInTopocentric(const TimePoint& time, Body* referenceBody, const GeodeticPoint& observerPosition, AER& aer);


/// @brief 光行差和像差标志位
enum class EAberrationFlags
{
    eReception            = 1,                       // 接收模式（观察者接收数据）
    eTransmission         = 1<<1,                    // 传输模式（观察者发送数据）
    eStellarAberration    = 1<<2,                    // 考虑像差
    eNoStellarAberration  = 1<<3,                    // 不考虑像差

    // 下面是与SPICE库兼容的标志
    eNone = 0,                                       // 不考虑光行差和像差
    eCN = eReception | eNoStellarAberration,         // 接收模式，且不考虑像差
    eCN_S = eReception | eStellarAberration,         // 接收模式，且考虑像差
    eXCN = eTransmission | eNoStellarAberration,     // 传输模式，且不考虑像差
    eXCN_S = eTransmission | eStellarAberration,     // 传输模式，且考虑像差
};

A_ENUM_CLASS_FLAGS(EAberrationFlags)


/// @brief 时钟主机（用于光时计算中选择时间参考对象）
enum class EClockHost
{
    eFirstObject,   ///< 以第一个对象为时钟参考
    eSecondObject   ///< 以第二个对象为时钟参考
};

/// @brief 时间方向（用于光时计算中定义传输/接收方向）
enum class ETimeSense
{
    eTransmit,  ///< 发射方向
    eReceive    ///< 接收方向
};

/// @brief 光行差类型
enum class EAberrationType
{
    eNone,      ///< 无光行差
    eAnnual,    ///< 周年光行差
    eDiurnal,   ///< 周日光行差
    eTotal      ///< 总光行差
};


/// @brief 进行像差校正
/// @param[in] relPosition 从观察者指向目标点的矢量（在ICRF系下表示）
/// @param[in] velObserver 观察者在ICRF系下的速度
/// @param[out] relPositionCorrected 校正后的从观察者指向目标点的矢量（在ICRF系下表示）
/// @return 错误码
AST_CORE_CAPI errc_t aStellarAberration(const Vector3d& relPosition, const Vector3d& velObserver, EAberrationFlags aberrationFlags, Vector3d& relPositionCorrected);


/// @brief 计算目标点的相对于观察者的视位置
/// @param[in] point 目标点
/// @param[in] time 时间点
/// @param[in] observerPosInSSBICRF 观察者在太阳系质心(SSBICRF)坐标系中的位置
/// @param[in] observerVelInSSBICRF 观察者在太阳系质心的(SSBICRF)坐标系中的速度
/// @param[in] aberrationFlags 光行差和像差标志位，用于定义观察者是接收数据还是发送数据，以及是否考虑像差
/// @param[out] apparentRelPosition 从观察者指向目标点的矢量（在ICRF系下表示）
/// @param[out] lightTime 光传播时间
/// @return 错误码
AST_CORE_CAPI errc_t aApparentPositionICRF(
    Point* point, const TimePoint& time, const Vector3d& observerPosInSSBICRF, const Vector3d& observerVelInSSBICRF, EAberrationFlags aberrationFlags,
    Vector3d& apparentRelPosition, double* lightTime
);


/// @brief 计算目标点的相对于观察者的视位置
/// @details 函数内部会根据观察者相对于太阳系质心的运动速度进行像差修正
/// @param point 目标点
/// @param time 时间点
/// @param frame 观察者在的坐标系
/// @param observerPos 观察者在坐标系中的位置
/// @param observerVel 观察者在坐标系中的速度
/// @param aberrationFlags 光行差和像差标志位，用于定义观察者是接收数据还是发送数据，以及是否考虑像差
/// @param apparentRelPosition 从观察者指向目标点的矢量（在坐标系frame下表示）
/// @param lightTime 光传播时间
/// @return 错误码
AST_CORE_CAPI errc_t aApparentPositionInFrame(
    Point* point, const TimePoint& time, Frame* frame, const Vector3d& observerPos, const Vector3d& observerVel, EAberrationFlags aberrationFlags,
    Vector3d& apparentRelPosition, double* lightTime
);


/// @brief 绕指定轴旋转向量
/// @todo 移动到其他头文件中
/// @param v 输入向量
/// @param axis 旋转轴
/// @param theta 旋转角度
/// @param r 输出向量
AST_CORE_CAPI void aVectorRotation(const Vector3d& v, const Vector3d& axis, double theta, Vector3d& r);


/*! @} */

AST_NAMESPACE_END
