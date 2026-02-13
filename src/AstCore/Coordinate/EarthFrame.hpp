///
/// @file      EarthFrame.hpp
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


/*!
    @ingroup   Core
    @defgroup  Coordinate 坐标系统
    @brief     提供实现不同坐标系之间的相互转换的函数
    @details   
    
    例如ECI、ECF、MOD、TOD、GTOD、TDR、GCR、PEF、EFG等坐标系之间的转换
    坐标系定义：
    - CBI(Central Body Inertial Frame): 中心天体惯性系
    - CBF(Central Body Fixed Frame): 中心天体固连系
    - ECI(Earth Centered Inertial Frame): 地球惯性系，根据相关的配置，ECI可能为ICRF或J2000
    - J2000: 地球时(TT)2000年1月1日12时0分0秒的地球平赤道系
    - MOD(Mean of Date): 平赤道系，考虑岁差
    - TOD(True of Date): 真赤道系，考虑岁差和章动
    - GTOD(Greenwich True of Date): 格林尼治真赤道系，XY平面为赤道平面，正X轴指向本初子午线，随地球旋转
        参考NASA文档：https://science-data.larc.nasa.gov/LITE/level1doc/gtod.html
        其又称为：
        - TDR(True of Date Rotating frame) 旋转真赤道系，参考orekit文档: https://www.orekit.org/site-orekit-12.0/apidocs/org/orekit/frames/GTODProvider.html
        - GCR(Greenwich Rotating Coordinate frame) 格林尼治旋转系，参考orekit文档
        - PEF(Pseudo Earth-fixed) 伪地球固连系，参考 Kayhan Space: https://app.kayhan.io/docs/data-standards/reference-frames/
        - EFG(Earth-Fixed Greenwich) 地球固连格林尼治系，参考 Kayhan Space
         
    
    - ECF(Earth Centered Fixed Frame): 地球固连系，在GTOD基础上考虑了极移
    - ICRF(International Celestial Reference Frame): 国际天球参考系
    - CIRF(Celestial Intermediate Reference Frame): 天球中间参考系
        天球中间参考系的第一极（z轴方向）为给定时刻的天球中间极（CIP），
        与之等价的基本平面为与该时刻的天球中间极垂直的中间赤道，经度坐标起量点为天球中间零点。
        它类似于传统说法中的真赤道和真春分点参考系，两者基本平面完全相同，
        不同之处在于天球中间参考系的赤经零点为天球中间零点，而真赤道参考系的赤经零点为春分点。
        两个参考系的转换为绕着天球中间极旋转一个角度EO，称为零点差，它的表达式为EO=ERA-GAST，即地球自转角与真恒星时之差。
        由此可以看出，真赤道真春分点参考系可以认为基于春分点的转换中的天球中间参考系。
        由于天球中间参考系的首字母缩略语与国际天球参考系（ICRF）非常接近，因此，在提及该参考系时建议尽量使用全名
    
    - TIRF(Terrestrial Intermediate Reference Frame): 地球中间参考系
        TIRF与国际地球参考系（ITRF）的联系由极移和地球中间零点的定位角确定。
        它通过绕天球中间极CIP的转动角度即地球自转角（ERA）与天球中间参考系（CIRS）相联系。
        天球中间极是这两个中间参考系TIRS和CIRS共同的Z轴。
        因为地球中间参考系的首字母缩写词与国际地球参考系ITRF接近，建议使用全名。

*/

/*!
    @addtogroup Coordinate
    @{
*/

//----------------
// ECI -> ECF
//----------------



/// @brief     从ECI转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_API void aECIToECFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从ECI转换为ECF的坐标旋转变换
/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换，带角速度信息
AST_CORE_API void aECIToECFTransform(const TimePoint& tp, KinematicRotation& rotation);


/// @brief     从ECI转换为ECF的坐标转换矩阵
/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_CAPI void aECIToECFMatrix(const TimePoint& tp, Matrix3d& matrix);


/// @brief     从ECI转换为ECF的坐标转换
/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。
/// @param     tp  时间点
/// @param     vecECI  ECI坐标
/// @param     vecECF  ECF坐标
AST_CORE_API void aECIToECF(const TimePoint& tp, const Vector3d& vecECI, Vector3d& vecECF);



/// @brief     从ECI转换为ECF的坐标转换
/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。
/// @param     tp  时间点
/// @param     vecECI  ECI坐标
/// @param     velECI  ECI速度
/// @param     vecECF  ECF坐标
/// @param     velECF  ECF速度
AST_CORE_API void aECIToECF(
    const TimePoint& tp,
    const Vector3d& vecECI,
    const Vector3d& velECI,
    Vector3d& vecECF,
    Vector3d& velECF
);

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


/// @brief     从J2000转换为ECF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecJ2000  J2000坐标
/// @param     vecECF  ECF坐标
AST_CORE_API void aJ2000ToECF(const TimePoint& tp, const Vector3d& vecJ2000, Vector3d& vecECF);



/// @brief     从J2000转换为ECF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecJ2000  J2000坐标
/// @param     velJ2000  J2000速度
/// @param     vecECF  ECF坐标
/// @param     velECF  ECF速度
AST_CORE_API void aJ2000ToECF(
    const TimePoint& tp,
    const Vector3d& vecJ2000,
    const Vector3d& velJ2000,
    Vector3d& vecECF,
    Vector3d& velECF
);


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
AST_CORE_API void aTODToGTOD(const TimePoint& tp, const Vector3d& vecTOD, Vector3d& vecGTOD);



/// @brief     从TOD转换为GTOD的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecTOD  TOD坐标
/// @param     velTOD  TOD速度
/// @param     vecGTOD  GTOD坐标
/// @param     velGTOD  GTOD速度
AST_CORE_API void aTODToGTOD(
    const TimePoint& tp, 
    const Vector3d& vecTOD, const Vector3d& velTOD,
    Vector3d& vecGTOC, Vector3d& velGTOC
);


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


/// @brief     从ICRF转换为ECF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecICRF  ICRF坐标
/// @param     vecECF  ECF坐标
AST_CORE_API void aICRFToECF(const TimePoint& tp, const Vector3d& vecICRF, Vector3d& vecECF);

/// @brief     从ECF转换为ICRF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecECF  ECF坐标
/// @param     vecICRF  ICRF坐标
AST_CORE_API void aECFToICRF(const TimePoint& tp, const Vector3d& vecECF, Vector3d& vecICRF);

/// @brief     从ICRF转换为ECF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecICRF  ICRF坐标
/// @param     velICRF  ICRF速度
/// @param     vecECF  ECF坐标
/// @param     velECF  ECF速度
AST_CORE_API void aICRFToECF(
    const TimePoint& tp, 
    const Vector3d& vecICRF, const Vector3d& velICRF, 
    Vector3d& vecECF, Vector3d& velECF
);

// ----------------
// ICRF -> CIRF
// ----------------


/// @brief     从ICRF转换为CIRF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_CAPI void aICRFToCIRFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从ICRF转换为CIRF的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_CAPI void aICRFToCIRFMatrix(const TimePoint& tp, Matrix3d& matrix);


/// @brief     从ICRF转换为CIRF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecICRF  ICRF坐标
/// @param     vecCIRF  CIRF坐标
AST_CORE_CAPI void aICRFToCIRF(const TimePoint& tp, const Vector3d& vecICRF, Vector3d& vecCIRF);



// ----------------
// CIRF -> TIRF
// ----------------

/// @brief     从CIRF转换为TIRF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_API void aCIRFToTIRFTransform(const TimePoint& tp, Rotation& rotation);

/// @brief     从CIRF转换为TIRF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换，带角速度信息
AST_CORE_API void aCIRFToTIRFTransform(const TimePoint& tp, KinematicRotation& rotation);

/// @brief     从CIRF转换为TIRF的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_CAPI void aCIRFToTIRFMatrix(const TimePoint& tp, Matrix3d& matrix);


/// @brief     从CIRF转换为TIRF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecCIRF  CIRF坐标
/// @param     vecTIRF  TIRF坐标
AST_CORE_API void aCIRFToTIRF(const TimePoint& tp, const Vector3d& vecCIRF, Vector3d& vecTIRF);


/// @brief     从CIRF转换为TIRF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecCIRF  CIRF坐标
/// @param     velCIRF  CIRF速度
/// @param     vecTIRF  TIRF坐标
/// @param     velTIRF  TIRF速度
AST_CORE_API void aCIRFToTIRF(
    const TimePoint& tp, 
     const Vector3d& vecCIRF, const Vector3d& velCIRF,
     Vector3d& vecTIRF, Vector3d& velTIRF
);


// ----------------
// TIRF -> ECF
// ----------------


/// @brief     从TIRF转换为ECF的坐标旋转变换
/// @details   ~
/// @param     tp  时间点
/// @param     rotation  坐标旋转变换
AST_CORE_CAPI void aTIRFToECFTransform(const TimePoint& tp, Rotation& rotation);


/// @brief     从TIRF转换为ECF的坐标转换矩阵
/// @details   ~
/// @param     tp  时间点
/// @param     matrix  坐标转换矩阵
AST_CORE_CAPI void aTIRFToECFMatrix(const TimePoint& tp, Matrix3d& matrix);

/// @brief     从TIRF转换为ECF的坐标转换
/// @details   ~
/// @param     tp  时间点
/// @param     vecTIRF  TIRF坐标
/// @param     vecECF  ECF坐标
AST_CORE_CAPI void aTIRFToECF(const TimePoint& tp, const Vector3d& vecTIRF, Vector3d& vecECF);



/*! @} */

AST_NAMESPACE_END
