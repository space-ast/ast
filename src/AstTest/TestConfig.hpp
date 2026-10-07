///
/// @file      TestConfig.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-21
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "AstUtil/StartupConfig.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 获取测试配置
/// @return 测试配置指针
AST_TEST_CAPI StartupConfig* aTestGetConfig();


/// @brief 获取测试数据目录
/// @details 解析顺序：
///          1. AST_TEST_DATA_DIR 环境变量；
///          2. 动态库所在文件夹的 test-data 目录
/// @return 测试数据目录路径
AST_TEST_API std::string aTestDataDirGet();


/// @brief 获取测试配置值
/// @param key 配置键
/// @return 配置值视图
AST_TEST_API ValueView aTestGetConfigValue(StringView key);



/// @brief 获取测试配置值向量
/// @param key 配置键
/// @return 配置值向量
AST_TEST_API std::vector<std::string> aTestGetConfigStringVector(StringView key);


/// @brief 获取测试配置值视图向量
/// @param key 配置键
/// @return 配置值视图向量
AST_TEST_API std::vector<StringView> aTestGetConfigStringViewVector(StringView key);


/*! @} */

AST_NAMESPACE_END
