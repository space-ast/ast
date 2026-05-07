///
/// @file      Mover.hpp
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
#include "AstCore/Object.hpp"
#include "AstCore/Point.hpp"
#include "AstSim/AttitudeProfile.hpp"
#include "AstSim/MotionProfile.hpp"
#include "AstCore/Ephemeris.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 运动对象
class AST_SIM_API Mover: public Point
{
public:
    AST_OBJECT(Mover)
    AST_PROPERT(MotionProfile)
    AST_PROPERT(AttitudeProfile)
    AST_PROPERT(Ephemeris)
    Mover() = default;
    ~Mover() override = default;

public:
    /// @brief 设置名称
    void setName(StringView name) override { name_ = std::string(name); }

    /// @brief 获取名称
    const std::string& getName() const override { return name_; }

public:
    // ScopedPtr<MotionProfile>& getMotionProfileHandle() { return motionProfile_; }

    /// @brief 获取星历句柄
    /// @return 星历句柄
    ScopedPtr<Ephemeris>& getEphemerisHandle() { return ephemeris_; }
public:
    /// @brief 生成星历
    /// @return 错误码
    errc_t generateEphemeris();

public: // 从Point继承重写的函数
    Frame* getFrame() const final;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const final;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const final;
PROPERTIES:
    /// @brief 获取运动定义
    /// @return 运动定义指针
    MotionProfile* getMotionProfile() const { return motionProfile_.get(); }
    /// @brief 设置运动定义
    /// @param profile 运动定义指针
    void setMotionProfile(MotionProfile* profile);
    /// @brief 获取姿态定义
    /// @return 姿态定义指针
    AttitudeProfile* getAttitudeProfile() const { return attitudeProfile_.get(); }
    /// @brief 设置姿态定义
    /// @param profile 姿态定义指针
    void setAttitudeProfile(AttitudeProfile* profile) { attitudeProfile_ = profile; }
    /// @brief 获取星历
    /// @return 星历指针
    Ephemeris* getEphemeris() const { return ephemeris_.get(); }
    /// @brief 设置星历
    /// @param ephemeris 星历指针
    void setEphemeris(Ephemeris* ephemeris) { ephemeris_ = ephemeris; }
public:
    
    /// @brief 获取初始状态
    /// @warning 如果运动定义不是轨道动力学模型，则返回空指针
    /// @return 初始状态指针
    State* getInitialState() const;

protected:
    std::string                 name_;                  ///< 名称
    WeakPtr<MotionProfile>      motionProfile_;         ///< 运动定义
    WeakPtr<AttitudeProfile>    attitudeProfile_;       ///< 姿态定义
    ScopedPtr<Ephemeris>        ephemeris_;             ///< 星历
};

/*! @} */

AST_NAMESPACE_END