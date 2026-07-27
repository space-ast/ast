///
/// @file      TLEDatabaseQuery.hpp
/// @brief     TLE 数据库查询条件
/// @details   定义对 TLE 数据库进行筛选查询的条件，支持按卫星名称、NORAD 编号、
///           历元时间范围、密级分类及轨道参数范围过滤。
/// @author    axel
/// @date      2026-07-24
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
#include "AstUtil/StringView.hpp"
#include "AstCore/TimePoint.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class TLE;

/// @brief TLE 数据库查询条件
/// @details 支持按卫星名称（子串匹配）、NORAD 编号（精确匹配）、
///          历元时间范围、密级分类及轨道参数范围进行过滤。
///          所有已设置的过滤条件为 AND 语义。
class AST_CORE_API TLEDatabaseQuery
{
public:
    /// @brief 默认构造（匹配所有条目）
    TLEDatabaseQuery();

    /// @brief 析构
    ~TLEDatabaseQuery();

    // -- 名称过滤（子串匹配）--

    /// @brief 按卫星名称过滤
    /// @param pattern 子串匹配模式，空字符串表示不按此字段过滤
    void setName(StringView pattern);

    /// @brief 获取卫星名称过滤模式
    const std::string& name() const { return name_; }

    // -- NORAD 编号过滤（精确匹配）--

    /// @brief 按 NORAD 目录编号过滤
    void setNoradId(int id);
    /// @brief 清除 NORAD 编号过滤条件
    void clearNoradId();
    /// @brief 是否设置了 NORAD 编号过滤
    bool hasNoradId() const { return hasNoradId_; }
    /// @brief 获取 NORAD 编号过滤值
    int noradId() const { return noradId_; }

    // -- 历元时间范围过滤 --

    /// @brief 设置最早历元（含）
    void setMinEpoch(const TimePoint& v);
    /// @brief 设置最晚历元（含）
    void setMaxEpoch(const TimePoint& v);
    /// @brief 清除最早历元过滤
    void clearMinEpoch();
    /// @brief 清除最晚历元过滤
    void clearMaxEpoch();
    /// @brief 是否启用最早历元过滤
    bool useMinEpoch() const { return useMinEpoch_; }
    /// @brief 是否启用最晚历元过滤
    bool useMaxEpoch() const { return useMaxEpoch_; }
    /// @brief 获取最早历元
    const TimePoint& minEpoch() const { return minEpoch_; }
    /// @brief 获取最晚历元
    const TimePoint& maxEpoch() const { return maxEpoch_; }

    // -- 密级过滤 --

    /// @brief 按密级分类过滤
    void setClassification(char c);
    /// @brief 清除密级过滤条件
    void clearClassification();
    /// @brief 是否设置了密级过滤
    bool hasClassification() const { return hasClassification_; }
    /// @brief 获取密级过滤值
    char classification() const { return classification_; }

    // -- 轨道参数范围过滤 --

    /// @brief 设置最小倾角 [rad]
    void setMinInclination(double v);
    /// @brief 设置最大倾角 [rad]
    void setMaxInclination(double v);
    /// @brief 清除最小倾角过滤
    void clearMinInclination();
    /// @brief 清除最大倾角过滤
    void clearMaxInclination();
    /// @brief 是否启用最小倾角过滤
    bool useMinInclination() const { return useMinInclination_; }
    /// @brief 是否启用最大倾角过滤
    bool useMaxInclination() const { return useMaxInclination_; }
    /// @brief 获取最小倾角 [弧度]
    double minInclination() const { return minInclination_; }
    /// @brief 获取最大倾角 [弧度]
    double maxInclination() const { return maxInclination_; }

    /// @brief 设置最小偏心率
    void setMinEccentricity(double v);
    /// @brief 设置最大偏心率
    void setMaxEccentricity(double v);
    /// @brief 清除最小偏心率过滤
    void clearMinEccentricity();
    /// @brief 清除最大偏心率过滤
    void clearMaxEccentricity();
    /// @brief 是否启用最小偏心率过滤
    bool useMinEccentricity() const { return useMinEccentricity_; }
    /// @brief 是否启用最大偏心率过滤
    bool useMaxEccentricity() const { return useMaxEccentricity_; }
    /// @brief 获取最小偏心率
    double minEccentricity() const { return minEccentricity_; }
    /// @brief 获取最大偏心率
    double maxEccentricity() const { return maxEccentricity_; }

    /// @brief 设置最小平均运动 [rad/s]
    void setMinMeanMotion(double v);
    /// @brief 设置最大平均运动 [rad/s]
    void setMaxMeanMotion(double v);
    /// @brief 清除最小平均运动过滤
    void clearMinMeanMotion();
    /// @brief 清除最大平均运动过滤
    void clearMaxMeanMotion();
    /// @brief 是否启用最小平均运动过滤
    bool useMinMeanMotion() const { return useMinMeanMotion_; }
    /// @brief 是否启用最大平均运动过滤
    bool useMaxMeanMotion() const { return useMaxMeanMotion_; }
    /// @brief 获取最小平均运动 [rad/s]
    double minMeanMotion() const { return minMeanMotion_; }
    /// @brief 获取最大平均运动 [rad/s]
    double maxMeanMotion() const { return maxMeanMotion_; }

    /// @brief 设置最小 B* 阻力系数
    void setMinBstar(double v);
    /// @brief 设置最大 B* 阻力系数
    void setMaxBstar(double v);
    /// @brief 清除最小 B* 过滤
    void clearMinBstar();
    /// @brief 清除最大 B* 过滤
    void clearMaxBstar();
    /// @brief 是否启用最小 B* 过滤
    bool useMinBstar() const { return useMinBstar_; }
    /// @brief 是否启用最大 B* 过滤
    bool useMaxBstar() const { return useMaxBstar_; }
    /// @brief 获取最小 B*
    double minBstar() const { return minBstar_; }
    /// @brief 获取最大 B*
    double maxBstar() const { return maxBstar_; }

    // -- 匹配 --

    /// @brief 判断给定 TLE 是否匹配当前查询条件
    /// @param tle 待检查的 TLE 条目
    /// @return 若匹配则返回 true
    bool matches(const TLE& tle) const;

    /// @brief 查询是否未设置任何过滤条件（匹配所有条目）
    bool isEmpty() const;

private:
    std::string name_{};              ///< 卫星名称过滤模式（空 = 不筛选）
    int noradId_{0};                  ///< NORAD 编号过滤值
    bool hasNoradId_{false};          ///< 是否启用 NORAD 编号过滤
    TimePoint minEpoch_{};            ///< 最早历元
    TimePoint maxEpoch_{};            ///< 最晚历元
    bool useMinEpoch_{false};         ///< 是否启用最早历元过滤
    bool useMaxEpoch_{false};         ///< 是否启用最晚历元过滤
    char classification_{'U'};        ///< 密级过滤值
    bool hasClassification_{false};   ///< 是否启用密级过滤
    double minInclination_{0.0};      ///< 最小倾角 [弧度]
    double maxInclination_{0.0};      ///< 最大倾角 [弧度]
    bool useMinInclination_{false};   ///< 是否启用最小倾角过滤
    bool useMaxInclination_{false};   ///< 是否启用最大倾角过滤
    double minEccentricity_{0.0};     ///< 最小偏心率
    double maxEccentricity_{0.0};     ///< 最大偏心率
    bool useMinEccentricity_{false};  ///< 是否启用最小偏心率过滤
    bool useMaxEccentricity_{false};  ///< 是否启用最大偏心率过滤
    double minMeanMotion_{0.0};       ///< 最小平均运动 [rad/s]
    double maxMeanMotion_{0.0};       ///< 最大平均运动 [rad/s]
    bool useMinMeanMotion_{false};    ///< 是否启用最小平均运动过滤
    bool useMaxMeanMotion_{false};    ///< 是否启用最大平均运动过滤
    double minBstar_{0.0};            ///< 最小 B*
    double maxBstar_{0.0};            ///< 最大 B*
    bool useMinBstar_{false};         ///< 是否启用最小 B* 过滤
    bool useMaxBstar_{false};         ///< 是否启用最大 B* 过滤
};

/*! @} */

AST_NAMESPACE_END
