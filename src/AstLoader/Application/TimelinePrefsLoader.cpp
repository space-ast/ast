///
/// @file      TimelinePrefsLoader.cpp
/// @brief     时间线视图偏好加载器实现
/// @details   使用 XMLNode（DOM）解析 STK 时间线偏好文件，
///            将 <Views Type="Content"> 下的 <Row> 及其 <Interval> 子元素填充到 TimelinePrefs::Row
///            将 <Views Type="Time"> 下的 <TimeView> 的全局起止时间填充到 TimelinePrefs::globalInterval_
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

#include "TimelinePrefsLoader.hpp"
#include "AstUtil/XMLNode.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringUtil.hpp"

AST_NAMESPACE_BEGIN

namespace{

/// @brief 读取元素的字符串属性，属性不存在时返回空串
std::string _aGetAttrString(const XMLNode& node, const char* name)
{
    const auto& attrs = node.getAttributes();
    auto it = attrs.find(name);
    if(it == attrs.end())
        return std::string();
    return it->second.value();
}

/// @brief 读取元素的布尔属性，属性不存在时返回默认值
bool _aGetAttrBool(const XMLNode& node, const char* name, bool defaultValue = false)
{
    const auto& attrs = node.getAttributes();
    auto it = attrs.find(name);
    if(it == attrs.end())
        return defaultValue;
    return aParseBool(it->second.value());
}

/// @brief 该节点是否为指定的元素
bool _aIsElement(const XMLNode& node, const char* name)
{
    return node.getKind() == EXMLNodeType::eElement && node.getName() == name;
}

} // namespace

/// @brief 加载单个 <Row> 元素
static errc_t _aLoadTimelineRow(const XMLNode& rowNode, TimelinePrefs::Row& row)
{
    row.className_     = _aGetAttrString(rowNode, "ClassName");
    row.componentName_ = _aGetAttrString(rowNode, "ComponentName");
    row.displayName_   = _aGetAttrString(rowNode, "DisplayName");
    row.path_          = _aGetAttrString(rowNode, "Path");
    row.type_          = _aGetAttrString(rowNode, "Type");
    row.isDefaultName_ = _aGetAttrBool(rowNode, "IsDefaultName");
    row.isExpanded_    = _aGetAttrBool(rowNode, "IsExpanded");
    row.isGroup_       = _aGetAttrBool(rowNode, "IsGroup");
    row.isHidden_      = _aGetAttrBool(rowNode, "IsHidden");

    for(const auto& child : rowNode.getChildren())
    {
        if(!_aIsElement(*child, "Interval"))
            continue;
        std::string startStr = _aGetAttrString(*child, "StartTime");
        std::string stopStr  = _aGetAttrString(*child, "StopTime");
        TimeInterval interval;
        errc_t rc = aTimeIntervalParse(startStr, stopStr, interval);
        if(rc != eNoError){
            aWarning(_("解析区间时间失败: StartTime='%s', StopTime='%s'，已跳过"), startStr.c_str(), stopStr.c_str());
            continue;
        }
        row.intervals_.push_back(interval);
    }
    return eNoError;
}

/// @brief 递归收集内容视图（<Views Type="Content">）下的所有 <Row>
/// @details 内容视图可能经 <ContentView> 等容器元素嵌套，这里递归下探，
///          凡是名为 <Row> 的元素都作为一行处理。
static void _aLoadTimelineRows(const XMLNode& viewNode, std::vector<TimelinePrefs::Row>& rows)
{
    for(const auto& child : viewNode.getChildren())
    {
        if(child->getKind() != EXMLNodeType::eElement)
            continue;
        if(child->getName() == "Row"){
            TimelinePrefs::Row row;
            _aLoadTimelineRow(*child, row);
            rows.push_back(std::move(row));
        }else{
            _aLoadTimelineRows(*child, rows);
        }
    }
}

/// @brief 加载时间视图（<Views Type="Time">）下的全局分析时段
static void _aLoadTimelineGlobalInterval(const XMLNode& viewNode, TimeInterval& globalInterval)
{
    for(const auto& child : viewNode.getChildren())
    {
        if(!_aIsElement(*child, "TimeView"))
            continue;
        std::string startStr = _aGetAttrString(*child, "GlobalStartTime");
        std::string stopStr  = _aGetAttrString(*child, "GlobalStopTime");
        if(startStr.empty() || stopStr.empty())
            continue;
        TimeInterval interval;
        errc_t rc = aTimeIntervalParse(startStr, stopStr, interval);
        if(rc != eNoError){
            aWarning(_("解析全局分析时段失败: StartTime='%s', StopTime='%s'"), startStr.c_str(), stopStr.c_str());
            continue;
        }
        globalInterval = interval;
    }
}

errc_t aLoadTimeLinePrefs(XMLNode& root, TimelinePrefs& prefs)
{
    if(root.getName() != "TimelineLine_Prefs"){
        aError(_("无效的时间线偏好文件：根元素应为 'TimelineLine_Prefs'，实际为 '%s'"), root.getName().c_str());
        return eErrorInvalidParam;
    }

    prefs = TimelinePrefs();

    for(const auto& child : root.getChildren())
    {
        if(!_aIsElement(*child, "Views"))
            continue;
        std::string type = _aGetAttrString(*child, "Type");
        if(aEqualsIgnoreCase(type, "Content")){
            _aLoadTimelineRows(*child, prefs.rows_);
        }else if(aEqualsIgnoreCase(type, "Time")){
            _aLoadTimelineGlobalInterval(*child, prefs.globalInterval_);
        }
    }
    return eNoError;
}

errc_t aLoadTimeLinePrefs(StringView filepath, TimelinePrefs& prefs)
{
    XMLNode root;
    errc_t rc = root.load(filepath);
    if(rc != eNoError){
        aError(_("打开时间线偏好文件 '%.*s' 失败"), static_cast<int>(filepath.size()), filepath.data());
        return eErrorInvalidFile;
    }
    return aLoadTimeLinePrefs(root, prefs);
}

AST_NAMESPACE_END
