/// @file      JplDe.cpp
/// @brief
/// @details   ~
/// @author    axel
/// @date      5.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 

#include "JplDe.hpp"
#include "AstCore/Vector.hpp"
#include "AstCore/TimeSystem.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/JulianDate.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstMath/Euler.hpp"
#include "AstUtil/IO.hpp"           // for ast_fopen
#include "AstUtil/Logger.hpp"       // for aError
#include "AstUtil/StringUtil.hpp"   // for aStripTrailingAsciiWhitespace
#include <assert.h>                 // for assert
#include <mutex>                    // for std::mutex
#include <cmath>                    // for std::sin, std::cos, std::sqrt, etc.
#include <memory>                   // for std::unique_ptr
#include <limits>                   // for std::numeric_limits
 
AST_NAMESPACE_BEGIN
 

/* 
 * De星历二进制数据索引
 * 索引数组ipt的意义， index for de ipt
 */
enum EDeDataCode
{
    eDeMercury = 0,           ///< Mercury            relative to SS BaryCenter
    eDeVenus = 1,             ///< Venus              relative to SS BaryCenter 
    eDeEMBaryCenter = 2,      ///< EMBaryCenter       relative to SS BaryCenter
    eDeMars = 3,              ///< Mars               relative to SS BaryCenter
    eDeJupiter = 4,           ///< Jupiter            relative to SS BaryCenter
    eDeSaturn = 5,            ///< Saturn             relative to SS BaryCenter
    eDeUranus = 6,            ///< Uranus             relative to SS BaryCenter
    eDeNeptune = 7,           ///< Neptune            relative to SS BaryCenter
    eDePluto = 8,             ///< Pluto              relative to SS BaryCenter
    eDeMoon = 9,              ///< Moon               relative to geocenter
    eDeSun = 10,              ///< Sun                relative to SS BaryCenter
    eDeNutation = 11,         ///< longitude and obliquity (IAU 1980 model) 地球黄经章动和交角章动
    eDeLibration = 12,        ///< 月面天平动
    eDeLunarAngVel = 13,      ///< 月面角速度
    eDeTT_TDB = 14            ///< (at geocenter)
};


/*
ipt:
1   0    Mercury 水星
2   1    Venus 金星
3   2    Earth-Moon barycenter 地月系质心
4   3    Mars 火星
5   4    Jupiter 木星
6   5    Saturn 土星
7   6    Uranus 天王星
8   7    Neptune 海王星
9   8    Pluto 冥王星
10  9    Moon (geocentric) 地心坐标系下的月球
11  10   Sun 太阳
12  11   Earth Nutations in longitude and obliquity (IAU 1980 model) 地球经线章动和倾斜
13  12   Lunar mantle libration 月面天平动
14  13   Lunar mantle angular velocity 月面角速度
15  14   TT-TDB (at geocenter)
*/


#define JPL_EPH_OUTSIDE_RANGE                (-1)
#define JPL_EPH_READ_ERROR                   (-2)
#define JPL_EPH_QUANTITY_NOT_IN_EPHEMERIS    (-3)
#define JPL_EPH_INVALID_INDEX                (-5)
#define JPL_EPH_FSEEK_ERROR                  (-6)

#define MAX_CHEBY 18



#define SWAP_MACRO( A, B, TEMP)   { TEMP = A;  A = B;  B = TEMP; }

static unsigned int log2n(unsigned int n)
{
    unsigned int result = 0;
    while (n >>= 1) {
        ++result;
    }
    return result;
}

static void swap_32_bit_val(void* ptr, size_t count=1)
{
    char* tptr = (char*)ptr, tchar;
    while (count--)
    {
        SWAP_MACRO(tptr[0], tptr[3], tchar);
        SWAP_MACRO(tptr[1], tptr[2], tchar);
        tptr += 4;
    }
}

static void swap_64_bit_val(void* ptr, size_t count=1)
{
    char* tptr = (char*)ptr, tchar;

    while (count--)
    {
        SWAP_MACRO(tptr[0], tptr[7], tchar);
        SWAP_MACRO(tptr[1], tptr[6], tchar);
        SWAP_MACRO(tptr[2], tptr[5], tchar);
        SWAP_MACRO(tptr[3], tptr[4], tchar);
        tptr += 8;
    }
}

#define START_400TH_CONSTANT_NAME   (84 * 3 + 400 * 6 + 5 * sizeof( double) \
                                                    + 41 * sizeof( int32_t))


static int dimension(const int idx)
{
    int rval;

    if (idx == eDeNutation)             /* Nutations */
        rval = 2;
    else if (idx == eDeTT_TDB)          /* TDT - TT */
        rval = 1;
    else                       /* planets, lunar mantle angles, librations */
        rval = 3;
    return(rval);
}


/*
   ------------------- DE 二进制星历文件格式说明 -------------------

   JPL DE 系列（DExxx/LEPxxx）二进制星历文件由若干"磁盘记录"串联构成，所有多字节数值均可按
   大端或小端存储（文件自报字节序）。整份文件布局如下：

   record 0 : 文件头（header）。含三个 84 字节文本行、常量名表、时间范围、ipt 索引表等。
   record 1 : 常量记录。存放 ncon 个星历常量（GM 值、半径、质量比等），每个 8 字节 double。
   record 2 及以后 : 数据记录（数据块）。每块覆盖 ephem_step 天，含一组切比雪夫系数。

   record n 的起始字节偏移 = (n + 2) * numCoeff_ * sizeof(double)。
   （numCoeff_ 即单个数据块含系数总数，"record 2"对应数据块 0。）

   ---------- 文件头（debin_header），各字段字节偏移 ----------

   偏移    字段                     含义
   ------   -----------------------  ---------------------------------------------------
   0        title[84]              第一文本行 "JPL Planetary Ephemeris DExxx/LExxx"。
   84       start_epoch[84]        第二文本行 "Start Epoch: JED = ... yyyy-MMM-dd ..."。
   168      final_epoch[84]        第三文本行 "Final Epoch: JED = ... yyyy-MMM-dd ..."。
   252      constant_names[400][6] 前 400 个常量名，每个 6 字符。
   2652     ephem_start           星历起始儒略历书日 JED。
   2660     ephem_end             星历结束儒略历书日 JED（开区间上界）。
   2668     ephem_step            每个数据块覆盖的天数（如 DE430 为 32 天）。
   2676     ncon                  常量个数（uint32）。
   2680     au                    天文单位长度（km），约 149597870.7。
   2688     emrat                 地球/月球质量比（约 81.300569）。
   2696     ipt[12][3]            12 组索引（见下）—— 这里只连续存放 ipt[0..11]。
   2840     numde                 DE 星历版本号，如 405、430、440（uint32）。
   2844     lpt[3]                ipt[12]（月球天平动）索引，即月面天平动的 ipt 条目。

   若 ncon <= 400，lpt 后直接存放 rpt 和 tpt（ipt[13]、ipt[14] 各占 3×4 字节）；
   若 ncon > 400，lpt 后先存放 (ncon-400)×6 字节的"额外常量名"（constant_names2），
   之后才是 rpt 和 tpt。本实现会在 ipt[13][0] 指向的偏移处读取 6 个 int32 取得 rpt/tpt，
   并以"一致性检查"判断其是否有效（见 open()）。

   ---------- ipt 索引表（每个天体/量一条） ----------

   数组元素含义：ipt[i] = { ipt0, ncf, ns }
     ipt0 : 该量首个系数在"数据块内"的 1 基起始位置（从块头第 1 个 double 算起，故行星类
            数据一定 >= 3，因为块头前两个 double 是块起止 JED）。
     ncf  : 每个坐标分量使用的切比雪夫系数个数（<= MAX_CHEBY=18）。
     ns   : 该量在每个数据块内被划分的子区间数。

   前端下标 0..14 分别对应（本实现 ipt_ 为 15x3，前 12 条由文件头 ipt[12][3] 得到，
   下标 12、13、14 分别来自 lpt、rpt、tpt；表中"SS"指太阳系 Solar System，相对地心者另注明）：

   下标  EDeDataCode       对应天体/量            维度  说明
   ----  -----------------  --------------------  ----  ----------------------------
   0     eDeMercury         水星(相对SS质心)        3
   1     eDeVenus           金星(相对SS质心)        3
   2     eDeEMBaryCenter    地月系质心(相对SS质心)  3
   3     eDeMars            火星(相对SS质心)        3
   4     eDeJupiter         木星(相对SS质心)        3
   5     eDeSaturn          土星(相对SS质心)        3
   6     eDeUranus          天王星(相对SS质心)      3
   7     eDeNeptune         海王星(相对SS质心)      3
   8     eDePluto           冥王星(相对SS质心)      3
   9     eDeMoon            月球(相对地心)          3
   10    eDeSun             太阳(相对SS质心)        3
   11    eDeNutation        地球黄经/倾角章动        2
   12    eDeLibration       月面天平动              3
   13    eDeLunarAngVel     月面角速度              3
   14    eDeTT_TDB          TT-TDB(地心处)          1

   每个数据块内前两个 double 固定为块起始、块结束 JED；其余系数按分量×子区间×ncf 排布。

   numCoeff_（单块总系数个数）= 2（起止 JED）+ Σ_i [ ipt_[i][1] * ipt_[i][2] * dimension(i) ]。
   dimension(i)：行星/天平动/角速度取 3，章动取 2，TT-TDB 取 1（见 dimension()）。

   ---------- 切比雪夫插值 ----------

   时间归一化：t = (JED - 块起始JED) / (块时长 / ns)，子区间号 idx = floor(t)，
   归一化切比雪夫时间 tc = 2*frac(t) - 1 ∈ [-1, 1)。系数块入口为
   dataBlock + (ipt0 - 1 + dim * idx * ncf)。

   位置：T0=1、T1=tc、Tk+1 = 2*tc*Tk - Tk-1，位置 = Σ_j coeff[j] * T_j(tc)。
   速度：T'0=0、T'1=1、T'k+1 = 2*tc*T'k + 2*Tk - T'k-1，
        速度 = (2 / tspan / 86400) * Σ_j coeff[j] * T'_j(tc)。

   ---------- 单位 ----------

   切比雪夫系数以 天 为时间单位、以 km 为长度单位存储：位置系数单位为 km，
   速度 = 位置系数/天，故上式再 /86400 折算为 km/s。
   getStateTDB() 返回 km 与 km/s；getPosVelICRF() 再统一乘以 1e3 ，
   对外输出为 m 与 m/s（ICRF 参考系）。
   注意：DE 头文件中的引力参数 GM（如 GM1/GMB/GMS）单位为 AU³/day²，
   换算为 km³/s² 需乘以 AU³/day²，其中 AU(k in km) 取自同头文件常量（149597870.7 km）。

   ---------- 字节序与常量 ----------

   通过 ncon > 65536 判断文件字节序与本机是否一致（isSameEndian_），不一致时逐段 32/64 位
   字节交换（swap_32_bit_val / swap_64_bit_val）。
   常量记录存放于文件头之后、首个数据块之前，共 ncon 个 double。若 ncon==400，需要越过
   可能存在的 (ncon-400)*6 字节额外常量名后再确定 ncon 真实数量（见 open()）。
*/

// 强制 1 字节对齐
#pragma pack(push, 1)
/// @brief De系列星历的二进制头
struct debin_header
{
    char        title[84];                  // 标题 JPL Planetary Ephemeris DExxx/LExxx
    char        start_epoch[84];            // Start Epoch: JED = ttttttt.t yyyy-MMM-dd 00:00:00
    char        final_epoch[84];            // Final Epoch: JED = ttttttt.t yyyy-MMM-dd 00:00:00
    char        constant_names[400][6];     // 常量名称
    // 
    double      ephem_start;                // 星历起始时间(Julian Ephemeris Date)
    double      ephem_end;                  // 星历结束时间(Julian Ephemeris Date)
    double      ephem_step;                 // 星历每个数据块的天数
    uint32_t    ncon;                       // 常量个数
    double      au;                         // 天文单位长度(千米) 约为 1.495978707e8
    double      emrat;                      // 地球/月球质量比
    uint32_t    ipt[12][3];                 /* Pointers to # coeffs for objects */
    //          most data is stored in the above struct
    uint32_t    numde;                      // De星历版本
    uint32_t    lpt[3];                     /* ipt[12]: Pointers to # coeffs for lunar librations */
    union
    {
        struct  // for ncon > 400
        {
            char    constant_names2[4][6];
        };
        struct  // for ncon <= 400
        {
            uint32_t rpt[3];     // ipt[13]: Pointers /  offsets to # the lunar euler angle rate
            uint32_t tpt[3];     // ipt[14]: Pointers to # the TT-TDB
        };
    };
};
#pragma pack(pop)

static_assert(
    offsetof(debin_header, numde) == 2840, 
    "offset is not correct, please check whether the compiler support `#pragma pack()`"
);


JplDe::JplDe()
    : 
      isSameEndian_{true}
    , numDataBlock_(0)
    , ephemStart_(0)
    , ephemEnd_(0)
    , deFile_(nullptr)
    , dataBlocks_ (nullptr)
    , constants_{}
    , mutex_()
{

}

JplDe::~JplDe()
{
    this->close();
}

errc_t JplDe::getInterval(TimeInterval &interval) const
{
    TimePoint start = TimePoint::FromTDB(JulianDate::FromImpreciseDay(ephemStart_));
    TimePoint stop  = TimePoint::FromTDB(JulianDate::FromImpreciseDay(ephemEnd_));
    interval.setBounds(start, stop);
    return eNoError;
}

int JplDe::readDataBlock(size_t idx)
{
    assert(idx < numDataBlock_ && idx >= 0);
    if (dataBlocks_ && idx < numDataBlock_ && idx >= 0)
    {
        std::lock_guard<std::mutex> _(mutex_);
        if (dataBlocks_[idx]) {  // has data cache
            return 0;
        }
        std::unique_ptr<double[]> block_ptr(new double[numCoeff_]);
        auto block = block_ptr.get();
        if (fseek(deFile_, long(idx + 2) * numCoeff_ * (int)sizeof(double), SEEK_SET))
            return JPL_EPH_READ_ERROR;
        if (fread(block, sizeof(double), (size_t)numCoeff_, deFile_) != (size_t)numCoeff_)
            return JPL_EPH_READ_ERROR;
        if (!isSameEndian_)
            swap_64_bit_val(block, numCoeff_);
        ;
        dataBlocks_[idx] = block_ptr.release();  // 转移所有权到 dataBlocks_[idx]
        return 0;
    }
    else {
        return JPL_EPH_OUTSIDE_RANGE;
    }
}

errc_t JplDe::getState(const TimePoint& time, int dataid, double pos[], double vel[])
{
    JulianDate jdTDB{};
    aTimePointToTDB(time, jdTDB);
    return getStateTDB(jdTDB, dataid, pos, vel);
}

errc_t JplDe::getState(const TimePoint& time, int datalist[11], double statelist[11][6])
{
    JulianDate jdTDB{};
    aTimePointToTDB(time, jdTDB);
    return getStateTDB(jdTDB, datalist, statelist);
}

void JplDe::close()
{
    if (deFile_) {
        fclose(deFile_);
        deFile_ = NULL;
        ephemStart_ = 0;
        ephemEnd_ = 0;
    }
    if (dataBlocks_) {
        double* block = NULL;
        for (size_t i = 0; i < numDataBlock_; i++) {
            block = dataBlocks_[i];
            if (block) {
                delete[] block;
            }
        }
        delete[] dataBlocks_;
        dataBlocks_ = NULL;
        numDataBlock_ = 0;
    }
    constants_.clear();
}

errc_t JplDe::loadConstants() const
{
    // 懒加载：首次调用 getConstant() 时把全部常量从文件读入 constants_。
    // 常量记录区与数据块共享同一个 deFile_，这里用 mutex_ 与 readDataBlock 串行化，
    // 避免并发读改动同一 FILE* 的文件位置。
    if (!constants_.empty()) return eNoError;   // 已加载
    if (!deFile_) return eErrorInvalidFile;
    std::lock_guard<std::mutex> _(mutex_);
    // constants_.empty() 必须在锁内再次重查 
    if (!constants_.empty()) return eNoError;   // 已加载

    ConstantMap tmp;   // 先载入临时表，成功后再 swap 提交，避免失败中途留下部分数据
    char nameBuf[7]{};
    const long baseValOff = (long)numCoeff_ * (long)sizeof(double);   // 常量记录区起始偏移
    for (uint32_t i = 0; i < numConstants_; i++)
    {
        // 数值
        double v = 0;
        if (fseek(deFile_, baseValOff + (long)i * (long)sizeof(double), SEEK_SET)) return JPL_EPH_FSEEK_ERROR;
        if (fread(&v, sizeof(double), 1, deFile_) != 1) return JPL_EPH_READ_ERROR;
        if (!isSameEndian_) swap_64_bit_val(&v);

        // 名称：i<400 取自头内 constant_names 表；i>=400 越过额外常量名块
        long nameOff = (i < 400)
            ? 84L + 84L + 84L + (long)i * 6
            : (long)offsetof(debin_header, constant_names2) + (long)(i - 400) * 6;
        if (fseek(deFile_, nameOff, SEEK_SET)) return JPL_EPH_FSEEK_ERROR;
        if (fread(nameBuf, 1, 6, deFile_) != 6) return JPL_EPH_READ_ERROR;
        std::string nm(aStripTrailingAsciiWhitespace(nameBuf));

        tmp.insert(nm, v);   // DE 常量名唯一，按索引顺序插入即保持顺序
    }
    constants_ = std::move(tmp);   // 全部成功后才提交
    return eNoError;
}

const JplDe::ConstantList& JplDe::getConstants() const
{
    errc_t rc = loadConstants();
    A_UNUSED(rc);
    const auto& items = constants_.items();
    return items;
}

errc_t JplDe::getConstant(size_t index, double &value, std::string *name) const
{
    if (!deFile_) return eErrorInvalidFile;

    errc_t rc = loadConstants();
    if (rc) return rc;

    const auto& items = constants_.items();    // 保持 DE 索引顺序
    if (index >= items.size()) return JPL_EPH_INVALID_INDEX;
    value = items[index].second;
    if (name) *name = items[index].first;
    return eNoError;
}

errc_t JplDe::getConstant(const std::string& name, double& value) const
{
    errc_t rc = loadConstants();
    if (rc) return rc;

    // OrderedMap 内部用 unordered_map，O(1) 按名查找。
    auto it = constants_.find(name);
    if (it == constants_.end()) return JPL_EPH_QUANTITY_NOT_IN_EPHEMERIS;
    value = it->second;
    return eNoError;
}

double JplDe::getConstant(const std::string &name) const
{
    double value;
    errc_t rc = getConstant(name, value);
    if (rc) return std::numeric_limits<double>::quiet_NaN();
    return value;
}


double JplDe::getBodyGM(EDataCode bodyCode) const
{
    double value;
    errc_t rc = getBodyGM(bodyCode, value);
    if (rc) return std::numeric_limits<double>::quiet_NaN();
    return value;
}

errc_t JplDe::getBodyGM(EDataCode bodyCode, double &gm) const
{

    // JPL DE 的 GM 下标按天体编号：1=水星,2=金星,B=地月质心,4=火星,5=木星,
    // 6=土星,7=天王星,8=海王星,9=冥王星,S=太阳。地球与月球没有独立 GM，
    // 需由地月质心 GMB 与地球/月球质量比 emRatio 拆分。
    double value;
    errc_t rc;
    switch (bodyCode)
    {
    case eMercury:      rc = getConstant("GM1", value); break;
    case eVenus:        rc = getConstant("GM2", value); break;
    case eMars:         rc = getConstant("GM4", value); break;
    case eJupiter:      rc = getConstant("GM5", value); break;
    case eSaturn:       rc = getConstant("GM6", value); break;
    case eUranus:       rc = getConstant("GM7", value); break;
    case eNeptune:      rc = getConstant("GM8", value); break;
    case ePluto:        rc = getConstant("GM9", value); break;
    case eSun:          rc = getConstant("GMS", value); break;
    case eEMBarycenter: rc = getConstant("GMB", value); break;
    case eEarth:        // GM_earth = GMB * emRatio / (1+emRatio)
        rc = getConstant("GMB", value);
        value = value * (emMassRatio_ / (1.0 + emMassRatio_)); 
        break;
    case eMoon:         // GM_moon  = GMB / (1+emRatio)
        rc = getConstant("GMB", value);
        value = value / (1.0 + emMassRatio_);
        break;
    case eSSBarycenter: // 太阳系质心无 GM 概念
    case eNotAvailable:
    default:
        return JPL_EPH_QUANTITY_NOT_IN_EPHEMERIS;
    }
    if (rc) return rc;   // 常量缺失 / 文件未打开等错误：value 未写，别再算 gm

    // GM 常量以 AU³/day² 存储；换算为 SI m³/s²（au_ 为天文单位长度 km）：
    //   km³ -> m³ 乘 1e9；AU³/day² -> km³/day² 再乘 au_³，除以 86400² 得 AU³/day² -> km³/s²。
    const double gmFac = au_ * au_ * au_ * 1e9 / (86400.0 * 86400.0);   // AU³/day² -> m³/s²

    gm = value * gmFac;
    return eNoError;
}

errc_t JplDe::open(const char* fileName)
{
    this->close();
    debin_header header{};

    FILE* file = ast_fopen(fileName, "rb");
    if (file == NULL) {
        goto fail;
    }
    if (fread(&header, 1, sizeof(debin_header), file) != sizeof(debin_header)) {
        goto fail;
    }
    if ((uint32_t)header.ncon > (uint32_t)65536) {
        isSameEndian_ = false;
    }
    else {
        isSameEndian_ = true;
    }
    ephemEnd_ = header.ephem_end;
    ephemStart_ = header.ephem_start;
    ephemStep_ = header.ephem_step;
    ephemVersion_ = header.numde;
    numConstants_ = header.ncon;
    au_ = header.au;
    emMassRatio_ = header.emrat;
    if (!isSameEndian_) {
        swap_64_bit_val(&ephemEnd_);
        swap_64_bit_val(&ephemStart_);
        swap_64_bit_val(&ephemStep_);
        swap_64_bit_val(&au_);
        swap_64_bit_val(&emMassRatio_);
        swap_32_bit_val(&ephemVersion_);
        swap_32_bit_val(&numConstants_);
    }
    if (emMassRatio_ > 81.3008 || emMassRatio_ < 81.30055)
        goto fail;

    // read ipt
    memcpy(&ipt_[0][0], &header.ipt[0][0], sizeof(header.ipt));
    memcpy(ipt_[12], &header.lpt[0], sizeof(header.lpt));


    /* It's a little tricky to tell if an ephemeris really has  */
    /* TT-TDB data (with offsets in ipt[13][] and ipt[14][]).   */
    /* Essentially,  we read the data and sanity-check it,  and */
    /* zero it if it "doesn't add up" correctly.                */
    /*    Also:  certain ephems I've generated with ncon capped */
    /* at 400 have no TT-TDB data.  So if ncon == 400,  don't   */
    /* try to read such data;  you may get garbage.             */
    if (ephemVersion_ >= 430 && numConstants_ != 400)
    {
        if (numConstants_ > 400) {
            fseek(file, (size_t)(numConstants_ - 400) * 6 + offsetof(debin_header, constant_names2), SEEK_SET);
        }
        if (fread(&ipt_[13][0], sizeof(int32_t), 6, file) != 6)
            goto fail;
    }
    else /* mark header data as invalid */
    {
        ipt_[13][0] = (uint32_t)-1;  // 0xFFFF
    }
    if (!isSameEndian_)
    {
        swap_32_bit_val(&ipt_[0][0], sizeof(ipt_) / 4);
    }
    /* if these don't add up correctly, */
    /* zero them out (they've garbage data) */
    if (ipt_[13][0] != ipt_[12][0] + ipt_[12][1] * ipt_[12][2] * 3 ||
        ipt_[14][0] != ipt_[13][0] + ipt_[13][1] * ipt_[13][2] * 3)
    {     /* not valid pointers to TT-TDB data */
        for (int i = 13; i < 15; i++)
            for (int j = 0; j < 3; j++)
                ipt_[i][j] = 0;
    }

    numCoeff_ = 2;  // start jed and end jed 
    for (int i = 0; i < 15; i++)
        numCoeff_ += ipt_[i][1] * ipt_[i][2] * dimension(i);


    /* If there are more than 400 constants,  the names of       */
    /* the extra constants are stored in what would normally     */
    /* be zero-padding after the header record.  However,        */
    /* older ephemeris-reading software won't know about that.   */
    /* So we store ncon=400,  then actually check the names to   */
    /* see how many constants there _really_ are.  Older readers */
    /* will just see 400 names and won't know about the others.  */
    /* But on the upside,  they won't crash.                     */
    if (numConstants_ == 400)
    {
        char buff[7]{};
        buff[6] = '\0';
        fseek(file, offsetof(debin_header, constant_names2), SEEK_SET);
        while (fread(buff, 6, 1, file) && strlen(buff) == 6)
            numConstants_++;
    }

    /* 常量采用懒加载：此处不读取，待第一次调用 getConstant() 时经 loadConstants() 载入。 */
    // loadConstants();

    numDataBlock_ = static_cast<uint32_t>((ephemEnd_ - ephemStart_) / ephemStep_);
    dataBlocks_ = new double* [numDataBlock_]{};
    memset(dataBlocks_, 0, sizeof(double*) * numDataBlock_);
    deFile_ = file;
    return eNoError;
fail:
    if (file) {
        fclose(file);
    }
    this->close();
    return eErrorInvalidFile;
}


errc_t JplDe::getPosVelICRF(
    const TimePoint& time,
    EDataCode ntarg,
    EDataCode ncent,
    Vector3d& pos,
    Vector3d* vel
)
{
    /* if moon from earth or earth from moon ..... */
    if (ntarg + ncent == eEarth + eMoon && ntarg * ncent == eEarth * eMoon)
    {
        // only interp Moon (geocenter)
        int rval = this->getState(time, eDeMoon, pos.data(), (double*)vel);
        if (rval) {
            return rval;
        }
        if (ncent == eMoon) {
            for (int i = 0; i < 3; ++i)
                pos[i] = -pos[i] * 1e3;
            if (vel) {
                for (int i = 0; i < 3; ++i)
                    vel->at(i) = -vel->at(i) * 1e3;
            }
        }
        else {
            for (int i = 0; i < 3; ++i)
                pos[i] *= 1e3;
            if (vel) {
                for (int i = 0; i < 3; ++i)
                    vel->at(i) *= 1e3;
            }
        }
        return eNoError;
    }


    const int quantities = (vel ? 2 : 1);
    double pv[eEMBarycenter + 1][6];
    int    list[eDeSun + 1];
    memset(list, 0, sizeof(list));
    memset(pv, 0, sizeof(pv));
    {
        /*
        * centerbody data deps:
        * Earth     ->  EM Bary, Moon
        * Moon      ->  EM Bary, Moon
        * Other     ->  Other
        * SS-Bary   ->  SS-Bary
        * EM-Bary   ->  EM-Bary
        */
        for (int i = 0; i < 2; i++) /* list[] IS NUMBERED FROM ZERO ! */
        {
            const unsigned k = (i ? ncent : ntarg);

            if (k <= eSun)           list[k] = quantities;            /* Major planets */
            if (k == eMoon)          list[eDeEMBaryCenter] = quantities;            /* for moon,  earth state is needed */
            if (k == eEarth)         list[eDeMoon] = quantities;            /* for earth,  moon state is needed */
            if (k == eEMBarycenter)  list[eDeEMBaryCenter] = quantities;            /* EMBary state additionally */
        }
    }
    int rval = this->getState(time, list, pv);
    if (rval) {
        return rval;
    }

    {
        /* Solar System Barycenter coordinates & velocities equal to zero */
        if (ntarg == eSSBarycenter || ncent == eSSBarycenter)
            memset(pv[eSSBarycenter], 0, sizeof(pv[eSSBarycenter]));
        /* Solar System barycentric EMBary state:  */
        if (ntarg == eEMBarycenter || ncent == eEMBarycenter)
            memcpy(pv[eEMBarycenter], pv[eDeEMBaryCenter], sizeof(pv[eEMBarycenter]));

        if (list[eEarth])           /* calculate earth state from EMBary */
            for (int i = 0; i < list[eDeEMBaryCenter] * 3; ++i)
                pv[eEarth][i] -= pv[eDeMoon][i] / (1.0 + emMassRatio_);

        if (list[eMoon]) /* calculate Solar System barycentric moon state */
            for (int i = 0; i < list[eMoon] * 3; ++i)
                pv[eMoon][i] += pv[eEarth][i];
    }
    for (int i = 0; i < 3; ++i)
        pos[i] = (pv[ntarg][i] - pv[ncent][i]) * 1e3;
    if (vel) {
        for (int i = 0; i < 3; ++i)
            vel->at(i) = (pv[ntarg][3 + i] - pv[ncent][3 + i]) * 1e3;
    }
    return eNoError;
}

errc_t JplDe::getPosVelICRF(const TimePoint& time, EDataCode target, EDataCode referenceBody, Vector3d& pos, Vector3d& vel)
{
    return getPosVelICRF(time, target, referenceBody, pos, &vel);
}



errc_t JplDe::getStateTDB(const JulianDate& jdTDB, int cblist[11], double pv[11][6])
{
    double jed  = jdTDB.impreciseDay();

    struct InterpolationCoeff
    {
        int     n_pos_coeff{ -1 };
        int     n_vel_coeff{};
        int     idx_interval{};
        double  pos_coeff[MAX_CHEBY]{};
        double  vel_coeff[MAX_CHEBY]{};
        double  twot{};
        double  tspan{};
    };

    //const int ncm = 3;  // for cb, dimension = 3
    enum
    {
        ncm = 3
    };
    double P_Sum[ncm], V_Sum[ncm];
    if (jed < ephemStart_ || jed >= ephemEnd_)
        return(JPL_EPH_OUTSIDE_RANGE);
    size_t idx_block = static_cast<size_t>((jed - ephemStart_) / ephemStep_);
    const double* dataBlock = dataBlocks_[idx_block];
    if (!dataBlock) {
        int res = this->readDataBlock(idx_block);
        if (res) {
            return res;
        }
        dataBlock = dataBlocks_[idx_block];
    }
    auto ts = dataBlock[0];
    auto te = dataBlock[1];
    // @todo: cache pv sun
    InterpolationCoeff interCoeffArray[4];


    for (int target = 0; target <= eSun; target++)
    {
        if (cblist[target]) // && num_interval == ipt_[target][2]) 
        {
            const auto ncf = ipt_[target][1];                   /*      Number of coeff's     */
            assert(ncf < MAX_CHEBY);

            // n_interval相同的话，tc也是相同的，切比雪夫多项式插值系数也相同，减少计算量;
            const auto num_interval = ipt_[target][2];
            auto& inter_coeff = interCoeffArray[log2n(num_interval)];
            if (inter_coeff.n_pos_coeff == -1) {
                inter_coeff.tspan = (te - ts) / num_interval;
                double  temp2 = ((jdTDB.day() - ts) + jdTDB.dayFractional() ) / inter_coeff.tspan;
                inter_coeff.idx_interval = static_cast<int>(temp2);
                double tc = 2.0 * fmod(temp2, 1.0) - 1.0;   // time for Tschebeyshev polynomial
                // -1 <= tc < 1
                assert(tc >= -1.);
                assert(tc <= 1.);
                inter_coeff.n_pos_coeff = 2;
                inter_coeff.n_vel_coeff = 2;
                inter_coeff.pos_coeff[0] = 1.0;
                inter_coeff.pos_coeff[1] = tc;
                inter_coeff.vel_coeff[0] = 0.0;
                inter_coeff.vel_coeff[1] = 1.0;
                inter_coeff.twot = tc * 2.0;
            }

            const auto coeff = dataBlock + (ipt_[target][0] - 1 + ncm * inter_coeff.idx_interval * ncf);
            /* ------------------------------------    */
            /*   Compute interpolating polynomials     */
            /* ----------------------------------------*/
            if (inter_coeff.n_pos_coeff < (int)ncf) {
                for (int j = inter_coeff.n_pos_coeff; j < (int)ncf; j++)
                {
                    inter_coeff.pos_coeff[j] = inter_coeff.twot * inter_coeff.pos_coeff[j - 1] - inter_coeff.pos_coeff[j - 2];
                }
                inter_coeff.n_pos_coeff = ncf;
            }
            for (int i = 0; i < ncm; i++)
            {
                P_Sum[i] = 0.0;
                // @todo: meybe SIMD
                for (int j = ncf - 1; j > -1; j--)
                    P_Sum[i] = P_Sum[i] + coeff[j + i * ncf] * inter_coeff.pos_coeff[j];
                pv[target][i] = P_Sum[i];
            }
            // if cblist[target] = 1, only interpolate position
            if (cblist[target] < 2) {
                continue;
            }
            if (inter_coeff.n_vel_coeff < (int)ncf) {
                for (int j = inter_coeff.n_vel_coeff; j < (int)ncf; j++)
                {
                    inter_coeff.vel_coeff[j] = inter_coeff.twot * inter_coeff.vel_coeff[j - 1] + 2.0 * inter_coeff.pos_coeff[j - 1] - inter_coeff.vel_coeff[j - 2];
                }
            }
            for (int i = 0; i < ncm; i++)
            {
                V_Sum[i] = 0.0;
                for (int j = ncf - 1; j > 0; j--)
                    V_Sum[i] = V_Sum[i] + coeff[j + i * ncf] * inter_coeff.vel_coeff[j];
                pv[target][3 + i] = V_Sum[i] * 2.0 / inter_coeff.tspan / 86400.;
            }
        }
    }
    return 0;
}



errc_t JplDe::getStateTDB(const JulianDate& jdTDB, int ntarg, double pos[], double vel[])
{
    // const int ncm = 3; // number of component
    // enum { ncm = 3};
    double jed = jdTDB.impreciseDay();
    const int ncm = dimension(ntarg);
    double   Cp[MAX_CHEBY], Up[MAX_CHEBY], P_Sum[3], V_Sum[3];
    int       i, j;

    // target > 10
    if (ntarg > eDeTT_TDB)
    {
        /* Also protects against weird input errors */
        aError("\n ntarg out of range, must <= eDeTT_TDB(14) \n");
        return JPL_EPH_INVALID_INDEX;
    }
    if (jed < ephemStart_ || jed >= ephemEnd_)
        return(JPL_EPH_OUTSIDE_RANGE);
    /*--------------------------------------------------------------------------*/
    /* Initialize memory coefficient array.                                      */
    /*--------------------------------------------------------------------------*/
    size_t idx_block = static_cast<size_t>((jed - ephemStart_) / ephemStep_);
    // @todo: 位于区间交界处，作为哪个区间计算？
    const double* dataBlock = dataBlocks_[idx_block];
    if (!dataBlock) {
        int res = this->readDataBlock(idx_block);
        if (res) {
            return res;
        }
        dataBlock = dataBlocks_[idx_block];
    }

    /*--------------------------------------------------------------------------*/
    /* Read the coefficients from the binary record.                            */
    /*--------------------------------------------------------------------------*/
    const auto idx = ipt_[ntarg][0];                   /*      Coeff array entry point */
    const auto ncf = ipt_[ntarg][1];                   /*      Number of coeff's       */
    const auto num_interval = ipt_[ntarg][2];                   /*      Number of subintervals  */
    if (idx == 0) {
        return JPL_EPH_QUANTITY_NOT_IN_EPHEMERIS;
    }
    assert(ncf < MAX_CHEBY);

    auto ts = dataBlock[0];
    auto te = dataBlock[1];

    double  tspan = (te - ts) / num_interval;
    double  temp2 = ((jdTDB.day() - ts) + jdTDB.dayFractional()) / tspan;
    int     idx_interval = static_cast<int>(temp2);
    double  tc = 2.0 * fmod(temp2, 1.0) - 1.0;   // time for Tschebeyshev polynomial

    // -1 <= tc < 1
    assert(tc >= -1.);
    assert(tc <= 1.);

    const double* coeff = &dataBlock[idx + ncm * idx_interval * ncf - 1];

    /* ------------------------------------    */
    /*   Compute interpolating polynomials     */
    /* ----------------------------------------*/
    double tc2 = tc * 2.0;

    Cp[0] = 1.0;
    Cp[1] = tc;


    for (j = 2; j < (int)ncf; j++)
    {
        Cp[j] = tc2 * Cp[j - 1] - Cp[j - 2];
    }

    /* ---------------------------------------- */
    /* Compute interpolated position & velocity */
    /* ---------------------------------------- */

    for (i = 0; i < ncm; i++)
    {
        P_Sum[i] = 0.0;
        for (j = ncf - 1; j > -1; j--)  P_Sum[i] = P_Sum[i] + coeff[j + i * ncf] * Cp[j];
        pos[i] = P_Sum[i];
    }
    if (!vel) {
        return 0;
    }
    Up[0] = 0.0;
    Up[1] = 1.0;

    for (j = 2; j < (int)ncf; j++)
    {
        Up[j] = tc2 * Up[j - 1] + 2.0 * Cp[j - 1] - Up[j - 2];
    }
    for (i = 0; i < ncm; i++)
    {
        V_Sum[i] = 0.0;
        for (j = ncf - 1; j > 0; j--)   V_Sum[i] = V_Sum[i] + coeff[j + i * ncf] * Up[j];
        vel[i] = V_Sum[i] * 2.0 / tspan / 86400.;
    }
    return 0;
}

errc_t JplDe::getNutation(
    const TimePoint& time,
    double& nutLong,
    double& nutObl)
{
    double val[3]{};
    int res = this->getState(time, eDeNutation, val, nullptr);
    nutLong = val[0];
    nutObl = val[1];
    return res;
}


errc_t JplDe::getLibration(
    const TimePoint& time,
    Euler& ang,
    Euler& angRate)
{
    return this->getState(time, eDeLibration, ang.data(), angRate.data());
}


errc_t JplDe::getLibration(const TimePoint &time, Euler &ang)
{
    return this->getState(time, eDeLibration, ang.data(), nullptr);
}

errc_t JplDe::getPosICRF(
    const TimePoint& time,
    EDataCode target,
    EDataCode referenceBody,
    Vector3d& pos
)
{
    return this->getPosVelICRF(time, target, referenceBody, pos, nullptr);
}

bool JplDe::isOpen() const
{
    return deFile_ != NULL;
}

    


AST_NAMESPACE_END
