///
/// @file      AccelerationTransform.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-10-01
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

#include "AccelerationTransform.hpp"

AST_NAMESPACE_BEGIN

static_assert(sizeof(AccelerationTransform) == sizeof(KinematicTransform) + sizeof(Vector3d) * 2, "Memory layout of AccelerationTransform must be compact");

/*!
这里不能像 KinematicTransform 那样，用 reinterpret_cast 把 Transform::rotation_ 直接当成
AccelerationRotation 来复用内存布局。按偏移算（Vector3d 24、Matrix3d 72、Transform 96、KinematicTransform 144）：

    24 ..  96   Transform::rotation_.matrix_          -> AccelerationRotation::matrix_    可以
    96 .. 120   KinematicTransform::angvel_           -> AccelerationRotation::angvel_    可以
   120 .. 144   KinematicTransform::velocity_         -> AccelerationRotation::angvelDot_ 不行，这一格被平移速度占了

即 AccelerationRotation 要求的 (matrix, angvel, angvelDot) 连续布局，
与 KinematicTransform 的(matrix, angvel, velocity) 在第 120 字节处冲突，无法同时成立。
因此 AccelerationTransform::getAccelerationRotation() 只能按值返回。

替代方案是让 AccelerationTransform 直接继承 Transform，并把成员排成
angvel_, angvelDot_, velocity_, acceleration_（偏移 96/120/144/168），这样两种 reinterpret_cast 都能用；
但代价是把 KinematicTransform 的一整套接口原样抄一遍，多出一份关于符号与换系约定的真值来源，
且丢失 is-a KinematicTransform 的降阶关系。相较之下，按值返回一个 120 字节的平凡可拷贝对象要划算得多
*/

AST_NAMESPACE_END
