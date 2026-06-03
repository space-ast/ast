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
#include "AstCore/JplDe.hpp"
#include "AstCore/EarthOrientation.hpp"
#include "AstCore/CelestialBodyLoader.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Constants.hpp"
#include <iostream>


AST_NAMESPACE_BEGIN

void SolarSystem::init()
{
    if(isInit_)
        return;
    isInit_ = true;

    bodies_.reserve(200);
    if(!solarSystemBarycenter_){
        solarSystemBarycenter_ = new CelestialBody(this);
        solarSystemBarycenter_->setReadOnly(true);
        solarSystemBarycenter_->setName("SolarSystemBarycenter");
        solarSystemBarycenter_->jplIndex_ = JplDe::eSSBarycenter;
        solarSystemBarycenter_->jplSpiceId_ = ESpiceId::eSolarSystemBarycenter;
        bodies_.push_back(solarSystemBarycenter_);
        nameMap_["SOLAR_SYSTEM_BARYCENTER"] = solarSystemBarycenter_;
        nameMap_["SSB"] = solarSystemBarycenter_;
        nameMap_["SOLAR SYSTEM BARYCENTER"] = solarSystemBarycenter_;
    }
    if(!earthMoonBarycenter_){
        earthMoonBarycenter_ = new CelestialBody(this);
        earthMoonBarycenter_->setReadOnly(true);
        earthMoonBarycenter_->setName("EarthMoonBarycenter");
        earthMoonBarycenter_->jplIndex_ = JplDe::eEMBarycenter;
        earthMoonBarycenter_->jplSpiceId_ = ESpiceId::eEarthBarycenter;
        bodies_.push_back(earthMoonBarycenter_);
        nameMap_["EARTH MOON BARYCENTER"] = earthMoonBarycenter_;
        nameMap_["EARTH-MOON BARYCENTER"] = earthMoonBarycenter_;
        nameMap_["EARTH BARYCENTER"] = earthMoonBarycenter_;
    }
    if(!mercury_){
        mercury_ = new CelestialBody(this);
        mercury_->setReadOnly(true);
        mercury_->setName("Mercury");
        mercury_->jplIndex_ = JplDe::eMercury;
        mercury_->jplSpiceId_ = ESpiceId::eMercury;
        mercury_->gm_ = kMercuryGrav;
        mercury_->radius_ = kMercuryRadius;
        bodies_.push_back(mercury_);
        nameMap_["Mercury"] = mercury_;
    }
    if(!venus_){
        venus_ = new CelestialBody(this);
        venus_->setReadOnly(true);
        venus_->setName("Venus");
        venus_->jplIndex_ = JplDe::eVenus;
        venus_->jplSpiceId_ = ESpiceId::eVenus;
        venus_->gm_ = kVenusGrav;
        venus_->radius_ = kVenusRadius;
        bodies_.push_back(venus_);
        nameMap_["Venus"] = venus_;
    }
    if(!earth_){
        earth_ = new CelestialBody(this);
        earth_->setReadOnly(true);
        earth_->setName("Earth");
        earth_->jplIndex_ = JplDe::eEarth;
        earth_->jplSpiceId_ = ESpiceId::eEarth;
        earth_->orientation_ = new EarthOrientation();
        earth_->gm_ = kEarthGrav_EGM2008;
        earth_->radius_ = kEarthRadius;
        bodies_.push_back(earth_);
        nameMap_["Earth"] = earth_;
    }
    if(!mars_)
    {
        mars_ = new CelestialBody(this);
        mars_->setReadOnly(true);
        mars_->setName("Mars");
        mars_->jplIndex_ = JplDe::eMars;
        mars_->jplSpiceId_ = ESpiceId::eMars;
        mars_->gm_ = kMarsGrav;
        mars_->radius_ = kMarsRadius;
        bodies_.push_back(mars_);
        nameMap_["Mars"] = mars_;
    }
    if(!jupiter_){
        jupiter_ = new CelestialBody(this);
        jupiter_->setReadOnly(true);
        jupiter_->setName("Jupiter");
        jupiter_->jplIndex_ = JplDe::eJupiter;
        jupiter_->jplSpiceId_ = ESpiceId::eJupiter;
        jupiter_->gm_ = kJupiterGrav;
        jupiter_->radius_ = kJupiterRadius;
        bodies_.push_back(jupiter_);
        nameMap_["Jupiter"] = jupiter_;
    }
    if(!saturn_){
        saturn_ = new CelestialBody(this);
        saturn_->setReadOnly(true);
        saturn_->setName("Saturn");
        saturn_->jplIndex_ = JplDe::eSaturn;
        saturn_->jplSpiceId_ = ESpiceId::eSaturn;
        saturn_->gm_ = kSaturnGrav;
        saturn_->radius_ = kSaturnRadius;
        bodies_.push_back(saturn_);
        nameMap_["Saturn"] = saturn_;
    }
    if(!uranus_){
        uranus_ = new CelestialBody(this);
        uranus_->setReadOnly(true);
        uranus_->setName("Uranus");
        uranus_->jplIndex_ = JplDe::eUranus;
        uranus_->jplSpiceId_ = ESpiceId::eUranus;
        uranus_->gm_ = kUranusGrav;
        uranus_->radius_ = kUranusRadius;
        bodies_.push_back(uranus_);
        nameMap_["Uranus"] = uranus_;
    }
    if(!neptune_){
        neptune_ = new CelestialBody(this);
        neptune_->setReadOnly(true);
        neptune_->setName("Neptune");
        neptune_->jplIndex_ = JplDe::eNeptune;
        neptune_->jplSpiceId_ = ESpiceId::eNeptune;
        neptune_->gm_ = kNeptuneGrav;
        neptune_->radius_ = kNeptuneRadius;
        bodies_.push_back(neptune_);
        nameMap_["Neptune"] = neptune_;
    }
    if(!pluto_){
        pluto_ = new CelestialBody(this);
        pluto_->setReadOnly(true);
        pluto_->setName("Pluto");
        pluto_->jplIndex_ = JplDe::ePluto;
        pluto_->jplSpiceId_ = ESpiceId::ePluto;
        pluto_->gm_ = kPlutoGrav;
        pluto_->radius_ = kPlutoRadius;
        bodies_.push_back(pluto_);
        nameMap_["Pluto"] = pluto_;
    }
    if(!moon_){
        moon_ = new CelestialBody(earth_);
        moon_->setReadOnly(true);
        moon_->setName("Moon");
        moon_->jplIndex_ = JplDe::eMoon;
        moon_->jplSpiceId_ = ESpiceId::eMoon;
        moon_->gm_ = kMoonGrav;
        moon_->radius_ = kMoonRadius;
        bodies_.push_back(moon_);
        nameMap_["Moon"] = moon_;
    }
    if(!sun_){
        sun_ = new CelestialBody(this);
        sun_->setReadOnly(true);
        sun_->setName("Sun");
        sun_->jplIndex_ = JplDe::eSun;
        sun_->jplSpiceId_ = ESpiceId::eSun;
        sun_->gm_ = kSunGrav;
        sun_->radius_ = kSunRadius;
        bodies_.push_back(sun_);
        nameMap_["Sun"] = sun_;
    }

    // init spice bodies
    #define _AST_REG_BODY(PARENT, NAME) \
    { \
        auto NAME = new CelestialBody(PARENT); \
        NAME->setReadOnly(true);\
        NAME->setName(#NAME); \
        NAME->jplSpiceId_ = ESpiceId::e##NAME; \
        bodies_.push_back(NAME); \
    }

    // 火星卫星
    _AST_REG_BODY(mars_, Phobos)
    _AST_REG_BODY(mars_, Deimos)
    
    // 木星卫星
    _AST_REG_BODY(jupiter_, Io)
    _AST_REG_BODY(jupiter_, Europa)
    _AST_REG_BODY(jupiter_, Ganymede)
    _AST_REG_BODY(jupiter_, Callisto)
    _AST_REG_BODY(jupiter_, Amalthea)
    _AST_REG_BODY(jupiter_, Himalia)
    _AST_REG_BODY(jupiter_, Elara)
    _AST_REG_BODY(jupiter_, Pasiphae)
    _AST_REG_BODY(jupiter_, Sinope)
    _AST_REG_BODY(jupiter_, Lysithea)
    _AST_REG_BODY(jupiter_, Carme)
    _AST_REG_BODY(jupiter_, Ananke)
    _AST_REG_BODY(jupiter_, Leda)
    _AST_REG_BODY(jupiter_, Thebe)
    _AST_REG_BODY(jupiter_, Adrastea)
    _AST_REG_BODY(jupiter_, Metis)
    
    // 土星卫星
    _AST_REG_BODY(saturn_, Mimas)
    _AST_REG_BODY(saturn_, Enceladus)
    _AST_REG_BODY(saturn_, Tethys)
    _AST_REG_BODY(saturn_, Dione)
    _AST_REG_BODY(saturn_, Rhea)
    _AST_REG_BODY(saturn_, Titan)
    _AST_REG_BODY(saturn_, Hyperion)
    _AST_REG_BODY(saturn_, Iapetus)
    _AST_REG_BODY(saturn_, Phoebe)
    _AST_REG_BODY(saturn_, Janus)
    _AST_REG_BODY(saturn_, Epimetheus)
    _AST_REG_BODY(saturn_, Helene)
    _AST_REG_BODY(saturn_, Telesto)
    _AST_REG_BODY(saturn_, Calypso)
    _AST_REG_BODY(saturn_, Atlas)
    _AST_REG_BODY(saturn_, Prometheus)
    _AST_REG_BODY(saturn_, Pandora)
    _AST_REG_BODY(saturn_, Pan)
    _AST_REG_BODY(saturn_, Methone)
    _AST_REG_BODY(saturn_, Pallene)
    _AST_REG_BODY(saturn_, Polydeuces)
    _AST_REG_BODY(saturn_, Daphnis)
    _AST_REG_BODY(saturn_, Anthe)
    _AST_REG_BODY(saturn_, Aegaeon)
    
    // 天王星卫星
    _AST_REG_BODY(uranus_, Ariel)
    _AST_REG_BODY(uranus_, Umbriel)
    _AST_REG_BODY(uranus_, Titania)
    _AST_REG_BODY(uranus_, Oberon)
    _AST_REG_BODY(uranus_, Miranda)
    _AST_REG_BODY(uranus_, Cordelia)
    _AST_REG_BODY(uranus_, Ophelia)
    _AST_REG_BODY(uranus_, Bianca)
    _AST_REG_BODY(uranus_, Cressida)
    _AST_REG_BODY(uranus_, Desdemona)
    _AST_REG_BODY(uranus_, Juliet)
    _AST_REG_BODY(uranus_, Portia)
    _AST_REG_BODY(uranus_, Rosalind)
    _AST_REG_BODY(uranus_, Belinda)
    _AST_REG_BODY(uranus_, Puck)
    
    // 海王星卫星
    _AST_REG_BODY(neptune_, Triton)
    _AST_REG_BODY(neptune_, Nereid)
    _AST_REG_BODY(neptune_, Naiad)
    _AST_REG_BODY(neptune_, Thalassa)
    _AST_REG_BODY(neptune_, Despina)
    _AST_REG_BODY(neptune_, Galatea)
    _AST_REG_BODY(neptune_, Larissa)
    _AST_REG_BODY(neptune_, Proteus)
    
    // 冥王星卫星
    _AST_REG_BODY(pluto_, Charon)
    
    // 其他重要天体
    _AST_REG_BODY(this, MercuryBarycenter)
    _AST_REG_BODY(this, VenusBarycenter)
    _AST_REG_BODY(this, MarsBarycenter)
    _AST_REG_BODY(this, JupiterBarycenter)
    _AST_REG_BODY(this, SaturnBarycenter)
    _AST_REG_BODY(this, UranusBarycenter)
    _AST_REG_BODY(this, NeptuneBarycenter)
    _AST_REG_BODY(this, PlutoBarycenter)
    
}



CelestialBody *SolarSystem::getBody(StringView name) const
{
    std::string nameStr(name);
    auto it = nameMap_.find(nameStr);
    if (it != nameMap_.end())
    {
        return it->second;
    }else{
        for(auto& it : bodies_){
            if(name == it->name()){
                nameMap_[nameStr] = it;
                return it;
            }
        }
    }
    return nullptr;
}

CelestialBody *SolarSystem::getBodyByJplIndex(int index) const
{
    auto it = jplIndexMap_.find(index);
    if (it != jplIndexMap_.end())
    {
        return it->second;
    }else{
        if(index < 0)
            return nullptr;
        for(auto& it : bodies_){
            if(it->jplIndex_ == index){
                jplIndexMap_[index] = it;
                return it;
            }
        }
    }
    return nullptr;
}

CelestialBody *SolarSystem::getBodyBySpiceId(int id) const
{
    auto it = spiceIdMap_.find(id);
    if (it != spiceIdMap_.end())
    {
        return it->second;
    }else{
        for(auto& it : bodies_){
            if(it->jplSpiceId_ == id){
                spiceIdMap_[id] = it;
                return it;
            }
        }
    }
    return nullptr;
}

CelestialBody *SolarSystem::addBody(StringView name)
{
    if (getBody(name))
        return nullptr;
    CelestialBody *body = new CelestialBody(this);
    body->setName(name);
    bodies_.push_back(body);
    nameMap_[std::string(name)] = body;
    return body;
}

CelestialBody *SolarSystem::addBody(HCelestialBody body)
{
    if (getBody(body->name()))
        return nullptr;
    bodies_.push_back(body);
    body->solarSystem_ = this;
    nameMap_[body->name()] = body;
    return body;
}

CelestialBody *SolarSystem::getOrAddBody(StringView name)
{
    CelestialBody *body = getBody(name);
    if (!body)
    {
        body = new CelestialBody(this);
        body->setName(name);
        bodies_.push_back(body);
        nameMap_[std::string(name)] = body;    
    }
    return body;
}

AST_NAMESPACE_END