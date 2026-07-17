///
/// @file      SPKParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-10
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

#include "SPKParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN


#define MAX_CHEBY 20

#pragma pack(push, 1)


struct SPK_Descriptor;

static_assert(sizeof(SPK_Descriptor) == sizeof(double) * 2 + sizeof(int32_t) * 6, "SPK_Descriptor size must be 40");

struct DAF_SPKSummaryRecords
{
    double next;                     ///< 下一个摘要记录号 (存储为 double)
    double prev;                     ///< 上一个摘要记录号 (存储为 double)
    double nsum;                     ///< 本记录中摘要个数 (存储为 double)
    SPK_Descriptor descriptors[25];  ///< 摘要记录描述符数组
};

static_assert(sizeof(DAF_SPKSummaryRecords) == 1024, "DAF_SPKSummaryRecords size must be 1024");

// -----------------------
// 常用 SPK 数据类型内部布局 
// -----------------------


// 类型 2/3 记录头部 (Chebyshev 多项式)
struct SPK_Type2_Record {
    double mid;                 ///< 区间中点
    double radius;              ///< 区间半长
    // 之后是系数，数量由记录大小决定
};

// 类型 2 段尾部目录
struct SPK_Type2_Trailer {
    double init;                ///< 第一个区间的起始时间(相对于J2000 TDB秒数)
    double intlen;              ///< 每个区间时间长度(秒数)
    double rsize;               ///< 每个区间的大小（双精度个数）
    double n;                   ///< 记录区间个数
};

// 类型 5 离散状态 (二体传播)
struct SPK_Type5_State {
    double x, y, z, vx, vy, vz;
};

// 类型 5 段尾部（不含目录）
struct SPK_Type5_Trailer {
    double gm;                  // 引力常数
    double n;                   // 状态个数
};

// 类型 8/9/12/13 状态向量
struct SPK_State {
    double x, y, z, vx, vy, vz;
};

// 类型 8 段尾部
struct SPK_Type8_Trailer {
    double epoch0;              // 第一个状态的时间
    double step;                // 时间步长
    double degree;              // 插值多项式次数
    double n;                   // 状态个数
};

// 类型 10 TLE 数据包 (14 个双精度)
struct SPK_Type10_Packet {
    double ndt20;               // 1st derivative of mean motion/2
    double ndd60;               // 2nd derivative of mean motion/6
    double bstar;               // B* drag term
    double incl;                // inclination
    double node0;               // right ascension of ascending node
    double ecc;                 // eccentricity
    double omega;               // argument of perigee
    double mo;                  // mean anomaly
    double no;                  // mean motion
    double epoch;               // epoch
    double nu_obliquity;        // nutation in obliquity
    double nu_longitude;        // nutation in longitude
    double d_obliquity_dt;      // rate of nutation in obliquity
    double d_longitude_dt;      // rate of nutation in longitude
};

// 类型 10 常量区 (8 个双精度)
struct SPK_Type10_Constants {
    double j2;
    double j3;
    double j4;
    double sqrt_gm;             // sqrt(GM) in ER^3/min^2
    double high_atm_bound;      // high altitude bound (km)
    double low_atm_bound;       // low altitude bound (km)
    double eq_radius;           // Earth equatorial radius (km)
    double dist_units_per_er;   // distance units per Earth radius
};

// 类型 15 进动圆锥曲线 (16 个双精度)
struct SPK_Type15_Data {
    double epoch_peri;          // epoch of periapsis
    double pole[3];             // trajectory pole vector
    double peri_vec[3];          // periapsis unit vector
    double semi_latus;          // semi-latus rectum
    double ecc;                 // eccentricity
    double j2_flag;             // J2 flag (0 or 1)
    double cbody_pole[3];       // central body pole vector
    double gm;                  // central body GM
    double j2;                  // central body J2
    double radius;              // central body equatorial radius
};

// 类型 17 Equinoctial Elements (12 个双精度)
struct SPK_Type17_Data {
    double epoch_peri;
    double semi_major;
    double h, k;                // equinoctial H, K
    double mean_long;           // mean longitude at epoch
    double p, q;                // equinoctial P, Q
    double peri_rate;           // rate of longitude of periapse
    double mean_long_rate;      // mean longitude rate
    double node_rate;           // longitude of ascending node rate
    double pole_ra, pole_dec;   // pole right ascension, declination
};

// 类型 18/19 数据包（子类型0：12个数；子类型1：6个数）
struct SPK_Type18_Packet_Sub0 {
    double pos[3];
    double vel1[3];
    double vel2[3];
    double acc[3];
};

struct SPK_Type18_Packet_Sub1 {
    double pos[3];
    double vel[3];
};

// 类型 18/19 段尾部
struct SPK_Type18_Trailer {
    double subtype;             // 子类型
    double window;              // 窗大小
    double n;                   // 数据包个数
};

// 类型 20 Chebyshev velocity only 记录
struct SPK_Type20_Record {
    // double x_coeff[?];          // 系数个数由记录大小决定
    // double y_coeff[?];
    // double z_coeff[?];
    double x_midpos;            // 中点位置 X
    double y_midpos;
    double z_midpos;
};

// 类型 20 段尾部
struct SPK_Type20_Trailer {
    double dscale;              // 距离缩放因子
    double tscale;              // 时间缩放因子
    double initjd;              // 初始 JD 整数部分
    double initfr;              // 初始 JD 小数部分
    double intlen;              // 记录区间长度（日）
    double rsize;               // 记录大小
    double n;                   // 记录个数
};

#pragma pack(pop)

SPKParser::SPKParser()
    : spkDescriptors_(), buffer_()
{
}

SPKParser::SPKParser(StringView filepath)
    : spkDescriptors_(), buffer_()
{
    parse(filepath);
}

errc_t SPKParser::parse(StringView filepath)
{
    open(filepath);
    return parse();
}

errc_t SPKParser::parse()
{
    errc_t rc;
    rc = DAFParser::parse();
    if(rc) return rc;
    std::vector<Record> records;

    static_assert(sizeof(DAF_SPKSummaryRecords) == sizeof(Record), "DAF_SPKSummaryRecords size must be 1024");
    static_assert(std::is_pod<DAF_SPKSummaryRecords>::value, "DAF_SPKSummaryRecords must be POD");
    static_assert(std::is_pod<Record>::value, "Record must be POD");

    rc = this->getSummaryRecords(records);
    if(rc) return rc;
    std::vector<SPK_Descriptor> spkDescriptors;
    int sum = 0;
    for(auto& record : records)
    {
        DAF_SPKSummaryRecords& spkRecord = reinterpret_cast<DAF_SPKSummaryRecords&>(record);
        sum += (int)spkRecord.nsum;
    }
    spkDescriptors.reserve(sum);
    for(auto& record : records)
    {
        DAF_SPKSummaryRecords& spkRecord = reinterpret_cast<DAF_SPKSummaryRecords&>(record);
        spkDescriptors.insert(spkDescriptors.end(), spkRecord.descriptors, spkRecord.descriptors + (int)spkRecord.nsum);
    }
    spkDescriptors_ = std::move(spkDescriptors);
    return rc;
}

errc_t SPKParser::getPosVelNative(double et, int target, Vector3d &pos, Vector3d &vel) const
{
    return getStateNative(et, target, pos, &vel);
}

errc_t SPKParser::getPosNative(double et, int target, Vector3d &pos) const
{
    return getStateNative(et, target, pos, nullptr);
}

const SPK_Descriptor *SPKParser::findSpkDescriptor(int target, double et) const
{
    /*!
    @note 根据SPK星历规范，越后面的段优先级越高
    */
    for (int i = (int)spkDescriptors_.size() - 1; i >= 0; --i) {
        auto& descr = spkDescriptors_[i];
        if(descr.target == target && descr.start_time <= et && descr.end_time >= et)
        {
            return &descr;
        }
    }
    return nullptr;
}

errc_t SPKParser::getStateNative(double et, int target, Vector3d &pos, Vector3d *vel) const
{
    const SPK_Descriptor* spkDescriptor = findSpkDescriptor(target, et);
    if(!spkDescriptor)
    {
        aError("failed to find spk descriptor for target %d", target);
        return eErrorNotFound;
    }
    if(spkDescriptor->type == 2)
    {
        return getStateType2(*spkDescriptor, et, target, pos, vel);
    }else{
        aError("spk type %d is not supported yet", spkDescriptor->type);
    }
    return -1;
}

errc_t SPKParser::getStateType2(const SPK_Descriptor& spkDescriptor, double et, int target, Vector3d &pos, Vector3d *vel) const
{
    int rsize;
    {
        SPK_Type2_Trailer trailer{};
        size_t offset = 8 * spkDescriptor.end_addr - sizeof(SPK_Type2_Trailer);
        size_t size = read(&trailer, sizeof(SPK_Type2_Trailer), offset);
        if(size != sizeof(SPK_Type2_Trailer))
        {
            aError("failed to read spk type 2 trailer for target %d", target);
            return eErrorNotFound;
        }
        rsize = (int)trailer.rsize;
        buffer_.resize(rsize);
        size_t idxseg = (size_t)((et - spkDescriptor.start_time) / trailer.intlen);
        if(idxseg >= trailer.n)
        {
            aError("et %f out of range for target %d", et, target);
            return eErrorNotFound;
        }
        size_t sseg = rsize * 8;
        offset = 8 * (spkDescriptor.begin_addr-1) + idxseg * sseg;
        size = read(buffer_.data(), sseg, offset);
        if(size != sseg)
        {
            aError("failed to read spk type 2 record for target %d", target);
            return eErrorNotFound;
        }
    }
    const double mid = buffer_[0];
    const double radius = buffer_[1];
    // double tspan = radius * 2;
    const double* coeff = buffer_.data() + 2;
    const double tc = (et - mid) / radius;
    const double twot = 2.0 * tc;
    const int ncf = (rsize - 2) / 3;
    assert(tc >= -1);
    assert(tc <= 1);
    assert(ncf < MAX_CHEBY);
    if (ncf >= MAX_CHEBY) {
        aError("ncf %d exceeds MAX_CHEBY", ncf);
        return eErrorNotFound;
    }
    double  pos_coeff[MAX_CHEBY];
    pos_coeff[0] = 1.0;
    pos_coeff[1] = tc;
    for(int j = 2; j <= ncf; ++j)
    {
        pos_coeff[j] = twot * pos_coeff[j - 1] - pos_coeff[j - 2];
    }
    for(int i=0;i<3;i++){
        double sum = 0;
        for(int j = ncf - 1; j > -1; j--)
            sum += coeff[j + i * ncf] * pos_coeff[j];
        pos[i] = sum * 1e3;
    }
    if(!vel)
        return eNoError;
        
    double  vel_coeff[MAX_CHEBY];
    vel_coeff[0] = 0.0;
    vel_coeff[1] = 1.0;
    for(int j=2;j<=ncf;j++){
        vel_coeff[j] = twot * vel_coeff[j - 1] + 2.0 * pos_coeff[j - 1] - vel_coeff[j - 2];
    }
    for(int i=0;i<3;i++){
        double sum = 0;
        for(int j = ncf - 1; j > -1; j--)
            sum += coeff[j + i * ncf] * vel_coeff[j];
        (*vel)[i] = sum / radius * 1e3;
    }
    return eNoError;
}

AST_NAMESPACE_END

