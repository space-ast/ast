///
/// @file      close_approach_analysis.cpp
/// @brief     接近分析示例：在 TLE 目录中搜索与参考目标的接近事件
/// @details   加载 TLE 数据库，以 NORAD 25544（国际空间站）为参考目标，
///            在给定时间窗口内搜索进入 5 km 距离阈值的其它空间目标。
/// @author    axel
/// @date      2026-08-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "ast/TLEDatabase.hpp"
#include "ast/TLE.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/CloseApproachAnalyzer.hpp"
#include "ast/CloseApproachOptions.hpp"
#include "ast/CloseApproachResult.hpp"
#include "ast/Environment.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

AST_USING_NAMESPACE

/// @brief TLE 数据库路径
static const char* kDatabasePath = "./data/Dev/CAT/TLE20230212.txt";

/// @brief 参考目标 NORAD 编号（国际空间站）
static const int kReferenceNoradId = 25544;

/// @brief 距离阈值 [m]
static const double kThreshold = 5000.0;   // 5 km

/// @brief 分析窗口：13 Feb 2023 04:00:00.000 UTCG - 18 Feb 2023 04:00:00.000 UTCG
static TimePoint AnalysisStart() { return TimePoint::FromUTC(2023, 2, 13, 4, 0, 0.0); }
static TimePoint AnalysisStop()  { return TimePoint::FromUTC(2023, 2, 18, 4, 0, 0.0); }

int main()
{
    if(aIsCI())
        return 0;
    setlocale(LC_ALL, ".UTF-8");
    std::cout << "========================================\n";
    std::cout << "  接近分析（Close Approach）示例\n";
    std::cout << "========================================\n\n";

    // ---- 1. 加载 TLE 数据库 ----
    TLEDatabase database;
    errc_t rc = database.load(kDatabasePath);
    if (rc != eNoError)
    {
        std::cerr << "错误：无法加载数据库 " << kDatabasePath << " (rc=" << rc << ")\n";
        return 1;
    }
    std::cout << "数据库: " << kDatabasePath << "\n";
    std::cout << "对象总数: " << database.size() << "\n\n";

    // ---- 2. 查找参考目标（NORAD 25544）----
    const TLE* reference = nullptr;
    for (const TLE& tle : database.getEntries())
    {
        if (tle.noradId() == kReferenceNoradId) { reference = &tle; break; }
    }
    if (!reference)
    {
        std::cerr << "错误：数据库中未找到 NORAD " << kReferenceNoradId << "\n";
        return 1;
    }
    std::cout << "参考目标: NORAD " << reference->noradId() << "\n";

    // ---- 3. 构造候选目录（排除参考目标自身）----
    std::vector<TLE> candidates;
    candidates.reserve(database.size());
    for (const TLE& tle : database.getEntries())
    {
        if (tle.noradId() != kReferenceNoradId) { candidates.push_back(tle); }
    }

    // ---- 4. 配置并执行分析 ----
    CloseApproachOptions opts;
    opts.window     = TimeInterval(AnalysisStart(), AnalysisStop());
    opts.threshold  = kThreshold;
    opts.sampleStep = 0.1;   // 5 km 阈值下接近窗口仅 ~0.3-1 s，需亚秒级采样

    std::cout << "分析窗口: " << opts.window.toString() << "\n";
    std::cout << "距离阈值: " << kThreshold / 1000.0 << " km\n";
    std::cout << "采样步长: " << opts.sampleStep << " s\n\n";

    CloseApproachAnalyzer analyzer(opts);
    CloseApproachReport   report;

    auto t0 = std::chrono::steady_clock::now();
    rc = analyzer.analyze(*reference, candidates, report);
    auto t1 = std::chrono::steady_clock::now();

    if (rc != eNoError)
    {
        std::cerr << "错误：分析失败 (rc=" << rc << ")\n";
        return 1;
    }

    // ---- 5. 输出结果 ----
    std::cout << "---- 预过滤存活统计 ----\n";
    std::cout << "  目录总数:              " << report.totalCatalog << "\n";
    std::cout << "  过期过滤后:            " << report.afterOutOfDate << "\n";
    std::cout << "  近/远地点过滤后:       " << report.afterApogeePerigee << "\n";
    std::cout << "  轨道路径过滤后:        " << report.afterOrbitPath << "\n";
    std::cout << "  时间过滤后:            " << report.afterTimeFilter << "\n";

    std::cout << "\n---- 各阶段耗时 ----\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "  过期过滤:      " << std::setw(8) << report.secondsOutOfDate << " s\n";
    std::cout << "  近/远地点过滤: " << std::setw(8) << report.secondsApogeePerigee << " s\n";
    std::cout << "  轨道路径过滤:  " << std::setw(8) << report.secondsOrbitPath << " s\n";
    std::cout << "  时间过滤:      " << std::setw(8) << report.secondsTimeFilter << " s\n";
    std::cout << "  暴力采样+精化: " << std::setw(8) << report.secondsBruteForce << " s\n\n";

    std::cout << "---- 接近事件 ----\n";
    std::cout << "  数量: " << report.results.size() << "\n";
    if (report.results.empty())
    {
        std::cout << "  在给定窗口内未发现进入 " << kThreshold / 1000.0 << " km 的目标。\n";
    }
    else
    {
        std::cout << std::setw(4)  << "#"
                  << std::setw(10) << "NORAD"
                  << "  " << std::setw(24) << "TCA"
                  << "  " << std::setw(12) << "距离(km)"
                  << "  " << std::setw(12) << "相对速度(km/s)" << "\n";
        std::cout << std::string(70, '-') << "\n";
        for (size_t i = 0; i < report.results.size(); ++i)
        {
            const CloseApproachResult& r = report.results[i];
            std::cout << std::fixed << std::setprecision(3)
                      << std::setw(4)  << (i + 1)
                      << std::setw(10) << r.noradId
                      << "  " << std::setw(24) << r.tca.toString()
                      << "  " << std::setw(12) << r.minDistance / 1000.0
                      << "  " << std::setw(12) << r.relVelocity / 1000.0 << "\n";
        }
    }

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "\n耗时: " << ms / 1000.0 << " s\n";
    std::cout << "\n示例运行完毕。\n";
    return 0;
}
