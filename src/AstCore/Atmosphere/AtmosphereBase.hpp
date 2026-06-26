///
/// @file      AtmosphereBase.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-25
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
#include "Atmosphere.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_CORE_API AtmosphereBase: public Atmosphere
{
public:
    AtmosphereBase(Frame* frame, BodyShape* bodyShape);
    ~AtmosphereBase() override = default;

    /// @brief 获取大气密度
    /// @param tp 当前时间
    /// @param posInBodyFixed 当前位置(相对于getFrame()返回的坐标系)
    /// @return 大气密度（单位：kg/m³）
    double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const override = 0;

    /// @brief 获取大气模型的参考坐标系
    Frame* getFrame() const override{return frame_;}
public:
    /// @brief 是否使用近似高度计算
    bool useApproximateAltitude() const {return useApproximateAltitude_;}

    /// @brief 设置是否使用近似高度计算
    void setUseApproximateAltitude(bool useApproximateAltitude) {useApproximateAltitude_ = useApproximateAltitude;}

    /// @brief 获取位置的地理坐标
    /// @param posInBodyFixed 当前位置(相对于getFrame()返回的坐标系)
    /// @param latitude 纬度（单位：弧度）
    /// @param longitude 经度（单位：弧度）
    /// @param altitude 高度（单位：米），根据是否使用近似高度计算，其返回值不同
    void getGeodetic(const Vector3d& posInBodyFixed, double& latitude, double& longitude, double& altitude) const;

    /// @brief 获取位置的高度
    /// @param posInBodyFixed 当前位置(相对于getFrame()返回的坐标系)
    /// @param altitude 高度（单位：米），根据是否使用近似高度计算，其返回值不同
    double getAltitude(const Vector3d& posInBodyFixed) const;
protected:
    Frame* frame_{nullptr};              ///< 大气模型的参考坐标系
    BodyShape* bodyShape_{nullptr};      ///< 大气模型对应天体形状
    bool useApproximateAltitude_{false}; ///< 是否使用近似高度计算
};

/*! @} */

AST_NAMESPACE_END
