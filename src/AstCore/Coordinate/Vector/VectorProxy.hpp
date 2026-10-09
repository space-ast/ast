///
/// @file      VectorProxy.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-09
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
#include "Vector.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Geometry
    @{
*/

/// @brief 向量代理类
/// @details 将向量接口原样转发给被代理的向量对象，自身不产生任何计算。
class AST_CORE_API VectorProxy final: public Vector
{
public:
    VectorProxy() = default;
    VectorProxy(Vector* vector):impl_(vector){}
    ~VectorProxy() override = default;

public:
    void setImpl(Vector* impl){impl_ = impl;}
    Vector* impl() const {return impl_.get();}
public:
    Axes* getAxes() const override;
    errc_t getVector(const TimePoint& tp, Vector3d& vec) const override;
    errc_t getVector(const TimePoint& tp, Vector3d& vec, Vector3d& vel) const override;
    errc_t getVector(const TimePoint& tp, Vector3d& vec, Vector3d& vel, Vector3d& acc) const override;
private:
    WeakPtr<Vector> impl_{};
};




/*! @} */

AST_NAMESPACE_END
