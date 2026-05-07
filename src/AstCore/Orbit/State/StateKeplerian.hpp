///
/// @file      StateKeplerian.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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
#include "AstCore/State.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/
class StateKeplerian;
using PStateKeplerian = StateKeplerian*;
using HStateKeplerian = SharedPtr<StateKeplerian>;


/// @brief 轨道大小描述方式
enum class ESizeType
{
    eSMA,                   ///< 半长轴
    eApoAlt,                ///< 远地点高度
    eApoRad,                ///< 远地点半径
    ePeriAlt,               ///< 近地点高度
    ePeriRad,               ///< 近地点半径
    ePeriod,                ///< 周期
    eMeanMotion,            ///< 平均角速度
};

/// @brief 轨道形状描述方式
enum class EShapeType
{
    eEcc,                   ///< 偏心率
    eApoAlt,                ///< 远地点高度
    eApoRad,                ///< 远地点半径
    ePeriAlt,               ///< 近地点高度
    ePeriRad,               ///< 近地点半径
};

/// @brief 轨道面方向描述方式
enum class EOrientationType
{
    eRAAN,                  ///< 升交点赤经
    eLAN,                   ///< 升交点经度
};

/// @brief 轨道位置描述方式
enum class EPositionType
{
    eTrueAnomaly,               ///< 真近点角
    eMeanAnomaly,               ///< 平近点角
    eEccAnomaly,                ///< 偏近点角   
    eArgLat,                    ///< 纬度幅角
    eTimePastPeri,              ///< 过近地点后经过的时间
    eTimePastAscNode,           ///< 过升交点后经过的时间
    eTimeOfPeriPassage,         ///< 过近地点时刻
    eTimeOfAscNodePassage,      ///< 过升交点时刻
};




/// @brief 开普勒轨道根数状态
class AST_CORE_API StateKeplerian final: public State
{
public:
    AST_OBJECT(StateKeplerian)
    AST_PROPERT(SMA)
    AST_PROPERT(Period)
    AST_PROPERT(MeanMotion)
    AST_PROPERT(ApoRadForSize)
    AST_PROPERT(ApoAltForSize)
    AST_PROPERT(PeriRadForSize)
    AST_PROPERT(PeriAltForSize)
    AST_PROPERT(Ecc)
    AST_PROPERT(ApoRadForShape)
    AST_PROPERT(ApoAltForShape)
    AST_PROPERT(PeriRadForShape)
    AST_PROPERT(PeriAltForShape)
    AST_PROPERT(Inc)
    AST_PROPERT(RAAN)
    AST_PROPERT(LAN)
    AST_PROPERT(ArgPeri)
    AST_PROPERT(TrueAnomaly)
    AST_PROPERT(MeanAnomaly)
    AST_PROPERT(EccAnomaly)
    AST_PROPERT(ArgLat)
    AST_PROPERT(TimePastPeri)
    AST_PROPERT(TimePastAscNode)
    
    static PStateKeplerian New();
    static HStateKeplerian MakeShared();
    static PStateKeplerian New(const ModOrbElem& modOrbElem);
    static HStateKeplerian MakeShared(const ModOrbElem& modOrbElem);
    static PStateKeplerian NewDefault();

    StateKeplerian();
    StateKeplerian(const ModOrbElem& modOrbElem);
    StateKeplerian(const State& state);
    ~StateKeplerian() override = default;
public:
    EStateType getStateType() const override { return EStateType::eKeplerian; }
    errc_t getState(CartState& state) const override;
    errc_t setState(const CartState& state) override;
public:
    void setState(OrbElem& orbElem);
    void getState(OrbElem& orbElem) const;
    errc_t setState(const ModOrbElem& modOrbElem) override { modOrbElem_ = modOrbElem; return eNoError; }
    errc_t getState(ModOrbElem& modOrbElem) const override { modOrbElem = modOrbElem_; return eNoError; }
public:
    /// @brief 获取在内部表示方式下的状态
    void getInnerRepresentationState(array6d& stateInRepresentation) const;
    /// @brief 获取在指定表示方式下的状态
    void getStateInRepresentation(
        ESizeType sizeType, EShapeType shapeType, 
        EOrientationType orientationType, EPositionType positionType,
        array6d& stateInRepresentation
    ) const;
    double getStateParam(int index);
    void setStateParam(int index, double value);
    /// @brief 获取定义轨道大小的参数
    double getSizeParam() const;
    double getSizeParam(ESizeType sizeType) const;
    void setSizeParam(double sizeParam, ESizeType sizeType);
    void setSizeParam(double sizeParam);
    void setSizeType(ESizeType sizeType);
    /// @brief 获取定义轨道形状的参数
    double getShapeParam() const;
    double getShapeParam(EShapeType shapeType) const;
    void setShapeParam(double shapeParam, EShapeType shapeType);
    void setShapeParam(double shapeParam);
    void setShapeType(EShapeType shapeType);
    /// @brief 获取定义轨道面方向的参数
    double getOrientationParam() const;
    double getOrientationParam(EOrientationType orientationType) const;
    void setOrientationParam(double orientationParam, EOrientationType orientationType);
    void setOrientationParam(double orientationParam);
    void setOrientationType(EOrientationType orientationType);
    /// @brief 获取定义轨道位置的参数
    double getPositionParam() const;
    double getPositionParam(EPositionType positionType) const;
    void setPositionParam(double positionParam, EPositionType positionType);
    void setPositionParam(double positionParam);
    void setPositionType(EPositionType positionType);
public:
    /// @brief 获取远地点高度
    double getApoAlt() const;

    /// @brief 获取远地点半径
    double getApoRad() const;

    /// @brief 获取近地点高度
    double getPeriAlt() const;

    /// @brief 获取近地点半径
    double getPeriRad() const;

    /// @brief 获取轨道大小的定义方式
    ESizeType getSizeType() const{return sizeType_;}
    
    /// @brief 获取轨道形状的定义方式
    EShapeType getShapeType() const{return shapeType_;}

    /// @brief 获取轨道面方向的定义方式
    EOrientationType getOrientationType() const{return orientationType_;}

    /// @brief 获取轨道位置的定义方式
    EPositionType getPositionType() const{return positionType_;}
PROPERTIES:
    /// @brief 获取半长轴
    length_d getSMA() const;
    /// @brief 设置半长轴
    void setSMA(length_d sma);

    /// @brief 获取周期
    time_d getPeriod() const;
    /// @brief 设置周期
    void setPeriod(time_d period);

    /// @brief 获取平均角速度
    angvel_d getMeanMotion() const;
    /// @brief 设置平均角速度
    void setMeanMotion(angvel_d meanMotion);

    length_d getApoRadForSize() const{return getApoRad();}
    /// @brief 设置远地点半径（描述轨道大小）
    void setApoRadForSize(length_d apoRad);

    length_d getApoAltForSize() const{return getApoAlt();}
    /// @brief 设置远地点高度（描述轨道大小）
    void setApoAltForSize(length_d apoAlt);

    length_d getPeriRadForSize() const{return getPeriRad();}
    /// @brief 设置近地点半径（描述轨道大小）
    void setPeriRadForSize(length_d periRad);

    length_d getPeriAltForSize() const{return getPeriAlt();}
    /// @brief 设置近地点高度（描述轨道大小）
    void setPeriAltForSize(length_d periAlt);

    /// @brief 获取离心率
    double getEcc() const;
    /// @brief 设置离心率
    void setEcc(double ecc);

    length_d getApoRadForShape() const{return getApoRad();}
    /// @brief 设置远地点半径（描述轨道形状）
    void setApoRadForShape(length_d apoRad);

    length_d getApoAltForShape() const{return getApoAlt();}
    /// @brief 设置远地点高度（描述轨道形状）
    void setApoAltForShape(length_d apoAlt);

    length_d getPeriRadForShape() const{return getPeriRad();}
    /// @brief 设置近地点半径（描述轨道形状）
    void setPeriRadForShape(length_d periRad);
    
    length_d getPeriAltForShape() const{return getPeriAlt();}
    /// @brief 设置近地点高度（描述轨道形状）
    void setPeriAltForShape(length_d periAlt);

    /// @brief 获取倾角
    angle_d getInc() const;
    /// @brief 设置倾角
    void setInc(angle_d inc);

    /// @brief 获取升交点赤经   
    angle_d getRAAN() const;
    /// @brief 设置升交点赤经
    void setRAAN(angle_d raan);

    /// @brief 获取升交点经度   
    angle_d getLAN() const;
    /// @brief 设置升交点经度
    void setLAN(angle_d lan);

    /// @brief 获取近地点幅角  
    angle_d getArgPeri() const;
    /// @brief 设置近地点幅角
    void setArgPeri(double argPeri);

    /// @brief 获取真近点角
    angle_d getTrueAnomaly() const;
    /// @brief 设置真近点角
    void setTrueAnomaly(angle_d trueAnomaly);

    /// @brief 获取平近点角
    angle_d getMeanAnomaly() const;
    /// @brief 设置平近点角
    void setMeanAnomaly(angle_d meanAnomaly);

    /// @brief 获取偏近点角
    angle_d getEccAnomaly() const;
    /// @brief 设置偏近点角
    void setEccAnomaly(angle_d eccAnomaly);

    /// @brief 获取纬度幅角  
    angle_d getArgLat() const;
    /// @brief 设置纬度幅角
    void setArgLat(angle_d argLat);

    /// @brief 获取过近地点后经过的时间
    time_d getTimePastPeri() const;
    /// @brief 设置过近地点后经过的时间
    void setTimePastPeri(time_d timePastPeri);

    /// @brief 获取过升交点后经过的时间
    time_d getTimePastAscNode() const;
    /// @brief 设置过升交点后经过的时间
    void setTimePastAscNode(time_d timePastAscNode);

public:
    /// @brief 获取过近地点时刻（输出时间点）
    void getTimeOfPeriPassage(TimePoint& tp) const;
    /// @brief 获取过近地点时刻（输出历元秒）
    TimePoint getTimeOfPeriPassage() const;
    /// @brief 设置过近地点时刻（输入时间点）
    void setTimeOfPeriPassage(const TimePoint& tp);
    /// @brief 设置过近地点时刻（输入历元秒）
    void setTimeOfPeriPassage(double epochsecond);

    /// @brief 获取过升交点时刻（输出时间点）
    void getTimeOfAscNodePassage(TimePoint& tp) const;
    /// @brief 获取过升交点时刻（输出历元秒）
    TimePoint getTimeOfAscNodePassage() const;
    /// @brief 设置过升交点时刻（输入时间点）
    void setTimeOfAscNodePassage(const TimePoint& tp);
    /// @brief 获取过近地点时刻（输入历元秒）
    void setTimeOfAscNodePassage(double epochsecond);
private:
    void changeEccHoldingOthers(double ecc, const ModOrbElem& originalElem);
    void changePeriRadHoldingOthers(double periRad, const ModOrbElem& originalElem);
    void changeArgPeriHoldingOthers(double argPeri, const ModOrbElem& originalElem);
private:
    void holdLAN(const ModOrbElem& originalElem);
    void holdMeanAnomaly(const ModOrbElem& originalElem);
    void holdEccAnomaly(const ModOrbElem& originalElem);
    void holdArgLat(const ModOrbElem& originalElem);
    void holdTimePastAscNode(const ModOrbElem& originalElem);
    void holdTimePastPeri(const ModOrbElem& originalElem);
protected:
    ModOrbElem modOrbElem_{};                                       ///< 轨道根数
    ESizeType  sizeType_{ESizeType::eSMA};                          ///< 大小描述方式
    EShapeType shapeType_{EShapeType::eEcc};                        ///< 形状描述方式
    EOrientationType orientationType_{EOrientationType::eRAAN};     ///< 轨道面方向描述方式
    EPositionType positionType_{EPositionType::eTrueAnomaly};       ///< 位置描述方式
};

/*! @} */

AST_NAMESPACE_END
