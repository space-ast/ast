///
/// @file      CentroidPosition.hpp
/// @brief     质心位置类
/// @author    axel
/// @date      2026-04-07
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstCore/GeodeticPoint.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 质心位置类
/// @details 表示对象在空间中的质心位置
class AST_SIM_API CentroidPosition
{
public:
    CentroidPosition() = default;
    ~CentroidPosition() = default;

    void setBody(Body* body);
    Body* body() const{return body_.get();}
    const GeodeticPoint& getPosition() const{return position_;}
    void setPosition(const GeodeticPoint& position){position_ = position;}
    void setLatitude(double latitude) {position_.setLatitude(latitude);}
    void setLongitude(double longitude) {position_.setLongitude(longitude);}
    void setAltitude(double altitude) {position_.setAltitude(altitude);}
    double latitude() const {return position_.latitude();}
    double longitude() const {return position_.longitude();}
    double altitude() const {return position_.altitude();}
private:
    SharedPtr<Body>     body_;                ///< 天体名称
    GeodeticPoint       position_{};          ///< 质心位置点
};

/*! @} */

AST_NAMESPACE_END