///
/// @file      AccessLoader.cpp
/// @brief
/// @details
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

#include "AccessLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "BasicComponentLoader.hpp"
#include "AstSim/Access.hpp"
#include "AstSim/Scenario.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include <algorithm>
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 解析时钟主体字段
static EClockHost _aParseClockHost(StringView value)
{
    if(aEqualsIgnoreCase(value, "Base"))
    {
        return EClockHost::eFirstObject;
    }
    else if(aEqualsIgnoreCase(value, "Target"))
    {
        return EClockHost::eSecondObject;
    }
    aWarning(_("不支持的 ClockHost 取值: '%.*s'"), (int)value.size(), value.data());
    return EClockHost::eFirstObject;
}

/// @brief 解析信号传输方向字段
static ETimeSense _aParseTimeSense(StringView value)
{
    if(aEqualsIgnoreCase(value, "Transmit"))
    {
        return ETimeSense::eTransmit;
    }
    else if(aEqualsIgnoreCase(value, "Receive"))
    {
        return ETimeSense::eReceive;
    }
    aWarning(_("不支持的 TimeSense 取值: '%.*s'"), (int)value.size(), value.data());
    return ETimeSense::eTransmit;
}

/// @brief 解析像差类型字段
static EAberrationType _aParseAberrationType(StringView value)
{
    if(aEqualsIgnoreCase(value, "None"))
    {
        return EAberrationType::eNone;
    }
    else if(aEqualsIgnoreCase(value, "Annual"))
    {
        return EAberrationType::eAnnual;
    }
    else if(aEqualsIgnoreCase(value, "Diurnal"))
    {
        return EAberrationType::eDiurnal;
    }
    else if(aEqualsIgnoreCase(value, "Total"))
    {
        return EAberrationType::eTotal;
    }
    aWarning(_("不支持的 AberrationType 取值: '%.*s'"), (int)value.size(), value.data());
    return EAberrationType::eAnnual;
}

/// @brief 由对象路径生成访问名称中的一段
/// @details 去掉开头的 "Scenario/<场景名>" 两段，再把剩余的 '/' 换成 '-'。
///          例如 "Scenario/testAccess/Satellite/Satellite1/Sensor/Sensor1" 得到
///          "Satellite-Satellite1-Sensor-Sensor1"。
/// @param path 对象路径
/// @return 名称片段
static std::string _aMakeAccessNamePart(std::string path)
{
    for(int i = 0; i < 2; ++i)
    {
        auto pos = path.find('/');
        if(pos == std::string::npos)
        {
            break;
        }
        path.erase(0, pos + 1);
    }
    std::replace(path.begin(), path.end(), '/', '-');
    return path;
}

/// @brief 加载单个 Access 块
/// @details 调用时已读到 BEGIN Access，解析到对应的 END Access 为止。
///          块首的两条裸路径依次为主对象和目标对象。
/// @param parser BKV解析器
/// @param scenario 所属场景
/// @param accesses 输出参数，加载得到的 Access 对象
/// @return 错误码
static errc_t _aLoadAccessBlock(BKVParser& parser, Scenario& scenario, std::vector<HAccess>& accesses)
{
    BKVItemView item;
    BKVParser::EToken token;
    SharedPtr<Access> access = aMakeShared<Access>();
    Object* baseObject = nullptr;
    Object* targetObject = nullptr;
    std::string basePath;
    std::string targetPath;
    AccessConfig config{};
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(item.value().size() == 0 && item.key().find("/") != StringView::npos)
            {
                // 裸路径行（无值），第一条为主对象，第二条为目标对象。
                // item.key() 指向解析器的内部缓冲区，下一轮解析就会被覆盖，先拷贝出来。
                std::string path(item.key());
                Object* object = aResolveObject(path);
                if(!object)
                {
                    aError(_("无法解析对象 '%s'"), path.c_str());
                }
                // 用"是否已见过第一条路径"来区分主/目标，解析失败也不会错位
                if(basePath.empty())
                {
                    basePath = path;
                    baseObject = object;
                }
                else
                {
                    targetPath = path;
                    targetObject = object;
                }
            }
            else if(aEqualsIgnoreCase(item.key(), "UseLightTimeDelay"))
            {
                config.useLightTimeDelay_ = item.value().toBool();
            }
            else if(aEqualsIgnoreCase(item.key(), "ClockHost"))
            {
                config.clockHost_ = _aParseClockHost(item.value());
            }
            else if(aEqualsIgnoreCase(item.key(), "TimeSense"))
            {
                config.timeSense_ = _aParseTimeSense(item.value());
            }
            else if(aEqualsIgnoreCase(item.key(), "AberrationType"))
            {
                config.aberrationType_ = _aParseAberrationType(item.value());
            }
            else if(aEqualsIgnoreCase(item.key(), "MaxTimeStep"))
            {
                config.maxTimeStep_ = item.value().toDouble();
            }
            else if(aEqualsIgnoreCase(item.key(), "MinTimeStep"))
            {
                config.minTimeStep_ = item.value().toDouble();
            }
            else if(aEqualsIgnoreCase(item.key(), "TimeConvergence"))
            {
                config.timeConvergence_ = item.value().toDouble();
            }
            else if(aEqualsIgnoreCase(item.key(), "AbsValueConvergence"))
            {
                config.absValueConvergence_ = item.value().toDouble();
            }
            else if(aEqualsIgnoreCase(item.key(), "RelValueConvergence"))
            {
                config.relValueConvergence_ = item.value().toDouble();
            }
            // 其余键（IsComputed、UpdateOnReference、UI*/Show*/Inherit/LineWidth 等）与访问计算无关，暂时忽略
        }
        else if(token == BKVParser::eBlockBegin)
        {
            // 跳过未知子块（如 BEGIN Crdn ... END Crdn）
            _aSkipUnknownBlock(parser, item.value());
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "Access"))
            {
                break;
            }
        }
    }while(token != BKVParser::eEOF);

    access->setBaseObject(baseObject);
    access->setTargetObject(targetObject);
    access->setConfig(config);
    access->setName(_aMakeAccessNamePart(basePath) + "-To-" + _aMakeAccessNamePart(targetPath));
    errc_t rc = aSetParentScope(access.get(), &scenario);
    if(rc == eNoError)
    {
        accesses.push_back(access);
    }
    return rc;
}

errc_t aLoadAccess(StringView filepath, Scenario& scenario)
{
    std::vector<HAccess> accesses;
    return aLoadAccess(filepath, scenario, accesses);
}

errc_t aLoadAccess(StringView filepath, Scenario& scenario, std::vector<HAccess>& accesses)
{
    BKVParser parser(filepath);
    if(!parser.isOpen())
    {
        aError(_("打开文件 '%.*s' 失败"), (int)filepath.size(), filepath.data());
        return eErrorInvalidFile;
    }
    accesses.clear();
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin)
        {
            if(aEqualsIgnoreCase(item.value(), "Access"))
            {
                if(errc_t rc = _aLoadAccessBlock(parser, scenario, accesses))
                {
                    return rc;
                }
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END
