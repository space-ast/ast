///
/// @file      EOP.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-07
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

#include "EOP.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/BKVParser.hpp"

AST_NAMESPACE_BEGIN

static err_t loadEOP(BKVParser& parser, int numlines, std::vector<EOP::Entry>& data)
{
    if(numlines<=0){
        return eErrorInvalidParam;
    }
    size_t reserved = numlines + numlines / 550;
    data.reserve(reserved + data.size());
    for(size_t i=0;i<numlines;i++){
        StringView line = parser.getLine();
        if(line.empty()){
            return eErrorParse;
        }
        int y,m,d;
        EOP::Entry entry{};
        int status = sscanf(
            line.data(), 
            "%d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %d", 
            &y, &m, &d, &entry.mjd,
            &entry.x, &entry.y, 
            &entry.ut1_utc, &entry.lod, 
            &entry.dpsi, &entry.deps,
            &entry.dx, &entry.dy,
            &entry.dat
        );
        if(status!=13){
            return eErrorParse;
        }
        if(!data.empty()){
            double last_dat = data.back().dat;
            if(last_dat != entry.dat){
                auto dump_entry = entry;
                dump_entry.ut1_utc = entry.ut1_utc - (entry.dat - last_dat);
                data.push_back(dump_entry);
            }
        }
        data.push_back(entry);
    }
    return eNoError;
}

err_t EOP::load(StringView filepath)
{
    return load(filepath, m_data);
}

err_t EOP::load(StringView filepath, std::vector<Entry>& data)
{
    BKVParser parser(filepath);

    if (!parser.isOpen())
    {
        return eErrorInvalidFile; // 文件打开失败
    }

    BKVItemView item;
    BKVParser::EToken token;
    int num_observed_points = 0;
    int num_predicted_points = 0;
    std::vector<EOP::Entry> datalist;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "NUM_OBSERVED_POINTS")){
                printf("EOP num_observed_points: %s\n", item.value().data());
                num_observed_points = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "NUM_PREDICTED_POINTS")){
                printf("EOP num_predicted_points: %s\n", item.value().data());
                num_predicted_points = item.value().toInt();
            }
            if(aEqualsIgnoreCase(item.key(), "VERSION")){
                printf("EOP version: %s\n", item.value().data());
            }else if(aEqualsIgnoreCase(item.key(), "UPDATED")){
                printf("EOP updated: %s\n", item.value().data());
            }
            // 键值对项
            // printf("key: %s, value: %s\n", item.key().data(), item.value().data());
        }else if(token == BKVParser::eBegin){
            // 块开始
            printf("begin: %s\n", item.value().data());
            if(aEqualsIgnoreCase(item.value(), "OBSERVED")){
                err_t err = loadEOP(parser, num_observed_points, datalist);
                if(err!=eNoError){
                    return err;
                }
            }else if(aEqualsIgnoreCase(item.value(), "OBSERVED_POINT")){
                err_t err = loadEOP(parser, num_predicted_points, datalist);
                if(err!=eNoError){
                    return err;
                }
            }
        }else if(token == BKVParser::eEnd){
            // 块结束
            printf("end: %s\n", item.value().data());
        }
    }while(token != BKVParser::eError);

    data = std::move(datalist);
    return eNoError;
}

AST_NAMESPACE_END
