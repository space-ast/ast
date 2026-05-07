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
#include "AstSim/CentroidPosition.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 设施对象
class AST_SIM_API Facility: public Point
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
    const std::string& getName() const override {return name_;}
    void setName(StringView name) override {name_ = std::string(name);}
public:
    void setPosition(const CentroidPosition& position){position_ = position;}
    const CentroidPosition& getPosition() const{return position_;}
    CentroidPosition& position(){return position_;}
    const GeodeticPoint& getGeodeticPosition() const {return position_.getPosition();}
    void setGeodeticPosition(const GeodeticPoint& position){position_.setPosition(position);}
PROPERTIES:
    angle_d latitude() const {return position_.latitude();}
    void setLatitude(angle_d latitude) {position_.setLatitude(latitude);}

    angle_d longitude() const {return position_.longitude();}
    void setLongitude(angle_d longitude) {position_.setLongitude(longitude);}

    length_d altitude() const {return position_.altitude();}
    void setAltitude(length_d altitude) {position_.setAltitude(altitude);}

    Body* body() const {return position_.body();}
    void setBody(Body* body) {position_.setBody(body);}
protected:
    std::string name_;
    CentroidPosition position_;
};

/*! @} */

AST_NAMESPACE_END