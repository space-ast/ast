///
/// @file      ReportWriter.cpp
/// @brief     数据报表生成实现
/// @details   根据 ReportStyle 模板从 Object 中提取数据并写出。
/// @author    axel
/// @date      2026-07-02
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

#include "ReportWriter.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/VariantVector.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"
#include "AstCore/TimeList.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/Resolve.hpp"

// DataGroup provider
#include "AstReport/DataGroupCartPos.hpp"
#include "AstReport/DataGroupCartVel.hpp"
#include "AstReport/DataGroupModOrbElem.hpp"
#include "AstReport/DataGroupLLAState.hpp"
#include "AstReport/DataGroupLLRState.hpp"
#include "AstReport/DataGroupSpherical.hpp"
#include "AstReport/DataGroupEquinElem.hpp"
#include "AstReport/DataGroupBetaAngle.hpp"
#include "AstReport/DataGroupQuats.hpp"
#include "AstReport/DataGroupEuler.hpp"
#include "AstReport/DataGroupMagField.hpp"
#include "AstReport/DataGroupInterval.hpp"
#include "AstReport/DataGroupEclipse.hpp"
#include "AstReport/DataGroupEclipseSummary.hpp"
#include "AstReport/DataGroupLightingTimes.hpp"
#include "AstReport/DataGroupSolarIntensity.hpp"

#include <unordered_map>
#include <memory>

AST_NAMESPACE_BEGIN

// ---- 内部辅助 ----

/// @brief 将 element.type_ 字符串转换为 Frame
/// @todo 暂硬编码地球，后续支持其他中心天体
static Frame* _aFrameFromType(StringView type)
{
    auto* earth = aGetEarth();

    return aObject_GetFrame(earth, type);
}

/// @brief 生成等步长时间列表
/// @todo 步长应可配置（从 options 或 .rst 中读取），当前固定 60s
static TimeList _aGenerateTimeList(const ReportGenerateOptions& options)
{
    /// @todo 步长应可配置（从 options 或 .rst 中读取），当前固定 60s
    const double kFixedStepSec = 60.0;
    TimeList timeList;
    options.interval_.discretize(kFixedStepSec, timeList);
    return timeList;
}

// ---- Service → DataGroup 工厂 ----

using ADataGroupFactory = DataGroup* (*)(const Object*, StringView type);

static const std::unordered_map<StringView, ADataGroupFactory> s_serviceFactories = {
    // ---- Point-based ----
    {"CartPos", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        Frame* frame = _aFrameFromType(type);
        if (!frame) return nullptr;
        auto* dg = new DataGroupCartPos();
        dg->setPoint(point);
        dg->setFrame(frame);
        return dg;
    }},
    {"CartVel", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        Frame* frame = _aFrameFromType(type);
        if (!frame) return nullptr;
        auto* dg = new DataGroupCartVel();
        dg->setPoint(point);
        dg->setFrame(frame);
        return dg;
    }},
    {"ModOrbElem", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        Frame* frame = _aFrameFromType(type);
        if (!frame) return nullptr;
        auto* dg = new DataGroupModOrbElem();
        dg->setPoint(point);
        dg->setFrame(frame);
        return dg;
    }},
    {"LLAState", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        Frame* frame = _aFrameFromType(type.empty() ? "Fixed" : type);
        if (!frame) return nullptr;
        auto* dg = new DataGroupLLAState();
        dg->setPoint(point);
        dg->setBody(frame->getBody());
        return dg;
    }},
    {"SpEnvMagFieldDP", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        auto* dg = new DataGroupMagField();
        dg->setPoint(point);
        return dg;
    }},
    {"LLRState", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        Frame* frame = _aFrameFromType(type);
        if (!frame) return nullptr;
        auto* dg = new DataGroupLLRState();
        dg->setPoint(point);
        dg->setFrame(frame);
        return dg;
    }},
    {"Spherical", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        Frame* frame = _aFrameFromType(type);
        if (!frame) return nullptr;
        auto* dg = new DataGroupSpherical();
        dg->setPoint(point);
        dg->setFrame(frame);
        return dg;
    }},
    {"EquinElem", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        Frame* frame = _aFrameFromType(type);
        if (!frame) return nullptr;
        auto* dg = new DataGroupEquinElem();
        dg->setPoint(point);
        dg->setFrame(frame);
        return dg;
    }},
    {"BetaAngle", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        Frame* frame = _aFrameFromType(type);
        if (!frame) return nullptr;
        auto* dg = new DataGroupBetaAngle();
        dg->setPoint(point);
        dg->setFrame(frame);
        dg->setSunPoint(aGetSun());
        dg->setMoonPoint(aGetMoon());
        return dg;
    }},
    {"SolarIntensity", [](const Object* obj, StringView) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        auto* dg = new DataGroupSolarIntensity();
        dg->setPoint(point);
        return dg;
    }},

    // ---- Axes-based ----
    {"Quats", [](const Object* obj, StringView) -> DataGroup* {
        auto* axes = aobject_cast<Axes*>(const_cast<Object*>(obj));
        if (!axes) return nullptr;
        auto* dg = new DataGroupQuats();
        dg->setAxes(axes);
        /// @todo 参考轴系应从 Object 属性树或 type_ 获取
        dg->setReferenceAxes(aGetEarth() ? aGetEarth()->getAxes("Inertial") : nullptr);
        return dg;
    }},
    {"Euler", [](const Object* obj, StringView) -> DataGroup* {
        auto* axes = aobject_cast<Axes*>(const_cast<Object*>(obj));
        if (!axes) return nullptr;
        auto* dg = new DataGroupEuler();
        dg->setAxes(axes);
        /// @todo 参考轴系应从 Object 属性树或 type_ 获取
        dg->setReferenceAxes(aGetEarth() ? aGetEarth()->getAxes("Inertial") : nullptr);
        // @todo 转序应从 type_ 或 .rst Element 属性获取，当前默认 121 (XYX)
        return dg;
    }},

    // ---- 区间型（事件/区间数据，如日食、访问时段） ----
    {"EclipseData", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        auto* dg = new DataGroupEclipse();
        dg->setPoint(point);
        return dg;
    }},
    {"EclipseSummary", [](const Object* obj, StringView) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        auto* dg = new DataGroupEclipseSummary();
        dg->setPoint(point);
        return dg;
    }},
    {"LightingData", [](const Object* obj, StringView type) -> DataGroup* {
        auto* point = aobject_cast<Point*>(const_cast<Object*>(obj));
        if (!point) return nullptr;
        auto* dg = new DataGroupLightingTimes();
        dg->setPoint(point);
        dg->setLightingType(DataGroupLightingTimes::LightingTypeFromString(type));
        return dg;
    }},
};


/// @brief 列数据 — extract 后的带类型列及其缓存 Span
struct ColData
{
    const ReportElement* element{};
    std::string title{};
    int  width{};

    EDataType dataType{};
    VariantVector data{};  // 持有列数据，Span 指向此处

    // 缓存 Span（每种类型一个有效，指向 data 内部缓冲区）
    Span<double>      dSpan{};
    Span<int>         iSpan{};
    Span<TimePoint>   tSpan{};
    Span<std::string> sSpan{};
};

/// @brief 从 ColData::data 推断类型并缓存 Span
static void _aCacheSpan(ColData& col)
{
    if (!col.data.hasType())
    {
        col.dataType = EDataType::eFloat;
        return;
    }

    const auto& ti = col.data.elementType();

    if (ti == typeid(TimePoint))
    {
        col.dataType = EDataType::eDateTime;
        col.tSpan    = col.data.asSpan<TimePoint>();
    }
    else if (ti == typeid(double))
    {
        col.dataType = EDataType::eFloat;
        col.dSpan    = col.data.asSpan<double>();
    }
    else if (ti == typeid(int))
    {
        col.dataType = EDataType::eInt;
        col.iSpan    = col.data.asSpan<int>();
    }
    else if (ti == typeid(std::string))
    {
        col.dataType = EDataType::eString;
        col.sSpan    = col.data.asSpan<std::string>();
    }
    else
    {
        aWarning("unknown element type '%s' in column '%s', skipping output",
                 ti.name(), col.title.c_str());
        col.dataType = EDataType::eFloat;
        col.data.clear();  // 清空数据，确保 hasType() 返回 false，输出时跳过
    }
}

/// @brief 写出表格报表
static errc_t _aWriteTabular(const ReportStyle& report, const Object* object, FILE* file, const ReportGenerateOptions& options)
{
    // ---- 1. 展平所有 Element → 列描述（保持顺序） ----
    std::vector<ColData> columns;
    columns.reserve(16);

    for (const auto& sec : report.sections_)
    {
        for (const auto& line : sec.lines_)
        {
            for (const auto& elem : line.elements_)
            {
                ColData col;
                col.element  = &elem;
                col.title    = elem.title_.empty() ? elem.name_ : elem.title_;
                col.width    = std::max(static_cast<int>(col.title.size()), 12);
                col.dataType = EDataType::eFloat;
                columns.push_back(col);
            }
        }
    }

    if (columns.empty())
        return eNoError;

    // ---- 2. 按 (service_, type_) 分组列索引 ----
    std::unordered_map<std::string, std::vector<size_t>> groups;

    for (size_t i = 0; i < columns.size(); ++i)
    {
        std::string key = columns[i].element->service_ + "|" + columns[i].element->type_;
        groups[key].push_back(i);
    }

    // ---- 3. 为每组创建 DataGroup → calculate → extract ----

    // 时间网格只生成一次，供所有时间变量型 DataGroup 复用
    TimeList timeList = _aGenerateTimeList(options);

    for (auto& group : groups)
    {
        auto& indices = group.second;
        if (indices.empty()) continue;

        const std::string& service = columns[indices[0]].element->service_;
        const std::string& type    = columns[indices[0]].element->type_;

        // 查找工厂并创建 DataGroup（统一返回基类，具体类型用 dynamic_cast 判定）
        auto it = s_serviceFactories.find(service);
        if (it == s_serviceFactories.end())
        {
            aWarning("unsupported report service: %s", service.c_str());
            continue;
        }
        std::unique_ptr<DataGroup> dg(it->second(object, type));
        if (!dg)
        {
            aError("failed to create DataGroup for service: %s", service.c_str());
            continue;
        }

        // 根据具体类型分派 calculate：时间变量型走 timeList，区间型走 interval
        VariantVector calcResult;
        errc_t err = eNoError;
        if (auto* dgTimeVar = dynamic_cast<DataGroupTimeVar*>(dg.get()))
        {
            err = dgTimeVar->calculate(timeList, calcResult);
        }
        else if (auto* dgInterval = dynamic_cast<DataGroupInterval*>(dg.get()))
        {
            if (options.interval_.isEmpty())
            {
                aError("interval is empty (start>stop) for service '%s'; set ReportGenerateOptions::interval_",
                       service.c_str());
                continue;
            }
            err = dgInterval->calculate(options.interval_, calcResult);
        }
        else
        {
            aError("unsupported DataGroup type for service: %s", service.c_str());
            continue;
        }

        if (err != eNoError)
        {
            aError("calculate failed for service %s: %d", service.c_str(), static_cast<int>(err));
            continue;
        }

        // 对该组内每个 element 提取列
        for (size_t idx : indices)
        {
            err = dg->extract(calcResult, columns[idx].element->element_, columns[idx].data);
            if (err != eNoError)
            {
                aError("extract '%s' failed for service %s: %d",
                       columns[idx].element->element_.c_str(), service.c_str(), static_cast<int>(err));
                continue;
            }

            _aCacheSpan(columns[idx]);
        }
    }

    // ---- 4. 写出表格 ----

    // 标题行
    std::fprintf(file, "%s\n", report.title_.c_str());
    for (size_t i = 0; i < report.title_.size(); ++i)
        std::fputc('=', file);
    std::fprintf(file, "\n\n");

    // 表头行
    for (const auto& col : columns)
        std::fprintf(file, "%*s  ", col.width, col.title.c_str());
    std::fprintf(file, "\n");

    // 分隔线
    for (const auto& col : columns)
    {
        for (int i = 0; i < col.width; ++i)
            std::fputc('-', file);
        std::fprintf(file, "  ");
    }
    std::fprintf(file, "\n");

    // 校验所有列行数一致，取最小值防越界（行数由实际列数据决定，兼容区间型列）
    int nRows = 0;
    bool nRowsInitialized = false;
    for (const auto& col : columns)
    {
        if (!col.data.hasType()) continue;
        int colSize = static_cast<int>(col.data.size());
        if (!nRowsInitialized)
        {
            nRows = colSize;
            nRowsInitialized = true;
        }
        else if (colSize < nRows)
        {
            aWarning("column '%s' has fewer rows (%d) than others (%d)",
                     col.title.c_str(), colSize, nRows);
            nRows = colSize;
        }
    }

    // 数据行
    for (int row = 0; row < nRows; ++row)
    {
        for (const auto& col : columns)
        {
            // 跳过无数据的列（extract 失败或未计算），输出占位符
            if (!col.data.hasType())
            {
                std::fprintf(file, "%-*s  ", col.width, "--");
                continue;
            }
            switch (col.dataType)
            {
            case EDataType::eFloat:
            {
                double v = col.dSpan[row];
                if (col.element && !col.element->format_.empty())
                {
                    std::string fmt = "%" + col.element->format_;
                    std::fprintf(file, fmt.c_str(), v);
                }
                else
                {
                    std::fprintf(file, "%*.*f", col.width, 6, v);
                }
                std::fprintf(file, "  ");
                break;
            }
            case EDataType::eDateTime:
            {
                std::string str;
                aTimePointFormat(col.tSpan[row], str);
                std::fprintf(file, "%-*s  ", col.width, str.c_str());
                break;
            }
            case EDataType::eInt:
            {
                int v = col.iSpan[row];
                std::fprintf(file, "%*d  ", col.width, v);
                break;
            }
            case EDataType::eString:
            {
                std::fprintf(file, "%-*s  ", col.width, col.sSpan[row].c_str());
                break;
            }
            }
        }
        std::fprintf(file, "\n");
    }

    return eNoError;
}


// ---- 公开接口 ----

errc_t aWriteReport(const ReportStyle& report, const Object* object, FILE* file, const ReportGenerateOptions& options)
{
    if (!file || !object)
        return eErrorInvalidFile;

    switch (report.styleType_)
    {
        case EStyleType::eTabular:
            return _aWriteTabular(report, object, file, options);

        case EStyleType::eGraph2D:
        case EStyleType::eGraph3D:
            // @todo 图形报表输出
            return eNoError;

        case EStyleType::eFixed:
            // @todo 固定格式报表输出
            return eNoError;

        default:
            return eNoError;
    }
}

AST_NAMESPACE_END
