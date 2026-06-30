///
/// @file      UiDataUpdate.hpp
/// @brief     动态数据更新对话框
/// @details   提供 Qt 界面对话框，让用户查看数据文件状态并手动触发更新。
/// @author    axel
/// @date      2026-06-30
/// @copyright 版权所有 (C) 2026-present, ast项目.

#pragma once

#include "AstUiDataUpdateGlobal.hpp"
#include "AstDataUpdate/DataUpdater.hpp"

#include <QDialog>
#include <atomic>

class QTableWidget;
class QPushButton;
class QLabel;

AST_NAMESPACE_BEGIN

/// @brief 动态数据更新对话框
/// @details
/// 展示所有可管理的动态数据文件，包括：
/// - 数据名称和描述
/// - 当前数据日期
/// - 备份文件数量
/// - 过期/最新状态
///
/// @note 更新后的数据文件将在软件下次启动时生效，更新完成后请重启软件。
class AST_UIDATAUPDATE_API UiDataUpdate : public QDialog
{
    Q_OBJECT

public:
    explicit UiDataUpdate(QWidget* parent = nullptr);
    ~UiDataUpdate() override;

    /// @brief 取消所有进行中的更新任务（关闭对话框时调用）
    void reject() override;

private slots:
    void onUpdateRow();
    void onUpdateAll();
    void onRefresh();
    void onOpenLocation();
    void onSelectionChanged();

private:
    void setupUi();
    void refreshTable();
    void setRowStatus(int row, const DataUpdater::DataFileEntry& entry);

    DataUpdater         updater_;
    std::atomic<bool>   cancelPending_{false};
    QTableWidget*  table_        = nullptr;
    QPushButton*   refreshBtn_   = nullptr;
    QPushButton*   updateAllBtn_ = nullptr;
    QPushButton*   closeBtn_     = nullptr;
    QLabel*        statusLabel_  = nullptr;
    QLabel*        descLabel_    = nullptr;
};

AST_NAMESPACE_END
