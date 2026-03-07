///
/// @file      CelestialBody.cpp
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

#include "CelestialBody.hpp"
#include "SolarSystem.hpp"
#include "AstCore/NoopOrientation.hpp"
#include "AstCore/RotationalData.hpp"
#include "AstCore/EarthOrientation.hpp"
#include "AstCore/MoonOrientation.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"



AST_NAMESPACE_BEGIN

CelestialBody::CelestialBody()
{
    orientation_  = new NoopOrientation();
    axesFixed_    = new AxesBodyFixed(this);
    axesInertial_ = new AxesBodyInertial(this);
    axesMOD_      = new AxesBodyMOD(this);
    axesTOD_      = new AxesBodyTOD(this);
}

err_t CelestialBody::load(StringView filepath)
{
    fs::path path = std::string(filepath);
    if(!fs::is_regular_file(path))
    {
        path = path / (path.filename().string() + ".cb");
    }
    BKVParser parser(path.string());
    if(!parser.isOpen())
    {
        aError("failed to open file %s", path.string().c_str());
        return eErrorInvalidFile;
    }
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Name")){
                name_ = item.value().toString();
            }
            else if(aEqualsIgnoreCase(item.key(), "ReadOnly"))
            {
                // todo
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "AstroDefinition")){
                err_t rc = loadAstroDefinition(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "SpinData")){
                err_t rc = loadSpinData(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "EphemerisData")){
                err_t rc = loadEphemerisData(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "Earth")){
                err_t rc = loadEarth(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "Moon")){
                err_t rc = loadMoon(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "MeanEarthDefinition")){
                err_t rc = loadMeanEarthDefinition(parser);
                if(rc) return rc;
            }
        }else if(token == BKVParser::eBlockEnd){

        }
    }while(token != BKVParser::eEOF);

    return eNoError;
}

err_t CelestialBody::setGravityModel(StringView model)
{
    err_t rc = this->loadGravityModel(model);
    if(rc){
        fs::path filepath = fs::path(SolarSystem::defaultSolarSystemDir()) / this->name_ / std::string(model);
        rc = this->loadGravityModel(filepath.string());
        if(rc){
            aError("failed to load gravity model %.*s", (int)model.size(), model.data());
        }
    }
    return rc;
}

err_t CelestialBody::loadGravityModel(StringView model)
{
    return gravityField_.load(model, 6, 6);
}

err_t CelestialBody::loadAstroDefinition(BKVParser &parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "GravityModel")){
                std::string model = item.value().toString();
                fs::path filepath = parser.getFilePath();
                /*!
                @bug
                如果model 被恶意写成 ../../../../etc/passwd，
                拼接后就成了 /home/user/project/configs/../../../../etc/passwd，
                经过路径解析后可能指向 /etc/passwd，从而读取系统关键文件。
                */
                filepath = filepath.parent_path() / model;
                err_t rc = this->loadGravityModel(filepath.string());
                if(rc) return rc;
            }
            else if(aEqualsIgnoreCase(item.key(), "Gm")){
                gm_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "SystemGM")){
                systemGM_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "ShapeName")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "Shape")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "Radius")){
                // todo
                radius_ = item.value().toDouble();
            }
            else if(aEqualsIgnoreCase(item.key(), "MajorAxis")){
                // todo
                radius_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "MiddleAxis")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "MinorAxis")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "MaxAltitude")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "RefDistance")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "MinRadius")){
                // todo 
            }else if(aEqualsIgnoreCase(item.key(), "DTEDSearchScale")){
                // todo 
            }else if(aEqualsIgnoreCase(item.key(), "ParentName")){
                // todo 
            }else if(aEqualsIgnoreCase(item.key(), "PathGenerator")){
                // todo
            }
        }else if(token == BKVParser::eBlockBegin){

        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "AstroDefinition")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

err_t CelestialBody::loadSpinData(BKVParser &parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "RotationDefinitionFile")){
                auto rotData = new RotationalData();
                std::string model = item.value().toString();
                fs::path filepath = parser.getFilePath();
                filepath = filepath.parent_path() / model;
                err_t rc = rotData->load(filepath.string());
                this->orientation_ = rotData;
                if(rc) return rc;
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "SpinData")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

err_t CelestialBody::loadEphemerisData(BKVParser & parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "EphemerisSource")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "JplSpiceId")){
                jplSpiceId_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "JplIndex")){
                jplIndex_ = item.value().toInt();
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "EphemerisData")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

err_t CelestialBody::loadEarth(BKVParser &parser)
{
    this->orientation_ = new EarthOrientation();

    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "UseFK5IAU76Theory")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "ICRFTheory")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "ICRF_XYS_Algorithm")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "NutationUpdateInterval")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "ApplyPoleWander")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "OceanTideFilename")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "IAU1980NUTATIONMETHOD")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "UseUpdatedEquationOfEquinox")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "UtmReferenceEllipsoid")){
                // todo
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "Earth")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

err_t CelestialBody::loadMoon(BKVParser &parser)
{
    this->orientation_ = new MoonOrientation();

    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "FixedFrame")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "FixedFrameForGravity")){
                // todo
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "Moon")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

err_t CelestialBody::loadMeanEarthDefinition(BKVParser &parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "DeNum")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "XAngle")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "YAngle")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "ZAngle")){
                // todo
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "MeanEarthDefinition")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END

