///
/// @file      EphemerisBinary.cpp
/// @brief     文件后备星历实现
/// @details
/// @author    axel
/// @date      2026-06-29
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "EphemerisBinary.hpp"
#include "EphemerisLagrangeVar.hpp"
#include "AstMath/LagrangeInterpolator.hpp"
#include "AstUtil/Math.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/EventTime.hpp"
#include "AstCore/Resolve.hpp"
#include "AstCore/CelestialBody.hpp"
#include <fstream>
#include <cstring>

AST_NAMESPACE_BEGIN

// 静态常量类外定义（GCC/MinGW 需要，MSVC 可省略）
constexpr size_t EphemerisBinary::WINDOW_SIZE;

EphemerisBinary* EphemerisBinary::New()
{
    return new EphemerisBinary();
}

errc_t EphemerisBinary::saveFrom(const Ephemeris* source, const std::string& filepath)
{
    if (!source)
        return eErrorNullPtr;

    // 获取源星历的坐标系
    Frame* srcFrame = source->getFrame();
    if (!srcFrame)
        return eErrorNullPtr;

    // 构造可被 aResolveFrame 往返解析的规范名称："BodyName FrameName"
    std::string fname = srcFrame->getRepresentation();

    // 尝试从 EphemerisLagrangeVar 直接读取原始数据（避免重新插值）
    auto lagrangeVar = dynamic_cast<const EphemerisLagrangeVar*>(source);
    if (!lagrangeVar)
        return eErrorUnsupported;

    const auto& times     = lagrangeVar->getTimes();
    const auto& positions = lagrangeVar->getPositions();
    const auto& velocities = lagrangeVar->getVelocities();

    if (times.empty())
        return eErrorOutOfRange;

    // 写二进制文件
    const TimePoint& epoch = lagrangeVar->getEpoch();
    int64_t  epochInt   = epoch.integerPart();
    double   epochFrac  = epoch.fractionalPart();
    uint64_t pointCount = times.size();

    {
        std::ofstream file(filepath, std::ios::binary | std::ios::trunc);
        if (!file.is_open())
            return eErrorInvalidFile;

        uint32_t magic = MAGIC;
        uint32_t version = VERSION;
        uint64_t nameLen = fname.size();
        double   startTime = times.front();
        double   stopTime  = times.back();

        file.write(reinterpret_cast<const char*>(&magic),      sizeof(magic));
        file.write(reinterpret_cast<const char*>(&version),     sizeof(version));
        file.write(reinterpret_cast<const char*>(&epochInt),    sizeof(epochInt));
        file.write(reinterpret_cast<const char*>(&epochFrac),   sizeof(epochFrac));
        file.write(reinterpret_cast<const char*>(&pointCount),  sizeof(pointCount));
        file.write(reinterpret_cast<const char*>(&startTime),   sizeof(startTime));
        file.write(reinterpret_cast<const char*>(&stopTime),    sizeof(stopTime));
        file.write(reinterpret_cast<const char*>(&nameLen),     sizeof(nameLen));
        file.write(fname.data(), static_cast<std::streamsize>(nameLen));

        for (size_t i = 0; i < pointCount; ++i)
        {
            double t = times[i];
            file.write(reinterpret_cast<const char*>(&t),             sizeof(double));
            file.write(reinterpret_cast<const char*>(positions[i].data()), 3 * sizeof(double));
            file.write(reinterpret_cast<const char*>(velocities[i].data()), 3 * sizeof(double));
        }

        file.close();
        if (!file.good())
            return eErrorInvalidFile;
    }

    // 直接用内存中的数据初始化内部状态，无需重新读取文件
    filepath_  = filepath;
    pointCount_ = pointCount;
    frameName_ = fname;
    frame_     = SharedPtr<Frame>(srcFrame);
    epoch_.duration_.integer_    = epochInt;
    epoch_.duration_.fractional_ = epochFrac;
    averageStep_ = (pointCount > 1) ? (times.back() - times.front()) / (pointCount - 1) : 60.0;
    interval_.setStartStop(times.front(), times.back());

    winTimes_.resize(WINDOW_SIZE);
    winPos_.resize(WINDOW_SIZE);
    winVel_.resize(WINDOW_SIZE);
    winStart_ = 0;

    size_t n = std::min(pointCount_, WINDOW_SIZE);
    for (size_t i = 0; i < n; ++i)
    {
        winTimes_[i] = times[i];
        winPos_[i]   = positions[i];
        winVel_[i]   = velocities[i];
    }

    return eNoError;
}

errc_t EphemerisBinary::open(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
        return eErrorInvalidFile;

    // Read header
    uint32_t magic = 0, version = 0;
    int64_t  epochInt = 0;
    double   epochFrac = 0, startTime = 0, stopTime = 0;
    uint64_t pointCount = 0, nameLen = 0;

    if (!file.read(reinterpret_cast<char*>(&magic),      sizeof(magic)) ||
        !file.read(reinterpret_cast<char*>(&version),     sizeof(version)) ||
        !file.read(reinterpret_cast<char*>(&epochInt),    sizeof(epochInt)) ||
        !file.read(reinterpret_cast<char*>(&epochFrac),   sizeof(epochFrac)) ||
        !file.read(reinterpret_cast<char*>(&pointCount),  sizeof(pointCount)) ||
        !file.read(reinterpret_cast<char*>(&startTime),   sizeof(startTime)) ||
        !file.read(reinterpret_cast<char*>(&stopTime),    sizeof(stopTime)) ||
        !file.read(reinterpret_cast<char*>(&nameLen),     sizeof(nameLen)))
    {
        return eErrorInvalidFile;
    }

    if (magic != MAGIC || version != VERSION)
        return eErrorInvalidFile;

    // Validate nameLen to prevent excessive memory allocation
    if (nameLen > 256)
        return eErrorInvalidFile;

    filepath_ = filepath;
    epoch_.duration_.integer_ = epochInt;
    epoch_.duration_.fractional_ = epochFrac;
    pointCount_ = pointCount;
    interval_.setStartStop(startTime, stopTime);

    // Read frame name
    if (nameLen > 0)
    {
        frameName_.resize(nameLen);
        if (!file.read(&frameName_[0], static_cast<std::streamsize>(nameLen)))
            return eErrorInvalidFile;
    }

    // Resolve frame by name
    Frame* resolvedFrame = aResolveFrame(frameName_);
    if (!resolvedFrame)
        return eErrorNotFound;
    frame_ = SharedPtr<Frame>(resolvedFrame);

    // Allocate window cache
    winTimes_.resize(WINDOW_SIZE);
    winPos_.resize(WINDOW_SIZE);
    winVel_.resize(WINDOW_SIZE);
    winStart_ = 0;

    if (pointCount_ > 0)
    {
        fillWindow(0);
        averageStep_ = (pointCount_ > 1) ? (stopTime - startTime) / (pointCount_ - 1) : 60.0;
    }

    return eNoError;
}

Frame* EphemerisBinary::getFrame() const
{
    return frame_.get();
}

errc_t EphemerisBinary::getInterval(TimeInterval& interval) const
{
    if (pointCount_ == 0)
        return eErrorOutOfRange;

    interval.setStartStop(epoch_, interval_.start(), interval_.stop());
    return eNoError;
}

errc_t EphemerisBinary::getPos(const TimePoint& tp, Vector3d& pos) const
{
    Vector3d vel;
    return getPosVel(tp, pos, vel);
}

errc_t EphemerisBinary::getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const
{
    if (pointCount_ == 0 || !frame_)
        return eErrorOutOfRange;

    double delta = tp - epoch_;

    // 确保窗口覆盖查询点
    int idx = findIndex(delta);
    if (idx < 0 || idx >= static_cast<int>(pointCount_))
        return eErrorOutOfRange;

    // 检查当前窗口是否覆盖插值所需的范围
    // Lagrange 插值需要 INTERP_ORDER/2 个点在 idx 两侧
    int halfOrder = INTERP_ORDER / 2;
    size_t count = std::min(WINDOW_SIZE, pointCount_ - winStart_);
    int localIdx = idx - static_cast<int>(winStart_);
    if (localIdx - halfOrder < 0 || localIdx + halfOrder + 1 >= static_cast<int>(count))
    {
        // 加载窗口时向左预留足够空间，确保插值区间完整
        size_t fillStart = static_cast<size_t>(std::max(0, idx - INTERP_ORDER));
        fillWindow(fillStart);
        count = std::min(WINDOW_SIZE, pointCount_ - winStart_);
        localIdx = idx - static_cast<int>(winStart_);
    }

    // 精确命中存储点
    if (localIdx >= 0 && localIdx < static_cast<int>(count) && winTimes_[localIdx] == delta)
    {
        pos = winPos_[localIdx];
        vel = winVel_[localIdx];
        return eNoError;
    }

    // Lagrange 插值
    int n = std::min(INTERP_ORDER + 1, static_cast<int>(count));
    int start = localIdx - n / 2;
    start = clamp(start, 0, static_cast<int>(count) - n);

    aLagrangeInterpolate(winTimes_.data() + start, winPos_[start].data(), n, 3, delta, pos.data());
    aLagrangeInterpolate(winTimes_.data() + start, winVel_[start].data(), n, 3, delta, vel.data());

    return eNoError;
}

void EphemerisBinary::fillWindow(size_t fileIdx) const
{
    if (pointCount_ == 0)
        return;

    size_t startIdx = fileIdx;
    size_t count = std::min(WINDOW_SIZE, pointCount_ - startIdx);
    if (count == 0)
        return;

    std::ifstream file(filepath_, std::ios::binary);
    if (!file.is_open())
        return;

    // Calculate body offset: header(56) + frameNameLen + body offset
    uint64_t nameLen = frameName_.size();
    size_t headerSize = HEADER_BASE + nameLen;
    size_t bodyOffset = headerSize + startIdx * POINT_BYTES;

    file.seekg(static_cast<std::streamoff>(bodyOffset));

    for (size_t i = 0; i < count; ++i)
    {
        double t = 0;
        double buf[6];
        file.read(reinterpret_cast<char*>(&t), sizeof(double));
        file.read(reinterpret_cast<char*>(buf), 6 * sizeof(double));
        winTimes_[i] = t;
        winPos_[i] = Vector3d{buf[0], buf[1], buf[2]};
        winVel_[i] = Vector3d{buf[3], buf[4], buf[5]};
    }
    winStart_ = startIdx;
}

int EphemerisBinary::findIndex(double delta) const
{
    // 与 EphemerisLagrangeVar::findIndex 保持一致的逻辑：
    // 估算索引 → 线性遍历直到满足 times_[index] <= delta < times_[index + 1]
    // 区别仅在于通过 timeAt() 按需加载窗口，而非直接访问内存数组

    const int num_points = static_cast<int>(pointCount_);
    if (num_points == 0)
        return -1;

    // 按需获取全局索引处的时间值
    auto timeAt = [this](int idx) -> double {
        size_t uIdx = static_cast<size_t>(idx);
        int local = idx - static_cast<int>(winStart_);
        size_t count = std::min(WINDOW_SIZE, pointCount_ - winStart_);
        if (local < 0 || local >= static_cast<int>(count))
        {
            fillWindow(uIdx);
            local = idx - static_cast<int>(winStart_);
        }
        return winTimes_[local];
    };

    // 估算初始索引
    int index = static_cast<int>(delta / averageStep_);
    if (index < 0)
        index = 0;
    else if (index > num_points - 1)
        index = num_points - 1;

    double t = timeAt(index);

    if (t > delta)
    {
        while (true)
        {
            --index;
            if (index < 0)
                return -1;
            if (timeAt(index) <= delta)
                return index;
        }
    }
    else if (t < delta)
    {
        while (true)
        {
            ++index;
            if (index > num_points - 1)
                return num_points;
            double t2 = timeAt(index);
            if (t2 >= delta)
            {
                if (t2 > delta)
                    return index - 1;
                else
                    return index;
            }
        }
    }
    else
    {
        return index;
    }
}

AST_NAMESPACE_END
