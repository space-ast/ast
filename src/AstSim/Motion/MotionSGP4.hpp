///
/// @file      MotionSGP4.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-14
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
#include "AstSim/MotionProfile.hpp"
#include "AstSim/MotionWithIntervalStep.hpp"
#include "AstCore/SGP4.hpp"
#include "AstCore/TLELines.hpp"
#include "AstCore/TLE.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/



/// @brief SGP4 运动模型
/// @details   
class AST_SIM_API MotionSGP4 : public MotionWithIntervalStep
{
public:
    MotionSGP4() = default;
    ~MotionSGP4() = default;
public:
    errc_t makeEphemerisSpec(ScopedPtr<Ephemeris>& eph) const override;
    errc_t makeEphemerisSimple(ScopedPtr<Ephemeris>& eph) const override;
    void accept(MotionProfileVisitor& visitor) override;
public:
    std::string SSCNumber_{};                ///< SSC 编号字符串
    std::string intlDesignator_{};           ///<
    std::string commonName_{};               ///< 通用名称
    std::vector<TLE> elements_{}; ///< TLE 列表
};

/*! @} */

AST_NAMESPACE_END