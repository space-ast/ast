///
/// @file      ObjectProtocol.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstUtil/StringView.hpp"
#include "AstAI/AgentInit.hpp"
#include <string>
#include <string_view>
#include <optional>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

#define _WASM_OBJ_CALL static

/// @brief WASM模块对象信息
struct WasmObjectInfo
{
    ObjectId id;                        ///< 对象ID
    std::optional<ObjectId> parent;     ///< 父对象ID
    std::string type;                   ///< 类型
    std::string name;                   ///< 名称
    std::vector<ObjectId> children;     ///< 子对象ID列表
};

/// @brief WASM模块类信息
struct WasmClassInfo
{
    std::string name;                   ///< 类名
    std::string parent;                 ///< 父类名
};


/// @brief WASM模块运行时协议，与对象管理相关的接口集合
class WasmRuntimeProtocol
{
public:
    /// @brief 设置对象属性
    /// @details id 对象索引/对象ID
    /// @param name 属性名
    /// @param value 属性值
    /// @return 错误码
    _WASM_OBJ_CALL errc_t setAttrString(ObjectId id, const std::string& name, const std::string& value);
    _WASM_OBJ_CALL errc_t setAttrDouble(ObjectId id, const std::string& name, double value);
    _WASM_OBJ_CALL errc_t setAttrInt(ObjectId id, const std::string& name, int value);
    _WASM_OBJ_CALL errc_t setAttrBool(ObjectId id, const std::string& name, bool value);
    _WASM_OBJ_CALL errc_t setAttrObject(ObjectId id, const std::string& name, ObjectId value);    

    /// @brief 获取对象属性
    /// @details id 对象索引/对象ID
    /// @param name 属性名
    /// @return 属性值
    _WASM_OBJ_CALL std::optional<std::string> getAttrString(ObjectId id, const std::string& name);
    _WASM_OBJ_CALL std::optional<double> getAttrDouble(ObjectId id, const std::string& name);
    _WASM_OBJ_CALL std::optional<int> getAttrInt(ObjectId id, const std::string& name);
    _WASM_OBJ_CALL std::optional<bool> getAttrBool(ObjectId id, const std::string& name);
    _WASM_OBJ_CALL std::optional<ObjectId> getAttrObject(ObjectId id, const std::string& name);

    /// @brief 获取对象类型
    /// @details id 对象索引/对象ID
    /// @return 对象类型名
    _WASM_OBJ_CALL std::optional<std::string> objectType(ObjectId id);

    /// @brief 获取对象信息
    /// @details id 对象索引/对象ID
    /// @return 对象信息
    _WASM_OBJ_CALL std::optional<WasmObjectInfo> objectInfo(ObjectId id);

    
    /// @brief 获取所有对象ID
    /// @details 所有对象ID
    /// @return 所有对象ID
    _WASM_OBJ_CALL std::vector<ObjectId> allObjects();

    /// @brief 获取所有对象信息
    /// @details 所有对象信息
    /// @return 所有对象信息
    _WASM_OBJ_CALL std::vector<WasmObjectInfo> allObjectInfoList();

    /// @brief 创建新对象
    /// @details typeName 对象类型名
    /// @param parentId 父对象ID
    /// @return 对象ID
    _WASM_OBJ_CALL std::optional<ObjectId> newObject(const std::string& typeName, std::optional<ObjectId> parentId = std::nullopt);

    /// @brief 移除对象
    /// @details id 对象索引/对象ID
    /// @return 错误码
    _WASM_OBJ_CALL errc_t removeObject(ObjectId id);


    /// @brief 将对象转换为JSON字符串
    /// @details id 对象索引/对象ID
    /// @return JSON字符串
    _WASM_OBJ_CALL std::optional<std::string> objectToJsonStr(ObjectId id);


    /// @brief 获取类的JSON Schema
    /// @details typeName 类名
    /// @return JSON模式
    _WASM_OBJ_CALL std::optional<std::string> classJsonSchemaStr(const std::string& typeName);


    /// @brief 获取所有类名
    /// @details 所有类名
    /// @return 所有类名
    _WASM_OBJ_CALL std::vector<std::string> getAllClassNames();


    /// @brief 处理单次智能体工具调用
    _WASM_OBJ_CALL std::string handleToolCall(const std::string& toolCallJsonStr);
};

/*! @} */

AST_NAMESPACE_END


