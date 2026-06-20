///
/// @file      UiToolCallTimeline.cpp
/// @brief     工具调用时间线部件实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiToolCallTimeline.hpp"
#include "UiToolCallCard.hpp"  // for EToolCallState
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QTextEdit>
#include <QScrollBar>

AST_NAMESPACE_BEGIN

// ============================================================
//  构造 / 析构
// ============================================================

UiToolCallTimeline::UiToolCallTimeline(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

UiToolCallTimeline::~UiToolCallTimeline() = default;

// ============================================================
//  setupUi
// ============================================================

static const char* kTimelineStyle = R"(
    QWidget#TimelineWidget {
        background-color: #FAFAFA;
        border: 1px solid #E0E0E0;
        border-radius: 8px;
    }
)";

void UiToolCallTimeline::setupUi()
{
    setObjectName(QStringLiteral("TimelineWidget"));
    setStyleSheet(kTimelineStyle);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标题栏按钮
    headerButton_ = new QPushButton(this);
    headerButton_->setCursor(Qt::PointingHandCursor);
    headerButton_->setStyleSheet(
        "QPushButton {"
        "  text-align: left;"
        "  padding: 6px 12px;"
        "  border: none;"
        "  border-radius: 8px;"
        "  background-color: transparent;"
        "  font-size: 12px;"
        "  color: #616161;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F5F5F5;"
        "}"
    );
    connect(headerButton_, &QPushButton::clicked,
            this, &UiToolCallTimeline::toggleCollapsed);
    mainLayout->addWidget(headerButton_);

    // 主体（默认隐藏 → 折叠状态）
    bodyWidget_ = new QWidget(this);
    bodyWidget_->setVisible(false);
    bodyLayout_ = new QVBoxLayout(bodyWidget_);
    bodyLayout_->setContentsMargins(8, 0, 8, 6);
    bodyLayout_->setSpacing(0);
    mainLayout->addWidget(bodyWidget_);

    updateHeaderText();
}

// ============================================================
//  添加条目
// ============================================================

void UiToolCallTimeline::addEntry(const QString& toolCallId,
                                  const QString& functionName,
                                  const QString& arguments)
{
    Entry entry;
    entry.toolCallId   = toolCallId;
    entry.functionName = functionName;
    entry.arguments    = arguments;
    entry.state        = EToolCallState::eRunning;
    entries_.append(entry);

    updateHeaderText();
    rebuildBody();
}

// ============================================================
//  设置条目结果
// ============================================================

void UiToolCallTimeline::setEntryResult(const QString& toolCallId,
                                        const QString& result,
                                        bool success)
{
    for (auto& e : entries_)
    {
        if (e.toolCallId == toolCallId)
        {
            e.result = result;
            e.state  = success ? EToolCallState::eSuccess : EToolCallState::eError;
            break;
        }
    }
    updateHeaderText();
    rebuildBody();
}

// ============================================================
//  折叠 / 展开
// ============================================================

void UiToolCallTimeline::toggleCollapsed()
{
    collapsed_ = !collapsed_;
    bodyWidget_->setVisible(!collapsed_);
    updateHeaderText();
}

// ============================================================
//  条目点击 → 展开/折叠详情
// ============================================================

void UiToolCallTimeline::onEntryClicked(int index)
{
    if (index < 0 || index >= entries_.size()) return;
    entries_[index].detailExpanded = !entries_[index].detailExpanded;
    rebuildBody();
}

// ============================================================
//  更新标题文字
// ============================================================

void UiToolCallTimeline::updateHeaderText()
{
    if (!headerButton_) return;

    int n = entries_.size();
    if (n == 0)
    {
        headerButton_->setText(QString::fromUtf8(
            "\xE2\x96\xB6 \xF0\x9F\x94\xA7 0 个工具调用"));
        headerButton_->setVisible(false);
        return;
    }

    headerButton_->setVisible(true);

    QString expandIcon = collapsed_
        ? QString::fromUtf8("\xE2\x96\xB6")   // ▶
        : QString::fromUtf8("\xE2\x96\xBC");  // ▼

    // 构建函数链
    QStringList names;
    int runningCount = 0, okCount = 0, errCount = 0;
    for (const auto& e : entries_)
    {
        names.append(e.functionName);
        switch (e.state)
        {
        case EToolCallState::eRunning: runningCount++; break;
        case EToolCallState::eSuccess: okCount++; break;
        case EToolCallState::eError:   errCount++; break;
        }
    }

    // 摘要文本
    QString statusStr;
    if (runningCount > 0)
        statusStr = QString::fromUtf8(" (%1 运行中)").arg(runningCount);
    else if (errCount > 0)
        statusStr = QString::fromUtf8(" (%1 失败)").arg(errCount);
    else
        statusStr = QString::fromUtf8(" (全部完成)");

    QString chain = names.join(QString::fromUtf8(" → "));
    if (chain.length() > 80)
        chain = chain.left(77) + "...";

    headerButton_->setText(
        QString::fromUtf8("%1 \xF0\x9F\x94\xA7 %2 个工具调用%3")
            .arg(expandIcon).arg(n).arg(statusStr));
}

// ============================================================
//  重建主体 UI
// ============================================================

static QString stateIcon(EToolCallState state)
{
    switch (state)
    {
    case EToolCallState::eRunning: return QString::fromUtf8("\xE2\x8F\xB3"); // ⏳
    case EToolCallState::eSuccess: return QString::fromUtf8("\xE2\x9C\x85"); // ✅
    case EToolCallState::eError:   return QString::fromUtf8("\xE2\x9D\x8C"); // ❌
    }
    return "?";
}

static QString stateColor(EToolCallState state)
{
    switch (state)
    {
    case EToolCallState::eRunning: return "#1976D2";
    case EToolCallState::eSuccess: return "#388E3C";
    case EToolCallState::eError:   return "#D32F2F";
    }
    return "#9E9E9E";
}

void UiToolCallTimeline::rebuildBody()
{
    if (!bodyLayout_) return;

    // 清除旧内容
    while (bodyLayout_->count() > 0)
    {
        QLayoutItem* item = bodyLayout_->takeAt(0);
        if (item->widget())
            delete item->widget();
        delete item;
    }

    for (int i = 0; i < entries_.size(); ++i)
    {
        const auto& entry = entries_[i];
        bool isLast = (i == entries_.size() - 1);

        // ── 条目容器 ──
        auto* entryFrame = new QFrame(bodyWidget_);
        entryFrame->setFrameShape(QFrame::NoFrame);
        entryFrame->setStyleSheet("QFrame { background: transparent; }");

        auto* entryLayout = new QVBoxLayout(entryFrame);
        entryLayout->setContentsMargins(0, 1, 0, 1);
        entryLayout->setSpacing(0);

        // ── 行：圆点 + 名称 + 状态 ──
        auto* row = new QWidget(entryFrame);
        auto* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(4, 2, 4, 2);
        rowLayout->setSpacing(6);

        // 时间线圆点
        auto* dot = new QLabel(row);
        QString dotColor = stateColor(entry.state);
        dot->setFixedSize(10, 10);
        dot->setStyleSheet(QString(
            "QLabel {"
            "  background-color: %1;"
            "  border-radius: 5px;"
            "  border: 1px solid %1;"
            "}").arg(dotColor));
        rowLayout->addWidget(dot);

        // 函数名（可点击展开详情）
        auto* nameBtn = new QPushButton(row);
        nameBtn->setCursor(Qt::PointingHandCursor);
        QString expandIcon = entry.detailExpanded
            ? QString::fromUtf8("\xE2\x96\xBC") : QString::fromUtf8("\xE2\x96\xB6");
        nameBtn->setText(QString::fromUtf8("%1 %2")
            .arg(expandIcon, entry.functionName));
        nameBtn->setStyleSheet(
            "QPushButton {"
            "  text-align: left;"
            "  border: none;"
            "  background: transparent;"
            "  font-size: 12px;"
            "  font-family: 'Consolas, Monaco, monospace';"
            "  color: #424242;"
            "  padding: 1px 0px;"
            "}"
            "QPushButton:hover {"
            "  color: #1976D2;"
            "}"
        );
        int idx = i;
        connect(nameBtn, &QPushButton::clicked, this, [this, idx]() {
            onEntryClicked(idx);
        });
        rowLayout->addWidget(nameBtn);

        // 参数预览（截断）
        QString argsPreview = entry.arguments;
        if (argsPreview.length() > 50)
            argsPreview = argsPreview.left(47) + "...";
        if (!argsPreview.isEmpty())
        {
            auto* argsLbl = new QLabel(argsPreview, row);
            argsLbl->setStyleSheet("color: #9E9E9E; font-size: 11px;");
            rowLayout->addWidget(argsLbl);
        }

        rowLayout->addStretch();

        // 状态图标
        auto* statusLbl = new QLabel(stateIcon(entry.state), row);
        statusLbl->setStyleSheet("font-size: 13px;");
        statusLbl->setToolTip(
            entry.state == EToolCallState::eRunning ? QString::fromUtf8("运行中") :
            entry.state == EToolCallState::eSuccess ? QString::fromUtf8("成功") :
                                                      QString::fromUtf8("失败"));
        rowLayout->addWidget(statusLbl);

        entryLayout->addWidget(row);

        // ── 详情区（参数 + 结果，默认隐藏） ──
        if (entry.detailExpanded)
        {
            auto* detail = new QWidget(entryFrame);
            auto* detailLayout = new QVBoxLayout(detail);
            detailLayout->setContentsMargins(22, 2, 4, 4);
            detailLayout->setSpacing(3);

            // 参数
            if (!entry.arguments.isEmpty())
            {
                auto* argsTitle = new QLabel(QString::fromUtf8("参数:"), detail);
                argsTitle->setStyleSheet("font-size: 10px; color: #9E9E9E; font-weight: bold;");
                detailLayout->addWidget(argsTitle);

                auto* argsView = new QTextEdit(detail);
                argsView->setReadOnly(true);
                argsView->setPlainText(entry.arguments);
                argsView->setMaximumHeight(60);
                argsView->setStyleSheet(
                    "QTextEdit {"
                    "  background: #F5F5F5; border: 1px solid #E0E0E0; border-radius: 3px;"
                    "  font-family: 'Consolas, Monaco, monospace'; font-size: 10px;"
                    "  padding: 2px 6px;"
                    "}"
                );
                detailLayout->addWidget(argsView);
            }

            // 结果
            if (!entry.result.isEmpty())
            {
                auto* resultTitle = new QLabel(QString::fromUtf8("结果:"), detail);
                resultTitle->setStyleSheet("font-size: 10px; color: #9E9E9E; font-weight: bold;");
                detailLayout->addWidget(resultTitle);

                auto* resultView = new QTextEdit(detail);
                resultView->setReadOnly(true);
                resultView->setPlainText(entry.result);
                resultView->setMaximumHeight(80);
                resultView->setStyleSheet(
                    "QTextEdit {"
                    "  background: #F5F5F5; border: 1px solid #E0E0E0; border-radius: 3px;"
                    "  font-family: 'Consolas, Monaco, monospace'; font-size: 10px;"
                    "  padding: 2px 6px;"
                    "}"
                );
                detailLayout->addWidget(resultView);
            }

            entryLayout->addWidget(detail);
        }

        // ── 连接线（非最后一项）──
        if (!isLast)
        {
            auto* sep = new QFrame(entryFrame);
            sep->setFrameShape(QFrame::HLine);
            sep->setStyleSheet("QFrame { color: #E0E0E0; margin-left: 22px; }");
            sep->setFixedHeight(1);
            entryLayout->addWidget(sep);
        }

        bodyLayout_->addWidget(entryFrame);
    }

    bodyLayout_->addStretch();
}

AST_NAMESPACE_END
