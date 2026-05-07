///
/// @file      CelestialBody.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      21.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include "AstCore/Object.hpp"
#include "AstCore/GravityField.hpp"
#include "AstCore/JplDe.hpp"
#include "AstCore/BodyEphemeris.hpp"
#include "AstCore/BodyOrientation.hpp"
#include "AstCore/AxesBodyInertial.hpp"
#include "AstCore/AxesBodyFixed.hpp"
#include "AstCore/AxesBodyMOD.hpp"
#include "AstCore/AxesBodyTOD.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/AxesAPI.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/ScopedPtr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup SolarSystem
    @{
*/

class CelestialBody;
using Body = CelestialBody;

/// @brief 天体
class AST_CORE_API CelestialBody : public Point
{
public:
    AST_OBJECT(CelestialBody)
    CelestialBody();
    CelestialBody(SolarSystem* solarSystem);
    CelestialBody(StringView name, SolarSystem* solarSystem = nullptr);
    ~CelestialBody();

    /// @brief 获取天体名称
    const std::string& getName() const override { return name_; }
    const std::string& name() const { return name_; }
    void setName(StringView name) override { name_ = std::string(name); }
    
    /// @brief 获取JPL SPICE ID
    int getJplSpiceId() const { return jplSpiceId_; }
    void setJplSpiceId(int id) { jplSpiceId_ = id; }

    /// @brief 获取JPL索引
    int getJplIndex() const { return jplIndex_; }
    void setJplIndex(int index);

    /// @brief 获取天体数据目录路径
    std::string getDirpath() const;

    /// @brief 获取太阳系
    SolarSystem* getSolarSystem() const;
    
    /// @brief 获取父天体
    CelestialBody* getParent() const { return parent_.get(); }
public:

    /// @brief 获取重力模型名称
    const std::string& getGravityModel() const{ return gravityField_.getModelName(); }

    /// @brief 从文件加载天体数据
    /// @param  filepath    - 天体数据文件路径
    /// @retval             - 错误码
    errc_t load(StringView filepath);

    /// @brief 获取天体半径
    double getRadius() const { return radius_; }

    /// @brief 获取引力常数
    double getGM() const { return gm_; }

    /// @brief 获取系统引力常数
    double getSystemGM() const { return systemGM_; }

    /// @brief 设置重力模型
    errc_t setGravityModel(StringView model);
    
    /// @brief 获取J2项
    double getJ2() const { return getJn(2); }

    /// @brief 获取J3项
    double getJ3() const { return getJn(3); }
    
    /// @brief 获取J4项
    double getJ4() const { return getJn(4); }
    
    /// @brief 获取J5项
    double getJ5() const { return getJn(5); }
    
    /// @brief 获取J6项
    double getJ6() const { return getJn(6); }

    /// @brief 是否为地球
    bool isEarth() const { return jplIndex_ == JplDe::eEarth; }

    /// @brief 获取天体位置（ICRF）
    /// @param  tp          - 时间点
    /// @param  pos         - 输出位置（ICRF）
    /// @retval             - 错误码
    errc_t getPosICRF(const TimePoint& tp, Vector3d& pos) const;


    /// @brief 获取天体位置和速度（ICRF）
    /// @param  tp          - 时间点
    /// @param  pos         - 输出位置（ICRF）
    /// @param  vel         - 输出速度（ICRF）
    /// @retval             - 错误码
    errc_t getPosVelICRF(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const;

public: // 从Point继承重写的函数

    Frame* getFrame() const final;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const final;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const final;

public: // 天体的形状、重力场、星历、姿态

    /// @brief 获取天体形状
    BodyShape* getShape() const { return shape_.get(); }

    /// @brief 获取天体重力场
    const GravityField& getGravityField() const { return gravityField_; }

    /// @brief 获取天体星历
    BodyEphemeris* getEphemeris() const { return ephemeris_.get(); }

    /// @brief 获取天体姿态
    BodyOrientation* getOrientation() const { return orientation_.get(); }

public:

    /// @brief 获取天体中心
    Point* getPointCenter() const { return const_cast<CelestialBody*>(this); }

    /// @brief 获取天体惯性轴系
    Axes* getAxesInertial() const { return axesInertial_.get(); }

    /// @brief 获取天体固定轴系
    Axes* getAxesFixed() const { return axesFixed_.get(); }
    
    /// @brief 获取天体MOD轴系
    Axes* getAxesMOD() const { return axesMOD_.get(); }
    
    /// @brief 获取天体TOD轴系
    Axes* getAxesTOD() const { return axesTOD_.get(); }

    /// @brief 获取天体轴系
    /// @param  name        - 轴系名称，例如Inertial、Fixed、MOD、TOD等
    /// @retval             - 轴系指针
    /// @note               - 如果轴系不存在，则返回nullptr
    Axes* getAxes(StringView name) const;
    
public:
    /// @brief 创建新的历元轴系
    /// @param  sourceAxes  - 源轴系
    /// @param  tp          - 时间点
    /// @param  reference   - 参考轴系
    /// @retval             - 新的历元轴系
    HAxes makeEpochAxes(Axes* sourceAxes, const TimePoint& tp, Axes* reference) const{return aMakeEpochAxes(sourceAxes, tp, reference);}

    /// @brief 创建新的历元轴系
    /// @param  sourceAxes  - 源轴系
    /// @param  time        - 时间点
    /// @param  reference   - 参考轴系
    /// @retval             - 新的历元轴系
    HAxes makeEpochAxes(Axes* sourceAxes, EventTime* time, Axes* reference) const{return aMakeEpochAxes(sourceAxes, time, reference);}


    /// @brief 创建新的历元坐标系
    /// @param  sourceAxes  - 源轴系
    /// @param  tp          - 时间点
    /// @param  reference   - 参考轴系
    /// @retval             - 新的历元坐标系
    HFrame makeEpochFrame(Axes* sourceAxes, const TimePoint& tp, Axes* reference) const;

    /// @brief 创建新的历元坐标系
    /// @param  sourceAxes  - 源轴系
    /// @param  time        - 时间点
    /// @param  reference   - 参考轴系
    /// @retval             - 新的历元坐标系
    HFrame makeEpochFrame(Axes* sourceAxes, EventTime* time, Axes* reference) const;

    /// @brief 创建新的历元平赤道轴系
    HAxes makeAxesMOE(const TimePoint& tp) const;

    /// @brief 创建新的历元平赤道轴系
    HAxes makeAxesMOE(EventTime* time) const;

    /// @brief 创建新的历元真赤道轴系
    HAxes makeAxesTOE(const TimePoint& tp) const;

    /// @brief 创建新的历元真赤道轴系
    HAxes makeAxesTOE(EventTime* time) const;

    /// @brief 创建新的历元平赤道坐标系
    HFrame makeFrameMOE(const TimePoint& tp) const;

    /// @brief 创建新的历元平赤道坐标系
    HFrame makeFrameMOE(EventTime* time) const;

    /// @brief 创建新的历元真赤道坐标系
    HFrame makeFrameTOE(const TimePoint& tp) const;

    /// @brief 创建新的历元真赤道坐标系
    HFrame makeFrameTOE(EventTime* time) const;

public:
    /// @brief 创建新的天体坐标系
    /// @param  axes        - 轴系
    HFrame makeFrame(Axes* axes) const;

    /// @brief 创建新的天体惯性坐标系
    HFrame makeFrameInertial() const;

    /// @brief 创建新的天体固连坐标系
    HFrame makeFrameFixed() const;

    /// @brief 创建新的天体MOD坐标系(天体平赤道系)
    HFrame makeFrameMOD() const;

    /// @brief 创建新的天体TOD坐标系(天体真赤道系)
    HFrame makeFrameTOD() const;

    /// @brief 创建新的天体J2000坐标系
    HFrame makeFrameJ2000() const;

    /// @brief 创建新的天体ICRF坐标系
    HFrame makeFrameICRF() const;
protected:

    Axes* getEpochAxesReference() const;
protected:
    /// @brief 获取Jn项
    double getJn(int n) const { return gravityField_.getJn(n); }

    /// @brief 加载重力模型
    errc_t loadGravityModel(StringView model);
protected:
    friend class SolarSystem;

    /// @brief 加载相关的天文参数
    errc_t loadAstroDefinition(BKVParser& parser);

    /// @brief 加载旋转数据
    errc_t loadSpinData(BKVParser& parser);

    /// @brief 加载星历相关参数
    errc_t loadEphemerisData(BKVParser& parser);

    /// @brief 加载地球相关参数
    errc_t loadEarth(BKVParser& parser);   
    
    /// @brief 加载月球相关参数
    errc_t loadMoon(BKVParser& parser);   

    /// @brief 加载MeanEarth定义
    errc_t loadMeanEarthDefinition(BKVParser& parser);

    A_DISABLE_COPY(CelestialBody)
protected:
    WeakPtr<SolarSystem>        solarSystem_;              ///< 太阳系指针
    SharedPtr<CelestialBody>    parent_;                   ///< 父天体
    std::string                 name_;                     ///< 天体名称
    double                      gm_{0.0};                  ///< 引力常数
    double                      systemGM_{0.0};            ///< 系统引力常数
    double                      radius_{0.0};              ///< 天体半径
    int                         jplSpiceId_{-1};           ///< JPL SPICE ID
    int                         jplIndex_{-1};             ///< JPL DE Index
    GravityField                gravityField_;             ///< 重力场
    SharedPtr<BodyShape>        shape_;                    ///< 天体形状
    SharedPtr<BodyOrientation>  orientation_;              ///< 天体姿态
    SharedPtr<BodyEphemeris>    ephemeris_;                ///< 天体星历

    SharedPtr<AxesBodyInertial> axesInertial_;             ///< 天体惯性轴
    SharedPtr<AxesBodyFixed>    axesFixed_;                ///< 天体固定轴
    SharedPtr<AxesBodyMOD>      axesMOD_;                  ///< 天体MOD轴
    SharedPtr<AxesBodyTOD>      axesTOD_;                  ///< 天体TOD轴

    /*!
    增加Frame类型成员变量会导致循环引用，导致内存泄漏
    */
};




/// @brief  SPICE 天体ID
/// @see spicelib/zzidmap.for
enum ESpiceId
{
    eSolarSystemBarycenter  = 0,
    eMercuryBarycenter      = 1,
    eVenusBarycenter        = 2,
    eEarthBarycenter        = 3,
    eMarsBarycenter         = 4,
    eJupiterBarycenter      = 5,
    eSaturnBarycenter       = 6,
    eUranusBarycenter       = 7,
    eNeptuneBarycenter      = 8,
    ePlutoBarycenter        = 9,
    eSun                    = 10,

    eMercury                = 199,

    eVenus                  = 299,

    eEarth                  = 399,
    eMoon                   = 301,

    eMars                   = 499,
    ePhobos                 = 401,
    eDeimos                 = 402,


    eJupiter                = 599,
    eIo                     = 501,
    eEuropa                 = 502,
    eGanymede               = 503,
    eCallisto               = 504,
    eAmalthea               = 505,
    eHimalia                = 506,
    eElara                  = 507,
    ePasiphae               = 508,
    eSinope                 = 509,
    eLysithea               = 510,
    eCarme                  = 511,
    eAnanke                 = 512,
    eLeda                   = 513,
    eThebe                  = 514,
    eAdrastea               = 515,
    eMetis                  = 516,

    eSaturn                 = 699,
    eMimas                  = 601,
    eEnceladus              = 602,
    eTethys                 = 603,
    eDione                  = 604,
    eRhea                   = 605,
    eTitan                  = 606,
    eHyperion               = 607,
    eIapetus                = 608,
    ePhoebe                 = 609,
    eJanus                  = 610,
    eEpimetheus             = 611,
    eHelene                 = 612,
    eTelesto                = 613,
    eCalypso                = 614,
    eAtlas                  = 615,
    ePrometheus             = 616,
    ePandora                = 617,
    ePan                    = 618,
    eMethone                = 632,
    ePallene                = 633,
    ePolydeuces             = 634,
    eDaphnis                = 635,
    eAnthe                  = 649,
    eAegaeon                = 653,


    eUranus                 = 799,
    eAriel                  = 701,
    eUmbriel                = 702,
    eTitania                = 703,
    eOberon                 = 704,
    eMiranda                = 705,
    eCordelia               = 706,
    eOphelia                = 707,
    eBianca                 = 708,
    eCressida               = 709,
    eDesdemona              = 710,
    eJuliet                 = 711,
    ePortia                 = 712,
    eRosalind               = 713,
    eBelinda                = 714,
    ePuck                   = 715,


    eNeptune                = 899,
    eTriton                 = 801,
    eNereid                 = 802,
    eNaiad                  = 803,
    eThalassa               = 804,
    eDespina                = 805,
    eGalatea                = 806,
    eLarissa                = 807,
    eProteus                = 808,

    ePluto                  = 999,
    eCharon                 = 901,
};



using HCelestialBody = SharedPtr<CelestialBody>;        ///< 天体句柄
using HBody = HCelestialBody;                           ///< 天体句柄

using PCelestialBody = CelestialBody*;                  ///< 天体指针
using PBody = PCelestialBody;                           ///< 天体指针


/*! @} */

AST_NAMESPACE_END
 


