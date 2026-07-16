///
/// @file      LandingSite.hpp
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
#include "AstCore/Segment.hpp"
#include "AstCore/GeodeticPoint.hpp"
#include "AstCore/CelestialBody.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_CORE_API LandingSite: public Segment
{
public:
    AST_OBJECT(LandingSite)

    LandingSite() = default;
    ~LandingSite() override = default;
public:
    errc_t execute() override;

public: /* 配置属性 */
    /// @brief 获取着陆天体
    Body* body() const { return body_.get(); }
    /// @brief 设置着陆天体
    void setBody(Body* body) { body_ = body; }

    /// @brief 获取着陆点位置
    const GeodeticPoint& position() const { return position_; }
    /// @brief 设置着陆点位置
    void setPosition(const GeodeticPoint& pos) { position_ = pos; }

    /// @brief 获取着陆时间
    const TimePoint& epoch() const { return epoch_; }
    /// @brief 设置着陆时间
    void setEpoch(const TimePoint& epoch) { epoch_ = epoch; }
private:
    TimePoint     epoch_{};        ///< 着陆时间
    WeakPtr<Body> body_{};          ///< 着陆天体
    GeodeticPoint position_{};     ///< 着陆点位置
};


/*! @} */

AST_NAMESPACE_END

