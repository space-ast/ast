///
/// @file      SGP4.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-18
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
#include "TLELines.hpp"  // TLELines (TLE.hpp 依赖但未包含)
#include "TLE.hpp"
#include "AstCore/Ephemeris.hpp"

struct elsetrec;

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/



/// @brief SGP4 预报器
class AST_CORE_API SGP4: public Ephemeris
{
public:
    SGP4();
    SGP4(const TLE& tle);
    SGP4(const TLELines& lines);
    ~SGP4();
public:
    Frame* getFrame() const override;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const override;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;
    errc_t getInterval(TimeInterval& interval) const override;
public:
    void setTLE(const TLE& tle);
    void setTLE(const TLELines& lines);
private:
    struct elsetrec& element() const;
private:
    mutable std::aligned_storage<1000>::type storage_;
};

/*! @} */

AST_NAMESPACE_END
