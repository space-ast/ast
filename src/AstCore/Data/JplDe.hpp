///
/// @file      JplDe.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      5.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
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
 
#pragma once
 
#include "AstGlobal.h"
#include "AstCore/RunTime.hpp"
#include <stdint.h>      // for uint32_t
#include <cstdio>       // for FILE
#include <mutex>         // for std::mutex
 
AST_NAMESPACE_BEGIN


/*!
    @addtogroup Data
    @{
*/



/// @brief  JPL DE文件接口类
class AST_CORE_API JplDe
{
public:
    enum EDataCode
    {
        eNotAvailable = -1,     ///< 不可用
        eMercury = 0,           ///< 水星   
        eVenus = 1,             ///< 金星     
        eEarth = 2,             ///< 地球     
        eMars = 3,              ///< 火星      
        eJupiter = 4,           ///< 木星   
        eSaturn = 5,            ///< 土星    
        eUranus = 6,            ///< 天王星    
        eNeptune = 7,           ///< 海王星   
        ePluto = 8,             ///< 冥王星     
        eMoon = 9,              ///< 月球
        eSun = 10,              ///< 太阳
        eSSBarycenter = 11,     ///< 太阳系质心
        eEMBaryCenter = 12,     ///< 地月系质心
    };

    JplDe();
    ~JplDe();

    /// @brief  打开JPL DE文件
    /// @param  filepath - JPL DE文件路径
    /// @retval          - 错误码
    err_t open(const char* filepath);


    err_t openDefault();

    /// @brief  检查JPL DE文件是否已打开
    /// @retval             - 是否已打开
    bool isOpen() const;


    /// @brief  关闭JPL DE文件
    void close();


    /// @brief  获取对应时间点的目标天体在参考天体下的位置和速度
    /// @details 位置向量和速度向量的参考系是参考天体为中心的ICRF系
    /// @param  time        - 时间点
    /// @param  target      - 目标天体
    /// @param  referenceBody- 参考天体
    /// @param  pos         - 位置向量
    /// @param  vel         - 速度向量
    /// @retval             - 错误码
    err_t getPosVelICRF(
        const TimePoint& time,
        EDataCode target,
        EDataCode referenceBody,
        Vector3d& pos,
        Vector3d& vel
    );

    err_t getPosVelICRF(
        const TimePoint& time,
        EDataCode target,
        EDataCode referenceBody,
        Vector3d& pos,
        Vector3d* vel
    );


    /// @brief  获取对应时间点的目标天体在参考天体下的位置
    /// @details 位置向量的参考系是参考天体为中心的ICRF系
    /// @param  time        - 时间点
    /// @param  target      - 目标天体
    /// @param  referenceBody- 参考天体
    /// @param  pos         - 位置向量
    /// @retval             - 错误码
    err_t getPosICRF(
        const TimePoint& time,
        EDataCode target,
        EDataCode referenceBody,
        Vector3d& pos
    );
    
    /// @brief  获取对应时间点的目标天体在参考天体下的位置
    /// @details 位置向量的参考系是参考天体为中心的ICRF系
    /// @param  jdTT        - 儒略日(动力学时间)
    /// @param  target      - 目标天体
    /// @param  referenceBody- 参考天体
    /// @param  pos         - 位置向量
    /// @retval             - 错误码
    err_t getPosICRF_TT(
        const JulianDate jdTT,
        EDataCode target,
        EDataCode referenceBody,
        Vector3d& pos
    );

    /// @brief  获取对应时间点的章动角
    /// @param  time        - 时间点
    /// @param  nutLong     - 章动经度(rad)
    /// @param  nutObl      - 章动倾角(rad)
    /// @retval             - 错误码
    err_t	getNutation(
        const TimePoint& time,
        double& nutLong,
        double& nutObl
    );

    
    /// @brief  获取对应时间点的月面天平动相关角度
    /// @param  time        - 时间点
    /// @param  ang         - 从ICRF到月球惯性主轴坐标系的转角(rad)
    ///                     - [omega,i,u],[进动角,章动角,自转角],按照313顺序旋转
    /// @param  angRate     - 转动角速率(rad/s)
    /// @retval             - 错误码
    err_t		getLibration(
        const TimePoint& time,
        Vector3d& ang,
        Vector3d& angRate
    );
    
    /// @brief  获取对应时间点的月面天平动角相关角度
    /// @param  time        - 时间点
    /// @param  ang         - 从ICRF到月球惯性主轴坐标系的转角(rad)
    ///                     - [omega,i,u],[进动角,章动角,自转角],按照313顺序旋转
    /// @retval             - 错误码
    err_t       getLibration(
        const TimePoint& time,
        Vector3d& ang
    );

    /// @brief  获取对应时间点的月面天平动相关角度（Euler）
    /// @param  time        - 时间点
    /// @param  ang         - 从ICRF到月球惯性主轴坐标系的转角(rad)
    ///                     - [omega,i,u],[进动角,章动角,自转角],按照313顺序旋转
    /// @retval             - 错误码
    err_t       getLibration(
        const TimePoint& time,
        Euler& ang
    );
    
    /// @brief  获取JPL DE文件的星历版本
    /// @retval             - 星历版本
    uint32_t getEphemVersion() const{ return m_EphemVerion; }

private:
    err_t         readDataBlock(size_t idx);
    err_t         getStateTDB(const JulianDate& jdTDB, int dataid, double pos[], double vel[]);
    err_t         getStateTDB(const JulianDate& jdTDB, int datalist[11], double statelist[11][6]);

    err_t         getState(const TimePoint& time, int dataid, double pos[], double vel[]);
    err_t         getState(const TimePoint& time, int datalist[11], double statelist[11][6]);
protected:
    bool        m_IsSameEndian;        // 二进制文件与系统大小端是否一致
    uint32_t    m_EphemVerion{ 0 };    // De星历版本
    uint32_t    m_NumConstants;        // 文件的常量个数
    uint32_t    m_NumDataBlock{0};     // 星历总的数据块个数      
    uint32_t    m_NumCoeff;            // 星历每个数据块的系数数量
    uint32_t    m_ipt[15][3]{};        // 数据指针[数据起始位置；每个坐标分量的数据量；子块个数：如4代表了32天要分为四块，每个子块包含了8天的数据]
    double      m_EphemStart{0};       // 星历起始时间(JED)
    double      m_EphemEnd{0};         // 星历结束时间(JED)
    double      m_EphemStep{0};        // 星历每个数据块的天数
    double      m_AU;                  // 光速 km
    double      m_EMMassRatio;         // 地球月球质量比
    FILE*       m_DeFile{ NULL };      // De二进制文件
    double**    m_DataBlocks{NULL};    // 星历数据块的内存缓存
    std::mutex  m_DataBlockMutex;      // 数据块缓存互斥锁
};



/*! @} */

AST_NAMESPACE_END
