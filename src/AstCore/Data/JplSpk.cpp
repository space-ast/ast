///
/// @file      JplSpk.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-26
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

#include "JplSpk.hpp"
#include "AstCore/TimeSystem.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/Interval.hpp"
#include "AstCore/SpiceAPI.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/RunTimeSpice.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/SPKParser.hpp"

AST_NAMESPACE_BEGIN

JplSpk::JplSpk()
    : handle_(0)
    , isIntervalCached_(false)
{
}

JplSpk::JplSpk(StringView spkfile)
{
    int rc = open(spkfile);
    if (rc != eNoError)
    {
        aError("failed to open spk file '%.*s'", (int)spkfile.size(), spkfile.data());
    }
}

JplSpk::~JplSpk()
{
    close();
}

errc_t JplSpk::open(StringView filepath)
{
    close();
    spkfile_ = std::string(filepath);
    return SpiceAPI::Instance()->spklef(spkfile_.c_str(), &handle_);
}

errc_t JplSpk::close()
{
    if (handle_ > 0)
    {
        int handle = handle_;
        handle_ = 0;
        spkfile_.clear();
        isIntervalCached_ = false;
        return SpiceAPI::Instance()->spkuef(handle);
    }
    return eNoError;
}

errc_t JplSpk::getPosICRF(
    const TimePoint &tp,
    int target,
    int referenceBody,
    Vector3d &pos)
{
    double et = aTimePointToSpiceEt(tp);
    CartState state{};
    double lt;
    errc_t rc = SpiceAPI::Instance()->spkgeo(target, et, "J2000", referenceBody, state.data(), &lt);
    pos = state.pos() * 1e3;
    return rc;
}

errc_t JplSpk::getPosVelICRF(
    const TimePoint& tp, 
    int target,
    int referenceBody,
    Vector3d& pos,
    Vector3d& vel
){
    double et = aTimePointToSpiceEt(tp);
    CartState state{};
    double lt;
    errc_t rc = SpiceAPI::Instance()->spkgeo(target, et, "J2000", referenceBody, state.data(), &lt);
    pos = state.pos() * 1e3;
    vel = state.vel() * 1e3;
    return rc;
}


errc_t aSpiceGetInterval(StringView filepath, int target, TimeInterval &timeInterval)
{
    /*!
    @note
    CSPICE 所提供的获取SPK段覆盖时间区间的方法涉及 `SpiceCell` 结构体，调用比较麻烦，
    而且CSPICE提供的方法也需要重新打开文件
    因此这里直接使用 `SPKParser` 类来获取SPK段描述符数组，然后从数组中提取目标体的时间间隔
    */

    SPKParser parser;
    errc_t rc = parser.parse(filepath);
    if(rc != eNoError)
    {
        aError("failed to parse spk file '%.*s'", (int)filepath.size(), filepath.data());
        return rc;
    }
    const std::vector<SPK_Descriptor>& spkDescriptors = parser.getDescriptors();
    bool found = false;
    Interval interval = Interval::Zero();
    for(int i = (int)spkDescriptors.size() - 1; i >= 0; i--)
    {
        const auto& desc = spkDescriptors[i];
        if(desc.target == target)
        {
            double start = desc.start_time;
            double end = desc.end_time;
            if(found)
            {
                errc_t rc = interval.merge({start, end});
                if(rc != eNoError)
                    aError("failed to merge time interval");
            }else{
                interval.setStartStop(start, end);
            }
            found = true;
        }
    }
    TimePoint startTime = aSpiceEtToTimePoint(interval.start());
    TimePoint endTime = aSpiceEtToTimePoint(interval.stop());
    timeInterval.setStartStop(startTime, endTime);
    
    return eNoError;
}


errc_t aSpiceGetBodyIds(StringView filepath, std::vector<int>& ids)
{
    SPKParser parser;
    errc_t rc = parser.parse(filepath);
    if(rc != eNoError)
    {
        aError("failed to parse spk file '%.*s'", (int)filepath.size(), filepath.data());
        return rc;
    }
    const std::vector<SPK_Descriptor>& spkDescriptors = parser.getDescriptors();
    for(const auto& desc : spkDescriptors)
    {
        auto iter = std::find(ids.begin(), ids.end(), desc.target);
        if(iter == ids.end())
        {
            ids.push_back(desc.target);
        }
    }
    return eNoError;
}


errc_t aSpiceGetBodyNames(StringView filepath, std::vector<std::string>& names)
{
    std::vector<int> ids;
    aSpiceGetBodyIds(filepath, ids);
    names.clear();
    names.reserve(ids.size());
    for(int id : ids)
    {
        names.push_back(JplSpk::getBodyName(id));
    }
    return eNoError;
}

std::string JplSpk::getBodyName(int id)
{
    std::string name;
    errc_t rc = SpiceAPI::Instance()->bodc2n(id, name);
    if(rc == eNoError){
        return name;
    }
    return "NAIF ID " + aFormatInt(id);
}

errc_t JplSpk::getInterval(int target, TimeInterval &interval) const
{
    if(isIntervalCached_)
    {
        interval = intervalCache_;
        return eNoError;
    }else{
        errc_t rc = aSpiceGetInterval(spkfile_, target, interval);
        if(rc == eNoError){
            isIntervalCached_ = true;
            intervalCache_ = interval;
        }
        return rc;
    }
}

errc_t JplSpk::getBodyNames(std::vector<std::string> &names) const
{
    return aSpiceGetBodyNames(spkfile_, names);
}

errc_t JplSpk::getBodyIds(std::vector<int> &ids) const
{
    return aSpiceGetBodyIds(spkfile_, ids);
}

AST_NAMESPACE_END
