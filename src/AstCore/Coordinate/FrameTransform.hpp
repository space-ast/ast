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


/*
 * 
 * 坐标系定义：
 * 
 * ECI(Earth Centered Inertial Frame): 地球惯性系，根据相关的配置，ECI可能为ICRF或J2000
 * ICRF(International Celestial Reference Frame): 国际天球参考系
 * J2000: 地球时(TT)2000年1月1日12时0分0秒的地球平赤道系
 * MOD(Mean of Date): 平赤道系，考虑岁差
 * TOD(True of Date): 真赤道系，考虑岁差和章动
 * GTOD(Greenwich True of Date): 格林尼治真赤道系，XY平面为赤道平面，正X轴指向本初子午线，随地球旋转
 *      参考NASA文档：https://science-data.larc.nasa.gov/LITE/level1doc/gtod.html
 *      其又称为：
 *      - TDR(True of Date Rotating frame) 旋转真赤道系，参考orekit文档: https://www.orekit.org/site-orekit-12.0/apidocs/org/orekit/frames/GTODProvider.html
 *      - GCR(Greenwich Rotating Coordinate frame) 格林尼治旋转系，参考orekit文档
 *      - PEF(Pseudo Earth-fixed) 伪地球固连系，参考 Kayhan Space: https://app.kayhan.io/docs/data-standards/reference-frames/
 *      - EFG(Earth-Fixed Greenwich) 地球固连格林尼治系，参考 Kayhan Space
 *      
 * ECF(Earth Centered Fixed Frame): 地球固连系，在GTOD基础上考虑了极移
 * 
 */




//----------------
// ECI -> ECF
//----------------


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

//----------------
// J2000 -> ECF
//----------------

/// @brief     从J2000转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_API void aJ2000ToECFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从J2000转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换，带角速度信息
AST_CORE_API void aJ2000ToECFTransform(const TimePoint& tp, KinematicRotation& rotation);



/// @brief     从J2000转换为ECF的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_API void aJ2000ToECFMatrix(const TimePoint& tp, Matrix3d& matrix);


//----------------
// ICRF -> ECF
//----------------


/// @brief     从ICRF转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_API void aICRFToECFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从ICRF转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换，带角速度信息
AST_CORE_API void aICRFToECFTransform(const TimePoint& tp, KinematicRotation& rotation);


/// @brief     从ICRF转换为ECF的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_CAPI void aICRFToECFMatrix(const TimePoint& tp, Matrix3d& matrix);


//----------------
// J2000 -> MOD
//----------------


/// @brief     从J2000转换为MOD的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_CAPI void aJ2000ToMODTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从J2000转换为MOD的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_CAPI void aJ2000ToMODMatrix(const TimePoint& tp, Matrix3d& matrix);



/// @brief     从J2000转换为MOD的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecJ2000  J2000坐标
/// @param     vecMOD  MOD坐标
AST_CORE_CAPI void aJ2000ToMOD(const TimePoint& tp, const Vector3d& vecJ2000, Vector3d& vecMOD); 


//----------------
// MOD -> TOD
//----------------


/// @brief     从MOD转换为TOD的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_CAPI void aMODToTODTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从MOD转换为TOD的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_CAPI void aMODToTODMatrix(const TimePoint& tp, Matrix3d& matrix);


/// @brief     从MOD转换为TOD的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecMOD  MOD坐标
/// @param     vecTOD  TOD坐标
AST_CORE_CAPI void aMODToTOD(const TimePoint& tp, const Vector3d& vecMOD, Vector3d& vecTOD);




//----------------
// TOD -> GTOD
//----------------


/// @brief     从TOD转换为GTOD的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_API void aTODToGTODTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从TOD转换为GTOD的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换，带角速度信息
AST_CORE_API void aTODToGTODTransform(const TimePoint& tp, KinematicRotation& rotation);


/// @brief     从TOD转换为GTOD的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_API void aTODToGTODMatrix(const TimePoint& tp, Matrix3d& matrix);


/// @brief     从TOD转换为GTOD的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecTOD  TOD坐标
/// @param     vecGTOD  GTOD坐标
AST_CORE_CAPI void aTODToGTOD(const TimePoint& tp, const Vector3d& vecTOD, Vector3d& vecGTOD);


//----------------
// GTOD -> ECF
//----------------


/// @brief     从GTOD转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_CAPI void aGTODToECFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从GTOD转换为ECF的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_CAPI void aGTODToECFMatrix(const TimePoint& tp, Matrix3d& matrix);



/// @brief     从GTOD转换为ECF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecGTOD  GTOD坐标
/// @param     vecECF  ECF坐标
AST_CORE_CAPI void aGTODToECF(const TimePoint& tp, const Vector3d& vecGTOD, Vector3d& vecECF);



AST_NAMESPACE_END
