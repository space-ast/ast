///
/// @file      CelestialBodyLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-06
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

#include "CelestialBodyLoader.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstCore/EarthOrientation.hpp"
#include "AstCore/MoonOrientation.hpp"
#include "AstCore/RotationalData.hpp"
#include "AstCore/BodyEphemerisDE.hpp"
#include "AstCore/BodyEphemerisSPK.hpp"
#include "AstCore/SphereShape.hpp"
#include "AstCore/SpheroidShape.hpp"
#include "AstCore/EllipsoidShape.hpp"

AST_NAMESPACE_BEGIN



errc_t CelestialBody::load(StringView filepath)
{
    fs::path path = std::string(filepath);
    if(!fs::is_regular_file(path))
    {
        path = path / (path.filename().string() + ".cb");
        if(!fs::is_regular_file(path))
        {
            return eErrorInvalidFile;
        }
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
                this->setName(item.value().toString());
            }
            else if(aEqualsIgnoreCase(item.key(), "ReadOnly"))
            {
                // todo
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "AstroDefinition")){
                errc_t rc = loadAstroDefinition(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "SpinData")){
                errc_t rc = loadSpinData(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "EphemerisData")){
                errc_t rc = loadEphemerisData(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "Earth")){
                errc_t rc = loadEarth(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "Moon")){
                errc_t rc = loadMoon(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "MeanEarthDefinition")){
                errc_t rc = loadMeanEarthDefinition(parser);
                if(rc) return rc;
            }
        }else if(token == BKVParser::eBlockEnd){

        }
    }while(token != BKVParser::eEOF);

    return eNoError;
}



errc_t CelestialBody::loadGravityModel(StringView model)
{
    return gravityField_.load(model, 6, 6, getDirpath());
}

errc_t CelestialBody::loadAstroDefinition(BKVParser &parser)
{
    struct{
        std::string shapeName_;
        std::string shape_;
        double radius_;
        double majorAxis_;
        double middleAxis_;
        double minorAxis_;
    } data{};

    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "GravityModel")){
                std::string model = item.value().toString();
                // 防止路径遍历攻击
                // 这里需要进行这样的判断吗？
                if (
                    (!model.empty() && (model[0] == '/' || model[0] == '\\'))
                    || model.find("..") != std::string::npos
                    || model.find(':') != std::string::npos
                )
                {
                    aError("Path traversal detected or absolute path not allowed in gravity model: %s", model.c_str());
                    return eErrorInvalidParam;
                }
                fs::path filepath = parser.getFilePath();
                filepath = filepath.parent_path() / model;
                errc_t rc = this->loadGravityModel(filepath.string());
                if(rc) return rc;
            }
            else if(aEqualsIgnoreCase(item.key(), "Gm")){
                gm_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "SystemGM")){
                systemGM_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "ShapeName")){
                data.shapeName_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "Shape")){
                data.shape_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "Radius")){
                this->radius_ = item.value().toDouble();
                data.radius_ = this->radius_;
            }
            else if(aEqualsIgnoreCase(item.key(), "MajorAxis")){
                this->radius_ = item.value().toDouble();
                data.majorAxis_ = this->radius_;
            }else if(aEqualsIgnoreCase(item.key(), "MiddleAxis")){
                data.middleAxis_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "MinorAxis")){
                data.minorAxis_ = item.value().toDouble();
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
            if(aEqualsIgnoreCase(item.value(), "AstroDefinition"))
            {
                SharedPtr<BodyShape> shape;
                if(data.shape_ == "OblateSpheroid")
                {
                    shape = SpheroidShape::NewFromMajorMinorAxis(data.majorAxis_, data.minorAxis_);
                }
                else if(data.shape_ == "TriaxialEllipsoid")
                {
                    shape = new EllipsoidShape(data.majorAxis_, data.middleAxis_, data.minorAxis_);
                }
                else if(data.shape_ == "Sphere")
                {
                    shape = new SphereShape(data.radius_);
                }
                else{
                    aWarning("unsupported shape type: %s", data.shape_.c_str());
                }

                if(shape)
                {
                    shape->setName(data.shapeName_.empty()? data.shape_ : data.shapeName_);
                    shape->setParentScope(this);
                    this->shape_ = shape;
                }
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t CelestialBody::loadSpinData(BKVParser &parser)
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
                errc_t rc = rotData->load(filepath.string());
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

errc_t CelestialBody::loadEphemerisData(BKVParser & parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "EphemerisSource")){
                if(aEqualsIgnoreCase(item.value(), "JplDe")){
                    ephemeris_ = new BodyEphemerisDE(jplIndex_);
                }else if(aEqualsIgnoreCase(item.value(), "JplSpice")){
                    auto ephemerisSPK = new BodyEphemerisSPK(jplSpiceId_);
                    std::string spkDir = aGetConfigValue("SPK_DIR").toString();
                    if(spkDir.empty())
                        spkDir = aGetDefaultSPKDir();
                    std::string spkFile = spkDir + "/" + aAsciiStrToLower(name()) + ".bsp";
                    if(fs::is_regular_file(spkFile)){
                        errc_t rc = ephemerisSPK->openSPKFile(spkFile);
                        if(rc){
                            aWarning("failed to open SPK file '%s'", spkFile.c_str());
                        }
                    }
                    ephemeris_ = ephemerisSPK;
                }
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

errc_t CelestialBody::loadEarth(BKVParser &parser)
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

errc_t CelestialBody::loadMoon(BKVParser &parser)
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

errc_t CelestialBody::loadMeanEarthDefinition(BKVParser &parser)
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



errc_t aLoadBody(StringView filepath, HCelestialBody& body)
{
    HBody newbody = body;
    if(!newbody)
        newbody = new CelestialBody();
    errc_t rc = newbody->load(filepath);
    if(rc)
        return rc;
    body = newbody;
    return 0;
}


AST_NAMESPACE_END
