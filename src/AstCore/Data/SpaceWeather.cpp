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
#include "AstUtil/Literals.hpp"
#include "AstCore/Date.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/JulianDate.hpp"

#define _AST_USE_FLUX_INTERPOLATION // 是否对Flux进行插值

AST_NAMESPACE_BEGIN

/// @brief 加载空间天气数据
/// @details 从BKVParser中加载空间天气数据，根据numPoints指定加载的条目数。
/// @param parser BKVParser实例，用于解析空间天气数据文件。
/// @param numPoints 要加载的条目数。
/// @param data 用于存储加载数据的Entry向量。
/// @return errc_t 加载成功返回eNoError，否则返回相应错误码。
errc_t loadSpaceWeather(BKVParser& parser, int numPoints, std::vector<SpaceWeather::Entry>& data)
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
        Date date{};
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
        aParseInt(line.substr(89, 3), entry.ISN);
        entry.F10p7Adj      = aParseDouble(line.substr(93, 5));
        errc_t rc = aParseInt(line.substr(99, 1), entry.fluxQualifier);
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

errc_t SpaceWeather::load(StringView filepath)
{
    std::vector<Entry> data;
    if(errc_t rc = load(filepath, data))
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

errc_t SpaceWeather::setEntry(int mjd, const Entry &entry)
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

errc_t SpaceWeather::load(StringView filepath, std::vector<Entry> &data)
{
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        aError("failed to open file %.*s", (int)filepath.size(), filepath.data());
        return eErrorInvalidFile;
    }
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
                if(errc_t rc = loadSpaceWeather(parser, numObservedPoints, data))
                {
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "DAILY_PREDICTED"))
            {
                if(errc_t rc = loadSpaceWeather(parser, numDailyPredictedPoints, data))
                {
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "MONTHLY_PREDICTED"))
            {
                if(errc_t rc = loadSpaceWeather(parser, numMonthlyPredictedPoints, data))
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
        index = (int)data_.size() - 1;
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
        index = (int)data_.size() - 1;
        frac = 0;
        return;
    }
}

// ============================================================
// 查询接口实现
// ============================================================

double SpaceWeather::getApDaily(const TimePoint& tp) const
{
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);
    return getApDaily_UTCMJD(aJDToMJD_Imprecise(jdUTC));
}

double SpaceWeather::getApDaily_UTCMJD(double mjdUTC) const
{
    if(data_.empty()){
        return 0.0;
    }
    int index;
    double frac;
    findEntryIndex(mjdUTC, index, frac);
    if(index < 0){
        return 0.0;
    }

    double val0 = (double)data_[index].ApAvg;
    return val0;

    // if(frac <= 0.0 || index + 1 >= (int)data_.size()){
    //     return val0;
    // }
    // 
    // double val1 = (double)data_[index + 1].ApAvg;
    // return val0 + (val1 - val0) * frac;
}

double SpaceWeather::getKpDaily(const TimePoint& tp) const
{
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);
    return getKpDaily_UTCMJD(aJDToMJD_Imprecise(jdUTC));
}

double SpaceWeather::getKpDaily_UTCMJD(double mjdUTC) const
{
    if(data_.empty()){
        return 0.0;
    }
    int index;
    double frac;
    findEntryIndex(mjdUTC, index, frac);
    if(index < 0){
        return 0.0;
    }

    // KpSum是8个Kp值的和(Kp*10单位)，日平均 = KpSum / 8 / 10 = KpSum / 80
    double val0 = data_[index].KpSum / 80.0;
    return val0;
    // if(frac <= 0.0 || index + 1 >= (int)data_.size()){
    //     return val0;
    // }
    // 
    // double val1 = data_[index + 1].KpSum / 80.0;
    // return val0 + (val1 - val0) * frac;
}

double SpaceWeather::getF10p7Daily(const TimePoint& tp) const
{
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);
    return getF10p7Daily_UTCMJD(aJDToMJD_Imprecise(jdUTC));
}

double SpaceWeather::getF10p7Daily_UTCMJD(double mjdUTC) const
{
    if(data_.empty()){
        return 0.0;
    }
    int index;
    double frac;
    findFluxIndex(mjdUTC, index, frac);
    
    if(index < 0){
        return 0.0;
    }

    // 优先使用F10.7观测值；若不可用则回退到调整值
    double val0 = data_[index].F10p7Obs;
    if(val0 <= 0.0){
        val0 = data_[index].F10p7Adj;
    }
    #ifndef _AST_USE_FLUX_INTERPOLATION
    return val0;
    #else
    if(frac <= 0.0 || index + 1 >= (int)data_.size()){
        return val0;
    }

    double val1 = data_[index + 1].F10p7Obs;
    if(val1 <= 0.0){
        val1 = data_[index + 1].F10p7Adj;
    }

    // 线性插值
    return val0 + (val1 - val0) * frac;
    #endif
}

double SpaceWeather::getF10p7Average(const TimePoint& tp) const
{
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);
    return getF10p7Average_UTCMJD(aJDToMJD_Imprecise(jdUTC));
}

double SpaceWeather::getF10p7Average_UTCMJD(double mjdUTC) const
{
    if(data_.empty()){
        return 0.0;
    }
    int index;
    double frac;
    findFluxIndex(mjdUTC, index, frac);
    if(index < 0){
        return 0.0;
    }

    // 使用以当天为中心的81天平均值
    double val0 = data_[index].F10p7ObsCtr81;
    if(val0 <= 0.0){
        // 回退：使用最近81天平均值
        val0 = data_[index].F10p7ObsLst81;
    }
    #ifndef _AST_USE_FLUX_INTERPOLATION
    return val0;
    #else

    if(frac <= 0.0 || index + 1 >= (int)data_.size()){
        return val0;
    }
    double val1 = data_[index + 1].F10p7ObsCtr81;
    if(val1 <= 0.0){
        val1 = data_[index + 1].F10p7ObsLst81;
    }
    
    // 线性插值
    return val0 + (val1 - val0) * frac;
    #endif
}

void SpaceWeather::findFluxIndex(double mjdUTC, int & index, double & frac) const
{
    /*
     * F10.7 观测时间偏移
     *
     * F10.7每天只测量一次，测量时刻为 UTC 20:00（1991-05-31 之前为 17:00 UTC，对应观测站搬迁）。
     *
     * @see https://celestrak.org/SpaceData/SpaceWx-format.php
     */
    constexpr double kF107RefMJD = 18408.0;  ///< 1991-05-31, 观测站搬迁日
    double f107Offset = (mjdUTC < kF107RefMJD) ? 17.0/24.0: 20.0/24.0;

    double queryMJD = mjdUTC - f107Offset;

    findEntryIndex(queryMJD, index, frac);
}

AST_NAMESPACE_END
