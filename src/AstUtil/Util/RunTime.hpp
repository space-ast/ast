///
/// @file      RunTime.hpp
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

#pragma once

#include "AstGlobal.h"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Util
    @{
*/


class DataContext;

#define AST_ENV_DATA_DIR  "AST_DATA_DIR"         // 环境变量 AST_DATA_DIR：数据文件夹路径
#define AST_DATA_DIR_NAME "data"                 // 默认的数据文件夹名称
#define AST_ENV_CACHE_DIR  "AST_CACHE_DIR"       // 环境变量 AST_CACHE_DIR：缓存文件夹路径
#define AST_CACHE_DIR_NAME "cache"               // 默认的缓存文件夹名称
#define AST_DEFAULT_DIR_AEP8                    "SolarSystem/Earth/aep8/"
#define AST_DEFAULT_DIR_IGRF                    "SolarSystem/Earth/igrf/"
#define AST_DEFAULT_DIR_IRBEM                   "SolarSystem/Earth/irbem/"


/// @brief 获取默认数据文件夹
/// 数据文件夹的顺序：
/// 1. AST_DATA_DIR 环境变量
/// 2. 动态库目录的data文件夹(AST_DATA_DIR_NAME)
/// 3. 可执行文件目录的data文件夹(AST_DATA_DIR_NAME)
/// 4. 当前运行目录的data文件夹(AST_DATA_DIR_NAME)
/// @return 
AST_UTIL_API std::string aDataDirGetDefault();

/// @brief 获取默认数据文件夹
/// @see aDataDirGetDefault()
/// @param[out] datadir 默认数据文件夹路径
/// @return 错误码
AST_UTIL_CAPI errc_t aDataDirGetDefault(std::string& datadir);

/// @brief 获取数据文件夹
/// @note 与 aDataDirGet 一样
AST_UTIL_API std::string aDataDir();


/// @brief 获取数据文件夹
/// @note 与 aDataDirGet 一样
AST_UTIL_CAPI errc_t aDataDir(std::string& datadir);


/// @brief 获取缓存文件夹
/// 缓存文件夹的顺序：
/// 1. AST_CACHE_DIR 环境变量
/// 2. Windows: %LOCALAPPDATA%/ast/cache；POSIX: $XDG_CACHE_HOME/ast 或 $HOME/.cache/ast
/// 3. 动态库目录的 cache 文件夹(AST_CACHE_DIR_NAME)
/// @note 只解析路径，不创建文件夹
/// @param[out] cachedir 缓存文件夹路径
/// @return 错误码
AST_UTIL_CAPI errc_t aCacheDir(std::string& cachedir);


/// @brief 获取缓存文件夹
/// @see aCacheDir(std::string&)
AST_UTIL_API std::string aCacheDir();


/*! @} */

AST_NAMESPACE_END
