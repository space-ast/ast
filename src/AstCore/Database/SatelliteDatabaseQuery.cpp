///
/// @file      SatelliteDatabaseQuery.cpp
/// @brief     卫星数据库查询条件实现
/// @details   实现基于子串的查询条件匹配逻辑。
/// @author    axel
/// @date      2026-07-23
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


#include "SatelliteDatabaseQuery.hpp"
#include "SatelliteDatabaseEntry.hpp"

AST_NAMESPACE_BEGIN


static bool match(StringView pattern, StringView value)
{
    if (pattern.empty())
        return true;
    return value.find(pattern) != std::string::npos;
}

SatelliteDatabaseQuery::SatelliteDatabaseQuery() = default;

SatelliteDatabaseQuery::~SatelliteDatabaseQuery() = default;

void SatelliteDatabaseQuery::setMission(StringView pattern)
{
    mission_ = std::string(pattern);
}

void SatelliteDatabaseQuery::setCommonName(StringView pattern)
{
    commonName_ = std::string(pattern);
}

bool SatelliteDatabaseQuery::matches(const SatelliteDatabaseEntry& entry) const
{
    if (!mission_.empty())
    {
        if (!match(mission_, entry.mission()))
        {
            return false;
        }
    }
    if (!commonName_.empty())
    {
        if (!match(commonName_, entry.commonName()))
        {
            return false;
        }
    }
    return true;
}

bool SatelliteDatabaseQuery::isEmpty() const
{
    return mission_.empty() && commonName_.empty();
}

AST_NAMESPACE_END
