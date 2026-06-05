///
/// @file      UiNewObjectQuickDialog.hpp
/// @brief     快速新建对象对话框，以分类网格展示常用可创建类型
/// @details   相较于 UiNewObjectDialog 列出所有可实例化类型，本对话框仅展示精选常用类型，
///            按类别分组排列为网格卡片，支持需要父对象的类型在创建时选择父对象。
///            老对话框通过"高级"按钮打开。
/// @author    axel
/// @date      2026-06-05
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
#include <QDialog>
#include <QString>
#include <vector>

class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QTreeWidget;
class QTreeWidgetItem;

AST_NAMESPACE_BEGIN

class Object;

/// @brief 快速创建类型条目配置
struct QuickTypeEntry
{
    const char* typeName;   ///< RTTI 类型名（用于创建）
    const char* category;   ///< 分类标签（用于分组标题）
    const char* parentType; ///< 需要的父对象类型名，nullptr 表示无需父对象
};

/// @brief 快速新建对象对话框
/// @details 以分类网格卡片形式展示常用可创建类型，
///          支持搜索过滤、父对象选择，提供"高级"入口到完整类型列表
class AST_GUI_API UiNewObjectQuickDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UiNewObjectQuickDialog(QWidget* parent = nullptr);
    ~UiNewObjectQuickDialog() override;

    /// @brief 获取创建成功后的对象显示名称（用于状态栏提示）
    QString createdObjectName() const;

private:
    void setupUi();
    void buildTypeGrid();
    void onTypeCardClicked(int entryIndex);
    void onChooseParent();
    void onAdvanced();
    void onCreate();
    QString generateUniqueName(const QString& typeName) const;

    /// @brief 弹出父对象选择对话框，返回选中的对象，取消则返回 nullptr
    Object* showParentDialog(const char* parentType);
    /// @brief 递归将匹配 parentType 的对象及其层级路径添加到指定树中
    void populateParentTree(QTreeWidget* tree, QTreeWidgetItem* parentItem,
                            Object* obj, const char* parentType);

    /// @brief 全部可快速创建的类型条目
    static std::vector<QuickTypeEntry> quickTypes();

    // 类型表格
    QTableWidget* typeTable_ = nullptr;

    // 父对象选择
    QPushButton* parentBtn_ = nullptr;

    // 名称 / 按钮
    QLineEdit*   nameEdit_ = nullptr;
    QPushButton* createBtn_ = nullptr;
    QPushButton* advancedBtn_ = nullptr;

    // 状态
    int           selectedEntry_ = -1;
    Object*       selectedParent_ = nullptr;
    QString       createdObjectName_;
};

AST_NAMESPACE_END
