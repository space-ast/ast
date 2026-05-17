///
/// @file      UiResultView.cpp
/// @brief     结果表格和进度条实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiResultView.hpp"
#include <QHeaderView>

AST_NAMESPACE_BEGIN

UiResultView::UiResultView(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiResultView::setupUi()
{
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);

    table_ = new QTableWidget(this);
    table_->setAlternatingRowColors(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->horizontalHeader()->setStretchLastSection(true);
    layout_->addWidget(table_);

    progressBar_ = new QProgressBar(this);
    progressBar_->setTextVisible(true);
    layout_->addWidget(progressBar_);

    auto* infoLayout = new QHBoxLayout();
    progressLabel_ = new QLabel(this);
    infoLayout->addWidget(progressLabel_);
    infoLayout->addStretch();
    timeLabel_ = new QLabel(this);
    infoLayout->addWidget(timeLabel_);
    layout_->addLayout(infoLayout);
}

void UiResultView::initColumns(const QStringList& variableNames, const QStringList& responseNames)
{
    table_->clear();
    QStringList headers;
    headers << "#";
    for (auto& name : variableNames) headers << name;
    for (auto& name : responseNames) headers << name;

    table_->setColumnCount(headers.size());
    table_->setHorizontalHeaderLabels(headers);
    table_->setRowCount(0);
}

void UiResultView::appendRow(int runIndex,
                              const std::vector<double>& varValues,
                              const std::vector<double>& respValues)
{
    int row = table_->rowCount();
    table_->insertRow(row);

    auto setCell = [&](int col, double val) {
        auto* item = new QTableWidgetItem(QString::number(val, 'g', 6));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        table_->setItem(row, col, item);
    };

    setCell(0, static_cast<double>(runIndex));
    int col = 1;
    for (auto v : varValues)  setCell(col++, v);
    for (auto v : respValues) setCell(col++, v);

    table_->scrollToBottom();
}

void UiResultView::clearResults()
{
    table_->setRowCount(0);
    progressBar_->setValue(0);
    progressLabel_->clear();
    timeLabel_->clear();
}

void UiResultView::setProgress(int current, int total)
{
    progressBar_->setRange(0, total);
    progressBar_->setValue(current);
    progressLabel_->setText(tr("Run %1 / %2").arg(current).arg(total));
}

void UiResultView::setElapsedTime(double seconds)
{
    if (seconds < 60.0)
        timeLabel_->setText(tr("Elapsed: %1 s").arg(seconds, 0, 'f', 1));
    else
        timeLabel_->setText(tr("Elapsed: %1 min").arg(seconds / 60.0, 0, 'f', 1));
}

AST_NAMESPACE_END
