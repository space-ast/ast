///
/// @file      LocalOrbitFrame.hpp
/// @brief     局部(轨道)坐标系
/// @details   局部(轨道)坐标系是一种基于位置或速度向量的坐标系
/// @author    axel
/// @date      2026-01-13
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

AST_NAMESPACE_BEGIN


/*
 *
 * 局部轨道坐标系的定义：
 * LVLH: X轴指向位置方向，Z轴指向轨道法向
 * LVLH_CCSDS: Z轴指向位置反方向，Y轴指向轨道法向反方向，CCSDS 定义的LVLH坐标系，FreeFlyer里的LVLH
 * VVLH: Z轴指向位置反方向，Y轴指向轨道法向反方向
 * VNC: X轴指向速度方向，Y轴指向轨道法向
 * ENU: 东北天，Z轴指向位置方向，北极轴在 (+Y +Z) 半平面内
 * NED: 北东地，Z轴指向位置反方向，北极轴在 (+Y ±Z) 半平面内
 * Launch: 发射坐标系，其跟随地球旋转，Y轴指向当地天顶方向，X轴指向发射方向
 * LCI: 发射惯性系，在天体惯性系下其空间指向不随时间变化，在发射时刻与Launch坐标系重合
 */


/// @brief     计算位置和速度向量所在坐标系到VVLH坐标系的转换矩阵
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     matrix 转换矩阵
AST_CORE_CAPI err_t aFrameToVVLHMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix);


/// @brief     计算VVLH坐标系到位置和速度向量所在坐标系的转换矩阵
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     matrix 转换矩阵
AST_CORE_CAPI err_t aVVLHToFrameMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix);


/// @brief     计算位置和速度向量所在坐标系到LVLH坐标系的转换矩阵
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     matrix 转换矩阵
AST_CORE_CAPI err_t aFrameToLVLHMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix);


/// @brief     计算LVLH坐标系到位置和速度向量所在坐标系的转换矩阵
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     matrix 转换矩阵
AST_CORE_CAPI err_t aLVLHToFrameMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix);


/// @brief     计算位置向量所在坐标系到ENU坐标系的转换矩阵
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     matrix 转换矩阵
AST_CORE_CAPI err_t aFrameToENUMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix);


/// @brief     计算ENU坐标系到位置和速度向量所在坐标系的转换矩阵
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     matrix 转换矩阵
AST_CORE_CAPI err_t aENUToFrameMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix);


/// @brief     计算位置和速度向量所在坐标系到VNC坐标系的转换矩阵
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     matrix 转换矩阵
AST_CORE_CAPI err_t aFrameToVNCMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix);


/// @brief     计算VNC坐标系到位置和速度向量所在坐标系的转换矩阵
/// @param     posInFrame 位置向量
/// @param     velInFrame 速度向量
/// @param     matrix 转换矩阵
AST_CORE_CAPI err_t aVNCToFrameMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix);



AST_NAMESPACE_END
