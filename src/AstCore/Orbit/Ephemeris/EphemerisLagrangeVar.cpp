///
/// @file      EphemerisLagrangeVar.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#include "EphemerisLagrangeVar.hpp"
#include "AstMath/LagrangeInterpolator.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN

Frame *EphemerisLagrangeVar::getFrame() const
{
    return frame_.get();
}

void EphemerisLagrangeVar::getTimes(TimeList &list) const
{
    list.setEpoch(epoch_);
    list.setSeconds(times_);
}

int EphemerisLagrangeVar::findIndex(double delta) const
{
    assert(!times_.empty());
    // A_ASSUME(!times_.empty());

    const int num_points = static_cast<int>(times_.size());
    
    // 根据平均步长猜测初始索引，然后调整索引直到满足以下条件
    // 条件：times_[index] <= delta < times_[index + 1]
    // 有效索引值的范围：[0, num_points - 1]
    // 当索引值返回的是num_points-1时，表示刚好在右侧边界上

    int index = static_cast<int>((delta - times_.front()) / averageStep_);
    if(index < 0)
        index = 0;
    else if(index > num_points - 1)
        index = num_points - 1;

    if(times_[index] > delta)
    {
        while(1)
        {
            --index;
            if(index<0)
                return -1;
            if(times_[index] <= delta)
                return index;
        };
    }
    else if(times_[index] < delta)
    {
        while(1)
        {
            ++index;
            if(index > num_points - 1)
                return num_points;
            if(times_[index] >= delta){
                if(times_[index] > delta)
                    return index - 1;
                else
                    return index;
            }
        };
    }
    else
    {
        return index;
    }
}

errc_t EphemerisLagrangeVar::getPos(const TimePoint &tp, Vector3d &pos) const
{
    const int num_points = static_cast<int>(times_.size());
    if(num_points == 0){
        return eErrorOutOfRange;
    }

    const double delta = tp - epoch_;

    const int index = findIndex(delta);
    
    // 有效索引值的范围：[0, num_points - 1]
    if(index < 0 || index > num_points - 1)
    {
        return eErrorOutOfRange;
    }

    // 刚好在时间点上
    if(delta == times_[index])
    {
        pos = positions_[index];
        return 0;
    }
    else // 插值
    {
        // 插值点数n
        int n = std::min(interpolateOrder_ + 1, num_points);
        int start = index - n / 2;
        start = clamp(start, 0, num_points - n);
        // int end = start + n;
        aLagrangeInterpolate(times_.data() + start, positions_[start].data(), n, 3, delta, pos.data());
    }
    return eNoError;
}

errc_t EphemerisLagrangeVar::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    const int num_points = static_cast<int>(times_.size());
    if(num_points == 0){
        return eErrorOutOfRange;
    }

    const double delta = tp - epoch_;

    const int index = findIndex(delta);
    
    // 有效索引值的范围：[0, num_points - 1]
    if(index < 0 || index > num_points - 1)
    {
        return eErrorOutOfRange;
    }
    
    // 刚好在时间点上
    if(delta == times_[index])
    {
        pos = positions_[index];
        vel = velocities_[index];
        return eNoError;
    }else{ // 插值
        // 插值点数n
        int n = std::min(interpolateOrder_ + 1, num_points);
        int start = index - n / 2;
        start = clamp(start, 0, num_points - n);
        // int end = start + n;

        /// @todo 这里有一点优化空间，可以避免重复计算插值点的系数
        aLagrangeInterpolate(times_.data() + start, positions_[start].data(), n, 3, delta, pos.data());
        aLagrangeInterpolate(times_.data() + start, velocities_[start].data(), n, 3, delta, vel.data());
    }
    return eNoError;
}

errc_t EphemerisLagrangeVar::getInterval(TimeInterval &interval) const
{
    double start = 0;
    double stop = 0;
    if(!times_.empty()){
        start = times_.front();
        stop = times_.back();
    }
    interval.setBounds(epoch_, start, stop);
    return eNoError;
}

void EphemerisLagrangeVar::setTimes(const std::vector<double> &times)
{
    times_ = times;
    if(times_.size() > 1){
        averageStep_ = (times_.back() - times_.front()) / static_cast<double>(times_.size() - 1);
    }
}

void EphemerisLagrangeVar::setTimes(std::vector<double> &&times)
{
    times_ = std::move(times);
    if(times_.size() > 1){
        averageStep_ = (times_.back() - times_.front()) / static_cast<double>(times_.size() - 1);
    }
}



AST_NAMESPACE_END

