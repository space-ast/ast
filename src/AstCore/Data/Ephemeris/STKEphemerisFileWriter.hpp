///
/// @file      STKEphemerisFileWriter.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-04
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
#include "EphemerisFileWriter.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/CelestialBody.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 保存STK星历文件
/// @param filepath 星历文件路径
/// @param ephemeris 星历对象
/// @param interval 时间间隔
/// @param step 时间步长
/// @return 错误码
AST_CORE_API errc_t aSaveSTKEphemeris(StringView filepath, Ephemeris& ephemeris, const TimeInterval& interval = TimeInterval::Whole(), double step = 60);


/// @brief STK星历文件写入器
/// @details 用于将星历以STK格式写入文件，支持自定义时间间隔、时间步长、参考天体、参考轴系等参数
class AST_CORE_API STKEphemerisFileWriter final: public EphemerisFileWriter
{
public:
    enum ECoordinate
    {
        eAuto = -1,     ///< 自动选择参考轴系
        eICRF,          ///< ICRF坐标系
        eJ2000,         ///< J2000坐标系
        eInertial,      ///< 惯性坐标系
        eFixed,         ///< 固定坐标系
    };

    STKEphemerisFileWriter() = default;
    ~STKEphemerisFileWriter() = default;
    A_DISABLE_COPY(STKEphemerisFileWriter)

    errc_t write(StringView filename, const Ephemeris& ephemeris) const override;
public:
    const TimeInterval& interval() const{return interval_;}
    void setInterval(TimeInterval interval){interval_ = interval;}
    double step() const{return step_;}
    void setStep(double step){step_ = step;}
    Body* body() const{return body_;}
    void setBody(Body* body){body_ = body;}
    ECoordinate coordinate() const{return coordinate_;}
    void setCoordinate(ECoordinate coordinate){coordinate_ = coordinate;}
private:
    TimeInterval interval_{TimeInterval::Whole()};     ///< 时间间隔，最终存储的时段是其和星历自身时段的交集
    double step_{};                                    ///< 时间步长（秒），非正值表示取星历自身的离散时间点
    Body* body_{nullptr};                              ///< 天体，如果为空则使用星历参考系对应的天体
    ECoordinate coordinate_{eAuto};                    ///< 参考轴系
};


/*! @} */

AST_NAMESPACE_END
