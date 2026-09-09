///
/// @file      VisCelestialBody.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-06
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
#include "VisObject.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstUtil/StringView.hpp"

#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


class AST_VISUALIZATION_API VisCelestialBody: public VisObject
{
public:
    VisCelestialBody() = default;
    ~VisCelestialBody() = default;

    /// @brief 访问者分派
    void accept(VisVisitor& visitor) override;

    /// @brief 获取天体指针
    Body* body() const { return body_.get(); }
    void setBody(Body* body) { body_ = body; }

    /// @brief 天体半径（米），无天体时返回 0
    double radius() const { Body* b = body_.get(); return b ? b->getRadius() : 0.0; }

    /// @brief 显式纹理路径（空表示自动探测）
    /// @return 纹理路径
    const std::string& texture() const { return texture_; }

    /// @brief 设置显式纹理路径
    /// @param path 纹理文件路径
    void setTexture(StringView path) { texture_ = std::string(path); }

    /// @brief 实际纹理路径：显式设置优先，否则按 <天体目录>/<天体名><扩展名> 自动探测
    /// @return 纹理文件路径；找不到时返回空
    std::string effectiveTexture() const;

private:
    WeakPtr<Body>     body_{};
    std::string       texture_{};
};


/// @brief 自动探测天体纹理文件路径
/// @details 按 <天体目录>/<天体名>.<扩展名> 依次尝试 .jpg/.jpeg/.png/.bmp
/// @param body 天体指针；可为 nullptr
/// @return 纹理文件路径；找不到时返回空
AST_VISUALIZATION_API std::string aFindBodyTexture(const Body* body);

/*! @} */

AST_NAMESPACE_END
