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
#include "MSISBase.hpp"
#include <type_traits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class NRLMSISE;

#define _AST_USE_MSIS_VERS_FOR_NRLMSIS00

#ifdef _AST_USE_MSIS_VERS_FOR_NRLMSIS00

/// @brief NRLMSIS00大气模型基类
class AST_CORE_API NRLMSIS00 final: public MSISBase
{
public:
    NRLMSIS00(Frame* frame, BodyShape* bodyShape, double f107Daily, double f107Average, double ap);
    NRLMSIS00(Frame* frame, BodyShape* bodyShape, SpaceWeatherProvider* spaceWeather);

    double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const override;
};

#else
/// @brief NRLMSIS00大气模型
class AST_CORE_API NRLMSIS00 final: public AtmosphereBase
{
public:
    NRLMSIS00(Frame* ecf, BodyShape* bodyShape, double f107Daily, double f107Average, double ap);
    ~NRLMSIS00() override;

    double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const override;

    
private:
    NRLMSISE& nrlmsise() const {return *reinterpret_cast<NRLMSISE*>(&storage_);}
private:
    mutable std::aligned_storage<624>::type storage_;
};

#endif


/*! @} */

AST_NAMESPACE_END

