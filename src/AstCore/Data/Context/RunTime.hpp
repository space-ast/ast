///
/// @file      RunTime.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      25.11.2025
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
#include "AstUtil/RunTime.hpp"
#include "RunTimeData.hpp"
#include <string>
#include <vector>
 
#define AST_DEFAULT_FILE_LEAPSECOND             "Time/Leap_Second.dat"
#define AST_DEFAULT_FILE_JPLDE                  "SolarSystem/plneph.430"
#define AST_DEFAULT_FILE_EOP                    "SolarSystem/Earth/EOP-All.txt"
#define AST_DEFAULT_FILE_SPACEWEATHER           "SolarSystem/Earth/SW-Last5Years.txt"
#define AST_DEFAULT_FILE_IAUX                   "IERS-conventions/2010/tab5.2a.txt"
#define AST_DEFAULT_FILE_IAUY                   "IERS-conventions/2010/tab5.2b.txt"
#define AST_DEFAULT_FILE_IAUS                   "IERS-conventions/2010/tab5.2d.txt"
#define AST_DEFAULT_FILE_IAUXYS_PRECOMPUTED     "Test/ICRF/IAU2006_XYS.dat"
#define AST_DEFAULT_DIR_SOLARSYSTEM             "SolarSystem/"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/

class DataContext;
class GlobalContext;
class EOP;
class IAUXYS;
class IAUXYSPrecomputed;
class SolarSystem;
class StartupConfig;
using StringVector = std::vector<std::string>;

/// @brief 初始化配置
struct InitalizeConfig
{
    std::string dataDir_{};                 ///< 数据目录路径
    std::string leapSecondFile_{};          ///< 跳秒文件路径
    std::string jplDeFile_{};                ///< JPL DE文件路径
    std::string eopFile_{};                  ///< EOP文件路径
    std::string spaceWeatherFile_{};         ///< 空间天气文件路径
    std::string iauxFile_{};                 ///< IAU-X系数文件路径
    std::string iauyFile_{};                 ///< IAU-Y系数文件路径
    std::string iausFile_{};                 ///< IAU-Z系数文件路径
    std::string iauXYSPrecomputedFile_{};    ///< IAU-XYS预计算数据文件路径
    std::string solarSystemDir_{};           ///< 太阳系目录路径
    StringVector spkFiles_{};                ///< SPK文件路径列表
};

/// @brief 获取初始化配置
/// @param[out] config 初始化配置
/// @return 错误码
AST_CORE_CAPI void aGetInitalizeConfig(InitalizeConfig& config);

/// @brief 初始化
AST_CORE_CAPI errc_t aInitialize();

/// @brief 通过配置文件初始化
AST_CORE_CAPI errc_t aInitializeByConfig(StringView configfile);

/// @brief 初始化配置
AST_CORE_CAPI errc_t aInitializeConfig(DataContext* context);

/// @brief 反初始化
AST_CORE_CAPI errc_t aUninitialize();


/// @brief 获取默认SPK目录
AST_CORE_API std::string aGetDefaultSPKDir();

 
/// @brief 获取数据文件夹
/// @return 数据文件夹路径
AST_CORE_API std::string aDataDirGet();

/// @brief 获取数据文件夹
/// @param[out] datadir 数据文件夹路径
/// @return 错误码
AST_CORE_CAPI errc_t aDataDirGet(std::string& datadir);


/// @brief 设置数据文件夹
/// @param dirpath 
AST_CORE_CAPI errc_t aDataDirSet(StringView dirpath);


// global context

/// @brief 获取全局上下文
AST_CORE_CAPI GlobalContext* aGlobalContext_Get();

/// @brief 获取全局上下文的IAUXYS
AST_CORE_CAPI IAUXYS* aGlobalContext_GetIAUXYS();


// data context


/// @brief 获取默认数据上下文
/// 如果默认数据上下文不存在，则创建一个新的默认数据上下文
/// @return 默认数据上下文
AST_CORE_CAPI DataContext* aDataContext_GetDefault();


/// @brief 获取当前线程的数据上下文
/// 如果当前线程的数据上下文不存在，则将默认数据上下文设置为当前线程的数据上下文
/// @return 当前线程的数据上下文
AST_CORE_CAPI DataContext* aDataContext_GetCurrent();

/// @brief 确保当前线程的数据上下文存在
/// 如果当前线程的数据上下文不存在，则将默认数据上下文设置为当前线程的数据上下文
/// 如果当前线程的数据上下文未初始化，则初始化
/// @return 当前线程的数据上下文
AST_CORE_CAPI DataContext* aDataContext_EnsureCurrent();

/// @brief 设置当前线程的数据上下文
AST_CORE_CAPI void aDataContext_SetCurrent(DataContext* context);

/// @brief 获取当前全局上下文的EOP
AST_CORE_CAPI EOP* aDataContext_GetEOP();

/// @brief 获取当前全局上下文的太阳系
AST_CORE_CAPI SolarSystem* aDataContext_GetSolarSystem();

/// @brief 获取当前线程的数据上下文的IAUXYSPrecomputed
AST_CORE_CAPI IAUXYSPrecomputed* aDataContext_GetIAUXYSPrecomputed();


/// @brief 获取当前线程的数据上下文的配置
/// @return 
AST_CORE_CAPI StartupConfig* aDataContext_GetConfig();

/// @brief 创建一个新的全局上下文
AST_CORE_CAPI DataContext* aDataContext_New();


/*! @} */

AST_NAMESPACE_END
 
