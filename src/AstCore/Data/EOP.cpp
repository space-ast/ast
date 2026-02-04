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
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/JulianDate.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

static err_t loadEOP(BKVParser& parser, int numlines, std::vector<EOP::Entry>& data)
{
    if(numlines<=0){
        return eErrorInvalidParam;
    }
    size_t reserved = numlines + numlines / 550;
    data.reserve(reserved + data.size());
    for(int i=0;i<numlines;i++){
        StringView line = parser.getLine();
        if(line.empty()){
            return eErrorParse;
        }
        int y,m,d;
        EOP::Entry entry{};
        // #pragma warning(suppress: 4996)
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
        entry.x *= kArcSecToRad;
        entry.y *= kArcSecToRad;
        entry.dpsi *= kArcSecToRad;
        entry.deps *= kArcSecToRad;
        entry.dx *= kArcSecToRad;
        entry.dy *= kArcSecToRad;

        if(status!=13){
            aError("parse line %d failed, status=%d", parser.getLineNumber(), status);
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
    err_t err = load(filepath, m_data);
    if(!m_data.empty()){
        m_startMJD = m_data.front().mjd;
        m_endMJD = m_data.back().mjd;
    }
    return err;
}

void EOP::unload()
{
    m_data.clear();
    m_startMJD = 0.0;
    m_endMJD = 0.0;
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
                num_observed_points = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "NUM_PREDICTED_POINTS")){
                num_predicted_points = item.value().toInt();
            }
            // if(aEqualsIgnoreCase(item.key(), "VERSION")){
            // 
            // }else if(aEqualsIgnoreCase(item.key(), "UPDATED")){
            // 
            // }
        }else if(token == BKVParser::eBlockBegin){
            // 块开始
            if(aEqualsIgnoreCase(item.value(), "OBSERVED")){
                err_t err = loadEOP(parser, num_observed_points, datalist);
                if(err!=eNoError){
                    return err;
                }
            }else if(aEqualsIgnoreCase(item.value(), "PREDICTED")){
                err_t err = loadEOP(parser, num_predicted_points, datalist);
                if(err!=eNoError){
                    return err;
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            // 块结束
        }
    }while(token != BKVParser::eEOF);

    data = std::move(datalist);
    return eNoError;
}

const EOP::Entry *EOP::getEntry(int mjd) const
{
    size_t index = 0;
    double frac = 0.0;
    findEntryIndex(mjd, index, frac);
    if(index < 0 || index >= m_data.size() - 1){
        return nullptr;
    }
    return &m_data[index];
}

err_t EOP::setEntry(int mjd, const Entry &entry)
{
    size_t index = 0;
    double frac = 0.0;
    findEntryIndex(mjd, index, frac);
    if(index < 0 || index >= m_data.size() - 1){
        return eErrorInvalidParam;
    }
    m_data[index] = entry;
    return eNoError;
}

double EOP::getUT1MinusUTC(const TimePoint &tp) const
{
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);
    return getUT1MinusUTC_UTC(jdUTC);
}

double EOP::getUT1MinusUTC_UTC(const JulianDate &jdUTC) const
{
    double mjd = aJDToMJD_Imprecise(jdUTC);
    return getUT1MinusUTC_UTCMJD(mjd);
}

double EOP::getUT1MinusUTC_UTCMJD(double mjdUTC) const
{
    size_t index = 0;
    double frac = 0.0;
    findEntryIndex(mjdUTC, index, frac);
    if(index < 0){
        return 0.0;
    }
    if(index >= m_data.size() - 1){
        return 0.0; 
    }
    return getValue<&Entry::ut1_utc>(index, frac);
}

void EOP::getPoleMotion(const TimePoint &tp, double &x, double &y) const
{
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);
    return getPoleMotionUTC(jdUTC, x, y);
}

void EOP::getPoleMotionUTC(const JulianDate &jdUTC, double &x, double &y) const
{
    double mjdUTC = aJDToMJD_Imprecise(jdUTC);
    return getPoleMotionUTCMJD(mjdUTC, x, y);
}

void EOP::getPoleMotionUTCMJD(double mjdUTC, double &x, double &y) const
{
    size_t index = 0;
    double frac = 0.0;
    findEntryIndex(mjdUTC, index, frac);
    if(index < 0 || index >= m_data.size() - 1){
        x = 0.0;
        y = 0.0;
        return;
    }
    x = getValue<&Entry::x>(index, frac);
    y = getValue<&Entry::y>(index, frac);
}

double EOP::getLOD(const TimePoint &tp) const
{
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);
    return getLOD_UTC(jdUTC);
}

double EOP::getLOD_UTC(const JulianDate &jdUTC) const
{
    double mjdUTC = aJDToMJD_Imprecise(jdUTC);
    return getLOD_UTCMJD(mjdUTC);
}

double EOP::getLOD_UTCMJD(double mjdUTC) const
{
    size_t index = 0;
    double frac = 0.0;
    findEntryIndex(mjdUTC, index, frac);
    if(index < 0 || index >= m_data.size() - 1){
        return 0.0;
    }
    return getValue<&Entry::lod>(index, frac);
}

void EOP::getXYCorrection(const TimePoint &tp, array2d &xyCorrection) const
{
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);
    return getXYCorrectionUTC(jdUTC, xyCorrection);
}

void EOP::getXYCorrectionUTC(const JulianDate &jdUTC, array2d &xyCorrection) const
{
    double mjdUTC = aJDToMJD_Imprecise(jdUTC);
    return getXYCorrectionUTCMJD(mjdUTC, xyCorrection);
}

void EOP::getXYCorrectionUTCMJD(double mjdUTC, array2d &xyCorrection) const
{
    size_t index = 0;
    double frac = 0.0;
    findEntryIndex(mjdUTC, index, frac);
    if(index < 0 || index >= m_data.size() - 1){
        xyCorrection = {0, 0};
        return;
    }
    xyCorrection[0] = getValue<&Entry::dx>(index, frac);
    xyCorrection[1] = getValue<&Entry::dy>(index, frac);
}

void EOP::findEntryIndex(double mjdUTC, size_t &index, double &frac) const
{
    // 猜测索引
    index = (size_t)(mjdUTC - m_startMJD);
    if(index < 0){
        index = -1;
        frac = 0;
        return;
    }
    if(index >= m_data.size()){
        index = m_data.size() - 1;
        frac = 0;
        return;
    }
    if(mjdUTC < this->m_data[index].mjd){
        for(size_t i=index-1;i >=0;i--){
            if(mjdUTC >= this->m_data[i].mjd){
                index = i;
                frac = (mjdUTC - m_data[index].mjd) / (m_data[index+1].mjd - m_data[index].mjd);
                return;
            }
        }
        index = -1;
        frac = 0;
        return;
    }else{
        for(size_t i=index+1;i < m_data.size();i++){
            if(mjdUTC < this->m_data[i].mjd){
                index = i-1;
                frac = (mjdUTC - m_data[index].mjd) / (m_data[index+1].mjd - m_data[index].mjd);
                return;
            }
        }
        index = m_data.size() - 1;
        frac = 0;
        return;
    }
}

AST_NAMESPACE_END
