///
/// @file      SpiceSPKParser.hpp
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

#pragma once

#include "AstGlobal.h"
#include "DAFParser.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup ParseFormat
    @{
*/


#pragma pack(push, 1)

/// @brief SPK 段描述符
/// 解包后的 SPK 段摘要信息，与SPK文件的二进制格式一致
struct SPK_Descriptor
{
    double start_time;          // 开始时间 (ET)
    double end_time;            // 结束时间
    int32_t target;             // 目标体 ID
    int32_t center;             // 中心体 ID
    int32_t frame;              // 参考系 ID
    int32_t type;               // 数据类型
    int32_t begin_addr;         // 数据起始字地址
    int32_t end_addr;           // 数据结束字地址
};

#pragma pack(pop)

/// @brief SPICE二进制SPK内核文件解析器(parser for SPICE SPK kernel file)
/// @see cspice库里的文档 doc/html/req/spk.html
/// @details 用于解析SPICE二进制SPK内核文件（.bsp），提供获取目标在指定时刻的原始位置和速度的功能。
///          “原始”意味着直接返回SPK文件中存储的相对于相应参考系的位置和速度，不进行任何额外的坐标转换。
///          使用前需通过 SpiceDAFParser::parse() 等方法打开并解析SPK文件
/// @warning 该类型的单个实例暂不支持多线程并发访问
/// @todo 处理不同类型的SPK数据，目前仅支持类型2
class AST_UTIL_API SPKParser: public DAFParser
{
public:
    SPKParser();
    explicit SPKParser(StringView filepath);
    ~SPKParser() = default;

    errc_t parse(StringView filepath);

    errc_t parse();
    
public:
    /// @brief 获取目标在指定时刻的原始位置和速度（相对于其星历参考系）
    /// @details 直接从SPK内核文件中读取数据，返回目标在给定时刻相对于该段定义的参考系的位置和速度。
    ///          不执行任何坐标转换
    /// @param et      相对于J2000 TDB的历元（秒）
    /// @param target  目标的NAIF ID代码（如399代表地球）
    /// @param[out] pos  位置向量 (x, y, z)，单位米(m)
    /// @param[out] vel  速度向量 (vx, vy, vz)，单位米/秒(m/s)
    /// @return 错误码，成功返回0，失败返回非0值（如时间超出时间区间、目标不存在等）
    errc_t getPosVelNative(double et, int target, Vector3d& pos, Vector3d& vel) const;
    
    /// @brief 获取目标在指定时刻的原始位置（相对于其星历参考系）
    /// @details 直接从SPK内核文件中读取数据，返回目标在给定时刻相对于该段定义的参考系的位置。
    ///          不执行任何坐标转换
    /// @param et      相对于J2000 TDB历元的秒数
    /// @param target  目标的NAIF ID代码
    /// @param[out] pos  位置向量 (x, y, z)，单位米(m)
    /// @return 错误码，成功返回0，失败返回非0值（如时间超出时间区间、目标不存在等）
    errc_t getPosNative(double et, int target, Vector3d& pos) const;

    /// @brief 查找目标在指定时间的SPK段描述符
    /// @details 从SPK文件中查找给定目标在给定时间的SPK段描述符。
    ///          如果目标在给定时间没有对应的段，返回nullptr。
    /// @param target  目标的NAIF ID代码
    /// @param et      相对于J2000 TDB历元的秒数
    /// @return 指向SPK段描述符的指针如果未找到）
    const SPK_Descriptor* findSpkDescriptor(int target, double et) const;
    
    /// @brief 获取目标在指定时刻的原始位置和速度（相对于其星历参考系）
    /// @details 直接从SPK内核文件中读取数据，返回目标在给定时刻相对于该段定义的参考系的位置和速度。
    ///          不执行任何坐标转换
    /// @param et      相对于J2000 TDB历元的秒数
    /// @param target  目标的NAIF ID代码
    /// @param[out] pos  位置向量 (x, y, z)，单位米(m)
    /// @param[out] vel  速度向量 (vx, vy, vz)，单位米/秒(m/s)
    /// @return 错误码，成功返回0，失败返回非0值（如时间超出时间区间、目标不存在等）
    errc_t getStateNative(double et, int target, Vector3d& pos, Vector3d* vel) const;

    /// @brief 获取SPK段描述符数组
    const std::vector<SPK_Descriptor>& getDescriptors() const{return spkDescriptors_;}
protected:

    errc_t getStateType2(const SPK_Descriptor& desc, double et, int target, Vector3d& pos, Vector3d* vel) const;
protected:
    std::vector<SPK_Descriptor> spkDescriptors_;     ///< SPK 段描述符数组
    mutable std::vector<double> buffer_;             ///< 数据读取缓冲区
};

/*! @} */

AST_NAMESPACE_END
