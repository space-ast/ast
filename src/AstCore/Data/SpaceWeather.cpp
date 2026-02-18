///
/// @file      SpaceWeather.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-16
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

#include "SpaceWeather.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/Date.hpp"

AST_NAMESPACE_BEGIN

/// @brief 加载空间天气数据
/// @details 从BKVParser中加载空间天气数据，根据numPoints指定加载的条目数。
/// @param parser BKVParser实例，用于解析空间天气数据文件。
/// @param numPoints 要加载的条目数。
/// @param data 用于存储加载数据的Entry向量。
/// @return err_t 加载成功返回eNoError，否则返回相应错误码。
err_t loadSpaceWeather(BKVParser& parser, int numPoints, std::vector<SpaceWeather::Entry>& data)
{
    if(numPoints<=0){
        return eErrorInvalidParam;
    }
    data.reserve(data.size() + numPoints);
    for(int i=0; i<numPoints; i++)
    {
        StringView line = parser.getLine();
        if(line.empty()){
            return eErrorParse;
        }
        /*
         * 格式解析规则参见：https://celestrak.org/SpaceData/SpaceWx-format.php
         */
        
        SpaceWeather::Entry entry{};
        Date date;
        date.year()  = aParseInt(line.substr(0, 4));
        date.month() = aParseInt(line.substr(5, 2));
        date.day()   = aParseInt(line.substr(8, 2));
        entry.mjd = aDateToMJD(date);
        entry.BSRN = aParseInt(line.substr(11, 4));
        entry.ND = aParseInt(line.substr(16, 2));

        aParseInt(line.substr(19, 2), entry.Kp1);
        aParseInt(line.substr(22, 2), entry.Kp2);
        aParseInt(line.substr(25, 2), entry.Kp3);
        aParseInt(line.substr(28, 2), entry.Kp4);
        aParseInt(line.substr(31, 2), entry.Kp5);
        aParseInt(line.substr(34, 2), entry.Kp6);
        aParseInt(line.substr(37, 2), entry.Kp7);
        aParseInt(line.substr(40, 2), entry.Kp8);
        aParseInt(line.substr(43, 3), entry.KpSum);
        aParseInt(line.substr(47, 3), entry.Ap1);
        aParseInt(line.substr(51, 3), entry.Ap2);
        aParseInt(line.substr(55, 3), entry.Ap3);
        aParseInt(line.substr(59, 3), entry.Ap4);
        aParseInt(line.substr(63, 3), entry.Ap5);
        aParseInt(line.substr(67, 3), entry.Ap6);
        aParseInt(line.substr(71, 3), entry.Ap7);
        aParseInt(line.substr(75, 3), entry.Ap8);
        aParseInt(line.substr(79, 3), entry.ApAvg);
        aParseDouble(line.substr(83, 3), entry.Cp);
        aParseInt(line.substr(87, 1), entry.C9);
        entry.ISN = aParseInt(line.substr(89, 3));
        entry.F10p7Adj      = aParseDouble(line.substr(93, 5));
        err_t rc = aParseInt(line.substr(99, 1), entry.fluxQualifier);
        if(rc)
        {
            entry.fluxQualifier = 3; // @fixme: 应该是3吗？
        }
        entry.F10p7AdjCtr81 = aParseDouble(line.substr(101,5));
        entry.F10p7AdjLst81 = aParseDouble(line.substr(107, 5));
        entry.F10p7Obs      = aParseDouble(line.substr(113, 5));
        entry.F10p7ObsCtr81 = aParseDouble(line.substr(119, 5));
        entry.F10p7ObsLst81 = aParseDouble(line.substr(125, 5));

        data.push_back(entry);
    }
    return eNoError;
}

err_t SpaceWeather::load(StringView filePath)
{
    std::vector<Entry> data;
    if(err_t rc = load(filePath, data))
    {
        return rc;
    }
    this->data_ = std::move(data);
    if(!data_.empty()){
        this->startMJD_ = data_.front().mjd;
        this->endMJD_ = data_.back().mjd;
    }else{
        this->startMJD_ = 0;
        this->endMJD_ = 0;
    }

    return eNoError;
}

const SpaceWeather::Entry *SpaceWeather::getEntry(int mjd) const
{
    int index;
    double frac;
    findEntryIndex(mjd, index, frac);
    if(index < 0 || index >= (int)data_.size())
    {
        return nullptr;
    }
    return &data_[index];
}

err_t SpaceWeather::setEntry(int mjd, const Entry &entry)
{
    int index;
    double frac;
    findEntryIndex(mjd, index, frac);
    if(index < 0 || index >= (int)data_.size())
    {
        return eErrorInvalidParam;
    }
    data_[index] = entry;
    return eNoError;
}

err_t SpaceWeather::load(StringView filePath, std::vector<Entry> &data)
{
    BKVParser parser(filePath);
    if(!parser.isOpen())
        return eErrorInvalidFile;
    BKVParser::EToken token;
    BKVItemView item;
    int numObservedPoints = 0;
    int numDailyPredictedPoints = 0;
    int numMonthlyPredictedPoints = 0;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin)
        {
            if(aEqualsIgnoreCase(item.value(), "OBSERVED"))
            {
                if(err_t rc = loadSpaceWeather(parser, numObservedPoints, data))
                {
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "DAILY_PREDICTED"))
            {
                if(err_t rc = loadSpaceWeather(parser, numDailyPredictedPoints, data))
                {
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "MONTHLY_PREDICTED"))
            {
                if(err_t rc = loadSpaceWeather(parser, numMonthlyPredictedPoints, data))
                {
                    return rc;
                }
            }

        }else if(token == BKVParser::eBlockEnd)
        {
            // pass
        }else if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "DATATYPE"))
            {
                if(!aEqualsIgnoreCase(item.value(), "CssiSpaceWeather"))
                {
                    aError("invalid DATATYPE, expect CssiSpaceWeather");
                    return eErrorInvalidFile;
                }
            }else if(aEqualsIgnoreCase(item.key(), "VERSION"))
            {
                // pass
            }else if(aEqualsIgnoreCase(item.key(), "UPDATED"))
            {
                // pass
            }else if(aEqualsIgnoreCase(item.key(), "NUM_OBSERVED_POINTS"))
            {
                numObservedPoints = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "NUM_DAILY_PREDICTED_POINTS"))
            {
                numDailyPredictedPoints = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "NUM_MONTHLY_PREDICTED_POINTS"))
            {
                numMonthlyPredictedPoints = item.value().toInt();
            }
        }

    }while(token != BKVParser::EToken::eEOF);
    return eNoError;
}

void SpaceWeather::findEntryIndex(double mjdUTC, int & index, double & frac) const
{
    // @todo: 与EOP.cpp中的实现是一致的，考虑怎么抽取公共代码

    // 猜测索引
    index = (int)(mjdUTC - startMJD_);
    if(index >= (int)data_.size()){
        index = data_.size() - 1;
        frac = 0;
    }
    if(index < 0){
        index = -1;
        frac = 0;
        return;
    }
    if(mjdUTC < this->data_[index].mjd){
        for(int i=index-1;i >=0;i--){
            if(mjdUTC >= this->data_[i].mjd){
                index = i;
                frac = (mjdUTC - data_[index].mjd) / (data_[index+1].mjd - data_[index].mjd);
                return;
            }
        }
        index = -1;
        frac = 0;
        return;
    }else{
        for(int i=index+1;i < (int)data_.size();i++){
            if(mjdUTC < this->data_[i].mjd){
                index = i-1;
                frac = (mjdUTC - data_[index].mjd) / (data_[index+1].mjd - data_[index].mjd);
                return;
            }
        }
        index = data_.size() - 1;
        frac = 0;
        return;
    }
}

AST_NAMESPACE_END
