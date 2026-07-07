///
/// @file      SolarSystemLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-07
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

#include "SolarSystemLoader.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/PCKParser.hpp"
#include "AstCore/CelestialBodyLoader.hpp"

AST_NAMESPACE_BEGIN

errc_t SolarSystem::load(StringView dirpath)
{
    initPredefined();
    errc_t rc = 0;
    fs::path path = std::string(dirpath);
    fs::file_status status = fs::status(path);
    if(fs::is_regular_file(status)){
        return loadPCK(dirpath);
    }else if(fs::is_directory(status)){
        dirpath_ = path.string();
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_directory(entry.status())) {
                std::string bodyname = entry.path().filename();
                CelestialBody* body = getBody(bodyname);
                if(body){
                    errc_t rc1 = body->load(entry.path().string());
                    if(rc1){
                        rc = rc1;
                        aError("failed to load body %s", bodyname.c_str());
                    }
                }else{
                    HBody newbody = new CelestialBody();
                    if(newbody->load(entry.path().string()) == 0)
                    {
                        addBody(newbody);
                    }
                }
            }
        }
        return rc;
    }else{
        aError("invalid file type: %s", path.string().c_str());
        return eErrorInvalidFile;
    }
}


errc_t SolarSystem::loadPCK(StringView filepath)
{
    PCKParser parser(filepath);
    if(!parser.isOpen()){
        return eErrorInvalidFile;
    }
    BKVItemView item;
    while(1)
    {
        errc_t rc = parser.getNext(item);
        if(rc != 0){
            break;
        }
        printf("key: %.*s\n", (int)item.key().size(), item.key().data());
    }
    return -1;
}

AST_NAMESPACE_END
