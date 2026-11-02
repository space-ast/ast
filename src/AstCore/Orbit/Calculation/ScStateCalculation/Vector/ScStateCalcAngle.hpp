///
/// @file      ScStateCalcAngle.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-02
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
#include "AstCore/ScStateCalculation.hpp"
#include "AstCore/Vector.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief 两向量夹角
/// @details 计算两个向量的夹角，结果范围为 [0, π]
class AST_CORE_API ScStateCalcAngle : public ScStateCalculation
{
public:
    AST_OBJECT(ScStateCalcAngle)
    AST_PROPERT(Vector1)
    AST_PROPERT(Vector2)
    ScStateCalcAngle() = default;
    ~ScStateCalcAngle() override = default;
public:
    errc_t calculate(const SpacecraftState& scState, double& result) override;
    Dimension getDimension() const override {return EDimension::eAngle;}
PROPERTIES:
    /// @brief 获取第一个向量
    Vector* getVector1() const { return vector1_.get(); }
    /// @brief 设置第一个向量
    void setVector1(Vector* vector) { vector1_ = vector; }

    /// @brief 获取第二个向量
    Vector* getVector2() const { return vector2_.get(); }
    /// @brief 设置第二个向量
    void setVector2(Vector* vector) { vector2_ = vector; }
private:
    WeakPtr<Vector> vector1_{};     ///< 第一个向量
    WeakPtr<Vector> vector2_{};     ///< 第二个向量
};

/*! @} */

AST_NAMESPACE_END