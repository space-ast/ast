///
/// @file      aep8.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-18
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

#include "aep8.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/RunTime.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/ParseFormat.hpp"
#include <array>
#include <cstdio>
#include <memory>
#include <cmath>

#ifndef AST_NO_LIBF2C
#include "f2c.h"

extern "C"{
    int trara1_(integer *descr, integer *map, doublereal *fl, 
	doublereal *bb0, doublereal *e, doublereal *f, integer *n);
}

#endif

AST_NAMESPACE_BEGIN

#define AST_DEFAULT_DIR_AEP8                    "SolarSystem/Earth/aep8/"


/// @brief     AEP8数据
class AEP8Data{
public:
    ~AEP8Data();
    err_t load(StringView filepath);
public:
    std::array<long, 8> header_;
    long* data_{nullptr};
};

/// @brief     AEP8数据集合
class AEPDataCollection{
public:
    AEPDataCollection();
    err_t loadDefault();
    err_t load(StringView dirpath);
public:
    AEP8Data ae8max_;
    AEP8Data ae8min_;
    AEP8Data ap8min_;
    AEP8Data ap8max_;
};

typedef std::array<AEP8Data, 4> AEP8DataArray;

AEP8Data::~AEP8Data()
{
    if(data_)
        delete[] data_;
    data_ = nullptr;
}

err_t AEP8Data::load(StringView filepath)
{
    if(data_)
        delete[] data_;
    data_ = nullptr;

    ScopedPtr<FILE> fp{fopen(filepath.data(), "rb")};
    if (!fp)
    {
        return eErrorInvalidFile;
    }
    char linebuf[1024];
    char* line = fgets(linebuf, sizeof(linebuf), fp.get());
    if(!line)
        return eErrorParse;
    // 读取头信息
    int ret = sscanf(line, "%ld %ld %ld %ld %ld %ld %ld %ld", 
        &header_[0], &header_[1], &header_[2], &header_[3],
        &header_[4], &header_[5], &header_[6], &header_[7]);
    if(ret != 8)
        return eErrorParse;
    // 读取数据
    int n = header_[7];
    std::unique_ptr<long[]> data(new long[n]);
    int nline = n / 12;
    int remain = n % 12;
    
    for(int i = 0; i < nline; i++)
    {
        line = fgets(linebuf, sizeof(linebuf), fp.get());
        // printf("line=%s", line);
        if(!line)
            return eErrorParse;
        StringView sv(linebuf);
        // FORMAT(1X,12I6)
        for(int j=0;j<12;j++){
            int value = 0;
            StringView sub = sv.substr(j*6 + 1, 6);
            err_t rc = aParseInt(sub, value);
            data[i*12+j] = value;
            if(A_UNLIKELY(rc != eNoError)){
                aError("failed to parse int value from %.*s", sub.size(), sub.data());
                return eErrorParse;
            }
        }
    }
    if(remain > 0)
    {
        char* line = fgets(linebuf, sizeof(linebuf), fp.get());
        if(!line)
            return eErrorParse;
        StringView sv(linebuf);
        for(int j=0;j<remain;j++){
            int value = 0;
            err_t rc = aParseInt(sv.substr(j*6 + 1, 6), value);
            data[nline*12+j] = value;
            if(A_UNLIKELY(rc != eNoError)){
                aError("failed to parse int value at line %d, column %d", nline, j);
                return eErrorParse;
            }
        }
    }
    this->data_ = data.release();
    return eNoError;
}

AEPDataCollection::AEPDataCollection()
{
    err_t rc = loadDefault();
    if(rc)
    {
        aError("failed to load default AEP8 data collection");   
    }
}

err_t AEPDataCollection::loadDefault()
{
    fs::path datadir = aDataDir();
    datadir /= AST_DEFAULT_DIR_AEP8;
    return load(datadir.string());
}

err_t AEPDataCollection::load(StringView dirpath)
{
    err_t rc;
    rc = ae8max_.load(dirpath.to_string() + "/ae8min.asc");
    if(rc != eNoError)
        return rc;
    rc = ae8min_.load(dirpath.to_string() + "/ae8max.asc");
    if(rc != eNoError)
        return rc;
    rc = ap8min_.load(dirpath.to_string() + "/ap8min.asc");
    if(rc != eNoError)
        return rc;
    rc = ap8max_.load(dirpath.to_string() + "/ap8max.asc");
    if(rc != eNoError)
        return rc;
    return eNoError;
}


static AEPDataCollection& aep8DataCollection()
{
    static AEPDataCollection collection;
    return collection;
}

A_ALWAYS_INLINE AEP8DataArray& aep8DataArray(){
    static_assert(sizeof(AEP8DataArray) == sizeof(AEP8Data) * 4, "AEP8DataArray size error");
    return (AEP8DataArray&)(aep8DataCollection());
}

double aep8(double e, double l, double bb0, int iname)
{
#ifndef AST_NO_LIBF2C
    int index = iname - 1;
    if(index < 0 || index >= 4)
    {
        aError("invalid iname %d", iname);
        return 0;
    }
    AEP8DataArray& data = aep8DataArray();
    AEP8Data& d = data[index];
    long* map = d.data_;
    if(!map)
    {
        aError("map data is null");
        return 0;
    }
    long* header = d.header_.data();
    double flux = 0.0;
    long n = 1;
    trara1_(header, map, &l, &bb0, &e, &flux, &n);
    if(flux > 0.0)
        flux = std::pow(10.0, flux);
    return flux;
#else
    aError("function 'aep8' is not implemented, please check whether libf2c package is enabled.");
#endif

}


AST_NAMESPACE_END

