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
#include "AstReport/DataSeries.hpp"
#include "AstReport/DataFrame.hpp"

AST_NAMESPACE_BEGIN

// ---- 内部辅助 ----

/// @brief 从对象中获取指定 Element 的数据值（字符串表示）
/// @todo 实现通过 service/type/element 从 Object 查询数据的机制
static std::string _aGetElementValue(const Object* object, const ReportElement& element)
{
    // @todo 根据 element.service_ / element.type_ / element.element_ 从 object 中查询数据
    (void)object;
    (void)element;
    return std::string();
}

/// @brief 写出表格报表
static errc_t _aWriteTabular(const ReportStyle& report, const Object* object, FILE* file, const ReportGenerateOptions& options)
{
    // 收集所有 Element（展平 Section → Line → Element 到表头行）
    struct ColumnInfo
    {
        const ReportElement* element;
        std::string         title;
        int                 width;
    };

    std::vector<ColumnInfo> columns;
    DataFrame df;
    df.reserve(20);

    int rows = 0; // @todo 根据 options.interval_ 计算数据行数

    for (const auto& sec : report.sections_)
    {
        for (const auto& line : sec.lines_)
        {
            for (const auto& elem : line.elements_)
            {
                ColumnInfo col;
                col.element = &elem;
                // 使用 Element 的 title，如为空则用 name
                col.title = elem.title_.empty() ? elem.name_ : elem.title_;
                col.width = std::max(static_cast<int>(col.title.size()), 12);
                columns.push_back(col);

                std::string service = elem.service_;
                std::string type = elem.type_;
                std::string element = elem.element_;
                EDataType dataType = elem.dataType_;

                df.addColumn(elem.title_, dataType, rows);

                (void)service;
                (void)type;
                (void)element;
                (void)dataType;
            }
        }
    }

    if (columns.empty())
        return eNoError;

    // ---- 写标题 ----
    std::fprintf(file, "%s\n", report.title_.c_str());
    for (int i = 0; i < static_cast<int>(report.title_.size()); ++i)
        std::fputc('=', file);
    std::fprintf(file, "\n\n");

    // ---- 写表头 ----
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

    // ---- 写数据行 ----
    // @todo 当前仅输出一行示例数据，后续应支持多时间步
    for (const auto& col : columns)
    {
        std::string value = _aGetElementValue(object, *col.element);
        if (value.empty())
        {
            // 占位输出 --
            std::fprintf(file, "%*s  ", col.width, "--");
        }
        else
        {
            std::fprintf(file, "%-*s  ", col.width, value.c_str());
        }
    }
    std::fprintf(file, "\n");

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
