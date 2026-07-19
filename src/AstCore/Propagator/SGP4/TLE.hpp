///
/// @file      TLE.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-18
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
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


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


class TLELines;


/// @brief 两行根数（TLE）
/// @details 存储从 TLE 文本解析出的轨道根数及元数据。
class TLE
{
public:
    TLE() = default;
    ~TLE() = default;

    /// @brief 从 TLE 行文本创建 TLE 对象
    /// @details 解析 TLE 行文本，提取轨道根数及元数据。
    /// @param lines TLE 行文本
    /// @return TLE 对象
    static TLE FromLines(const TLELines& lines);
    static TLE FromLines(StringView l1, StringView l2);
    
    double meanMotionDot() const { return meanMotionDotTime_; }
    double motionDotDot() const { return motionDotDot_; }

public:
    bool enabled_{true};              ///< 是否启用
    ETLESource source_                ///< 来源类型
        {ETLESource::eFile};
    ESwitchMethod switch_method_      ///< 切换方法
        {ESwitchMethod::eEpoch};
    TimePoint switchEpoch_{};         ///< 切换历元
    TLELines lines_{};                ///< TLE 原始行文本
    TimePoint epochTime_{};           ///< 历元时间
    double meanMotionDotTime_{0.0};   ///< 平均运动导数
    double motionDotDot_{0.0};        ///< 平均运动二阶导数
    double bstar_{0.0};               ///< B* 阻力系数
    double inclination_{0.0};         ///< 倾角
    double rightAscenOfNode_{0.0};    ///< 升交点赤经
    double eccentricity_{0.0};        ///< 偏心率
    double argOfPerigee_{0.0};        ///< 近地点幅角
    double meanAnomaly_{0.0};         ///< 平近点角
    double meanMotion_{0.0};          ///< 平均运动
    int revNumber_{0};                ///< 轨道数
    int ephType_{0};                  ///< 星历类型
    int elementNumber_{0};            ///< 元素编号
    char classification_{'U'};        ///< 分类
};


/*! @} */

AST_NAMESPACE_END
