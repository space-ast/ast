///
/// @file      Vector.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-05-14
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
#include "AstMath/Vector.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

class AST_CORE_API Vector : public ObjectNamed
{
public:
  AST_OBJECT(Vector)

  Vector() = default;
  ~Vector() override = default;

  /// @brief 获取向量所在的参考坐标系
  /// @return 向量所在的参考坐标系指针
  virtual Axes *getAxes() const = 0;

  /// @brief 获取向量在指定时间点的值，相对于向量的参考坐标系
  /// @param tp 时间点
  /// @param vec 输出参数，向量的值
  /// @return 错误码
  virtual errc_t getVector(const TimePoint &tp, Vector3d &vec) const = 0;

  /// @brief 获取向量在指定时间点的位置和速度，相对于向量的参考坐标系
  /// @param tp 时间点
  /// @param vec 输出参数，向量的值
  /// @param vel 输出参数，向量的速度向量
  /// @return 错误码
  virtual errc_t getVector(const TimePoint &tp, Vector3d &vec, Vector3d &vel) const = 0;

  /// @brief 获取向量在指定轴系下的值
  /// @param targetAxes 目标轴系指针
  /// @param tp 时间点
  /// @param vec 输出参数，向量在目标轴系下的值
  /// @return 错误码
  errc_t getVectorIn(Axes *targetAxes, const TimePoint &tp, Vector3d &vec) const;

  /// @brief 获取向量在指定轴系下的值和速度
  /// @param targetAxes 目标轴系指针
  /// @param tp 时间点
  /// @param vec 输出参数，向量在目标轴系下的值
  /// @param vel 输出参数，向量的速度向量在目标轴系下的值
  /// @return 错误码
  errc_t getVectorIn(Axes *targetAxes, const TimePoint &tp, Vector3d &vec, Vector3d &vel) const;

  /// @brief 获取向量在指定坐标系下的值（仅使用坐标系的轴系部分，忽略原点平移）
  /// @param targetFrame 目标坐标系指针
  /// @param tp 时间点
  /// @param vec 输出参数，向量在目标坐标系下的值
  /// @return 错误码
  errc_t getVectorIn(Frame *targetFrame, const TimePoint &tp, Vector3d &vec) const;

  /// @brief 获取向量在指定坐标系下的值和速度（仅使用坐标系的轴系部分，忽略原点平移）
  /// @param targetFrame 目标坐标系指针
  /// @param tp 时间点
  /// @param vec 输出参数，向量在目标坐标系下的值
  /// @param vel 输出参数，向量的速度向量在目标坐标系下的值
  /// @return 错误码
  errc_t getVectorIn(Frame *targetFrame, const TimePoint &tp, Vector3d &vec, Vector3d &vel) const;
};

AST_NAMESPACE_END
