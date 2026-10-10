///
/// @file      TimelinePrefsLoader.hpp
/// @brief     时间线视图偏好加载器
/// @details   加载 STK 时间线（Timeline）视图偏好文件（*TimelinePrefs.xml）
///            将内容视图中的各行（每行一组区间）与时间视图的全局分析时段解析为 TimelinePrefs 结构
/// @author    axel
/// @date      2026-10-08
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
#include "AstCore/TimeInterval.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

// 前置声明
class XMLNode;

/*!
    @addtogroup AstLoader
    @{
*/

/// @brief 时间线偏好数据
/// @details 对应 STK 时间线偏好文件（根元素 TimelineLine_Prefs）
class AST_LOADER_API TimelinePrefs
{
public:
    /// @brief 时间线内容视图中的一行
    /// @details 描述某个对象（或对象组合）在时间线上展示的一组时间区间，
    ///          ClassName/Path 等元数据用于标识这组区间的归属（例如 Sensor1 对 Facility1 的访问区间）
    struct Row
    {
        std::string className_{};                 ///< ClassName 属性（如 "Scenario"、"Access"）
        std::string componentName_{};             ///< ComponentName 属性（如 "AvailabilityIntervals"、"AccessIntervals"）
        std::string displayName_{};               ///< DisplayName 属性
        std::string path_{};                      ///< Path 属性（对象路径，区间归属的唯一标识）
        std::string type_{};                      ///< Type 属性（如 "IntervalList"）
        bool isDefaultName_{false};               ///< IsDefaultName 属性
        bool isExpanded_{false};                  ///< IsExpanded 属性
        bool isGroup_{false};                     ///< IsGroup 属性
        bool isHidden_{false};                    ///< IsHidden 属性
        std::vector<TimeInterval> intervals_{};   ///< 时间区间列表（<Interval> 元素）
    };
    
    /// @brief 根据 DisplayName 属性查找行
    const Row* findRowByDisplayName(StringView displayName);
public:
    std::vector<Row> rows_{};            ///< 内容视图中的各行（每行一组区间）
    TimeInterval globalInterval_{};      ///< 时间视图的全局分析时段（GlobalStart/Stop）
};

/// @brief 从 XML 文件加载时间线视图偏好
/// @param filepath 文件路径（*TimelinePrefs.xml）
/// @param[out] prefs     时间线偏好数据
/// @return 错误码
AST_LOADER_API errc_t aLoadTimeLinePrefs(StringView filepath, TimelinePrefs& prefs);

/// @brief 从已解析的 XML 根节点加载时间线视图偏好
/// @param root  已解析的 XML 根节点（应为 TimelineLine_Prefs 元素）
/// @param[out] prefs     时间线偏好数据
/// @return 错误码
AST_LOADER_API errc_t aLoadTimeLinePrefs(XMLNode& root, TimelinePrefs& prefs);

/*! @} */

AST_NAMESPACE_END
