///
/// @file      DataPort.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-17
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
// #include "AstUtil/Dimension.hpp"    // for Dimension

AST_NAMESPACE_BEGIN

/// @brief 数据端口
class DataPort
{
public:
    enum EValueType{
        eDouble,
        eFloat,
        eInt,
    };
    DataPort() = default;
    ~DataPort() = default;

    AST_CORE_API
    static err_t connect(DataPort* src, DataPort* dst);

    AST_CORE_API
    err_t connect(DataPort* dst);

    /// @brief 设置数据端口的宽度
    /// @param width 数据宽度
    void setWidth(int width){ width_ = width; }
    
    /// @brief 获取数据端口的宽度
    /// @return 数据宽度
    int getWidth() const{ return width_; }

    /// @brief 获取数据端口的数据类型
    /// @return 数据类型
    EValueType getDataType() const{ return type_; }

    /// @brief 获取数据端口的实数值指针
    /// @warning 该接口不保证类型安全，调用者需自行确保类型匹配
    /// @return 实数值指针
    double* getSignalDouble() const{ return this->getSignal<double>(); }

    /// @brief 获取数据端口的整数值指针
    /// @warning 该接口不保证类型安全，调用者需自行确保类型匹配
    /// @return 整数值指针
    int* getSignalInt() const{ return this->getSignal<int>(); }

    /// @brief 获取数据端口的向量3值指针
    /// @warning 该接口不保证类型安全，调用者需自行确保类型匹配
    /// @return 向量3值指针
    Vector3d* getSignalVector3d() const{ return this->getSignal<Vector3d>(); }
    
    /// @brief 获取数据端口的任意类型值指针
    /// @warning 该接口不保证类型安全，调用者需自行确保类型匹配
    /// @return 任意类型值指针
    template<typename T=void>
    T* getSignal() const{ return (T*)*signal_; }

    /// @brief 获取数据端口的信号指针(指向数据指针的指针)
    /// @warning 该接口不保证类型安全，调用者需自行确保类型匹配
    /// @return 信号指针(指向数据指针的指针)
    ptr_t* getSignalPtr() const{ return signal_; }

public:
    Identifier* name_;              ///< 端口名称
    ptr_t*      signal_;            ///< 信号指针(指向数据指针的指针)
    int         width_;             ///< 数据宽度
    EValueType  type_;              ///< 数据类型
    // Dimension   dimension_;      ///< 数据量纲 @todo: 支持数据量纲
};


AST_NAMESPACE_END
