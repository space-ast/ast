///
/// @file      MotionProfile.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Ephemeris.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstCore/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class MotionProfile;
using PMotionProfile = MotionProfile*;
// using HMotionProfile = ScopedPtr<MotionProfile>;

class MotionTwoBody;
class MotionHPOP;
class MotionSPICE;
class MotionBallistic;
class MotionSimpleAscent;
class MotionProfileVisitor;
class MotionMissionCommand;
class MotionSGP4;

/// @brief 运动定义接口
class AST_SIM_API MotionProfile: public ObjectNamed
{
public:
    AST_OBJECT(MotionProfile)
    MotionProfile() = default;
    virtual ~MotionProfile() = default;
public:
    MotionHPOP* toHPOP() const;
    MotionTwoBody* toTwoBody() const;
    MotionSPICE* toSPICE() const;
    MotionBallistic* toBallistic() const;
    MotionSimpleAscent* toSimpleAscent() const;
public:
    /// @brief 生成特定星历
    /// @param eph 星历指针
    /// @return 错误码
    virtual errc_t makeEphemerisSpec(ScopedPtr<Ephemeris>& eph) const = 0;

    /// @brief 生成简单星历
    /// @param eph 星历指针
    /// @return 错误码
    virtual errc_t makeEphemerisSimple(ScopedPtr<Ephemeris>& eph) const = 0;

    /// @brief 接受访问者
    /// @param visitor 访问者
    virtual void accept(MotionProfileVisitor& visitor) = 0;
};

/*! @} */

AST_NAMESPACE_END