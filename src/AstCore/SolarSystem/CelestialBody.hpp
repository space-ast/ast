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
    CelestialBody() = default;
    ~CelestialBody() = default;

    /// @brief 获取天体名称
    const std::string& getName() const { return name_; }

    /// @brief 获取重力模型名称
    const std::string& getGravityModel() const{ return gravityField_.getModelName(); }

    /// @brief 从文件加载天体数据
    /// @param  filepath    - 天体数据文件路径
    /// @retval             - 错误码
    err_t load(StringView filepath);

    /// @brief 获取引力常数
    double getGM() const { return gm_; }


protected:
    /// @brief 加载相关的天文参数
    err_t loadAstroDefinition(BKVParser& parser);

    /// @brief 加载旋转数据
    err_t loadSpinData(BKVParser& parser);

    /// @brief 加载星历相关参数
    err_t loadEphemerisData(BKVParser& parser);

    /// @brief 加载地球相关参数
    err_t loadEarth(BKVParser& parser);    

PROPERTIES:
    SharedPtr<CelestialBody> parent_;          ///< 父天体
    std::string     name_;                     ///< 天体名称
    double          gm_{0.0};                  ///< 引力常数
    double          systemGm_{0.0};            ///< 系统引力常数
    int             jplSpiceId_{-1};           ///< JPL SPICE ID
    int             jplIndex_{-1};             ///< JPL Index
    GravityField    gravityField_;             ///< 重力场
};

/*! @} */

AST_NAMESPACE_END
 


