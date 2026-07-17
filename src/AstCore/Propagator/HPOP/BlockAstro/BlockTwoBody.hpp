///
/// @file      BlockTwoBody.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-19
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
#include "AstCore/BlockAstro.hpp"
#include "AstCore/BlockDerivative.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/// @brief 二体引力函数块
/// @details
/// 计算中心天体的点质量引力加速度（牛顿万有引力）。
///
/// 方程：
///   a = -GM / r³ · r
///
/// 其中：
///   GM  = 中心天体引力常数（twoBodyGM_）
///   r   = 航天器位置向量（以中心天体为参考）
///
/// 输入端口：
///   - "Pos"：位置向量（3维，预报坐标系下）
///
/// 输出端口：
///   - "AccTwoBody"：二体引力加速度（3维）
///
/// 导数端口：
///   - "Vel"：速度导数（3维），累加二体引力加速度
///
/// @note 当重力场阶数为 0 时，BlockGravity 退化为本块
class AST_CORE_API BlockTwoBody : public BlockDerivative
{
public:
    BlockTwoBody();

    BlockTwoBody(const BlockTwoBody&) = delete;
    BlockTwoBody& operator=(const BlockTwoBody&) = delete;

    explicit BlockTwoBody(double twoBodyGM);

    errc_t run(const SimTime& simTime) override;
private:
    void init();
protected:
    Vector3d* posCBI;                               ///< 位置
    Vector3d* accTwoBody;                           ///< 二体加速度
    Vector3d* velocityDerivative_{&vectorBuffer};   ///< 速度导数
    Vector3d  vectorBuffer;                         ///< 向量缓冲区
PROPERTIES:
    double    twoBodyGM_;                           ///< 中心天体的引力参数
};

AST_NAMESPACE_END
