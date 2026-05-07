///
/// @file      UiFilePath.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-04
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
#include "UiValueEdit.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup GUI
    @{
*/

/// @brief 文件路径输入框
/// @details 用于输入文件路径，支持通过文件对话框选择文件
class AST_GUI_API UiFilePath: public QWidget
{
    Q_OBJECT

public:
    UiFilePath(QWidget* parent = nullptr);
    
    /// @brief 设置文件路径
    /// @param path 文件路径
    void setPath(const QString& path);

    /// @brief 设置文件路径
    /// @param path 文件路径
    void setPath(StringView path);

    
    /// @brief 获取文件路径
    /// @return 文件路径
    QString path() const;
    
    /// @brief 设置文件过滤器
    /// @param filter 文件过滤器，例如 "SPK文件 (*.bsp);;所有文件 (*.*)"
    void setFilter(const QString& filter);
    
    /// @brief 获取文件过滤器
    /// @return 文件过滤器
    QString filter() const;

signals:
    void pathChanged(const QString& path);
private slots:
    void onBrowse();
    void onEditingFinished();
private:
    UiValueEdit* lineEdit_{nullptr};
    QString currentPath_;
    QString filter_;
};

/*! @} */

AST_NAMESPACE_END