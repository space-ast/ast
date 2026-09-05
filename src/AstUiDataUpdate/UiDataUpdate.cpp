///
/// @file      UiDataUpdate.cpp
/// @brief     动态数据更新对话框实现
/// @author    axel
/// @date      2026-06-30
/// @copyright 版权所有 (C) 2026-present, ast项目.

#include "UiDataUpdate.hpp"

#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QList>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#ifndef A_WASM   // wasm 上无 QtConcurrent，且 QFuture/QFutureWatcher 依赖的 future 特性被禁用
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#endif
#include <atomic>
#include <memory>

AST_NAMESPACE_BEGIN

enum EColumn
{
    eColName = 0,
    eColDate,
    eColFile,
    eColUrl,
    eColStatus,
    eColAction,
    eColCount
};

UiDataUpdate::UiDataUpdate(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    refreshTable();
}

UiDataUpdate::~UiDataUpdate() = default;

void UiDataUpdate::reject()
{
    cancelPending_.store(true, std::memory_order_release);
    QDialog::reject();
}

void UiDataUpdate::setupUi()
{
    setWindowTitle(QString::fromUtf8("动态数据更新"));
    resize(1050, 520);

    auto* mainLayout = new QVBoxLayout(this);

    table_ = new QTableWidget(0, eColCount, this);
    table_->setHorizontalHeaderLabels({
        QString::fromUtf8("名称"),
        QString::fromUtf8("数据日期"),
        QString::fromUtf8("本地文件"),
        QString::fromUtf8("远程网址"),
        QString::fromUtf8("状态"),
        QString::fromUtf8("操作"),
    });
    table_->horizontalHeader()->setStretchLastSection(false);
    table_->horizontalHeader()->setSectionResizeMode(eColName, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(eColDate, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(eColFile, QHeaderView::Interactive);
    table_->setColumnWidth(eColFile, 160);
    table_->horizontalHeader()->setSectionResizeMode(eColUrl,  QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(eColStatus, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(eColAction, QHeaderView::ResizeToContents);
    table_->verticalHeader()->setDefaultSectionSize(42);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->verticalHeader()->setVisible(false);
    connect(table_, &QTableWidget::currentCellChanged,
            this, &UiDataUpdate::onSelectionChanged);
    mainLayout->addWidget(table_);

    // 描述面板
    descLabel_ = new QLabel(this);
    descLabel_->setWordWrap(true);
    descLabel_->setMinimumHeight(36);
    descLabel_->setStyleSheet(
        "QLabel { background: #f5f5f5; border-radius: 4px; padding: 8px 12px; color: #555; }");
    descLabel_->setVisible(false);
    mainLayout->addWidget(descLabel_);

    auto* hintLabel = new QLabel(
        QString::fromUtf8("更新后的数据文件将在软件下次启动时生效，请更新完成后重启软件。"), this);
    hintLabel->setStyleSheet("color: #856404; padding: 6px 10px;");
    hintLabel->setWordWrap(true);
    mainLayout->addWidget(hintLabel);

    auto* btnLayout = new QHBoxLayout();

    statusLabel_ = new QLabel(this);
    btnLayout->addWidget(statusLabel_, 1);

    refreshBtn_ = new QPushButton(QString::fromUtf8("刷新"), this);
    connect(refreshBtn_, &QPushButton::clicked, this, &UiDataUpdate::onRefresh);
    btnLayout->addWidget(refreshBtn_);

    updateAllBtn_ = new QPushButton(QString::fromUtf8("全部更新"), this);
    connect(updateAllBtn_, &QPushButton::clicked, this, &UiDataUpdate::onUpdateAll);
    btnLayout->addWidget(updateAllBtn_);

    closeBtn_ = new QPushButton(QString::fromUtf8("关闭"), this);
    connect(closeBtn_, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(closeBtn_);

    mainLayout->addLayout(btnLayout);
}

void UiDataUpdate::refreshTable()
{
    auto entries = updater_.refreshFileList();
    table_->setRowCount(static_cast<int>(entries.size()));

    for (size_t i = 0; i < entries.size(); ++i)
    {
        const auto& entry = entries[i];
        int row = static_cast<int>(i);

        // 名称
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(entry.name));
        nameItem->setData(Qt::UserRole, QString::fromStdString(entry.description));
        table_->setItem(row, eColName, nameItem);

        // 数据日期
        QString dateText = entry.fileDate.empty()
            ? QString::fromUtf8("（无更新日期）")
            : QString::fromStdString(entry.fileDate);
        table_->setItem(row, eColDate, new QTableWidgetItem(dateText));

        // 本地文件
        QString localPath = QString::fromStdString(entry.localPath);
        auto* fileItem = new QTableWidgetItem(QFileInfo(localPath).fileName());
        fileItem->setToolTip(localPath);
        table_->setItem(row, eColFile, fileItem);

        // 远程网址
        QString urlStr = QString::fromStdString(entry.url);
        auto* urlItem = new QTableWidgetItem(urlStr);
        urlItem->setToolTip(urlStr);
        table_->setItem(row, eColUrl, urlItem);

        setRowStatus(row, entry);
    }

    updateAllBtn_->setEnabled(true);
    statusLabel_->setText(QString::fromUtf8("共 %1 个数据文件").arg(entries.size()));
    descLabel_->setVisible(false);
}

void UiDataUpdate::setRowStatus(int row, const DataUpdater::DataFileEntry& entry)
{
    // 状态
    QString statusText;
    QColor  statusColor;

    if (entry.fileDate.empty() && entry.isOutdated)
    {
        statusText  = QString::fromUtf8("文件缺失");
        statusColor = QColor(200, 50, 50);
    }
    else if (entry.fileDate.empty())
    {
        statusText  = QString::fromUtf8("已是最新");
        statusColor = QColor(50, 150, 50);
    }
    else if (entry.isOutdated)
    {
        statusText  = QString::fromUtf8("建议更新");
        statusColor = QColor(200, 150, 0);
    }
    else
    {
        statusText  = QString::fromUtf8("已是最新");
        statusColor = QColor(50, 150, 50);
    }

    auto* statusItem = new QTableWidgetItem(statusText);
    statusItem->setForeground(statusColor);
    table_->setItem(row, eColStatus, statusItem);

    // 操作按钮
    auto* actionWidget = new QWidget();
    auto* actionLayout = new QHBoxLayout(actionWidget);
    actionLayout->setContentsMargins(2, 2, 2, 2);
    actionLayout->setSpacing(2);

    auto* updateBtn = new QPushButton(QString::fromUtf8("更新"));
    updateBtn->setProperty("row", row);
    connect(updateBtn, &QPushButton::clicked, this, &UiDataUpdate::onUpdateRow);
    actionLayout->addWidget(updateBtn);

    auto* openBtn = new QPushButton(QString::fromUtf8("打开位置"));
    openBtn->setProperty("row", row);
    connect(openBtn, &QPushButton::clicked, this, &UiDataUpdate::onOpenLocation);
    actionLayout->addWidget(openBtn);

    table_->setCellWidget(row, eColAction, actionWidget);
}

void UiDataUpdate::onSelectionChanged()
{
    int row = table_->currentRow();
    if (row < 0)
    {
        descLabel_->setVisible(false);
        return;
    }

    auto* item = table_->item(row, eColName);
    if (!item) return;

    QString desc = item->data(Qt::UserRole).toString();
    if (desc.isEmpty())
    {
        descLabel_->setVisible(false);
    }
    else
    {
        descLabel_->setText(desc);
        descLabel_->setVisible(true);
    }
}

void UiDataUpdate::onUpdateRow()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = btn->property("row").toInt();
    auto entries = updater_.refreshFileList();
    if (row < 0 || row >= static_cast<int>(entries.size())) return;

    btn->setEnabled(false);
    btn->setText(QString::fromUtf8("更新中..."));
    statusLabel_->setText(QString::fromUtf8("正在更新「%1」...").arg(
        QString::fromStdString(entries[row].name)));

    // 拷贝数据条目，在工作线程中执行更新
    DataUpdater::DataFileEntry entryCopy = entries[row];

    // 单次更新完成后的公共处理：取消时不刷新界面，否则更新状态并展示结果
    auto handleResult = [this, entryCopy](errc_t err) {
        if (cancelPending_.load(std::memory_order_acquire))
            return;
        if (err == eNoError)
        {
            statusLabel_->setText(QString::fromUtf8("「%1」更新成功").arg(
                QString::fromStdString(entryCopy.name)));
        }
        else
        {
            statusLabel_->setText(QString::fromUtf8("「%1」更新失败 (err=%2)").arg(
                QString::fromStdString(entryCopy.name)).arg(err));
            QMessageBox::warning(this,
                QString::fromUtf8("更新失败"),
                QString::fromUtf8("「%1」下载或校验失败。\n旧文件未被修改。\n错误码：%2").arg(
                    QString::fromStdString(entryCopy.name)).arg(err));
        }
        refreshTable();
    };

#ifndef A_WASM
    auto* watcher = new QFutureWatcher<errc_t>(this);
    connect(watcher, &QFutureWatcher<errc_t>::finished, this, [watcher, handleResult]() {
        handleResult(watcher->result());
        watcher->deleteLater();
    });
    watcher->setFuture(QtConcurrent::run([this, entryCopy]() mutable {
        return updater_.updateFile(entryCopy);
    }));
#else
    // wasm 无多线程与 QtConcurrent，退化为同步执行；释放事件循环保持界面响应
    QApplication::processEvents();
    handleResult(updater_.updateFile(entryCopy));
#endif
}

void UiDataUpdate::onUpdateAll()
{
    updateAllBtn_->setEnabled(false);
    refreshBtn_->setEnabled(false);

    auto entries = updater_.refreshFileList();

    // 筛选出需要更新的条目
    QList<DataUpdater::DataFileEntry> pending;
    for (const auto& e : entries)
        if (e.isOutdated)
            pending.append(e);

    if (pending.isEmpty())
    {
        statusLabel_->setText(QString::fromUtf8("所有数据已是最新，无需更新"));
        updateAllBtn_->setEnabled(true);
        refreshBtn_->setEnabled(true);
        return;
    }

    statusLabel_->setText(QString::fromUtf8("正在并发更新 %1 个文件...").arg(pending.size()));

    // 共享计数，用 atomic 保证线程安全
    struct Context
    {
        std::atomic<int> done{0};
        std::atomic<int> success{0};
        int total{};
    };
    auto ctx = std::make_shared<Context>();
    ctx->total = pending.size();

    // 单条更新完成后的公共处理：计数 + 状态刷新 + 全部完成收尾
    auto onOneDone = [this, ctx](errc_t err) {
        if (cancelPending_.load(std::memory_order_acquire))
            return;
        if (err == eNoError)
            ctx->success.fetch_add(1);
        int done = ctx->done.fetch_add(1) + 1;
        statusLabel_->setText(QString::fromUtf8("更新中 %1 / %2 ...").arg(done).arg(ctx->total));

        if (done >= ctx->total)
        {
            int s = ctx->success.load();
            statusLabel_->setText(QString::fromUtf8("更新完成：%1 / %2 成功").arg(s).arg(ctx->total));
            updateAllBtn_->setEnabled(true);
            refreshBtn_->setEnabled(true);
            refreshTable();
        }
    };

#ifndef A_WASM
    for (int i = 0; i < pending.size(); ++i)
    {
        DataUpdater::DataFileEntry entryCopy = pending[i];
        auto* watcher = new QFutureWatcher<errc_t>(this);
        connect(watcher, &QFutureWatcher<errc_t>::finished, this, [this, watcher, onOneDone]() {
            if (cancelPending_.load(std::memory_order_acquire))
            {
                watcher->deleteLater();
                return;
            }
            onOneDone(watcher->result());
            watcher->deleteLater();
        });
        watcher->setFuture(QtConcurrent::run([this, entryCopy]() mutable {
            return updater_.updateFile(entryCopy);
        }));
    }
#else
    // wasm 无多线程与 QtConcurrent，退化为同步串行更新
    for (int i = 0; i < pending.size(); ++i)
    {
        if (cancelPending_.load(std::memory_order_acquire))
            return;
        DataUpdater::DataFileEntry entryCopy = pending[i];
        onOneDone(updater_.updateFile(entryCopy));
        QApplication::processEvents();
    }
#endif
}

void UiDataUpdate::onRefresh()
{
    statusLabel_->setText(QString::fromUtf8("正在刷新..."));
    QApplication::processEvents();
    refreshTable();
}

void UiDataUpdate::onOpenLocation()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = btn->property("row").toInt();
    auto entries = updater_.refreshFileList();
    if (row < 0 || row >= static_cast<int>(entries.size())) return;

    QString filePath = QString::fromStdString(entries[row].localPath);
    QFileInfo fi(filePath);
    QString dir = fi.exists() ? fi.absolutePath() : QFileInfo(filePath).absolutePath();

    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

AST_NAMESPACE_END
