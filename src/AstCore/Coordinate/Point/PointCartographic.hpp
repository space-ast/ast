///
/// @file      PointCartographic.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-01
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
#include "AstCore/Point.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/GeodeticPoint.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 天地制图坐标点(天体大地坐标点)
class AST_CORE_API PointCartographic final: public Point
{
public:
    PointCartographic() = default;
    /// @param body 天体
    PointCartographic(Body* body);
    /// @param body 天体
    /// @param position 在天体上的位置点（大地坐标）
    PointCartographic(Body* body, const GeodeticPoint& position);
    ~PointCartographic() override = default;
public:
    Frame* getFrame() const override;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const override;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;
public:
    Body* body() const { return body_.get(); }
    void setBody(Body* body);
    BodyShape* shape() const { return shape_.get(); }
    const GeodeticPoint& position() const { return position_; }
    void setPosition(const GeodeticPoint& position) { position_ = position; }
public:
    void setLatitude(double latitude) {position_.setLatitude(latitude);}
    void setLongitude(double longitude) {position_.setLongitude(longitude);}
    void setAltitude(double altitude) {position_.setAltitude(altitude);}
    double latitude() const {return position_.latitude();}
    double longitude() const {return position_.longitude();}
    double altitude() const {return position_.altitude();}
private: // 模型配置参数
    GeodeticPoint position_{};              ///< 在天体上的位置点（大地坐标）
    WeakPtr<Body> body_{};                  ///< 天体
private: // 内部缓存
    WeakPtr<BodyShape> shape_{};            ///< 天体形状
};


/*! @} */

AST_NAMESPACE_END
