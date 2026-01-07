///
/// @file      FrameTransform.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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

class TimePoint;
class Rotation;
class KinematicRotation;

/// @brief     从ECI转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
/// @return    err_t  错误码
AST_CORE_API err_t aECIToECFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从ECI转换为ECF的坐标旋转变换
/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换，带角速度信息
/// @return    err_t  错误码
AST_CORE_API err_t aECIToECFTransform(const TimePoint& tp, KinematicRotation& rotation);


/// @brief     从ECI转换为ECF的坐标转换矩阵
/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
/// @return    err_t  错误码
AST_CORE_CAPI err_t aECIToECFMatrix(const TimePoint& tp, Matrix3d& matrix);


/// @brief     从J2000转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
/// @return    err_t  错误码
AST_CORE_API err_t aJ2000ToECFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从J2000转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换，带角速度信息
/// @return    err_t  错误码
AST_CORE_API err_t aJ2000ToECFTransform(const TimePoint& tp, KinematicRotation& rotation);



/// @brief     从J2000转换为ECF的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
/// @return    err_t  错误码
AST_CORE_API err_t aJ2000ToECFMatrix(const TimePoint& tp, Matrix3d& matrix);



/// @brief     从ICRF转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
/// @return    err_t  错误码
AST_CORE_API err_t aICRFToECFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从ICRF转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换，带角速度信息
/// @return    err_t  错误码
AST_CORE_API err_t aICRFToECFTransform(const TimePoint& tp, KinematicRotation& rotation);


/// @brief     从ICRF转换为ECF的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
/// @return    err_t  错误码
AST_CORE_CAPI err_t aICRFToECFMatrix(const TimePoint& tp, Matrix3d& matrix);


/// @brief     从J2000转换为MOD的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
/// @return    err_t  错误码
AST_CORE_CAPI void aJ2000ToMODTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从J2000转换为MOD的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
/// @return    err_t  错误码
AST_CORE_CAPI void aJ2000ToMODMatrix(const TimePoint& tp, Matrix3d& matrix);



/// @brief     从J2000转换为MOD的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecJ2000  J2000坐标
/// @param     vecMOD  MOD坐标
/// @return    err_t  错误码
AST_CORE_CAPI void aJ2000ToMOD(const TimePoint& tp, const Vector3d& vecJ2000, Vector3d& vecMOD); 


/// @brief     从MOD转换为TOD的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
/// @return    err_t  错误码
AST_CORE_CAPI void aMODToTODTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从MOD转换为TOD的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
/// @return    err_t  错误码
AST_CORE_CAPI void aMODToTODMatrix(const TimePoint& tp, Matrix3d& matrix);


/// @brief     从MOD转换为TOD的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecMOD  MOD坐标
/// @param     vecTOD  TOD坐标
/// @return    err_t  错误码
AST_CORE_CAPI void aMODToTOD(const TimePoint& tp, const Vector3d& vecMOD, Vector3d& vecTOD);


AST_NAMESPACE_END
