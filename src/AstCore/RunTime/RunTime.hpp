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
#include "RunTimeData.hpp"
#include <string>
 
#define AST_ENV_DATA_DIR  "AST_DATA_DIR"         // 环境变量 AST_DATA_DIR：数据文件夹路径
#define AST_DATA_DIR_NAME "data"                 // 默认的数据文件夹名称


AST_NAMESPACE_BEGIN

class DataContext;
class EOP;
class IAUXYSPrecomputed;


/// @brief 初始化
AST_CORE_CAPI err_t aInitialize();

/// @brief 反初始化
AST_CORE_CAPI err_t aUninitialize();

 
/// @brief 获取数据文件夹
AST_CORE_API std::string aDataDirGet();

/// @brief 设置数据文件夹
/// @param dirpath 
AST_CORE_CAPI err_t aDataDirSet(StringView dirpath);

/// @brief 获取默认数据文件夹
/// 数据文件夹的顺序：
/// 1. AST_DATA_DIR 环境变量
/// 2. 动态库目录的data文件夹(AST_DATA_DIR_NAME)
/// 3. 可执行文件目录的data文件夹(AST_DATA_DIR_NAME)
/// 4. 当前运行目录的data文件夹(AST_DATA_DIR_NAME)
/// @return 
AST_CORE_API std::string aDataDirGetDefault();

/// @brief 获取当前线程的数据上下文
/// @return 
AST_CORE_CAPI DataContext* aDataContext_GetCurrent();

/// @brief 获取默认数据上下文
AST_CORE_CAPI DataContext* aDataContext_GetDefault();


/// @brief 确保默认数据上下文存在
/// 如果默认数据上下文不存在，则创建一个新的默认数据上下文。
/// @return 
AST_CORE_CAPI DataContext* aDataContext_EnsureDefault();


/// @brief 确保当前线程的数据上下文存在
/// 如果当前线程的数据上下文不存在，则将默认数据上下文设置为当前线程的数据上下文。
/// @return 
AST_CORE_CAPI DataContext* aDataContext_EnsureCurrent();

/// @brief 设置当前线程的数据上下文
AST_CORE_CAPI void aDataContext_SetCurrent(DataContext* context);

/// @brief 获取当前全局上下文的EOP
AST_CORE_CAPI EOP* aDataContext_GetEOP();

/// @brief 获取当前线程的数据上下文的IAUXYSPrecomputed
AST_CORE_CAPI IAUXYSPrecomputed* aDataContext_GetIAUXYSPrecomputed();


/// @brief 创建一个新的全局上下文
AST_CORE_CAPI DataContext* aDataContext_New();



AST_NAMESPACE_END
 
