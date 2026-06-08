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



/*! @} */

AST_NAMESPACE_END
