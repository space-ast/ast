///
/// @file      Facility.hpp
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
#include "AstCore/Point.hpp"
#include "AstCore/GeodeticPoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/PointCartographic.hpp"
#include "AstSim/Platform.hpp"
#include "AstSim/CentroidPosition.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class Facility;
using HFacility = SharedPtr<Facility>;
using PFacility = Facility*;


/// @brief 设施对象
class AST_SIM_API Facility: public Platform
{
public:
    AST_OBJECT(Facility)
    AST_PROPERT(latitude)
    AST_PROPERT(longitude)
    AST_PROPERT(altitude)
    AST_PROPERT(body)
    Facility();
    ~Facility() override = default;
public: // 从Point继承重写的函数
    Frame* getFrame() const final;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const final;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const final;
public:
    const GeodeticPoint& position() const {return cartographic().position();}
    void setPosition(const GeodeticPoint& position){cartographic().setPosition(position);}
PROPERTIES:
    angle_d latitude() const {return cartographic().latitude();}
    void setLatitude(angle_d latitude) {cartographic().setLatitude(latitude);}

    angle_d longitude() const {return cartographic().longitude();}
    void setLongitude(angle_d longitude) {cartographic().setLongitude(longitude);}

    length_d altitude() const {return cartographic().altitude();}
    void setAltitude(length_d altitude) {cartographic().setAltitude(altitude);}

    Body* body() const {return cartographic().body();}
    void setBody(Body* body) {cartographic().setBody(body);}
protected:
    PointCartographic& cartographic() {return static_cast<PointCartographic&>(*location());}
    const PointCartographic& cartographic() const {return static_cast<const PointCartographic&>(*location());}
private: // 屏蔽基类public函数
    void setLocation(Point* location) = delete;
};

/*! @} */

AST_NAMESPACE_END