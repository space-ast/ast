///
/// @file      FOVCustom.hpp
/// @brief     自定义视场类
/// @details   实现自定义视场的属性和方法
/// @author    axel
/// @date      2026-04-09
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

#include "FieldOfView.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/// @brief 自定义视场类
class AST_CORE_API FOVCustom: public FieldOfView
{
public:
    /// @brief 构造函数
    FOVCustom() = default;
    
    /// @brief 析构函数
    virtual ~FOVCustom() = default;
    
    /// @brief 获取视场类型
    /// @return 视场类型
    virtual EFOVType getFOVType() const override { return EFOVType::eCustom; }
    
    /// @brief 接受访问者
    /// @param visitor 访问者
    virtual void accept(FieldOfViewVisitor& visitor) override { visitor.visit(*this); }
    
    /// @brief 添加顶点
    /// @param vertex 顶点坐标
    void addVertex(const Vector3d& vertex) { vertices_.push_back(vertex); }
    
    /// @brief 获取顶点列表
    /// @return 顶点列表
    const std::vector<Vector3d>& getVertices() const { return vertices_; }
    
private:
    std::vector<Vector3d> vertices_{};  ///< 自定义视场的顶点列表
};

AST_NAMESPACE_END