///
/// @file      UiFeasibleRegionStudy.hpp
/// @brief     FeasibleRegionStudy 段编辑器
/// @details   编辑可行域研究的变量、约束和关联命令
/// @author    axel
/// @date      2026-06-11
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
#include "AstGUI/UiObject.hpp"
#include <QWidget>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

AST_NAMESPACE_BEGIN

class FeasibleRegionStudy;
class FeasibleRegionVariable;
class FeasibleRegionConstraint;
class Expr;

/// @brief FeasibleRegionStudy 段编辑器
/// @details 以标签页形式编辑可行域研究的变量、约束和关联命令
class AST_GUI_API UiFeasibleRegionStudy : public UiObject
{
    Q_OBJECT
public:
    explicit UiFeasibleRegionStudy(Object* object, QWidget* parent = nullptr);
    explicit UiFeasibleRegionStudy(QWidget* parent = nullptr);
    ~UiFeasibleRegionStudy() override;
    UiFeasibleRegionStudy(const UiFeasibleRegionStudy&) = delete;
    UiFeasibleRegionStudy& operator=(const UiFeasibleRegionStudy&) = delete;

    void setStudy(FeasibleRegionStudy* study);
    FeasibleRegionStudy* getStudy() const;

private slots:
    // ---- 变量表 ----
    void onAddVariable();
    void onRemoveVariable();
    void onVariableCellChanged(int row, int col);
    void onVariableExprClicked(int row);

    // ---- 约束表 ----
    void onAddConstraint();
    void onRemoveConstraint();
    void onConstraintCellChanged(int row, int col);
    void onConstraintExprClicked(int row);

private:
    void setupUi();
    void setupVariablesTab(QWidget* tab);
    void setupConstraintsTab(QWidget* tab);
    void setupSettingsTab(QWidget* tab);
    void refreshFromStudy();
    void refreshVariablesTable();
    void refreshConstraintsTable();
    void updateTotalRunsLabel();

    /// @brief 弹出表达式浏览器，返回选中的表达式（调用方负责管理生命周期）
    static Expr* browseExpression(QWidget* parent);

    // ---- 变量列索引 ----
    enum VarCol { VAR_NAME = 0, VAR_EXPR = 1, VAR_LOWER = 2, VAR_UPPER = 3, VAR_STEPS = 4, VAR_COUNT = 5 };

    // ---- 约束列索引 ----
    enum CtrCol { CTR_NAME = 0, CTR_EXPR = 1, CTR_USE_LOWER = 2, CTR_LOWER = 3,
                  CTR_USE_UPPER = 4, CTR_UPPER = 5, CTR_EXCLUDE = 6, CTR_COUNT = 7 };

    QTabWidget*    tabWidget_ = nullptr;

    // 变量页
    QTableWidget*  varTable_ = nullptr;
    QPushButton*   addVarBtn_ = nullptr;
    QPushButton*   removeVarBtn_ = nullptr;

    // 约束页
    QTableWidget*  ctrTable_ = nullptr;
    QPushButton*   addCtrBtn_ = nullptr;
    QPushButton*   removeCtrBtn_ = nullptr;

    // 设置页
    QLabel*        totalRunsLabel_ = nullptr;
};

AST_NAMESPACE_END
