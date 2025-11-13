#pragma once
#include "AstTypesForward.h"
#include "AstCore/AstCoreExport.h"

AST_NAMESPACE_BEGIN

class CelestialBody;
class SolarSystem;

AST_CORE_CAPI CelestialBody* aGetEarth(SolarSystem* ss);
AST_CORE_CAPI CelestialBody* aGetMoon(SolarSystem* ss);
AST_CORE_CAPI CelestialBody* aGetMars(SolarSystem* ss);


/// @brief 太阳系
class AST_CORE_API SolarSystem
{
public:
	SolarSystem();
	CelestialBody* getEarth();
	CelestialBody* getMoon();
	CelestialBody* getMars();
};


AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(SolarSystem)
