///
/// @file      SolarSystem.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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

#include "SolarSystem.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/FileSystem.hpp"
#include <iostream>


AST_NAMESPACE_BEGIN

void SolarSystem::init()
{
    if(!mercury_)
        mercury_ = new CelestialBody();
    if(!venus_)
        venus_ = new CelestialBody();
    if(!earth_)
        earth_ = new CelestialBody();
    if(!mars_)
        mars_ = new CelestialBody();
    if(!jupiter_)
        jupiter_ = new CelestialBody();
    if(!saturn_)
        saturn_ = new CelestialBody();
    if(!uranus_)
        uranus_ = new CelestialBody();
    if(!neptune_)
        neptune_ = new CelestialBody();
    if(!pluto_)
        pluto_ = new CelestialBody();
    if(!moon_)
        moon_ = new CelestialBody();
    if(!sun_)
        sun_ = new CelestialBody();

    bodies_["Mercury"] = mercury_;
    bodies_["Venus"] = venus_;
    bodies_["Earth"] = earth_;
    bodies_["Mars"] = mars_;
    bodies_["Jupiter"] = jupiter_;
    bodies_["Saturn"] = saturn_;
    bodies_["Uranus"] = uranus_;
    bodies_["Neptune"] = neptune_;
    bodies_["Pluto"] = pluto_;
    bodies_["Moon"] = moon_;
    bodies_["Sun"] = sun_;
}

err_t SolarSystem::load(StringView dirpath)
{
    init();
    err_t rc = 0;
    for (const auto& entry : fs::directory_iterator(dirpath.to_string())) {
        if (fs::is_directory(entry.status())) {
            std::string bodyname = entry.path().filename();
            CelestialBody *body = getOrAddBody(bodyname);
            rc |= body->load(entry.path().string());
        }
    }
    return rc;
}


CelestialBody *SolarSystem::getBody(StringView name) const
{
    auto it = bodies_.find(name.to_string());
    if (it != bodies_.end())
    {
        return it->second.get();
    }
    return nullptr;
}

CelestialBody *SolarSystem::addBody(StringView name)
{
    if (getBody(name))
        return nullptr;
    CelestialBody *body = new CelestialBody();
    bodies_[name.to_string()] = body;
    return body;
}

CelestialBody *SolarSystem::getOrAddBody(StringView name)
{
    CelestialBody *body = getBody(name);
    if (!body)
    {
        body = new CelestialBody();
        bodies_[name.to_string()] = body;    
    }
    return body;
}

AST_NAMESPACE_END


