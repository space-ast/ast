///
/// @file      Sensor.hpp
/// @brief     传感器对象
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
#include "AstUtil/Object.hpp"
#include "AstUtil/StringView.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/FieldOfView.hpp"
#include "AstSim/CentroidPosition.hpp"
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 传感器对象
class AST_SIM_API Sensor: public Point
{
public:
    AST_OBJECT(Sensor)
    Sensor();
    ~Sensor() override = default;
public:
    const std::string& getName() const override { return name_; }
    void setName(StringView name) override { name_ = std::string(name); }

public: // 从 Point 类继承的方法
    Frame* getFrame() const final;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const final;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const final;
public:
    /// @brief 设置传感器视场
    /// @param fov 视场对象指针
    void setFieldOfView(FieldOfView* fov);

    /// @brief 获取传感器视场
    /// @return 视场对象指针
    FieldOfView* getFieldOfView() const { return fov_; }

private:
    std::string name_;               ///< 传感器名称
    WeakPtr<Point> location_;        ///< 传感器位置点, 这里使用弱引用
    SharedPtr<FieldOfView> fov_;     ///< 传感器视场定义
};

/*! @} */

AST_NAMESPACE_END