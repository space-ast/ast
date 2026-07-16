///
/// @file      MotionSGP4.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-14
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
#include "AstSim/MotionProfile.hpp"
#include "AstSim/MotionWithIntervalStep.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class TLE
{
public:
    bool empty() const{return line1_.empty();}
public:
    std::string name_{};     ///< 卫星名称
    std::string line1_{};    ///< 第一行
    std::string line2_{};    ///< 第二行
};

enum class ETLESource
{
    eFile,      ///< 来自文件
    eEdited,    ///< 来自编辑
    eServer,    ///< 来自服务器
};

enum class ESwitchMethod
{
    eEpoch,
};

class TwoLineElement
{
public:
    TwoLineElement() = default;
    ~TwoLineElement() = default;

public:
    bool enabled_{true};         ///< 是否启用
    ETLESource source_           ///< 来源类型
        {ETLESource::eFile};                 
    ESwitchMethod switch_method_ ///< 切换方法
        {ESwitchMethod::eEpoch};  
    TimePoint switchEpoch_{};     ///< 切换历元
    TLE tle_{};                   ///< TLE 数据
    double epochTime_{0.0};           ///< 历元时间
    double meanMotionDotTime_{0.0};   ///< 平均运动导数
    double motionDotDot_{0.0};        ///< 平均运动二阶导数
    int iexp_{0};                   ///< 平均运动二阶导数指数
    double bstar_{0.0};               ///< B* 阻力系数
    int ibexp_{0};                  ///< B* 阻力系数指数
    double inclination_{0.0};         ///< 倾角
    double rightAscenOfNode_{0.0};    ///< 升交点赤经
    double eccentricity_{0.0};        ///< 偏心率
    double argOfPerigee_{0.0};        ///< 近地点幅角
    double meanAnomaly_{0.0};         ///< 平近点角
    double meanMotion_{0.0};          ///< 平均运动
    int revNumber_{0};              ///< 轨道数
    int ephType_{0};                ///< 星历类型
    int elementNumber_{0};          ///< 元素编号
    char classification_{'U'};        ///< 分类
};

/// @brief SGP4 运动模型
/// @details   
class AST_SIM_API MotionSGP4 : public MotionWithIntervalStep
{
public:
    MotionSGP4() = default;
    ~MotionSGP4() = default;
public:
    errc_t makeEphemerisSpec(ScopedPtr<Ephemeris>& eph) const override;
    errc_t makeEphemerisSimple(ScopedPtr<Ephemeris>& eph) const override;
    void accept(MotionProfileVisitor& visitor) override;
public:
    std::string SSCNumber_{};                ///< SSC 编号字符串
    std::string intlDesignator_{};           ///<
    std::string commonName_{};               ///< 通用名称
    std::vector<TwoLineElement> elements_{}; ///< 两行元素列表
};

/*! @} */

AST_NAMESPACE_END