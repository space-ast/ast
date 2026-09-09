///
/// @file      UiInsertObjectDialog.hpp
/// @brief     插入对象对话框
/// @details   
/// @author    axel
/// @date      2026-09-07
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

class QTableWidget;
class QListWidget;
class QComboBox;
class QPushButton;

AST_NAMESPACE_BEGIN

/// @brief 插入对象对话框
///
/// 界面布局：
/// - 左侧：对象类型表格，按"场景对象""附加对象"分组，每行两个条目，自带白色列表背景
/// - 右侧：所选对象对应的可选插入方法列表
/// - 底部："创建默认卫星"复选、"中心天体"下拉、以及"插入 / 关闭 / 帮助"按钮
class AST_GUI_API UiInsertObjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UiInsertObjectDialog(QWidget* parent = nullptr);
    ~UiInsertObjectDialog() override;
    UiInsertObjectDialog(const UiInsertObjectDialog&) = delete;
    UiInsertObjectDialog& operator=(const UiInsertObjectDialog&) = delete;

    /// @brief 当前选中的对象类型名（如 "Satellite"）
    QString selectedObjectType() const;

    /// @brief 当前选中的插入方法名（如 "Orbit Wizard"）
    QString selectedMethod() const;

    /// @brief 中心天体名称（如 "Earth"）
    QString centralBody() const;

signals:
    /// @brief 用户点击"插入..."后发出，携带本次选择，交由调用方真正插入对象
    void objectInsertRequested(const QString& objectType, const QString& method,
                               const QString& centralBody);

private:
    void setupUi();
    void setupConnections();
    void onObjectTableSelectionChanged();
    void selectObjectByType(const QString& type);
    void onInsertClicked();

private:
    QString         currentObjectType_{};         ///< 当前选中的对象类型
    QTableWidget*   objectTable_ = nullptr;      ///< 场景对象表格（每行两条目）
    QTableWidget*   attachedTable_ = nullptr;    ///< 附加对象表格（每行两条目）
    QListWidget*    methodList_ = nullptr;       ///< 插入方法列表
    QComboBox*      centralBodyCombo_ = nullptr; ///< 中心天体下拉框
    QPushButton*    insertButton_ = nullptr;     ///< 插入按钮
    QPushButton*    closeButton_ = nullptr;      ///< 关闭按钮
};

AST_NAMESPACE_END
