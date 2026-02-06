///
/// @file      IAUXYSPrecomputed.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-22
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

#include "IAUXYSPrecomputed.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/BKVItemView.hpp"
#include "AstUtil/String.hpp"
#include "AstCore/RunTime.hpp"      // for aXYS_IERS2010_NoCorrection
#include "AstCore/TimePoint.hpp"
#include "AstMath/LagrangeInterpolator.hpp"

AST_NAMESPACE_BEGIN

IAUXYSPrecomputed::IAUXYSPrecomputed()
{
    
}

err_t IAUXYSPrecomputed::getValue(const TimePoint &tp, array3d &xys) const
{
    JulianDate jdTT = tp.toTT();
    return getValueTT(jdTT, xys);
}

err_t IAUXYSPrecomputed::getValueTT(const JulianDate &jdTT, array3d &xys) const
{
    double deltaDay = (jdTT.day() - this->refepoch_jed_) + jdTT.second() / kSecondsPerDay;
    double indexMiddleDouble = deltaDay / this->step_size_;
    if(A_UNLIKELY(indexMiddleDouble < 0 || indexMiddleDouble > (int)this->xys_points_.size()-1))
        return eErrorOutOfRange;
    //int indexMiddle = static_cast<int>(floor(indexMiddleDouble));
    int indexMiddle = static_cast<int>(indexMiddleDouble); // 不用floor，因为indexMiddleDouble一定大于等于0
    int n = this->interp_order_ + 1;        // 插值点数
    int half_n = n / 2;                     
    int indexBegin = indexMiddle - half_n;  // 插值点开始索引(含)
    if(A_UNLIKELY(indexBegin < 0))
    {
        indexBegin = 0;
    }
    int indexEnd = indexBegin + n;          // 插值点结束索引(不含)
    if(A_UNLIKELY(indexEnd > (int)this->xys_points_.size()))
    {
        indexEnd = (int)this->xys_points_.size();
        indexBegin = indexEnd - n;
        if(A_UNLIKELY(indexBegin < 0))
        {
            return eErrorOutOfRange;
        }
    }
    double startJD = this->refepoch_jed_ + indexBegin * this->step_size_;
    double t = (jdTT.day() - startJD) + jdTT.second() / kSecondsPerDay;
    aLagrangeInterpolateFixedStep(time_array_.data(), xys_points_[indexBegin].data(), n, 3, t, xys.data(), denom_array_.data());
    return 0;
}

err_t IAUXYSPrecomputed::load(StringView filepath)
{
    BKVParser parser(filepath);
    if (!parser.isOpen())
    {
        return eErrorInvalidFile; // 文件打开失败
    }
    BKVParser::EToken token;
    BKVItemView item;
    int num_points = 0;
    int num_columns = 0;
    int interp_order = 0;
    double step_size = 0.0;
    double refepoch_jed = 0.0;
    std::vector<array3d> xys_points;
    // std::string xys_algorithm;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "VERSION")){
                // pass
            }else if(aEqualsIgnoreCase(item.key(), "UPDATED"))
            {

            }else if(aEqualsIgnoreCase(item.key(), "INTERP_ORDER"))
            {
                interp_order = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "NUM_COLUMNS"))
            {
                num_columns = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "REFEPOCH_JED"))
            {
                refepoch_jed = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "STEPSIZE"))
            {
                step_size = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "NUM_POINTS"))
            {
                num_points = item.value().toInt();
                xys_points.reserve(num_points);
            }else if(aEqualsIgnoreCase(item.key(), "XYS_ALGORITHM"))
            {
                // xys_algorithm = item.value().toString();
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "TABLE"))
            {
                for(int i=0;i<num_points;i++)
                {
                    StringView line = parser.getLineSkipComment();
                    array3d xys;
                    // #pragma warning(suppress: 4996)
                    int ret = sscanf(line.data(), "%lf %lf %lf", &xys[0], &xys[1], &xys[2]);
                    if(ret != 3){
                        aError("expect 3 values, error %d, line %d, %s\n", ret, parser.getLineNumber(), line.data());
                        return eErrorInvalidFile;
                    }
                    xys[0] *= kArcSecToRad;
                    xys[1] *= kArcSecToRad;
                    xys[2] *= kArcSecToRad;
                    xys_points.push_back(xys);
                }
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {

        }
    }while(token != BKVParser::eEOF);
    this->num_columns_ = num_columns;
    this->interp_order_ = interp_order;
    this->refepoch_jed_ = refepoch_jed;
    this->step_size_ = step_size;
    this->xys_points_ = std::move(xys_points);
    initInterpolation();
    return 0;
}

void IAUXYSPrecomputed::precompute(double start_jed, double stop_jed, double step_size)
{
    JulianDate jdTT = JulianDate::FromImpreciseDay(start_jed);
    int dayStep = static_cast<int>(step_size);
    double secStep = (step_size - dayStep) * kSecondsPerDay;
    std::vector<array3d> xys_points;
    int size = (int)((stop_jed - start_jed) / step_size) + 1;
    xys_points.reserve(size);
    // double jed = start_jed;
    for(int i=0;i<size;i++)
    {
        array3d xys;
        aTheoreticalXYS_IERS2010_TT(jdTT, xys);
        xys_points.push_back(xys);
        jdTT.day() += dayStep;
        jdTT.second() += secStep;
    }
    this->xys_points_ = std::move(xys_points);
    this->refepoch_jed_ = start_jed;
    this->step_size_ = step_size;
    this->initInterpolation();
}

void IAUXYSPrecomputed::setInterpOrder(int order)
{
    interp_order_ = order;
    initInterpolation();
}

void IAUXYSPrecomputed::initInterpolation()
{
    denom_array_.resize(interp_order_ + 1);
    time_array_.resize(interp_order_ + 1);
    aLagrangeInterpolateDenomArray(interp_order_ + 1, step_size_, denom_array_.data());
    for(int i=0;i<=interp_order_;i++)
    {
        time_array_[i] = i * step_size_;
    }
}



AST_NAMESPACE_END

