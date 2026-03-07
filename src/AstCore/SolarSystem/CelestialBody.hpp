///
/// @file      CelestialBody.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      21.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include "AstCore/Object.hpp"
#include "AstCore/GravityField.hpp"
#include "AstCore/JplDe.hpp"
#include "AstCore/BodyEphemeris.hpp"
#include "AstCore/BodyOrientation.hpp"
#include "AstCore/AxesBodyInertial.hpp"
#include "AstCore/AxesBodyFixed.hpp"
#include "AstCore/AxesBodyMOD.hpp"
#include "AstCore/AxesBodyTOD.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/ScopedPtr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup SolarSystem
    @{
*/

/// @brief 天体
class AST_CORE_API CelestialBody : public Object
{
    AST_OBJECT(CelestialBody)
public:
    CelestialBody();
    ~CelestialBody() = default;

    /// @brief 获取天体名称
    const std::string& getName() const { return name_; }

    /// @brief 获取重力模型名称
    const std::string& getGravityModel() const{ return gravityField_.getModelName(); }

    /// @brief 从文件加载天体数据
    /// @param  filepath    - 天体数据文件路径
    /// @retval             - 错误码
    err_t load(StringView filepath);

    /// @brief 获取天体半径
    double getRadius() const { return radius_; }

    /// @brief 获取引力常数
    double getGM() const { return gm_; }

    /// @brief 获取系统引力常数
    double getSystemGM() const { return systemGM_; }

    /// @brief 设置重力模型
    err_t setGravityModel(StringView model);
    
    /// @brief 获取J2项
    double getJ2() const { return getJn(2); }

    /// @brief 获取J3项
    double getJ3() const { return getJn(3); }
    
    /// @brief 获取J4项
    double getJ4() const { return getJn(4); }
    
    /// @brief 获取J5项
    double getJ5() const { return getJn(5); }
    
    /// @brief 获取J6项
    double getJ6() const { return getJn(6); }

    /// @brief 是否为地球
    bool isEarth() const { return jplIndex_ == JplDe::eEarth; }

    /// @brief 获取天体姿态
    BodyOrientation* getOrientation() const { return orientation_.get(); }

    /// @brief 获取天体星历
    BodyEphemeris* getEphemeris() const { return ephemeris_.get(); }

    /// @brief 获取天体惯性轴系
    Axes* getAxesInertial() const { return axesInertial_.get(); }

    /// @brief 获取天体固定轴系
    Axes* getAxesFixed() const { return axesFixed_.get(); }
    
    /// @brief 获取天体MOD轴系
    Axes* getAxesMOD() const { return axesMOD_.get(); }
    
    /// @brief 获取天体TOD轴系
    Axes* getAxesTOD() const { return axesTOD_.get(); }

protected:
    /// @brief 获取Jn项
    double getJn(int n) const { return gravityField_.getJn(n); }

    /// @brief 加载重力模型
    err_t loadGravityModel(StringView model);
protected:
    friend class SolarSystem;

    /// @brief 加载相关的天文参数
    err_t loadAstroDefinition(BKVParser& parser);

    /// @brief 加载旋转数据
    err_t loadSpinData(BKVParser& parser);

    /// @brief 加载星历相关参数
    err_t loadEphemerisData(BKVParser& parser);

    /// @brief 加载地球相关参数
    err_t loadEarth(BKVParser& parser);   
    
    /// @brief 加载月球相关参数
    err_t loadMoon(BKVParser& parser);   

    /// @brief 加载MeanEarth定义
    err_t loadMeanEarthDefinition(BKVParser& parser);

    A_DISABLE_COPY(CelestialBody)
PROPERTIES:
    SharedPtr<CelestialBody>    parent_;                   ///< 父天体
    std::string                 name_;                     ///< 天体名称
    double                      gm_{0.0};                  ///< 引力常数
    double                      systemGM_{0.0};            ///< 系统引力常数
    double                      radius_{0.0};              ///< 天体半径
    int                         jplSpiceId_{-1};           ///< JPL SPICE ID
    int                         jplIndex_{-1};             ///< JPL DE Index
    GravityField                gravityField_;             ///< 重力场
    ScopedPtr<BodyOrientation>  orientation_;              ///< 天体姿态
    ScopedPtr<BodyEphemeris>    ephemeris_;                ///< 天体星历
    ScopedPtr<AxesBodyInertial> axesInertial_;             ///< 天体惯性轴
    ScopedPtr<AxesBodyFixed>    axesFixed_;                ///< 天体固定轴
    SharedPtr<AxesBodyMOD>      axesMOD_;                  ///< 天体MOD轴
    SharedPtr<AxesBodyTOD>      axesTOD_;                  ///< 天体TOD轴
};


using HCelestialBody = SharedPtr<CelestialBody>;        ///< 天体句柄
using HBody = HCelestialBody;                           ///< 天体句柄

using PCelestialBody = CelestialBody*;                  ///< 天体指针
using PBody = PCelestialBody;                           ///< 天体指针


/*! @} */

AST_NAMESPACE_END
 


