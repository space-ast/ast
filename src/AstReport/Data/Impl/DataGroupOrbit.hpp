///
/// @file      DataGroupOrbit.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-06
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
#include "AstReport/DataGroupTimeVar.hpp"
#include "AstCore/Point.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Span.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 轨道数据组
class DataGroupOrbit : public DataGroupTimeVar
{
public:
    struct Data
    {
        double gm_{};
        Vector3d pos_{};
        Vector3d vel_{};
    };
    DataGroupOrbit() = default;
    ~DataGroupOrbit() = default;
public:
    errc_t calculate(const TimeList& timeList, VariantVector& result) const override;
public:
    errc_t calculate(const TimeList& timeList, std::vector<Data>& result) const;
    errc_t calculate(const TimeList& timeList, Span<Data> result) const;
public:
    Point* getPoint() const;
    Frame* getFrame() const;
    Body* getBody() const;
    double getGM() const;
private:
    WeakPtr<Point> point_;              ///< 点
    WeakPtr<Frame> frame_;              ///< 坐标系
    WeakPtr<Body> body_;                ///< 天体
    bool useBodyInertial_{true};        ///< 是否使用天体惯性坐标系
};


/*! @} */

AST_NAMESPACE_END
