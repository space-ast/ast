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
#include "AstUtil/StringView.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"

AST_NAMESPACE_BEGIN

err_t CelestialBody::load(StringView filepath)
{
    fs::path path = filepath.to_string();
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
            }
        }else if(token == BKVParser::eBlockEnd){

        }
    }while(token != BKVParser::eEOF);

    return eNoError;
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
                filepath = filepath.parent_path() / model;
                err_t rc = this->gravityField_.load(filepath.string());
                if(rc) return rc;
            }
            else if(aEqualsIgnoreCase(item.key(), "Gm")){
                gm_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "SystemGM")){
                systemGm_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "ShapeName")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "Shape")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "MajorAxis")){
                // todo
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
                // todo
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

AST_NAMESPACE_END

