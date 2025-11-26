///
/// @file      RunTime.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
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
#include <string>
 
#define AST_ENV_DATA_DIR  "AST_DATA_DIR"         // 环境变量 AST_DATA_DIR：数据文件夹路径
#define AST_DATA_DIR_NAME "data"                 // 默认的数据文件夹名称


AST_NAMESPACE_BEGIN

class GlobalContext;


/// @brief 初始化
AST_CORE_CAPI err_t aInitialize();

 
/// @brief 获取数据文件夹
AST_CORE_API std::string aDataDirGet();

/// @brief 设置数据文件夹
/// @param dirpath 
AST_CORE_CAPI err_t aDataDirSet(const std::string& dirpath);

/// @brief 获取默认数据文件夹
/// 数据文件夹的顺序：
/// 1. AST_DATA_DIR 环境变量
/// 2. 动态库目录的data文件夹(AST_DATA_DIR_NAME)
/// 3. 可执行文件目录的data文件夹(AST_DATA_DIR_NAME)
/// 4. 当前运行目录的data文件夹(AST_DATA_DIR_NAME)
/// @return 
AST_CORE_API std::string aDataDirGetDefault();

/// @brief 获取当前全局上下文
AST_CORE_CAPI GlobalContext* aGlobalContext_GetCurrent();


AST_CORE_CAPI GlobalContext* aGlobalContext_GetDefault();


AST_CORE_CAPI GlobalContext* aGlobalContext_Ensure();

/// @brief 设置当前全局上下文
AST_CORE_CAPI void aGlobalContext_SetCurrent(GlobalContext* context);

/// @brief 创建一个新的全局上下文
AST_CORE_CAPI GlobalContext* aGlobalContext_New();

 
AST_NAMESPACE_END
 
