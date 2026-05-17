///
/// @file      UiVariableList.hpp
/// @brief     变量列表面板，管理扫参变量列表
/// @details   ~
/// @author    axel
/// @date      2026-05-17
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
#include "AstAnalyzer/AnalyzerVariable.hpp"
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <vector>

class QListWidgetItem;

AST_NAMESPACE_BEGIN

/// @brief 变量列表面板，显示所有扫参变量，支持添加和选中编辑
class AST_GUI_API UiVariableList : public QWidget
{
    Q_OBJECT
public:
    explicit UiVariableList(QWidget* parent = nullptr);

    /// @brief 用变量列表刷新显示
    void setVariables(const std::vector<SharedPtr<AnalyzerVariable>>& variables);

    /// @brief 获取当前选中的变量
    AnalyzerVariable* selectedVariable() const;

signals:
    /// @brief 选中变量变化
    void variableSelected(AnalyzerVariable* variable);

    /// @brief 请求添加新变量
    void addVariableRequested();

private slots:
    void onSelectionChanged();

private:
    void setupUi();

    QVBoxLayout*  layout_;
    QListWidget*  listWidget_;
    QPushButton*  addButton_;
    std::vector<SharedPtr<AnalyzerVariable>> variables_;
};

AST_NAMESPACE_END
