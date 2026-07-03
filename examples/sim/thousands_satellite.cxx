///
/// @file      thousands_satellite.cpp
/// @brief     10万颗卫星 HPOP 预报仿真场景
/// @details   使用 AstSim 工程中的 Satellite 和 MotionHPOP 对象，
///            创建10万颗卫星，每颗配置 HPOP 高精度轨道预报模型，
///            仿真时长为1天，输出星历数据。
///            支持 std::thread 和 OpenMP 两种并行方式切换。
/// @author    axel
/// @date      2026-06-29
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "ast/Satellite.hpp"
#include "ast/MotionHPOP.hpp"
#include "ast/MotionTwoBody.hpp"
#include "ast/MotionProfile.hpp"
#include "ast/MotionOrbitDynamics.hpp"
#include "ast/Scenario.hpp"
#include "ast/StateCartesian.hpp"
#include "ast/State.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/Frame.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/EventInterval.hpp"
#include "ast/Ephemeris.hpp"
#include "ast/EphemerisBinary.hpp"
#include "ast/FileSystem.hpp"
#include "ast/ScopedPtr.hpp"
#include "ast/RunTime.hpp"
#include "ast/Literals.hpp"
#include "ast/Constants.h"
#include "ast/Vector.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <omp.h>

AST_USING_NAMESPACE
using namespace _AST literals;

// ================================================================
// 并行方式枚举
// ================================================================
enum class EParallelMethod
{
    StdThread,   // std::thread 手动分块
    OpenMP,      // OpenMP parallel for
};

// ================================================================
// 星历存储模式枚举
// ================================================================
enum class EEphemerisMode
{
    Memory,      // EphemerisLagrangeVar 常驻内存
    FileBacked,  // EphemerisBinary 磁盘后备
};

// ================================================================
// 运动模型枚举
// ================================================================
enum class EMotionModel
{
    TwoBody,   // 二体问题 (开普勒轨道)
    HPOP,      // 高精度轨道预报器 (多摄动力)
};

// ================================================================
// 星历生成结果
// ================================================================
struct EphemerisResult
{
    int    successCount;
    int    failCount;
    double elapsedMs;
};

// ================================================================
// std::thread 并行实现
// ================================================================
EphemerisResult generateEphemerisStdThread(
    std::vector<Mover*>& satellites,
    int progressStep,
    EEphemerisMode ephMode,
    const std::chrono::high_resolution_clock::time_point& startTime)
{
    int hwConcurrency = std::thread::hardware_concurrency();
    int numThreads = (hwConcurrency > 0) ? hwConcurrency : 4;
    int numWorkers = (numThreads > 1) ? (numThreads - 1) : 1;

    std::cout << "  并行方式: std::thread 手动分块" << std::endl;
    std::cout << "  检测到 " << hwConcurrency << " 个硬件线程"
              << ", 创建 " << numWorkers << " 个 worker + main 线程 = " << numThreads << " 个并行任务" << std::endl;
    if (hwConcurrency == 0)
    {
        std::cout << "    (无法检测硬件线程数, 回退为 " << numThreads << " 个线程)" << std::endl;
    }
    std::cout << std::endl;

    std::atomic<size_t> completedCount{0};
    std::atomic<int> successCount{0};
    std::atomic<int> failCount{0};
    std::mutex coutMutex;

    std::string outputDir = "output";
    fs::create_directories(outputDir);

    auto worker = [&](size_t startIdx, size_t endIdx)
    {
        for (size_t i = startIdx; i < endIdx; ++i)
        {
            errc_t result = satellites[i]->generateEphemerisSimple();

            if (result == eNoError)
            {
                if (ephMode == EEphemerisMode::FileBacked)
                {
                    std::string ephPath = outputDir + "/sat_" + std::to_string(i) + ".eph";
                    auto ephBin = EphemerisBinary::New();
                    if (ephBin->saveFrom(satellites[i]->getEphemeris(), ephPath) == eNoError)
                        satellites[i]->setEphemeris(ephBin);
                    else
                        delete ephBin;
                }
                successCount++;
            }
            else
            {
                int localFails = failCount++;
                if (localFails < 5)
                {
                    std::lock_guard<std::mutex> lock(coutMutex);
                    std::cout << "  警告: 卫星 " << i << " (" << satellites[i]->getName()
                              << ") 星历生成失败, 错误码=" << result << std::endl;
                }
            }

            size_t done = ++completedCount;
            if (done % progressStep == 0 || done == satellites.size())
            {
                std::lock_guard<std::mutex> lock(coutMutex);

                auto now = std::chrono::high_resolution_clock::now();
                auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
                double elapsedSec = elapsedMs / 1000.0;
                double progress = (double)done / satellites.size() * 100.0;
                double rate = done / elapsedSec;
                double etaSec = (satellites.size() - done) / rate;

                int etaMin = (int)(etaSec / 60.0);
                int etaSecRem = (int)(etaSec) % 60;

                std::cout << "  [" << std::setw(6) << std::setprecision(1) << progress << "%] "
                          << "已处理 " << done << " / " << satellites.size()
                          << " | 成功=" << successCount << " 失败=" << failCount
                          << " | 已用时 " << std::setw(5) << std::setprecision(1) << elapsedSec << "s"
                          << " | 速率 " << std::setprecision(1) << rate << " 颗/s"
                          << " | 预计剩余 " << etaMin << "m" << etaSecRem << "s"
                          << std::endl;
            }
        }
    };

    // 均匀分块
    size_t totalCount = satellites.size();
    size_t chunkSize = (totalCount + numThreads - 1) / numThreads;

    struct TaskChunk
    {
        int    id;
        size_t startIdx;
        size_t endIdx;
        bool   isMain;
    };
    std::vector<TaskChunk> tasks;
    tasks.reserve(numThreads);

    for (int t = 0; t < numWorkers; ++t)
    {
        size_t startIdx = t * chunkSize;
        size_t endIdx = std::min(startIdx + chunkSize, totalCount);
        tasks.push_back({t, startIdx, endIdx, false});
    }
    {
        size_t startIdx = numWorkers * chunkSize;
        size_t endIdx = totalCount;
        tasks.push_back({numWorkers, startIdx, endIdx, true});
    }

    // 打印任务分配
    std::cout << "  任务分配:" << std::endl;
    for (const auto& task : tasks)
    {
        std::cout << "    " << (task.isMain ? "Main   " : "Worker #" + std::to_string(task.id))
                  << ": [" << task.startIdx << ", " << task.endIdx
                  << ") = " << (task.endIdx - task.startIdx) << " 颗" << std::endl;
    }
    std::cout << std::endl;

    // 创建 worker 线程，main 线程处理最后一块
    std::vector<std::thread> threads;
    threads.reserve(numWorkers);

    auto ephStartTime = std::chrono::high_resolution_clock::now();

    for (const auto& task : tasks)
    {
        if (task.isMain)
        {
            worker(task.startIdx, task.endIdx);
        }
        else
        {
            threads.emplace_back(worker, task.startIdx, task.endIdx);
        }
    }

    for (auto& th : threads)
    {
        th.join();
    }

    auto ephEndTime = std::chrono::high_resolution_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(ephEndTime - ephStartTime).count();

    return {successCount.load(), failCount.load(), (double)elapsedMs};
}

// ================================================================
// OpenMP 并行实现
// ================================================================
EphemerisResult generateEphemerisOpenMP(
    std::vector<Mover*>& satellites,
    int progressStep,
    EEphemerisMode ephMode,
    const std::chrono::high_resolution_clock::time_point& startTime)
{
    int maxThreads = omp_get_max_threads();
    std::cout << "  并行方式: OpenMP parallel for" << std::endl;
    std::cout << "  OpenMP 最大线程数: " << maxThreads << std::endl;
    std::cout << std::endl;

    size_t totalCount = satellites.size();
    int printFailLimit = 5;

    auto ephStartTime = std::chrono::high_resolution_clock::now();

    // 必须在 parallel 块外部声明 shared 变量供 omp 使用
    size_t completedCount = 0;
    int successCount = 0;
    int failCount = 0;
    std::string outputDir = "output";
    fs::create_directories(outputDir);

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < (int)totalCount; ++i)
    {
        errc_t result = satellites[i]->generateEphemeris();

        if (result == eNoError)
        {
            if (ephMode == EEphemerisMode::FileBacked)
            {
                std::string ephPath = outputDir + "/sat_" + std::to_string(i) + ".eph";
                auto ephBin = EphemerisBinary::New();
                if (ephBin->saveFrom(satellites[i]->getEphemeris(), ephPath) == eNoError)
                    satellites[i]->setEphemeris(ephBin);
                else
                    delete ephBin;
            }

            #pragma omp atomic
            successCount++;
        }
        else
        {
            int localFails;
            #pragma omp critical
            {
                localFails = failCount++;
            }

            if (localFails < printFailLimit)
            {
                #pragma omp critical
                {
                    std::cout << "  警告: 卫星 " << i << " (" << satellites[i]->getName()
                              << ") 星历生成失败, 错误码=" << result << std::endl;
                }
            }
        }

        // 全局原子递增，用完成数判断是否该输出进度
        size_t done;
        #pragma omp critical
        {
            done = ++completedCount;
        }

        if (done % progressStep == 0 || done == totalCount)
        {
            #pragma omp critical
            {
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - ephStartTime).count();
                double elapsedSec = elapsedMs / 1000.0;
                double progress = (double)done / totalCount * 100.0;
                double rate = done / elapsedSec;
                double etaSec = (totalCount - done) / rate;

                int etaMin = (int)(etaSec / 60.0);
                int etaSecRem = (int)(etaSec) % 60;

                std::cout << "  [" << std::setw(6) << std::setprecision(1) << progress << "%] "
                          << "已处理 " << done << " / " << totalCount
                          << " | 成功=" << successCount << " 失败=" << failCount
                          << " | 已用时 " << std::setw(5) << std::setprecision(1) << elapsedSec << "s"
                          << " | 速率 " << std::setprecision(1) << rate << " 颗/s"
                          << " | 预计剩余 " << etaMin << "m" << etaSecRem << "s"
                          << std::endl;
            }
        }
    }

    auto ephEndTime = std::chrono::high_resolution_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(ephEndTime - ephStartTime).count();

    return {successCount, failCount, (double)elapsedMs};
}

// ================================================================
// HPOP 力模型工厂函数
// ================================================================
HPOPForceModel configureHPOPForceModel(CelestialBody* earthBody)
{
    HPOPForceModel fm;
    fm.setCentralBody(earthBody);

    fm.gravity().model_      = "JGM3";
    fm.gravity().maxDegree_  = 4;
    fm.gravity().maxOrder_   = 4;

    fm.useDrag(true);
    fm.drag().f10p7Average_ = 150;
    fm.drag().f10p7Daily_   = 150;

    fm.useSRP(true);

    fm.addThirdBody("Moon");
    fm.addThirdBody("Sun");

    fm.useRelativityCorrection(true);

    std::cout << "HPOP 力模型配置:" << std::endl;
    std::cout << "  重力场: " << fm.gravity().model_
              << " (阶数=" << fm.gravity().maxDegree_
              << ", 次数=" << fm.gravity().maxOrder_ << ")" << std::endl;
    std::cout << "  大气阻力: " << (fm.useDrag() ? "启用" : "禁用") << std::endl;
    std::cout << "  太阳辐射压: " << (fm.useSRP() ? "启用" : "禁用") << std::endl;
    std::cout << "  第三体引力: " << fm.getThirdBodies().size() << " 个" << std::endl;
    std::cout << "  相对论修正: " << (fm.useRelativityCorrection() ? "启用" : "禁用") << std::endl;
    std::cout << std::endl;

    return fm;
}

// ================================================================
// 运动模型工厂函数
// ================================================================

// 创建 HPOP 运动模型
MotionOrbitDynamics* createMotionHPOP(const HPOPForceModel& forceModel,
                                      double stepSize,
                                      const TimeInterval& interval)
{
    auto motion = MotionHPOP::New();
    motion->setForceModel(forceModel);
    motion->setStepSize(stepSize);
    motion->setInterval(interval);
    return motion;
}

// 创建 TwoBody 运动模型
MotionOrbitDynamics* createMotionTwoBody(double stepSize,
                                         const TimeInterval& interval)
{
    auto motion = MotionTwoBody::New();
    motion->setStepSize(stepSize);
    motion->setInterval(interval);
    return motion;
}

// ================================================================
// 主函数
// ================================================================
int main()
{
    // --- 选择运动模型、星历模式和并行方式 ---
    const EMotionModel    MOTION_MODEL     = EMotionModel::TwoBody;
    const EEphemerisMode  EPHEMERIS_MODE   = EEphemerisMode::FileBacked;
    const EParallelMethod PARALLEL_METHOD  = EParallelMethod::OpenMP;

    // 初始化运行时环境
    aInitialize();
    setlocale(LC_ALL, ".UTF8");
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "==============================================" << std::endl;
    std::cout << "  10万颗卫星 HPOP 预报仿真场景" << std::endl;
    std::cout << "  运动模型: " << (MOTION_MODEL == EMotionModel::HPOP ? "HPOP" : "TwoBody") << std::endl;
    std::cout << "  使用 AstSim 工程对象" << std::endl;
    std::cout << "  星历存储: " << (EPHEMERIS_MODE == EEphemerisMode::FileBacked ? "FileBacked (EphemerisBinary)" : "Memory (EphemerisLagrangeVar)") << std::endl;
    std::cout << "  并行方式: " << (PARALLEL_METHOD == EParallelMethod::OpenMP ? "OpenMP" : "std::thread") << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << std::endl;

    // ================================================================
    // 仿真参数
    // ================================================================
    const int    NUM_SATELLITES = 100000;          // 卫星数量
    const double DURATION       = 10_day;           // 预报时长: 1天
    const double STEP_SIZE      = 60.0;            // 星历步长: 60秒
    const int    PROGRESS_STEP  = 100;             // 每处理 N 颗卫星输出一次进度

    // ================================================================
    // 创建仿真场景 (AstSim::Scenario)
    // ================================================================
    auto scenario = new Scenario();
    scenario->setName("100K_Satellite_HPOP_Scenario");

    // 设置场景历元时间: 2026-01-01 00:00:00 UTC
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    scenario->setEpoch(epoch);

    // 设置场景时间间隔: 1天
    TimeInterval interval(epoch, epoch + DURATION);
    scenario->setInterval(interval);

    // 设置中心天体 (地球)
    auto earthBody = aGetBody("Earth");
    if (earthBody)
    {
        scenario->setPrimaryBody(earthBody);
        std::cout << "中心天体: " << earthBody->getName() << std::endl;
    }
    else
    {
        std::cout << "警告: 无法获取地球天体对象" << std::endl;
    }

    std::cout << "场景历元: 2026-01-01 00:00:00 UTC" << std::endl;
    std::cout << "预报时长: " << DURATION / 3600.0 << " 小时 (" << DURATION / 86400.0 << " 天)" << std::endl;
    std::cout << "星历步长: " << STEP_SIZE << " 秒" << std::endl;
    std::cout << "场景对象创建完毕" << std::endl;
    std::cout << std::endl;

    // ================================================================
    // 配置力模型 (仅在 HPOP 模式下)
    // ================================================================
    HPOPForceModel forceModel;
    if (MOTION_MODEL == EMotionModel::HPOP)
    {
        forceModel = configureHPOPForceModel(earthBody);
    }

    // ================================================================
    // 随机数生成器
    // ================================================================
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> altDist(300_km, 1500_km);
    std::uniform_real_distribution<double> incDist(10_deg, 170_deg);
    std::uniform_real_distribution<double> angleDist(0.0, 360_deg);
    std::uniform_real_distribution<double> eccDist(0.0, 0.01);

    std::cout << "轨道参数分布:" << std::endl;
    std::cout << "  高度范围: 300 ~ 1500 km (LEO)" << std::endl;
    std::cout << "  倾角范围: 10° ~ 170°" << std::endl;
    std::cout << "  偏心率范围: 0.0 ~ 0.01 (近圆轨道)" << std::endl;
    std::cout << "  RAAN / 近地点角 / 真近点角: 随机均匀分布" << std::endl;
    std::cout << std::endl;

    // ================================================================
    // 创建 10 万颗卫星 (AstSim::Satellite)
    // ================================================================
    std::cout << "正在创建 " << NUM_SATELLITES << " 颗卫星对象..." << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();

    std::vector<Mover*> satellites;
    satellites.reserve(NUM_SATELLITES);

    for (int i = 0; i < NUM_SATELLITES; ++i)
    {
        auto sat = Satellite::New();
        sat->setName("Sat_" + std::to_string(i));

        MotionOrbitDynamics* motion = nullptr;
        switch (MOTION_MODEL)
        {
        case EMotionModel::HPOP:
            motion = createMotionHPOP(forceModel, STEP_SIZE, interval);
            break;
        case EMotionModel::TwoBody:
            motion = createMotionTwoBody(STEP_SIZE, interval);
            break;
        }

        auto initialState = StateCartesian::New();
        initialState->setFrameByName("Earth ICRF");
        initialState->setGM(kEarthGrav);
        initialState->setStateEpoch(epoch);

        OrbElem elem;
        elem.a_      = kEarthRadius + altDist(rng);
        elem.e_      = eccDist(rng);
        elem.i_      = incDist(rng);
        elem.raan_   = angleDist(rng);
        elem.argper_ = angleDist(rng);
        elem.trueA_  = angleDist(rng);

        Vector3d pos, vel;
        errc_t result = aOrbElemToCart(elem, kEarthGrav, pos, vel);
        if (result != eNoError)
        {
            std::cout << "错误: 卫星 " << i << " 轨道根数转换失败, 错误码=" << result << std::endl;
            continue;
        }

        CartState cartState;
        cartState.pos() = pos;
        cartState.vel() = vel;
        initialState->setState(cartState);

        motion->setInitialState(initialState);
        motion->setPropagationFrame(initialState->getFrame());

        sat->setMotionProfile(motion);
        satellites.push_back(sat);

        if ((i + 1) % PROGRESS_STEP == 0)
        {
            std::cout << "  已创建 " << (i + 1) << " / " << NUM_SATELLITES << " 颗卫星..." << std::endl;
        }
    }

    auto createEndTime = std::chrono::high_resolution_clock::now();
    auto createDuration = std::chrono::duration_cast<std::chrono::milliseconds>(createEndTime - startTime).count();

    std::cout << std::endl;
    std::cout << "卫星对象创建完成!" << std::endl;
    std::cout << "  实际创建: " << satellites.size() << " 颗卫星" << std::endl;
    std::cout << "  耗时: " << createDuration / 1000.0 << " 秒" << std::endl;
    std::cout << std::endl;

    // ================================================================
    // 生成星历
    // ================================================================
    std::cout << "正在生成星历数据..." << std::endl;
    std::cout << "  每颗卫星预报 " << DURATION / 86400.0 << " 天 (" << DURATION / STEP_SIZE << " 个星历点)" << std::endl;
    std::cout << std::endl;

    EphemerisResult ephResult;
    auto ephStartTime = std::chrono::high_resolution_clock::now();

    switch (PARALLEL_METHOD)
    {
    case EParallelMethod::StdThread:
        ephResult = generateEphemerisStdThread(satellites, PROGRESS_STEP, EPHEMERIS_MODE, ephStartTime);
        break;
    case EParallelMethod::OpenMP:
        ephResult = generateEphemerisOpenMP(satellites, PROGRESS_STEP, EPHEMERIS_MODE, ephStartTime);
        break;
    }

    auto ephEndTime = std::chrono::high_resolution_clock::now();
    auto ephDuration = std::chrono::duration_cast<std::chrono::milliseconds>(ephEndTime - ephStartTime).count();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(ephEndTime - startTime).count();

    // ================================================================
    // 输出仿真结果统计
    // ================================================================
    std::cout << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "  仿真结果统计" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "并行方式:        " << (PARALLEL_METHOD == EParallelMethod::OpenMP ? "OpenMP" : "std::thread") << std::endl;
    std::cout << "卫星总数:        " << satellites.size() << " 颗" << std::endl;
    std::cout << "预报成功:        " << ephResult.successCount << " 颗" << std::endl;
    std::cout << "预报失败:        " << ephResult.failCount << " 颗" << std::endl;
    std::cout << "预报成功率:      " << (double)ephResult.successCount / satellites.size() * 100.0 << " %" << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "运动模型:        " << (MOTION_MODEL == EMotionModel::HPOP ? "HPOP" : "TwoBody") << std::endl;
    std::cout << "预报时长:        " << DURATION / 86400.0 << " 天 (" << DURATION / 3600.0 << " 小时)" << std::endl;
    std::cout << "星历步长:        " << STEP_SIZE << " 秒" << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "对象创建耗时:    " << createDuration / 1000.0 << " 秒" << std::endl;
    std::cout << "星历生成耗时:    " << ephResult.elapsedMs / 1000.0 << " 秒" << std::endl;
    std::cout << "总耗时:          " << totalDuration / 1000.0 << " 秒" << std::endl;
    if (ephResult.successCount > 0)
    {
        std::cout << "平均每颗卫星:    " << (ephResult.elapsedMs / 1000.0) / ephResult.successCount << " 秒" << std::endl;
    }
    std::cout << "==============================================" << std::endl;

    // ================================================================
    // 抽样验证
    // ================================================================
    std::cout << std::endl;
    std::cout << "抽样验证 — 前3颗卫星的星历概况:" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    for (size_t i = 0; i < std::min<size_t>(3, satellites.size()); ++i)
    {
        auto sat = satellites[i];
        auto eph = sat->getEphemeris();

        std::cout << std::endl;
        std::cout << "卫星 [" << i << "]: " << sat->getName() << std::endl;

        if (!eph)
        {
            std::cout << "  星历为空" << std::endl;
            continue;
        }

        TimeInterval ephInterval;
        errc_t result = eph->getInterval(ephInterval);
        if (result == eNoError)
        {
            std::cout << "  星历起始: " << ephInterval.getStart().toString() << std::endl;
            std::cout << "  星历结束: " << ephInterval.getStop().toString() << std::endl;
        }

        State* initState = sat->getInitialState();
        if (initState)
        {
            CartState cs;
            result = initState->getState(cs);
            if (result == eNoError)
            {
                std::cout << "  初始位置: (" << cs.pos().x() / 1000.0 << ", "
                          << cs.pos().y() / 1000.0 << ", "
                          << cs.pos().z() / 1000.0 << ") km" << std::endl;
                std::cout << "  初始速度: (" << cs.vel().x() / 1000.0 << ", "
                          << cs.vel().y() / 1000.0 << ", "
                          << cs.vel().z() / 1000.0 << ") km/s" << std::endl;
            }
        }

        std::cout << "  星历采样点:" << std::endl;
        std::cout << "    时间(h)\t\tX(km)\t\tY(km)\t\tZ(km)" << std::endl;

        for (double h = 0; h <= 24; h += 6)
        {
            double sec = h * 3600.0;
            TimePoint tp = epoch + sec;
            Vector3d pos, vel;
            result = eph->getPosVel(tp, pos, vel);
            if (result == eNoError)
            {
                std::cout << "    " << std::setw(6) << h << "\t\t"
                          << std::setw(11) << pos.x() / 1000.0 << "\t"
                          << std::setw(11) << pos.y() / 1000.0 << "\t"
                          << std::setw(11) << pos.z() / 1000.0 << std::endl;
            }
            else
            {
                std::cout << "    " << std::setw(6) << h << "\t\t"
                          << "错误码=" << result << std::endl;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "仿真完成!" << std::endl;
    std::cin.get();

    // ================================================================
    // 清理资源
    // ================================================================
    for (auto& sat : satellites)
    {
        delete sat;
    }
    satellites.clear();
    delete scenario;

    aUninitialize();
    return 0;
}
